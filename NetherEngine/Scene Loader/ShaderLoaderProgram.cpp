#include "ShaderLoaderProgram.h"



NE::ShaderLoaderProgram::ShaderLoaderProgram(const std::vector<ShaderCompiler>& shadersNeededForScene) :
_object(0)
{
    if(shadersNeededForScene.size() <= 0)
        throw std::runtime_error("No shaders were provided to create the program");
    
    //create the program object
    _object = glCreateProgram();
    if(_object == 0)
        throw std::runtime_error("glCreateProgram failed");
    
    //attach all the shaders
    for(unsigned i = 0; i < shadersNeededForScene.size(); ++i)
        glAttachShader(_object, shadersNeededForScene[i].object());
    
    //link the shaders together
    glLinkProgram(_object);
    
    //detach all the shaders
    for(unsigned i = 0; i < shadersNeededForScene.size(); ++i)
        glDetachShader(_object, shadersNeededForScene[i].object());
    
    //throw exception if linking failed
    GLint status;
    glGetProgramiv(_object, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        std::string msg("Program linking failure: ");
        
        GLint infoLogLength;
        glGetProgramiv(_object, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog(_object, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;
        
        glDeleteProgram(_object); _object = 0;
        throw std::runtime_error(msg);
    }
}

NE::ShaderLoaderProgram::ShaderLoaderProgram() {
    //might be 0 if ctor fails by throwing exception
    if(_object != 0) glDeleteProgram(_object);
}

GLuint NE::ShaderLoaderProgram::object() const {
    return _object;
}

GLint NE::ShaderLoaderProgram::attrib(const GLchar* attribName) const {
    if(!attribName)
        throw std::runtime_error("attribName was NULL");
    
    GLint attrib = glGetAttribLocation(_object, attribName);
    if(attrib == -1)
        throw std::runtime_error(std::string("Program attribute not found: ") + attribName);
    
    return attrib;
}

GLint NE::ShaderLoaderProgram::uniform(const GLchar* uniformName) const {
    if(!uniformName)
        throw std::runtime_error("uniformName was NULL");
    
    GLint uniform = glGetUniformLocation(_object, uniformName);
    if(uniform == -1)
        throw std::runtime_error(std::string("Program uniform not found: ") + uniformName);
    
    return uniform;
}
