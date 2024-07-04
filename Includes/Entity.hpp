#ifndef ENTITY_HPP
# define ENTITY_HPP

class Inventory;

class Entity
{
	public:
		Entity( Chunk* chunk, Inventory* inventory, glm::vec3 position, glm::vec3 dir, bool thrown, t_item item );
		virtual ~Entity( void );

		void setChunk( Chunk* chunk );
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

		void getBlasted( glm::vec3 pos, float blast_radius ) override;
		bool update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime ) override;
};

class TNTEntity : public Entity
{
	public:
		TNTEntity( Chunk* chunk, glm::vec3 position, glm::vec3 dir );

		void getBlasted( glm::vec3 pos, float blast_radius ) override;
		bool update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime ) override;
};

class ArrowEntity : public Entity
{
	public:
		ArrowEntity( Chunk* chunk, glm::vec3 position, glm::vec3 dir );

		void getBlasted( glm::vec3 pos, float blast_radius ) override;
		bool update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime ) override;
		bool updateCurrentBlock( void );
	
	private:
		bool _stuck;
		glm::ivec3 _currentBlock;
};

class MovingPistonEntity : public Entity
{
	public:
		MovingPistonEntity( Chunk* chunk, glm::ivec3 source, glm::ivec3 position, glm::ivec3 dir, bool piston_head, bool retraction, int value );

		int pistonedBy( glm::ivec3 pos, glm::ivec3 target ) override;
		bool update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime ) override;

	private:
		bool _piston_head, _retraction, _softKill;
		int _tickStart;
		glm::ivec3 _source, _endPos;
};

class ItemFrameEntity : public Entity
{
	public:
		ItemFrameEntity( Chunk* chunk, glm::ivec3 position, int value );

		void setContent( t_item item );
		t_item popContent( void );
		bool rotate( t_item item );
		int getRotation( void );
		bool isAt( glm::ivec3 pos ) override;

		bool update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime ) override;

		void saveString( std::string& str );

	private:
		short _rotation;
		glm::vec3 _front, _right, _up;
};

#endif
