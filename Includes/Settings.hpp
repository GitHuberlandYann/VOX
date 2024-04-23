#ifndef SETTINGS_HPP
# define SETTINGS_HPP

# include <mutex>
# include <array>

# define RENDER_DISTANCE 10//20
# define FOV_START     	 70.0f // if fov = -fov, world is upside down

namespace SETTINGS {
	enum {
		SKYBOX = 0,
		NBR_BOOL,

		RENDER_DIST = 0,
		NBR_INT,

		FOV = 0,
		NBR_FLOAT
	};
};

class Settings
{
	private:
		static Settings *_settingsInstance;
		static std::mutex _mtx;

		std::array<bool, SETTINGS::NBR_BOOL> _bools;
		std::array<int, SETTINGS::NBR_INT> _ints;
		std::array<float, SETTINGS::NBR_FLOAT> _floats;

		Settings( void );
		~Settings( void );

	public:
		Settings( Settings &other ) = delete;
		void operator=( const Settings &other ) = delete;

		static Settings *Get( void );
		static void Destroy( void );

		bool getBool( int target );
		void setBool( int target, bool value );
		int getInt( int target );
		void setInt( int target, int value );
		float getFloat( int target );
		void setFloat( int target, float value );

// 		std::string saveString( void );
// 		void loadWorld( std::ofstream & ofs, std::string line );
};

#endif
