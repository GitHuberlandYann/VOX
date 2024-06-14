#ifndef AMOBS_HPP
# define AMOBS_HPP

#include "Blocks.hpp"

typedef struct s_item t_item;

class AMob {
	public:
		AMob( void );
		virtual ~AMob( void );

		virtual void update( std::vector<t_shaderInput>& modArr, double deltaTime ) = 0;

	private:
		std::string _name;      // tag above head.
		short _air;             // how much air the entity has, in ticks. Decreases by 1 per tick when unable to breathe. Increase by 1 per tick when it can breathe. If -20 while still unable to breathe, the entity loses 1 health and its air is reset to 0.
		// short _fire;            // Number of ticks until the fire is put out. Negative values reflect how long the entity can stand in fire before burning. Default -20 when not on fire.
		short _deathTime;       // number of ticks the mob has been dead for. Controls death animations. 0 when alive.
		short _hurtTime;        // number of ticks the mob turns red for after being hit. 0 when not recently hit.
		// short _portalCooldown; // number of ticks before which the entity may be teleported back through a nether portal. Initially starts at 300 ticks (15 seconds) after teleportation and counts down to 0.
		float _fallDistance;    // distance entity has fallen.
		float _health;          // amount of health the entity has.
		// glm::vec3 _motion;   // velocity of entity in meters per tick
		glm::vec3 _position;    // position of entity.
		glm::vec2 _rotation;    // rotation of entity. yaw [-180:180] pitch [-90:90]
		glm::ivec3 _right, _front, _up;
		// std::array<t_item, 2> _holding; // main hand and off hand content.
		// std::vector<AMobs*> _passengers; // data of the entity(s) that is riding this entity.
		// bool _visibleName;      // if true, name is displayed even if cursor not on entities head.
		bool _invulnerable;     // true if the entity should not take damage.
		bool _touchGround;      // true if the entity is touching the ground.
		// bool _silent;        // if true, entity is silenced
		// bool _glowing;       // true if the entity has a glowing outline.
		// bool _hasVisualFire; // if true, the entity visually appears on fire.
		bool _noAI;             // setting to true disables the mob's AI.
		bool _persistenceRequired; // true if the mob must not despawn naturally.
		Chunk* _chunk;
};

#endif
