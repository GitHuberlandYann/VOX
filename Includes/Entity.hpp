#ifndef ENTITY_HPP
# define ENTITY_HPP

# define ENTITY_LIFETIME 5

class Entity
{
	private:
		int _value, _amount, _dura, _tool_dura;
		float _rotation;
		double _startTime;
		glm::vec3 _position, _dir;

	public:
		Entity( glm::vec3 position, glm::vec2 dir, int value, int amount = 1 );
		~Entity( void );

		bool update( Chunk *chunk, std::vector<std::pair<int, glm::vec3>> &arr, double currentTime );
};

#endif
