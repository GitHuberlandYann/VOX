#ifndef ENTITY_HPP
# define ENTITY_HPP

# define ENTITY_LIFETIME 5

class Inventory;

class Entity
{
	private:
		int _value, _amount, _dura;
		bool _thrown;
		double _lifeTime;
		glm::vec3 _pos, _dir;
		glm::ivec2 _chunk_pos;
		Chunk *_chunk;
		Inventory *_inventory;

	public:
		Entity( Chunk *chunk, Inventory *inventory, glm::vec3 position, glm::vec2 dir, bool thrown, int value, int amount = 1, int dura = 0 );
		~Entity( void );

		bool update( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camPos, double deltaTime );
};

#endif
