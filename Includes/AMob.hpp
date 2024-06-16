#ifndef AMOBS_HPP
# define AMOBS_HPP

#include "Blocks.hpp"

typedef struct s_item t_item;

class AMob {
	public:
		AMob( glm::vec3 position );
		virtual ~AMob( void );

		virtual bool update( std::vector<t_shaderInput>& modArr, float deltaTime ) = 0;
		glm::vec3 getPos( void );
		virtual glm::vec3 getEyePos( void ) = 0;
		void applyGravity( void );
		void touchCeiling( float value );
		void touchGround( float value );
		void setTouchGround( bool state );
		virtual bool getHitBox( void ) = 0;
		void receiveDamage( const float damage, const glm::vec3 source);

	protected:
		std::string _name;      // tag above head.
		short _air;             // how much air the entity has, in ticks. Decreases by 1 per tick when unable to breathe. Increase by 1 per tick when it can breathe. If -20 while still unable to breathe, the entity loses 1 health and its air is reset to 0.
		// short _fire;            // Number of ticks until the fire is put out. Negative values reflect how long the entity can stand in fire before burning. Default -20 when not on fire.
		float _deathTime;       // number of seconds the mob has been dead for. Controls death animations. 0 when alive.
		float _hurtTime;        // number of seconds the mob turns red for after being hit. >= 0 when not recently hit.
		// short _portalCooldown; // number of ticks before which the entity may be teleported back through a nether portal. Initially starts at 300 ticks (15 seconds) after teleportation and counts down to 0.
		float _fallDistance;    // distance entity has fallen.
		float _fallTime;        // seconds entity has been falling.
		float _z0;              // z coord of entity when it started falling.
		float _walkTime;        // used to animate arms and legs
		float _health;          // amount of health the entity has.
		float _deltaTime;       // used to compute movement
		// glm::vec3 _motion;   // velocity of entity in meters per tick
		glm::vec3 _position;    // position of entity.
		glm::vec2 _rotation;    // rotation of entity. yaw [-180:180] pitch [-90:90]
		glm::vec3 _front, _right, _up;
		glm::vec2 _bodyFront;
		// std::array<t_item, 2> _holding; // main hand and off hand content.
		// std::vector<AMobs*> _passengers; // data of the entity(s) that is riding this entity.
		// bool _visibleName;      // if true, name is displayed even if cursor not on entities head.
		bool _invulnerable;     // true if the entity should not take damage.
		bool _touchGround;      // true if the entity is touching the ground.
		bool _walking;          // if entity is walking
		bool _inJump;
		// bool _silent;        // if true, entity is silenced
		// bool _glowing;       // true if the entity has a glowing outline.
		// bool _hasVisualFire; // if true, the entity visually appears on fire.
		bool _noAI;             // setting to true disables the mob's AI.
		bool _persistenceRequired; // true if the mob must not despawn naturally.
		Chunk* _chunk;

		void addQuads( std::vector<t_shaderInput>& arr, std::array<glm::vec3, 4> pts, int spec, int dx, int dy );

		void move( int direction, bool move );
		void restorePos( glm::vec3 position );
		bool customObstacle( float minZ, float maxZ );
};

#endif
