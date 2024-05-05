#ifndef ENTITY_HPP
# define ENTITY_HPP

# define ENTITY_LIFETIME 5

class Inventory;

class Entity
{
	private:
		t_item _item;
		bool _solid, _thrown, _falling_block, _stuck;
		double _lifeTime;
		glm::vec3 _pos, _dir;
		glm::ivec2 _chunk_pos;
		Chunk *_chunk;
		Inventory *_inventory;

		bool updateTNT( std::vector<t_shaderInput> &arr, double deltaTime );
		bool updateFallingBlock( std::vector<t_shaderInput> &arr, double deltaTime );
		bool updateArrow( std::vector<t_shaderInput> &arr, float deltaTime );

	public:
		Entity( Chunk *chunk, Inventory *inventory, glm::vec3 position, glm::vec3 dir, bool solid, bool thrown, t_item item );
		~Entity( void );

		void setLifetime( double lifetime );
		void getBlasted( glm::vec3 pos, float blast_radius );
		bool update( std::vector<t_shaderInput> &arr,  std::vector<t_shaderInput> &partArr, glm::vec3 camPos, double deltaTime );
};

#endif
