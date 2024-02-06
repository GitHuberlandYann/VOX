#ifndef ENTITY_HPP
# define ENTITY_HPP

# define ENTITY_LIFETIME 5

class Inventory;

class Entity
{
	private:
		int _value, _amount, _dura;
		bool _solid, _thrown, _falling_block;
		double _lifeTime;
		glm::vec3 _pos, _dir;
		glm::ivec2 _chunk_pos;
		Chunk *_chunk;
		Inventory *_inventory;

		bool updateFallingBlock( std::vector<std::pair<int, glm::vec3>> &arr, double deltaTime );
		bool updateArrow( std::vector<std::pair<int, glm::vec3>> &arr, double deltaTime );

	public:
		Entity( Chunk *chunk, Inventory *inventory, glm::vec3 position, glm::vec3 dir, bool solid, bool thrown, int value, int amount = 1, int dura = 0 );
		~Entity( void );

		bool update( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camPos, double deltaTime );
};

#endif
