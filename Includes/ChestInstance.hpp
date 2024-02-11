#ifndef CHESTINSTANCE_HPP
# define CHESTINSTANCE_HPP

# include <glm/glm.hpp>
# include <array>
# include <vector>
# include "Inventory.hpp"

# define CHEST_ANIM_TIME 0.3f

namespace chest_state {
	enum {
		OPEN,
		CLOSED,
		OPENING,
		CLOSING
	};
}

class ChestInstance
{
	private:
		int _orientation, _state;
		float _timer;
		glm::ivec3 _pos;
		glm::ivec2 _chunk_pos;
		std::array<t_item, 27> _content;
		Chunk *_chunk;

		void display_open( std::vector<std::pair<int, glm::vec3>> &arr );
		void display_moving( std::vector<std::pair<int, glm::vec3>> &arr );
		void display_closed( std::vector<std::pair<int, glm::vec3>> &arr );

	public:
		ChestInstance( Chunk *chunk, glm::ivec3 pos, int orientation );
		~ChestInstance( void );

		void setChunk( Chunk *chunk, glm::ivec3 pos );
		void setState( int state );
		int getOrientation( void );
		t_item *getItem( int index );
		void update( std::vector<std::pair<int, glm::vec3>> &arr, float deltaTime );

		// std::string getInfoString( void );
		void loadContent( std::ofstream & ofs, std::string &line, int &index );
};

#endif
