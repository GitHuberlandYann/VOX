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
		FULLSCREEN,
		SKYBOX,
		PARTICLES,
		FACE_CULLING,
		VISIBLE_CHUNK_BORDER,
		SMOOTH_LIGHTING,
		FLAT_WORLD,
		NBR_BOOL
	};

	enum INT {
		RENDER_DIST,
		CLOUDS,
		MONITOR_WIDTH,
		MONITOR_HEIGHT,
		AVAILABLE_RES,
		FLAT_WORLD_BLOCK,
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

		MAIN_FRAGMENT_SHADER = NBR_TEXTURES,
		MAIN_VERTEX_SHADER,
		ITEM_FRAGMENT_SHADER,
		ITEM_VERTEX_SHADER,
		PARTICLE_FRAGMENT_SHADER,
		PARTICLE_VERTEX_SHADER,
		SKY_FRAGMENT_SHADER,
		SKY_VERTEX_SHADER,
		SKYBOX_FRAGMENT_SHADER,
		SKYBOX_VERTEX_SHADER,
		TEXT_FRAGMENT_SHADER,
		TEXT_VERTEX_SHADER,
		UI_FRAGMENT_SHADER,
		UI_VERTEX_SHADER,
		NBR_SHADERS = UI_VERTEX_SHADER + 1 - NBR_TEXTURES,

		VERSION = UI_VERTEX_SHADER + 1,

		NBR_STRING,
	};
	const std::string CURRENT_VERSION = "v1.0";

	enum {
		FANCY,
		FAST,
		OFF,
	};
	const std::array<int, 5> fw_types = {blocks::sand, blocks::grass_block, blocks::water, blocks::stone, blocks::oak_log};
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
		void setResourcePacks( std::vector<std::string> &packs );
		std::vector<std::string> &getResourcePacks( void );
		bool loadResourcePacks( void );
};

#endif
