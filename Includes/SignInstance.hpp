#ifndef SIGNINSTANCE_HPP
# define SIGNINSTANCE_HPP

class SignInstance;
# include "Chunk.hpp"

class SignInstance
{
	public:
		SignInstance( Chunk* chunk, int value, glm::ivec3 pos );
		~SignInstance( void );

		int getValue( void );
		std::string getContent( int index );
		void setContent( std::vector<std::string> content );
		void setChunk( Chunk* chunk );
		void displayText( std::vector<t_shaderInputPart> &partArr );

	private:
		int _value;
		glm::vec3 _pos;
		std::array<std::string, 4> _content;
		Chunk *_chunk;
};

#endif
