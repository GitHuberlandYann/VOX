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

		namespace tex {
			const int blocks = 0;
			const int ui = 1;
			const int inventory = 2;
			const int crafting_table = 3;
			const int furnace = 4;
			const int chest = 5;
		};

		namespace depth {
			const int chat_back = 210;
			const int debug = 140;
			const int chat = 200;
			const int inv_back = 190;
			const int dura_back = 178;
			const int dura = 176;
			const int inv_str = 170;
			const int inv_select = 160;
			
			namespace menu {
				const int occlusion = 150;
				const int occlusion_bis = 140;
				const int bars = 130;
				const int slider = 125;
				const int str = 120;
				const int container = 130;
				const int selection = 125;
				const int dura_back = 118;
				const int dura = 116;
				const int item_str = 110;
				const int held_str = 50;

				namespace controls {
					const int occlusion = 100;
					const int bars = 90;
					const int str = 80;
				};
			};
			namespace item {
				const int inv = 3; // * 60 = 180
				const int menu = 2; // * 60 = 120
				const int held = 1; // * 60 = 60
			};
		};

		namespace shader {
			namespace uniform {
				const size_t fog = 0;
				const size_t view = 1;
				const size_t proj = 2;
				const size_t internal_light = 3;
				const size_t brightness = 4;
				const size_t color = 5;
				const size_t animation = 6;
			};
			namespace texture {
				const int zombie = 0;
			};
		};

		namespace math {
			const glm::vec3 world_up = {0, 0, 1};
			const float standard_gravity = -9.81f;
		};

		namespace speed {
			const float initial_jump = 9.317f;
			const float initial_fall = -6.605f;
			const float zombie = 2.28378822f;
		};

		namespace hitbox { // https://minecraft.fandom.com/wiki/Hitbox#List_of_entity_hitboxes
			const float player = 1.8f;
			const float zombie = 1.95f;
		};

		namespace eyeLevel { // meaning eye level = hitbox / 32 * 28.8
			const float player = 0.62f;
			const float zombie = 0.755f;
		}
	};

	namespace state_machine {
		const short idle = 0;
		const short wandle = 1;
		const short chase = 2;
	}

	namespace defaults {
		const int   render_distance = 10;
		const float fov             = 70.0f;  // if fov = -fov, world is upside down
		const float brightness      = 0.05f;  // [0:1]
	};

	namespace bools {
		enum {
			fullscreen,
			skybox,
			particles,
			face_culling,
			visible_chunk_border,
			smooth_lighting,
			flat_world,
			mobAI,
			size
		};
	};

	namespace ints {
		enum {
			render_dist,
			clouds,
			monitor_width,
			monitor_height,
			available_res,
			flat_world_block,
			size
		};
	};

	namespace floats {
		enum {
			fov,
			brightness,
			size
		};
	};

	namespace strings {
		enum {
			block_atlas,
			ascii_atlas,
			ui_atlas,
			tex_inventory,
			tex_crafting_table,
			tex_furnace,
			tex_chest,
			tex_zombie,
			particle_atlas,
			model_atlas,
			water_still,
			water_flow,
			size_textures,

			main_fragment_shader = size_textures,
			main_vertex_shader,
			item_fragment_shader,
			item_vertex_shader,
			model_fragment_shader,
			model_vertex_shader,
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

			size,
		};
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

		std::array<bool, settings::bools::size> _bools;
		std::array<int, settings::ints::size> _ints;
		std::array<float, settings::floats::size> _floats;
		std::array<std::string, settings::strings::size> _strings;
		std::vector<std::string> _packs;

		Settings( void );
		~Settings( void );
};

#endif
