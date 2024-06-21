#ifndef ENTITY_HPP
# define ENTITY_HPP

class Inventory;

class Entity
{
	public:
		Entity( Chunk* chunk, Inventory* inventory, glm::vec3 position, glm::vec3 dir, bool thrown, t_item item );
		virtual ~Entity( void );

		void setLifetime( double lifetime );
		virtual bool isAt( glm::ivec3 pos );
		virtual void getBlasted( glm::vec3 pos, float blast_radius );
		virtual int pistonedBy( glm::ivec3 pos, glm::ivec3 target );
		virtual bool update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime );

	protected:
		t_item _item;
		double _lifeTime;
		glm::vec3 _pos, _dir;
		glm::ivec2 _chunk_pos;
		Chunk *_chunk;
		Inventory *_inventory;

	private:
		bool _thrown;
};

class FallingBlockEntity : public Entity
{
	public:
		FallingBlockEntity( Chunk* chunk, glm::vec3 position, t_item item );

		virtual void getBlasted( glm::vec3 pos, float blast_radius );
		virtual bool update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime );
};

class TNTEntity : public Entity
{
	public:
		TNTEntity( Chunk* chunk, glm::vec3 position, glm::vec3 dir );

		virtual void getBlasted( glm::vec3 pos, float blast_radius );
		virtual bool update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime );
};

class ArrowEntity : public Entity
{
	public:
		ArrowEntity( Chunk* chunk, glm::vec3 position, glm::vec3 dir );

		virtual void getBlasted( glm::vec3 pos, float blast_radius );
		virtual bool update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime );
	
	private:
		bool _stuck;
};

class MovingPistonEntity : public Entity
{
	public:
		MovingPistonEntity( Chunk* chunk, glm::ivec3 source, glm::ivec3 position, glm::ivec3 dir, bool piston_head, bool retraction, int value );

		virtual int pistonedBy( glm::ivec3 pos, glm::ivec3 target );
		virtual bool update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime );

	private:
		bool _piston_head, _retraction, _softKill;
		int _tickStart;
		glm::ivec3 _source, _endPos;
};

class ItemFrameEntity : public Entity
{
	public:
		ItemFrameEntity( Chunk* chunk, glm::ivec3 position, int value );

		void setContent( int type );
		int getContent( void );
		bool rotate( int type );
		int getRotation( void );
		virtual bool isAt( glm::ivec3 pos );

		virtual bool update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime );

	private:
		short _rotation;
		glm::vec3 _front, _right, _up;
};

#endif
