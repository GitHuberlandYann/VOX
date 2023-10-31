#ifndef DAYCYCLE_HPP
# define DAYCYCLE_HPP

# define MINECRAFT_MINUTE 0.8333333333333334

enum dayCycle_state {
	DAYTIME, // start 06h, mid 12h, end 18h
	SUNSET, // end 19h
	NIGHTTIME, // mid 00h, end 05h
	SUNRISE // end 06h
};

class DayCycle
{
	private:
		static DayCycle *_dayCycleInstance;
		static std::mutex _mtx;

		double _gameTime;
		int _day, _hour, _minute, _internal_light, _time_multiplier;
		GLint _uniInternalLight;
		dayCycle_state _state;

		DayCycle( void );
		~DayCycle( void );

		void updateInternalLight( void );

	public:
		DayCycle( DayCycle &other ) = delete;
		void operator=( const DayCycle &other ) = delete;

		static DayCycle *Get( void );
		void setUniInternalLight( GLint internal_light_location );
		void setCloudsColor( GLint uniform_location );
		void update( double deltaTime );
		void updateTimeMultiplier( GLint mul );
		std::string getInfos( void );
		std::string saveString( void );
		void loadWorld( std::ofstream & ofs, std::string line );
};


#endif
