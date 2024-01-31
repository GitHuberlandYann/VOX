#ifndef DAYCYCLE_HPP
# define DAYCYCLE_HPP

# include <mutex>

# define MINECRAFT_MINUTE 16.6

/*enum dayCycle_state {
	DAYTIME, // start 06h, mid 12h, end 18h
	SUNSET, // end 19h
	NIGHTTIME, // mid 00h, end 05h
	SUNRISE // end 06h
};*/

class DayCycle
{
	private:
		static DayCycle *_dayCycleInstance;
		static std::mutex _mtx;

		double _gameTime;
		int _ticks, _day, _hour, _minute, _internal_light, _time_multiplier;
		bool _forceReset;
		GLint _uniInternalLight;

		DayCycle( void );
		~DayCycle( void );

		void setInternals( void );

	public:
		DayCycle( DayCycle &other ) = delete;
		void operator=( const DayCycle &other ) = delete;

		static DayCycle *Get( void );
		static void Reset( void );
		void setUniInternalLight( GLint internal_light_location );
		void setCloudsColor( GLint uniform_location );
		void tickUpdate( void );
		void setTicks( int ticks );
		void addTicks( int ticks );
		void updateTimeMultiplier( GLint mul );
		std::string getInfos( void );
		std::string getTime( void );
		std::string saveString( void );
		void loadWorld( std::ofstream & ofs, std::string line );
};


#endif
