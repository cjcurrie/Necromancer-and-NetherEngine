#ifndef NEInc_ShaderLoaderProgram_h
  #define NEInc_ShaderLoaderProgram_h

  #include "ShaderCompiler.h"

  #include <vector>


  namespace NE
  {
    class ShaderLoaderProgram {
      public:
        ShaderLoaderProgram(const std::vector<ShaderCompiler>& shaders);
        ShaderLoaderProgram();
        
        /**
         @result The program's object id, as returned from glCreateProgram
         */
        GLuint object() const;
        
        
        /**
         @result The attribute index for the given name, as returned from glGetAttribLocation.
         */
        GLint attrib(const GLchar* attribName) const;
        
        
        /**
         @result The uniform index for the given name, as returned from glGetUniformLocation.
         */
        GLint uniform(const GLchar* uniformName) const;

          
      private:
        GLuint _object;
        
        //copying disabled
        ShaderLoaderProgram(const ShaderLoaderProgram&);
        const ShaderLoaderProgram& operator=(const ShaderLoaderProgram&);
    };
  }

#endif