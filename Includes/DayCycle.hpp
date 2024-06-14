#ifndef DAYCYCLE_HPP
# define DAYCYCLE_HPP

# include <mutex>
# include "Shader.hpp"

# define MINECRAFT_MINUTE 16.6

/*enum dayCycle_state {
	DAYTIME, // start 06h, mid 12h, end 18h
	SUNSET, // end 19h
	NIGHTTIME, // mid 00h, end 05h
	SUNRISE // end 06h
};*/

class Chat;

class DayCycle
{
	private:
		static DayCycle *_dayCycleInstance;
		static std::mutex _mtx;

		double _gameTime;
		int _ticks, _gameTicks, _day, _hour, _minute, _internal_light, _time_multiplier, _game_time_multiplier;
		bool _forceReset;
		Shader* _shader, *_particleShader, *_modelShader;

		DayCycle( void );
		~DayCycle( void );

		void setInternals( void );

	public:
		DayCycle( DayCycle &other ) = delete;
		void operator=( const DayCycle &other ) = delete;

		static DayCycle *Get( void );
		static void Destroy( void );

		void setShaderPtrs( Shader* shader, Shader* particleShader, Shader* modelShader );
		void setCloudsColor( GLint uniform_location );
		bool tickUpdate( void );
		int getTicks( void );
		int getGameTicks( void );
		int getGameTimeMultiplier( void );
		void setTicks( int ticks );
		void addTicks( int ticks );

		void updateTimeMultiplier( GLint mul );
		void freeze( Chat *chat );
		void step( Chat *chat, int steps );

		std::string getInfos( void );
		std::string getTime( void );
		std::string saveString( void );
		void loadWorld( std::ofstream & ofs, std::string line );
};


#endif
