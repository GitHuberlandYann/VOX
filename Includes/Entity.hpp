#ifndef ENTITY_HPP
# define ENTITY_HPP

# define ENTITY_LIFETIME 5

class Entity
{
	private:
		int _value, _dura, _tool_dura;
		double _start_time;
		glm::vec3 _position, _dir;

	public:
		Entity( void );
		~Entity( void );
};

#endif
