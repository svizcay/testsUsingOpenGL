// Local Headers
#include "shader.hpp"

// Standard Headers
#include <cassert>
#include <iostream>
#include <fstream>
#include <memory>


// Define Namespace
namespace Mirage
{
    Shader & Shader::activate()
    {
        // std::cout << "program: " << mProgram << "\n";
        glUseProgram(mProgram);
        return *this;
    }

    void Shader::bind(unsigned int location, float value)
    {
        glUniform1f(location, value);
    }

    void Shader::bind(unsigned int location, float value, float value2)
    {
        glUniform2f(location, value, value2);
    }

    void Shader::bind(unsigned int location, float value, float value2, float value3)
    {
        glUniform3f(location, value, value2, value3);
    }

    void Shader::bind(unsigned int location, glm::mat4 const & matrix)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    GLint Shader::getAttrLocation(std::string const & attr) {
        return glGetAttribLocation(mProgram, attr.c_str());
    }

    GLint Shader::getUniformLocation(std::string const & uniform) {
        GLint location =  glGetUniformLocation(mProgram, uniform.c_str());
        if (location == -1) fprintf(stderr, "Missing Uniform: %s\n", uniform.c_str());
        return location;
    }

    Shader & Shader::attach(std::string const & filename)
    {
        // Load GLSL Shader Source from File
        // std::cout << "PROJECT_SOURCE_DIR: " << PROJECT_SOURCE_DIR << "\n";
        std::string path = PROJECT_SOURCE_DIR "/Glitter/Shaders/";
        std::ifstream fd(path + filename);
        auto src = std::string(std::istreambuf_iterator<char>(fd),
                              (std::istreambuf_iterator<char>()));

        // Create a Shader Object
        const char * source = src.c_str();
        auto shader = create(filename);
        glShaderSource(shader, 1, & source, nullptr);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, & mStatus);

        // Display the Build Log on Error
        if (mStatus == false)
        {
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, & mLength);
            std::unique_ptr<char[]> buffer(new char[mLength]);
            glGetShaderInfoLog(shader, mLength, nullptr, buffer.get());
            fprintf(stderr, "%s\n%s", filename.c_str(), buffer.get());
        }

        // Attach the Shader and Free Allocated Memory
        glAttachShader(mProgram, shader);
        glDeleteShader(shader);
        return *this;
    }

    GLuint Shader::create(std::string const & filename)
    {
        // auto index = filename.rfind(".");
        auto index = filename.find(".");
        auto ext = filename.substr(index + 1);
        // std::cout << "extension: " << ext << "\n";
             if (ext == "comp.glsl") return glCreateShader(GL_COMPUTE_SHADER);
        else if (ext == "frag.glsl") return glCreateShader(GL_FRAGMENT_SHADER);
        else if (ext == "geom.glsl") return glCreateShader(GL_GEOMETRY_SHADER);
        else if (ext == "vert.glsl") return glCreateShader(GL_VERTEX_SHADER);
        else                    return false;
    }

    Shader & Shader::link()
    {
        glLinkProgram(mProgram);
        glGetProgramiv(mProgram, GL_LINK_STATUS, & mStatus);
        if(mStatus == false)
        {
            glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, & mLength);
            std::unique_ptr<char[]> buffer(new char[mLength]);
            glGetProgramInfoLog(mProgram, mLength, nullptr, buffer.get());
            fprintf(stderr, "%s", buffer.get());
        }
        assert(mStatus == true);
        return *this;
    }
}
