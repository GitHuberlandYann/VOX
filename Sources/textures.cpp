#include "OpenGL_Manager.hpp"

void Text::loadTexture( void )
{
	if (_texture) {
		glDeleteTextures(1, &_texture);
	}
	glGenTextures(1, &_texture);

	_shader.useProgram();
	loadTextureShader(1, _texture, Settings::Get()->getString(settings::strings::ascii_atlas));
	glUniform1i(glGetUniformLocation(_shader.getProgram(), "asciiAtlas"), 1); // sampler2D #index in fragment shader
}

void UI::loadTextures( void )
{
	_text->loadTexture();

	_itemShader.useProgram();
	glUniform1i(glGetUniformLocation(_itemShader.getProgram(), "blockAtlas"), 0); // we reuse texture from main shader

	if (_texture) {
		glDeleteTextures(1, &_texture);
	}
	glGenTextures(1, &_texture);

	_shader.useProgram();
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _texture);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 256, 256, settings::consts::tex::size);
	loadSubTextureArray(256, 256, settings::consts::tex::ui, Settings::Get()->getString(settings::strings::ui_atlas));
	loadSubTextureArray(256, 256, settings::consts::tex::inventory, Settings::Get()->getString(settings::strings::tex_inventory));
	loadSubTextureArray(256, 256, settings::consts::tex::crafting_table, Settings::Get()->getString(settings::strings::tex_crafting_table));
	loadSubTextureArray(256, 256, settings::consts::tex::furnace, Settings::Get()->getString(settings::strings::tex_furnace));
	loadSubTextureArray(256, 256, settings::consts::tex::chest, Settings::Get()->getString(settings::strings::tex_chest));
	loadSubTextureArray(256, 256, settings::consts::tex::book, Settings::Get()->getString(settings::strings::tex_book));
	glUniform1i(glGetUniformLocation(_shader.getProgram(), "textures"), 2);
	check_glstate("Successfully loaded img[2] texture array 2D", true);
}

