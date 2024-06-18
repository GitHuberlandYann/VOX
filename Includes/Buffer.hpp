#ifndef BUFFER_HPP
# define BUFFER_HPP

typedef struct s_attribute {
	int index;
	size_t size;
	GLenum type; // one of GL_INT, GL_UNSIGNED_INT, GL_FLOAT
}				t_attribute;

class Buffer
{
    public:
        Buffer( void );
        ~Buffer( void );

		void genBuffers( void );
		void addAttribute( int index, size_t size, GLenum type );
    	void uploadData( size_t size, const void* data );
		void bindVertexArray( void );
		void deleteBuffers( void );
    
    private:
        GLuint _vao, _vbo;
		size_t _size;
        std::vector<t_attribute> _attributes;
};

#endif
