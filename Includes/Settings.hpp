#ifndef SETTINGS_HPP
# define SETTINGS_HPP

# include <mutex>

# define RENDER_DISTANCE 10//20
# define FOV_START     	 70.0f // if fov = -fov, world is upside down

enum SETTINGS {
	RENDER_DIST,
	FOV,
};

class Settings
{
	private:
		static Settings *_settingsInstance;
		static std::mutex _mtx;

		int _render_distance;
		float _fov;

		Settings( void );
		~Settings( void );

	public:
		Settings( Settings &other ) = delete;
		void operator=( const Settings &other ) = delete;

		static Settings *Get( void );
		static void Destroy( void );

		int getInt( int target );
		float getFloat( int target );
		void setInt( int target, int value );
		void setFloat( int target, float value );

// 		std::string saveString( void );
// 		void loadWorld( std::ofstream & ofs, std::string line );
};

#endif
