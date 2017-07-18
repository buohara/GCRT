#include "shader.h"

/**
 * [Shader::Create description]
 * @param nameIn     [description]
 * @param vsFileName [description]
 * @param fsFileName [description]
 */

void Shader::Create(string &nameIn, string &vsFileName, string &fsFileName)
{
    name = nameIn;

    // Read the files into strings.

    ifstream vifs(vsFileName.c_str());
    string vsShaderTxt;
    vsShaderTxt.assign(istreambuf_iterator<char>(vifs), istreambuf_iterator<char>());

    ifstream fifs(fsFileName.c_str());
    string fsShaderTxt;
    fsShaderTxt.assign(istreambuf_iterator<char>(fifs), istreambuf_iterator<char>());

    GLint isCompiled = 0;
    GLchar compileBuf[512];

    // Compile VS.

    const char* vsSource = vsShaderTxt.c_str();

    GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vsID, 1, &vsSource, NULL);
    glCompileShader(vsID);

#ifdef _DEBUG
    glGetShaderiv(vsID, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vsID, GL_INFO_LOG_LENGTH, &maxLength);
        glGetShaderInfoLog(vsID, maxLength, &maxLength, compileBuf);
        glDeleteShader(vsID);
        printf("VS compile error: %s\n", compileBuf);
        __debugbreak();
    }
#endif

    // Compile FS.

    const char* fsSource = fsShaderTxt.c_str();

    GLuint psID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(psID, 1, &fsSource, NULL);
    glCompileShader(psID);

#ifdef _DEBUG
    glGetShaderiv(psID, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(psID, GL_INFO_LOG_LENGTH, &maxLength);

        if (maxLength > 512)
        {
            maxLength = 512;
        }

        glGetShaderInfoLog(psID, maxLength, &maxLength, compileBuf);
        glDeleteShader(psID);
        printf("PS compile error: %s\n", compileBuf);
        __debugbreak();
    }
#endif

    // Link the program.

    GLint programID = glCreateProgram();
    glAttachShader(programID, vsID);
    glAttachShader(programID, psID);
    glLinkProgram(programID);

    program = programID;
}