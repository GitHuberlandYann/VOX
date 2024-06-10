#ifndef SETTINGS_HPP
# define SETTINGS_HPP

# include <mutex>
# include <array>
# include <vector>
# include <string>

extern int WIN_WIDTH;
extern int WIN_HEIGHT;

namespace settings {
	namespace consts {
		const double tick          = 0.05f;

		const int    sea_level     = 64;
		const int    light_recurse = 32;
		const int    chunk_shift   = 4;
		const int    world_height  = 256;
		const int    world_shift   = 8;
		const int    alloc_size    = 1 << 16;  // represents CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT
		const int    chunk_size    = 16;

		const int    gui_min       = 1;
		inline int   gui_max()     { return (glm::max(1, glm::min(WIN_WIDTH / 320, WIN_HEIGHT / 240))); } // TODO check this

		namespace json {
			const std::string current_version = "v1.1";
			const int nbr_past_versions = 2;
			const std::array<std::string, nbr_past_versions> past_versions = {"v0.0", "v1.0"};
		};

		namespace gamemode {
			const int survival  = 0;
			const int creative  = 1;
			const int adventure = 2;
			const std::array<std::string, 3> str = {"SURVIVAL", "CREATIVE", "ADVENTURE"};
		};
	};

	namespace defaults {
		const int   render_distance = 10;
		const float fov             = 70.0f;  // if fov = -fov, world is upside down
		const float brightness      = 0.05f;  // [0:1]
	};

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

		json_version = ui_vertex_shader + 1,

		size_strings,
	};

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
