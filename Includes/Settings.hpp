#ifndef SETTINGS_HPP
# define SETTINGS_HPP

# include <mutex>
# include <array>
# include <vector>
# include <string>

# define RENDER_DISTANCE  10
# define FOV_START     	  70.0f // if fov = -fov, world is upside down
# define BRIGHTNESS_START 0.05f


namespace settings {
	enum bools {
		fullscreen,
		skybox,
		particles,
		face_culling,
		visible_chunk_border,
		smooth_lighting,
		flat_world,
		size_bools
	};

	enum ints {
		render_dist,
		clouds,
		monitor_width,
		monitor_height,
		available_res,
		flat_world_block,
		size_ints
	};

	enum floats {
		fov,
		brightness,
		size_floats
	};

	enum strings {
		block_atlas,
		ascii_atlas,
		ui_atlas,
		container_atlas,
		particle_atlas,
		model_atlas,
		water_still,
		water_flow,
		size_textures,

		main_fragment_shader = size_textures,
		main_vertex_shader,
		item_fragment_shader,
		item_vertex_shader,
		particle_fragment_shader,
		particle_vertex_shader,
		sky_fragment_shader,
		sky_vertex_shader,
		skybox_fragment_shader,
		skybox_vertex_shader,
		text_fragment_shader,
		text_vertex_shader,
		ui_fragment_shader,
		ui_vertex_shader,
		size_shaders = ui_vertex_shader + 1 - size_textures,

		version = ui_vertex_shader + 1,

		size_strings,
	};
	const std::string current_version = "v1.0";

	enum {
		FANCY,
		FAST,
		OFF,
	};
	const std::array<int, 5> fw_types = {blocks::sand, blocks::grass_block, blocks::water, blocks::stone, blocks::oak_log};
};

class Settings
{
	public:
		Settings( Settings& other ) = delete;
		void operator=( const Settings& other ) = delete;

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
		std::vector<std::string>& getResourcePacks( void );
		bool loadResourcePacks( void );

	private:
		static Settings *_settingsInstance;
		static std::mutex _mtx;

		std::array<bool, settings::bools::size_bools> _bools;
		std::array<int, settings::ints::size_ints> _ints;
		std::array<float, settings::floats::size_floats> _floats;
		std::array<std::string, settings::strings::size_strings> _strings;
		std::vector<std::string> _packs;

		Settings( void );
		~Settings( void );
};

#endif
