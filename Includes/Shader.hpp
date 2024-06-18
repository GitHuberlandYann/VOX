#ifndef SHADER_HPP
# define SHADER_HPP

class Shader
{
    public:
        Shader( void );
        ~Shader( void );

        void setUniformLocation( size_t index, std::string location );
        GLint getUniform( size_t index );

        GLuint getProgram( void );
        void useProgram( void );
        GLuint createProgram( std::string vertex, std::string geometry, std::string fragment );
		void bindFragData( int index, const char* location );
		void bindAttribute( int index, const char* location );
		void linkProgram( void );
        void deleteProgram( void );
    
    private:
        GLuint _program;
        std::vector<GLint> _uniforms;
};

#endif
