#include "ShaderCompiler.h"

DEFINE_THIS_FILE;  // used by NEAssert.h to handle errors


  NE::ShaderCompiler::ShaderCompiler(const std::string& shaderCode, GLenum shaderType) :  _object(0), _refCount(NULL)
  {
    std::cout << "Pre-glCreateShader() errors:" << glGetError() << std::endl;
    
    //create the shader object
    _object = glCreateShader(shaderType);   // Returns a non-zero reference id for the shader object. (glGetShader(GLInt id))"
    
    std::cout << "After creating the shader, glGetError() yielded " << glGetError() << " when given the shaderType enum number " << shaderType << std::endl;
    
    

    ASSERT(_object == 0 && "glCreateShader failed");

    //set the source code
    const char* code = shaderCode.c_str();
    glShaderSource(_object, 1, (const GLchar**)&code, NULL);

    //compile
    glCompileShader(_object);

    //throw exception if compile error occurred
    GLint status;
    glGetShaderiv(_object, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        std::string msg("Compile failure in shader:\n");
        
        GLint infoLogLength;
        glGetShaderiv(_object, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        glGetShaderInfoLog(_object, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;
        
        glDeleteShader(_object); _object = 0;
        throw std::runtime_error(msg);
    }

    _refCount = new unsigned;
    *_refCount = 1;
  }


  // Constructor
  NE::ShaderCompiler::ShaderCompiler(const ShaderCompiler& other) :
      _object(other._object),
      _refCount(other._refCount)
  {
      _retain();
  }

  // Destructor
  NE::ShaderCompiler::~ShaderCompiler() {
      //_refCount will be NULL if constructor failed and threw an exception
      if(_refCount)
          _release();
  }


  GLuint NE::ShaderCompiler::object() const {
      return _object;
  }

  NE::ShaderCompiler& NE::ShaderCompiler::operator = (const NE::ShaderCompiler& other) {
      _release();
      _object = other._object;
      _refCount = other._refCount;
      _retain();
      return *this;
  }

  NE::ShaderCompiler NE::ShaderCompiler::shaderFromFile(const std::string& filePath, GLenum shaderType) {
      //open file
      std::ifstream f;
      f.open(filePath.c_str());
      if(!f.is_open()){
          throw std::runtime_error(std::string("Failed to open file: ") + filePath);
      }
      
      //get length of file
      f.seekg(0, std::ios::end);
      std::streampos length = f.tellg();
      f.seekg(0, std::ios::beg);
      
      //read whole file into a char buffer
      char* sourceCode = new char[(size_t)length+1];
      f.read(sourceCode, length);
      sourceCode[(size_t)length] = 0;
      
      //convert char buffer into std::string
      std::string sourceCodeStr(sourceCode);
      delete[] sourceCode; sourceCode = NULL;
      
      //return new shader
      return ShaderCompiler(sourceCodeStr, shaderType);
  }

  void NE::ShaderCompiler::_retain() {
      ASSERT(_refCount);
      *_refCount += 1;
  }

  void NE::ShaderCompiler::_release() {
      ASSERT(_refCount && *_refCount > 0);
      *_refCount -= 1;
      if(*_refCount == 0){
          glDeleteShader(_object); _object = 0;
          delete _refCount; _refCount = NULL;
      }
  }
