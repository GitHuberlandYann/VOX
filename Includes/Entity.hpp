#ifndef ENTITY_HPP
# define ENTITY_HPP

class Inventory;

class Entity
{
	private:
		bool _thrown;

	protected:
		t_item _item;
		double _lifeTime;
		glm::vec3 _pos, _dir;
		glm::ivec2 _chunk_pos;
		Chunk *_chunk;
		Inventory *_inventory;

	public:
		Entity( Chunk *chunk, Inventory *inventory, glm::vec3 position, glm::vec3 dir, bool thrown, t_item item );
		virtual ~Entity( void );

		void setLifetime( double lifetime );
		virtual void getBlasted( glm::vec3 pos, float blast_radius );
		virtual int pistonedBy( glm::ivec3 pos );
		virtual bool update( std::vector<t_shaderInput> &arr,  std::vector<t_shaderInput> &partArr, glm::vec3 camPos, double deltaTime );
};

class FallingBlockEntity : public Entity
{
	private:
	
	public:
		FallingBlockEntity( Chunk *chunk, glm::vec3 position, t_item item );

		virtual void getBlasted( glm::vec3 pos, float blast_radius );
		virtual bool update( std::vector<t_shaderInput> &arr,  std::vector<t_shaderInput> &partArr, glm::vec3 camPos, double deltaTime );
};

class TNTEntity : public Entity
{
	private:
	
	public:
		TNTEntity( Chunk *chunk, glm::vec3 position, glm::vec3 dir );

		virtual void getBlasted( glm::vec3 pos, float blast_radius );
		virtual bool update( std::vector<t_shaderInput> &arr,  std::vector<t_shaderInput> &partArr, glm::vec3 camPos, double deltaTime );
};

class ArrowEntity : public Entity
{
	private:
		bool _stuck;
	
	public:
		ArrowEntity( Chunk *chunk, glm::vec3 position, glm::vec3 dir );

		virtual void getBlasted( glm::vec3 pos, float blast_radius );
		virtual bool update( std::vector<t_shaderInput> &arr,  std::vector<t_shaderInput> &partArr, glm::vec3 camPos, double deltaTime );
};

class MovingPistonEntity : public Entity
{
	private:
		bool _piston_head, _retraction;
		int _tickStart;
		glm::ivec3 _source, _endPos;
	
	public:
		MovingPistonEntity( Chunk *chunk, glm::ivec3 source, glm::ivec3 position, glm::ivec3 dir, bool piston_head, bool retraction, int value );

		virtual int pistonedBy( glm::ivec3 pos );
		virtual bool update( std::vector<t_shaderInput> &arr,  std::vector<t_shaderInput> &partArr, glm::vec3 camPos, double deltaTime );
};

#endif
