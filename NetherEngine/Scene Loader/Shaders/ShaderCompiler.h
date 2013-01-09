#ifndef NEInc_ShaderCompiler_h
#define NEInc_ShaderCompiler_h

  #ifndef Inc_glew_h
  #define Inc_glew_h
  #include <GL/glew.h>
  #endif
  
  #ifndef NEInc_NEAssert_h
  #include "NEAssert.h"
  #endif


  //#include <string>

  #include <stdexcept>

#include <fstream>

namespace NE
{
    class ShaderCompiler { 
    public:
        
        /**
         Creates a shader from a text file.
         
         @param filePath    The path to the text file containing the shader source.
         @param shaderType  Same as the argument to glCreateShader. For example GL_VERTEX_SHADER
                            or GL_FRAGMENT_SHADER.
         
         @throws std::exception if an error occurs.
         */
        static ShaderCompiler shaderFromFile(const std::string& filePath, GLenum shaderType);
        
        
        /**
         Creates a shader from a string of shader source code.
         
         @param shaderCode  The source code for the shader.
         @param shaderType  Same as the argument to glCreateShader. For example GL_VERTEX_SHADER
                            or GL_FRAGMENT_SHADER.
         
         @throws std::exception if an error occurs.
         */
        ShaderCompiler(const std::string& shaderCode, GLenum shaderType);
        
        
        /**
         @result The shader's object ID, as returned from glCreateShader
         */
        GLuint object() const;
        
        // tdogl::Shader objects can be copied and assigned because they are reference counted
        // like a shared pointer
        ShaderCompiler(const ShaderCompiler& other);
        ShaderCompiler& operator =(const ShaderCompiler& other);
        ShaderCompiler();
        ~ShaderCompiler();
      
    private:
        GLuint _object;
        unsigned* _refCount;
        
        void _retain();
        void _release();
    };
}
#endif