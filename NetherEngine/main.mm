/*
    main.mm
    A demonstration of the NetherEngine game engine
    by CJ Currie
    cj@cjcurrie.net
    All rights reserved.
 
    Inspired by a variety of wonderful guides. My thanks to:
      - Richard "superpig" Fine for the Enginuity series
      - Tom Dalling for the Modern OpenGL Guide series
*/



// standard C++ libraries
//  @TODO: Verify the include sequence
// Note we don't need to #ifndef before, because main.mm is guaranteed to be the first thing compiled.
//   Consequently, we ought to define macros showing which non-NE libraries have been included. (Currently commented-out)

// --- Standard libs ---
  #include <iostream>
  //#define Inc_iostream

  #include <cmath>
  //#define Inc_cmath

  #include <stdexcept>
  //#define Inc_stdexcept



// --- Third-party libraries ---
  #import <Foundation/Foundation.h>
  #define Inc_Foundation_h

  #include <GL/glew.h>
  #define Inc_glew_h

  #include <GL/glfw.h>    // also includes glu.h
  #define Inc_glfw_h

  #include <glm/glm.hpp>
  #define Inc_glm_hpp



// --- NE classes ---


  // ---- Memory, errors, and logging

    /* Before NEAssert.h, we can #define NOASSERT in order to strip out all ASSERT() and ALLEGE() macros from NEAssert.h
        Note that REQUIRE() ENSURE() and INVARIANT() will still evaluate and null-execute (do nothing). For now they stays.
     */
    // #define NOASSERT
    #include "NEAssert.h"       // This works like c assert, but passes on
                                //  to NEAssert::onAssert__(char const *file, unsigned line) afterward

    
#include "ManagedMemObj.h"
#include "StaticBufferWrapper.h"
#include "DynamicBufferWrapper.h"
#include "ObjFunctor.h"
#include "Functor.h"
#include "ManMemObjFunctor.h"
#include "Singleton.h"
#include "RingBuffer.h"


  // ---- Rendering

    #include "ShaderLoaderProgram.h"    // this #defines NEInc_ShaderLoaderProgram_h




// === Program ===
using namespace NE;   // For the NetherEngine

// constants
const glm::vec2 SCREEN_SIZE(1200, 700);

// globals
ShaderLoaderProgram* g_ShaderLoaderProgram = NULL;
GLuint g_VAO = 0;
GLuint g_VBO = 0;

// ============
//  Since Obj-C headers are hard, here are some forward
//  declarations instead
// ============

static void LoadShaders();
static void LoadTriangles();
static void Render();

// === Main ===
int main(int argc, char *argv[]) {

  //DEFINE_THIS_FILE;   // Required to use ASSERTions
  
  
  // --- Initialise GLFW
  if(!glfwInit())     // Equivalent to returning GL_FALSE for failed initialization
      throw std::runtime_error("glfwInit failed");


  // --- Open a window with GLFW
  glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);       // Unfortunately, GLFW only supports 3.2 on Mac
  glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);

  //Initialize window. Sets window width, height, red, blue, green, and alpha bit depth, depth ('z') buffer bit depth, stencil bit depth, and wheter to use windowed or fullscreen mode
  if(!glfwOpenWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, 8, 8, 8, 8, 0, 0, GLFW_WINDOW))
      throw std::runtime_error("glfwOpenWindow failed. Can your hardware handle OpenGL 3.2?");


  // -- Initialise GLEW
  glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
  if(glewInit() != GLEW_OK)
      throw std::runtime_error("glewInit failed for an unknown reason.");

  // Clears OpenGL errors
  glGetError();

  // print out some info about the graphics drivers
  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  
  
  // load vertex and fragment shaders into opengl
  LoadShaders();

  // create buffer and fill it with the points of the triangle
  LoadTriangles();
  
  
  //ASSERT(glGetError() != 0 && "OpenGL error detected.");
  // If the code gets past here, then initialization worked.
  //  If it didn't, then use glGetError and convert the result from hex to decimal. See the OpenGL docs to debug.
  

  // run while the window is open
  while(glfwGetWindowParam(GLFW_OPENED)){
      // draw one frame
      Render();
  }

  // clean up and exit
  glfwTerminate();        // All resources are freed. After this is called, glwfInit() must be executed before the library can be used again.0

  // Clean up any object left in our managed memory.
  ManagedMemObj::CollectRemainingObjects( true );

  return EXIT_SUCCESS;
}


// --- FOR MAC ONLY ---
// returns the full path to the file `fileName` in the resources directory of the app bundle
static std::string ResourcePath(std::string fileName) {
    NSString* fname = [NSString stringWithCString:fileName.c_str() encoding:NSUTF8StringEncoding];
    NSString* path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:fname];
    return std::string([path cStringUsingEncoding:NSUTF8StringEncoding]);
}
// @TODO: other OS filepaths ResourcePath()


// Including here, because this include and the shader loading needs to be moved to another class
#include <vector>
// @TODO: move shader loading