void OpenGL_Manager::loadTextures( void )
{
	_ui->loadTextures();

	if (_textures[0]) {
		glDeleteTextures(_textures.size(), &_textures[0]);
		_textures[0] = 0;
	}
	glGenTextures(_textures.size(), &_textures[0]);

	_shader.useProgram();
	std::string root = Settings::Get()->getString(settings::strings::block_atlas) + "Blocks/";
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _textures[0]);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 16, 16, settings::consts::shader::blocks_size);
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex0, root + "tex0.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex1, root + "tex1.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex2, root + "tex2.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex3, root + "tex3.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex4, root + "tex4.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex5, root + "tex5.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex6, root + "tex6.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex7, root + "tex7.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex8, root + "tex8.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex9, root + "tex9.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex10, root + "tex10.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex11, root + "tex11.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex12, root + "tex12.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex13, root + "tex13.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex14, root + "tex14.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tex15, root + "tex15.png");

	loadSubTextureArray(16, 16, settings::consts::shader::block::allium, root + "allium.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::arrow, root + "arrow.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::bedrock, root + "bedrock.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::birch_planks, root + "birch_planks.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::black_wool, root + "black_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::blue_orchid, root + "blue_orchid.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::blue_wool, root + "blue_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::brown_wool, root + "brown_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::cactus_bottom, root + "cactus_bottom.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::cactus_side, root + "cactus_side.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::cactus_top, root + "cactus_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::chest_front, root + "chest_front.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::chest_inner_bottom, root + "chest_inner_bottom.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::chest_inner_top, root + "chest_inner_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::chest_side, root + "chest_side.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::chest_top, root + "chest_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::chiseled_quartz_block, root + "chiseled_quartz_block.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::chiseled_quartz_block_top, root + "chiseled_quartz_block_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::coal_block, root + "coal_block.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::coal_ore, root + "coal_ore.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::cobblestone, root + "cobblestone.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::comparator, root + "comparator.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::comparator_on, root + "comparator_on.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::cornflower, root + "cornflower.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::cracked_stone_bricks, root + "cracked_stone_bricks.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::crafting_table_front, root + "crafting_table_front.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::crafting_table_side, root + "crafting_table_side.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::crafting_table_top, root + "crafting_table_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::cyan_wool, root + "cyan_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::dandelion, root + "dandelion.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::dead_bush, root + "dead_bush.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::debug, root + "debug.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage, root + "destroy_stage.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_0, root + "destroy_stage_0.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_1, root + "destroy_stage_1.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_2, root + "destroy_stage_2.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_3, root + "destroy_stage_3.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_4, root + "destroy_stage_4.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_5, root + "destroy_stage_5.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_6, root + "destroy_stage_6.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_7, root + "destroy_stage_7.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_8, root + "destroy_stage_8.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_9, root + "destroy_stage_9.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::diamond_block, root + "diamond_block.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::diamond_ore, root + "diamond_ore.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::dirt, root + "dirt.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::dirt_path_side, root + "dirt_path_side.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::dirt_path_top, root + "dirt_path_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::dust_dot, root + "dust_dot.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::dust_line, root + "dust_line.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::farmland, root + "farmland.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::farmland_moist, root + "farmland_moist.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::furnace_front, root + "furnace_front.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::furnace_front_on, root + "furnace_front_on.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::furnace_side, root + "furnace_side.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::furnace_top, root + "furnace_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::glass, root + "glass.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::glass_pane_top, root + "glass_pane_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::glowstone, root + "glowstone.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::grass, root + "grass.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::grass_block_side, root + "grass_block_side.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::grass_block_top, root + "grass_block_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::gravel, root + "gravel.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::gray_wool, root + "gray_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::green_wool, root + "green_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::iron_block, root + "iron_block.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::iron_ore, root + "iron_ore.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::item_frame, root + "item_frame.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::lever, root + "lever.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::light_blue_wool, root + "light_blue_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::light_gray_wool, root + "light_gray_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::lime_wool, root + "lime_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::magenta_wool, root + "magenta_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::netherrack, root + "netherrack.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::oak_door_bottom, root + "oak_door_bottom.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::oak_door_top, root + "oak_door_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::oak_leaves, root + "oak_leaves.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::oak_log, root + "oak_log.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::oak_log_spin, root + "oak_log_spin.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::oak_log_top, root + "oak_log_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::oak_planks, root + "oak_planks.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::oak_sapling, root + "oak_sapling.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::oak_sign, root + "oak_sign.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::oak_trapdoor, root + "oak_trapdoor.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::observer_back, root + "observer_back.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::observer_back_on, root + "observer_back_on.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::observer_front, root + "observer_front.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::observer_side, root + "observer_side.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::observer_top, root + "observer_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::obsidian, root + "obsidian.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::orange_wool, root + "orange_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::pink_tulip, root + "pink_tulip.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::pink_wool, root + "pink_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::piston_bottom, root + "piston_bottom.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::piston_inner, root + "piston_inner.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::piston_side, root + "piston_side.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::piston_top, root + "piston_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::piston_top_sticky, root + "piston_top_sticky.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::poppy, root + "poppy.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::purple_wool, root + "purple_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::quartz_block, root + "quartz_block.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::quartz_bricks, root + "quartz_bricks.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::quartz_pillar, root + "quartz_pillar.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::quartz_ore, root + "quartz_ore.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::red_wool, root + "red_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::redstone_block, root + "redstone_block.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::redstone_lamp, root + "redstone_lamp.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::redstone_lamp_on, root + "redstone_lamp_on.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::redstone_ore, root + "redstone_ore.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::redstone_torch, root + "redstone_torch.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::redstone_torch_off, root + "redstone_torch_off.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::repeater, root + "repeater.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::repeater_on, root + "repeater_on.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::sand, root + "sand.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::sandstone, root + "sandstone.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::sandstone_bottom, root + "sandstone_bottom.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::sandstone_top, root + "sandstone_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::smooth_stone, root + "smooth_stone.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::smooth_quartz_block, root + "smooth_quartz_block.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::stone, root + "stone.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::stone_bricks, root + "stone_bricks.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::sugar_cane, root + "sugar_cane.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::target_side, root + "target_side.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::target_top, root + "target_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tnt_bottom, root + "tnt_bottom.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tnt_side, root + "tnt_side.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::tnt_top, root + "tnt_top.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::torch, root + "torch.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage0, root + "wheat_stage0.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage1, root + "wheat_stage1.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage2, root + "wheat_stage2.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage3, root + "wheat_stage3.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage4, root + "wheat_stage4.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage5, root + "wheat_stage5.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage6, root + "wheat_stage6.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage7, root + "wheat_stage7.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::white_tulip, root + "white_tulip.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::white_wool, root + "white_wool.png");
	loadSubTextureArray(16, 16, settings::consts::shader::block::yellow_wool, root + "yellow_wool.png");

	root = Settings::Get()->getString(settings::strings::block_atlas) + "Items/";

	loadSubTextureArray(16, 16, settings::consts::shader::item::apple, root + "apple.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::arrow, root + "arrow.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::book, root + "book.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::book_and_quill, root + "book_and_quill.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::bow, root + "bow.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::bow_pulling_0, root + "bow_pulling_0.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::bow_pulling_1, root + "bow_pulling_1.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::bow_pulling_2, root + "bow_pulling_2.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::bread, root + "bread.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::bucket, root + "bucket.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::charcoal, root + "charcoal.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::coal, root + "coal.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::comparator, root + "comparator.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::diamond, root + "diamond.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::diamond_axe, root + "diamond_axe.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::diamond_hoe, root + "diamond_hoe.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::diamond_pickaxe, root + "diamond_pickaxe.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::diamond_shovel, root + "diamond_shovel.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::feather, root + "feather.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::flint, root + "flint.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::flint_and_steel, root + "flint_and_steel.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::glowstone_dust, root + "glowstone_dust.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::ink_sac, root + "ink_sac.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::iron_axe, root + "iron_axe.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::iron_hoe, root + "iron_hoe.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::iron_ingot, root + "iron_ingot.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::iron_pickaxe, root + "iron_pickaxe.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::iron_shovel, root + "iron_shovel.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::item_frame, root + "item_frame.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::leather, root + "leather.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::oak_door, root + "oak_door.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::oak_sign, root + "oak_sign.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::paper, root + "paper.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::quartz, root + "quartz.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::redstone, root + "redstone.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::repeater, root + "repeater.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::skeleton_egg, root + "skeleton_egg.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::stick, root + "stick.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::stone_axe, root + "stone_axe.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::stone_hoe, root + "stone_hoe.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::stone_pickaxe, root + "stone_pickaxe.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::stone_shovel, root + "stone_shovel.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::string, root + "string.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::sugar_cane, root + "sugar_cane.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::water_bucket, root + "water_bucket.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::wheat, root + "wheat.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::wheat_seeds, root + "wheat_seeds.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::wooden_axe, root + "wooden_axe.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::wooden_hoe, root + "wooden_hoe.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::wooden_pickaxe, root + "wooden_pickaxe.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::wooden_shovel, root + "wooden_shovel.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::written_book, root + "written_book.png");
	loadSubTextureArray(16, 16, settings::consts::shader::item::zombie_egg, root + "zombie_egg.png");
	glUniform1i(glGetUniformLocation(_shader.getProgram(), "textures"), 0);
	check_glstate("Successfully loaded img[0] texture array 2D", true);

	_skyShader.useProgram();
	loadTextureShader(7, _textures[5], "Resources/Textures/Blocks/glass.png");
	glUniform1i(glGetUniformLocation(_skyShader.getProgram(), "glass"), 7);

	loadTextureShader(4, _textures[1], Settings::Get()->getString(settings::strings::water_still));
	glUniform1i(glGetUniformLocation(_skyShader.getProgram(), "waterStill"), 4);

	loadTextureShader(5, _textures[2], Settings::Get()->getString(settings::strings::water_flow));
	glUniform1i(glGetUniformLocation(_skyShader.getProgram(), "waterFlow"), 5);

	_particleShader.useProgram();
	loadTextureShader(6, _textures[3], Settings::Get()->getString(settings::strings::particle_atlas));
	glUniform1i(glGetUniformLocation(_particleShader.getProgram(), "particleAtlas"), 6);

	_modelShader.useProgram();
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _textures[4]);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 64, 64, 3);
	loadSubTextureArray(64, 64, settings::consts::shader::texture::zombie, Settings::Get()->getString(settings::strings::tex_zombie));
	loadSubTextureArray(64, 64, settings::consts::shader::texture::skeleton, Settings::Get()->getString(settings::strings::tex_skeleton));
	loadSubTextureArray(64, 64, settings::consts::shader::texture::player, Settings::Get()->getString(settings::strings::tex_player));
	glUniform1i(glGetUniformLocation(_modelShader.getProgram(), "textures"), 3);
	check_glstate("Successfully loaded img[3] texture array 2D", true);
}
