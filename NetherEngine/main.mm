jh/*
    main.mm
    A demonstration of the NetherEngine game engine
    by CJ Currie
    cj@cjcurrie.net
    All rights reserved.
 
    Inspired by a variety of wonderful guides. My thanks to:
      - Richard "superpig" Fine for the Enginuity series
      - Tom Dalling for the Modern OpenGL Guide series
*/

// @Damien: Make sure this main.mm file has an extension your compiler can use. *.mm is for Objective-C (and reg. c++) files on Mac.

// ======================
//      Global flags
// ======================
#define NE_DEBUG    // For testing builds only (debug on)

//#define NOASSERT    // For published builds only (conserves space, eliminates ASSERT-family checks)

#define MAC_BUILD
//#define WIN_BUILD     // @Damien: toggle from mac to win here.
//#define LIN_BUILD


// ======================
// Standard C++ libraries
// ======================
  //#ifndef Inc_iostream
  #include <iostream>
  #define Inc_iostream
  //#endif

  //#ifndef Inc_cmath
  #include <cmath>
  //#define Inc_cmath
  //#endif

  #include <stdexcept>


// ======================
//  Third-party libraries
// ======================
  #ifndef Inc_glew_h
  #include <GL/glew.h>
  #define Inc_glew_h
  #endif

  #ifndef Inc_glfw_h
  #include <GL/glfw.h>    // also includes glu.h
  #define Inc_glfw_h
  #endif

  #ifndef Inc_glm_hpp
  #include <glm/glm.hpp>
  #define Inc_glm_hpp
  #endif


// ======================
//    Platform-specific
// ======================
  #ifdef MAC_BUILD

    #ifndef Inc_Foundation_h
    #import <Foundation/Foundation.h>
    #define Inc_Foundation_h
    #endif

    #ifndef Inc_CoreFoundation_h
    #include "CoreFoundation/CoreFoundation.h"
    #define Inc_CoreFoundation_h
    #endif
    
  #endif


// ======================
// NetherEngine libraries
// ======================

  // ---- Memory, errors, and logging ---
    #include "NEAssert.h"       // This works like c assert, but passes on
                                //  to NEAssert::onAssert__(char const *file, unsigned line) afterward

    #include "ManagedMemObj.h"

    #include "Log.h"


  // ---- Rendering ---

    #include "ShaderLoaderProgram.h"    // this #defines NEInc_ShaderLoaderProgram_h



// ======================
//   Program Declaration
// ======================
  using namespace NE;   // For the NetherEngine

  // Constant objects
  const glm::vec2 SCREEN_SIZE(1200, 700);

  // Global objects
  ShaderLoaderProgram* g_ShaderLoaderProgram = NULL;
  GLuint g_VAO = 0;   // Vertex array object
  GLuint g_VBO = 0;   // Vertex buffer object


  // Since Obj-C headers are hard, here are some forward  declarations instead
  static void LoadShaders(), LoadTriangles(), Render(), EstablishWorkingDirectory();



// ======================
//          Main
// ======================
  int main(int argc, char *argv[]) {
    
    DEFINE_THIS_FILE;
    
    
    EstablishWorkingDirectory();
    
    // --- Initialize the Log singleton
      // We REQUIRE (like ENSURE and INVARIANT) instead of ASSERT because the evaluation operation needs must
      //  be done even when NOASSERT is defined.
    REQUIRE(Log::Get().Init());    // msg = "Log initialization failed."
    
    
    // --- Initialise GLFW
    REQUIRE(glfwInit() != GL_FALSE);    // msg = "glfwInit failed");


    // --- Open a window with GLFW
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);       // Unfortunately, GLFW only supports 3.2 on Mac
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);

    //Initialize window. Sets window width, height, red, blue, green, and alpha bit depth, depth ('z') buffer bit depth, stencil bit depth, and wheter to use windowed or fullscreen mode
    REQUIRE(glfwOpenWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, 8, 8, 8, 8, 0, 0, GLFW_WINDOW) == GL_TRUE);    // msg = "glfwOpenWindow failed. Can your hardware handle OpenGL 3.2?"


    // -- Initialise GLEW
    #ifdef MAC_BUILD
      glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
    #endif
    
    REQUIRE(glewInit() == GLEW_OK);    // msg = "glewInit failed for an unknown reason."

    // Clears OpenGL errors
    ENSURE(glGetError() != 0);  // msg = "GL error encountered"

    
    // print out some info about the graphics drivers. Condensing it like this is much faster than using separate calls.
                                // Format
    Log::Get().Write( LOG_USER, "OpenGL version: %s\n"
                                "GLSL version: %s\n"
                                "Vendor: %s\n"
                                "Renderer: %s\n",
                                // Arguments
                                glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION),
                                glGetString(GL_VENDOR), glGetString(GL_RENDERER)
                    );
    
    
    // load vertex and fragment shaders into opengl
    LoadShaders();

    // create buffer and fill it with the points of the triangle
    LoadTriangles();
    
    
    
    // See if the initialization worked
    ENSURE(glGetError() == 0);    // msg = "GL error encountered"
    

    
    // run while the window is open
    while(glfwGetWindowParam(GLFW_OPENED)){
        // draw one frame
        Render();
    }

    // clean up and exit
    glfwTerminate();        // All resources are freed. After this is called, glwfInit() must be executed before the library can be used again.

    // Clean up any object left in our managed memory.
    ManagedMemObj::CollectRemainingObjects( true );

    return EXIT_SUCCESS;
  }


// ========================
//  Program Implementation
// ========================

// --- Platform-specific filesystem access ---
#ifdef MAC_BUILD
  // returns the full path to the file `fileName` in the resources directory of the app bundle
  static std::string ResourcePath(std::string fileName)
  {
    NSString* fname = [NSString stringWithCString:fileName.c_str() encoding:NSUTF8StringEncoding];
    NSString* path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:fname];
    return std::string([path cStringUsingEncoding:NSUTF8StringEncoding]);
  }

#elif defined WIN_BUILD  
// @Damien: You won't be able to load the shaders from the filesystem until this method is completed.
  static std::string ResourcePath(std::string fileName)
  {
    return "failed";
  }

#elif defined LIN_BUILD
  static std::string ResourcePath(std::string fileName)
  {
    return "failed";
  }
#endif



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

  GLfloat vertexData[] =
  {
    // X    Y       Z
    0.0f,   0.8f,   0.0f,
    -0.8f,  -0.8f,  0.0f,
    0.8f,   -0.8f,  0.0f,
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

static void EstablishWorkingDirectory()
{
  DEFINE_THIS_FILE;
  
  // @Damien: This method needs a windows version
  
  char path[PATH_MAX];
    
  #ifdef MAC_BUILD
    // Note that "CoreFoundation/CoreFoundation.h" is used here
  
    // This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle
  
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
  
    ENSURE( CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX ) );     // msg = "idk why fail, bro"

    CFRelease(resourcesURL);
    
    chdir(path);
  
  #endif
  
  // @TODO: other builds
  
  Log::Get().Write( LOG_USER, "Working directory: %s", path);
}