// loads the vertex shader and fragment shader, and links them to make the global gShaderLoaderProgram
static void LoadShaders()
{
  std::vector<NE::ShaderCompiler> shaders;
  shaders.push_back(ShaderCompiler::shaderFromFile(ResourcePath("Shaders/vertex-shader.txt"), GL_VERTEX_SHADER));
  shaders.push_back(ShaderCompiler::shaderFromFile(ResourcePath("Shaders/fragment-shader.txt"), GL_FRAGMENT_SHADER));
  g_ShaderLoaderProgram = new ShaderLoaderProgram(shaders);
}


// loads a triangle into the VAO global
static void LoadTriangles   ()
{
  // make and bind the VAO
  glGenVertexArrays(1, &g_VAO);
  glBindVertexArray(g_VAO);

  // make and bind the VBO
  glGenBuffers(1, &g_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, g_VBO);


  /*
   
   Hex y-axis
   
   
   Triangle sides and ordering
   0,1 hex
   
   
   /+y axis
   /
    /\-------------\------------1,1 hex
   /  \          /  \           /  \  par 1   /
  /    \        /    \         /    \   1    /
        \      /      \       /      \      /
         \    /        \     /  par1  \    /
          \  /          \   /     0    \  /
           \/            \ /            \/
            b------------0,0 both-------/\
          / |\           /|\           /  \
  \       /  | \         / | \         /    \
  \     / s2|s3\       /  |  \       /      \
   \   /    |   \     /   |   \     /        \
    \ /     | s1 \   /    |    \   /          \
   -1,-1--------------------------------------1,0 hex
   side 3
   
   */

  int i_chunkSideLength = 64,
      i_indicesPerSide = i_chunkSideLength*3;
  GLfloat i_side1Length = 12,
  i_side2Length = 16, side2over2 = 8,
  i_side3Length = 20;     // This is a 3-4-5 triangle scaled by 4

  //float flArr_chunkVertexData[i_chunkSideLength*3][i_chunkSideLength*3][3];
  //flArr_chunkVertexData.length = (i_chunkSideLength*i_chunkSideLength + 2) * 3u;
  //GLfloat GLflArr_vertexData[i_chunkSideLength*i_chunkSideLength*27];



  // This is the origin point for both axis systems
  //flArr_chunkVertexData[0][0][0] = 0.0f;
  //flArr_chunkVertexData[0][0][1] = 0.0f;
  //flArr_chunkVertexData[0][0][2] = 0.0f;

  //    for (unsigned int y=0; y<i_chunkSideLength; y++)
  //    {
  //        for (unsigned int x=0; x<i_chunkSideLength; x++)
  //        {
  //            if (y==0)       // This is a new row, along the cartesian y axis
  //            {
  //              //flArr_chunkVertexData[x][y][0] = side2over2;
  //              //flArr_chunkVertexData[x][y][1] = i_side2Length;
  //              //flArr_chunkVertexData[x][y][2] = 0.0f;
  //              
  //              GLflArr_vertexData[x*3 + y*i_chunkSideLength*3] = side2over2;
  //              GLflArr_vertexData[x*3 + y*i_chunkSideLength*3 + 1] = i_side2Length;
  //              GLflArr_vertexData[x*3 + y*i_chunkSideLength*3 + 2] = 0.0f;
  //            }
  //            else
  //            {
  //                if (x%2==0)         // First half of parallelogram, needs a "down" points
  //                {
  //                  GLflArr_vertexData[x*3 + y*i_chunkSideLength*3] = x*i_side3Length;
  //                  GLflArr_vertexData[x*3 + y*i_chunkSideLength*3 + 1] = y*i_side2Length;
  //                  GLflArr_vertexData[x*3 + y*i_chunkSideLength*3 + 2] = 0.0f;
  //                }
  //                else        // Second half, needs "up" point
  //                {
  //                  GLflArr_vertexData[x*3 + y*i_chunkSideLength*3] = x*i_side3Length + i_side1Length;
  //                  GLflArr_vertexData[x*3 + y*i_chunkSideLength*3 + 1] = y*i_side2Length + i_side2Length;
  //                  GLflArr_vertexData[x*3 + y*i_chunkSideLength*3 + 2] = 0.0f;
  //                }
  //            }
  //        }
  //    }

  GLfloat vertexData[] = {
  //  X     Y     Z
  0.0f, 0.8f, 0.0f,
  -0.8f,-0.8f, 0.0f,
  0.8f,-0.8f, 0.0f,
  };

  // Put the triangle array into buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

  // connect the xyz to the "vert" attribute of the vertex shader
  glEnableVertexAttribArray(g_ShaderLoaderProgram->attrib("vert"));
  glVertexAttribPointer(g_ShaderLoaderProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

  // connect the x, y, and z to the "vert" attribute of the fragment shader
  //glVertexAttribPointer(gProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

  // unbind the VBO and VAO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}


// draws a single frame
static void Render() {
    // clear everything
    glClearColor(0, 0, 0, 1); // black
    glClear(GL_COLOR_BUFFER_BIT);
    
    // bind the program (the shaders)
    glUseProgram(g_ShaderLoaderProgram->object());
    
    // bind the VAO (the triangle)
    glBindVertexArray(g_VAO);
    
    // draw the VAO
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    // unbind the VAO
    glBindVertexArray(0);
    
    // unbind the program
    glUseProgram(0);
    
    // swap the display buffers (displays what was just drawn)
    glfwSwapBuffers();
}