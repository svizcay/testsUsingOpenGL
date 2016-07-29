#pragma once

// System Headers
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Standard Headers
#include <string>

// Define Namespace
namespace Mirage
{
    class Shader
    {
    public:

        // Implement Custom Constructor and Destructor
         Shader() { mProgram = glCreateProgram(); }
        ~Shader() { glDeleteProgram(mProgram); }

        // Public Member Functions
        Shader & activate();
        Shader & attach(std::string const & filename);
        GLuint   create(std::string const & filename);
        GLuint   get() { return mProgram; }
        Shader & link();

        // TODO: add this functionality only in development to avoid overhead
        /*
         * check wheter the program can execute given the current pipeline state
         * call to glValidateProgram(program);
         */
        // Shader & validate();

        // Wrap Calls to glUniform
        void bind(unsigned int location, float value);
        void bind(unsigned int location, float value, float value2);
        void bind(unsigned int location, float value, float value2, float value3);
        void bind(unsigned int location, glm::mat4 const & matrix);

        template<typename T> Shader & bind(std::string const & name, T&& value)
        {
            int location = glGetUniformLocation(mProgram, name.c_str());
            if (location == -1) fprintf(stderr, "Missing Uniform: %s\n", name.c_str());
            else bind(location, std::forward<T>(value));
            return *this;
        }
        template<typename T> Shader & bind(std::string const & name, T&& value, T&& value2)
        {
            int location = glGetUniformLocation(mProgram, name.c_str());
            if (location == -1) fprintf(stderr, "Missing Uniform: %s\n", name.c_str());
            else bind(location, std::forward<T>(value), std::forward<T>(value2));
            return *this;
        }
        template<typename T> Shader & bind(std::string const & name, T&& value, T&& value2, T&& value3)
        {
            fprintf(stdout, "entro\n");
            int location = glGetUniformLocation(mProgram, name.c_str());
            fprintf(stdout, "location %d\n", location);
            if (location == -1) fprintf(stderr, "Missing Uniform: %s\n", name.c_str());
            else bind(location, std::forward<T>(value), std::forward<T>(value2), std::forward<T>(value3));
            return *this;
        }

        // Wrap to glGetAttribLocation
        GLint getAttrLocation(std::string const & attr);
        GLint getUniformLocation(std::string const & uniform);

    private:

        // Disable Copying and Assignment
        Shader(Shader const &) = delete;
        Shader & operator=(Shader const &) = delete;

        // Private Member Variables
        GLuint mProgram;
        GLint  mStatus;
        GLint  mLength;

    };
}
