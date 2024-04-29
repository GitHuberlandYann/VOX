#ifndef SETTINGS_HPP
# define SETTINGS_HPP

# include <mutex>
# include <array>
# include <vector>
# include <string>

# define RENDER_DISTANCE  10
# define FOV_START     	  70.0f // if fov = -fov, world is upside down
# define BRIGHTNESS_START 0.05f

namespace SETTINGS {
	enum BOOL {
		SKYBOX,
		NBR_BOOL
	};

	enum INT {
		RENDER_DIST,
		CLOUDS,
		NBR_INT
	};

	enum FLOAT {
		FOV,
		BRIGHTNESS,
		NBR_FLOAT
	};

	enum STRING {
		BLOCK_ATLAS,
		ASCII_ATLAS,
		UI_ATLAS,
		CONTAINER_ATLAS,
		PARTICLE_ATLAS,
		MODEL_ATLAS,
		WATER_STILL,
		WATER_FLOW,
		NBR_TEXTURES,
		NBR_STRING = NBR_TEXTURES
	};

	enum {
		FANCY,
		FAST,
		OFF,
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
		std::array<std::string, SETTINGS::NBR_STRING> _strings;
		std::vector<std::string> _packs;

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

		std::string getString( int target );
		void setString( int target, std::string value );

		void pushResourcePack( std::string pack );
		std::string getPacksString( void );
		void loadResourcePacks( void );
};

#endif
