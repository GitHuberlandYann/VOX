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
		inline int   gui_max()     { return (glm::max(1, glm::min(WIN_WIDTH / 320, WIN_HEIGHT / 240))); }

		namespace json {
			const int nbr_versions = 7;
			const std::array<std::string, nbr_versions> versions = {"v0.0", "v1.0", "v1.1", "v1.2", "v1.3", "v1.4", "v1.5"};
		};

		namespace gamemode {
			const int survival  = 0;
			const int creative  = 1;
			const int adventure = 2;
			const std::array<std::string, 3> str = {"SURVIVAL", "CREATIVE", "ADVENTURE"};
		};

		namespace tex {
			const int ui = 0;
			const int inventory = 1;
			const int crafting_table = 2;
			const int furnace = 3;
			const int chest = 4;
			const int book = 5;
			const int anvil = 6;
			const int size = 7;
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
				const int item_info_back = 100;
				const int item_info_str = 90;
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
			const int outColor = 0; // location of outColor fragData in fragment shaders

			namespace uniform {
				const size_t fog = 0;
				const size_t view = 1;
				const size_t proj = 2;
				const size_t internal_light = 3;
				const size_t brightness = 4;
				const size_t color = 5;
				const size_t animation = 6;
				const size_t camPos = 7;
				const size_t ticks = 8;
			};

			namespace texture {
				const int zombie = 0;
				const int skeleton = 1;
				const int player = 2;
			};

			namespace attributes {
				const int specifications = 0;
				const int texture = 1;
				const int light = 2;
				const int color = 3;
				const int position = 4;
			};
		};

		namespace math {
			const glm::vec3 world_up = {0.0f, 0.0f, 1.0f};
			const float standard_gravity = -9.81f;
		};
	};

	namespace defaults {
		const int   render_distance = 10;
		const int   game_mode       = settings::consts::gamemode::survival;
		const float fov             = 70.0f;  // if fov = -fov, world is upside down
		const float brightness      = 0.05f;  // [0:1]
		const float yaw             = 90.0f;
		const float pitch        	= 0.0f;
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
			hide_ui,
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
			json_version,
			game_mode,
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
			tex_book,
			tex_anvil,
			tex_zombie,
			tex_skeleton,
			tex_player,
			particle_atlas,
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

			size = ui_vertex_shader + 1,
		};
	};

	enum {
		FANCY,
		FAST,
		OFF,
	};
	const std::array<int, 5> fw_types = {blocks::sand_stone, blocks::grass_block, blocks::water, blocks::stone, blocks::oak_log};
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
