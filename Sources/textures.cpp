#include "OpenGL_Manager.hpp"

void Text::loadTexture( void )
{
	if (_texture) {
		glDeleteTextures(1, &_texture);
	}
	glGenTextures(1, &_texture);

	_shader.useProgram();
	utils::shader::loadTextureShader(1, _texture, Settings::Get()->getString(settings::strings::ascii_atlas));
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
	utils::shader::loadSubTextureArray(256, 256, settings::consts::tex::ui, Settings::Get()->getString(settings::strings::ui_atlas));
	utils::shader::loadSubTextureArray(256, 256, settings::consts::tex::inventory, Settings::Get()->getString(settings::strings::tex_inventory));
	utils::shader::loadSubTextureArray(256, 256, settings::consts::tex::crafting_table, Settings::Get()->getString(settings::strings::tex_crafting_table));
	utils::shader::loadSubTextureArray(256, 256, settings::consts::tex::furnace, Settings::Get()->getString(settings::strings::tex_furnace));
	utils::shader::loadSubTextureArray(256, 256, settings::consts::tex::chest, Settings::Get()->getString(settings::strings::tex_chest));
	utils::shader::loadSubTextureArray(256, 256, settings::consts::tex::book, Settings::Get()->getString(settings::strings::tex_book));
	utils::shader::loadSubTextureArray(256, 256, settings::consts::tex::anvil, Settings::Get()->getString(settings::strings::tex_anvil));
	glUniform1i(glGetUniformLocation(_shader.getProgram(), "textures"), 2);
	utils::shader::check_glstate("Successfully loaded img[2] texture array 2D", true);
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
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex0, root + "tex0.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex1, root + "tex1.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex2, root + "tex2.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex3, root + "tex3.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex4, root + "tex4.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex5, root + "tex5.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex6, root + "tex6.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex7, root + "tex7.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex8, root + "tex8.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex9, root + "tex9.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex10, root + "tex10.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex11, root + "tex11.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex12, root + "tex12.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex13, root + "tex13.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex14, root + "tex14.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tex15, root + "tex15.png");

	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::allium, root + "allium.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::anvil_top, root + "anvil_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::anvil, root + "anvil.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::arrow, root + "arrow.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::bedrock, root + "bedrock.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::birch_planks, root + "birch_planks.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::black_wool, root + "black_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::blue_orchid, root + "blue_orchid.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::blue_wool, root + "blue_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::bookshelf, root + "bookshelf.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::brown_wool, root + "brown_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::cactus_bottom, root + "cactus_bottom.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::cactus_side, root + "cactus_side.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::cactus_top, root + "cactus_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::chest_front, root + "chest_front.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::chest_inner_bottom, root + "chest_inner_bottom.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::chest_inner_top, root + "chest_inner_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::chest_side, root + "chest_side.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::chest_top, root + "chest_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::chiseled_quartz_block, root + "chiseled_quartz_block.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::chiseled_quartz_block_top, root + "chiseled_quartz_block_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::coal_block, root + "coal_block.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::coal_ore, root + "coal_ore.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::cobblestone, root + "cobblestone.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::comparator, root + "comparator.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::comparator_on, root + "comparator_on.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::cornflower, root + "cornflower.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::cracked_stone_bricks, root + "cracked_stone_bricks.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::crafting_table_front, root + "crafting_table_front.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::crafting_table_side, root + "crafting_table_side.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::crafting_table_top, root + "crafting_table_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::cyan_wool, root + "cyan_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::dandelion, root + "dandelion.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::dead_bush, root + "dead_bush.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::debug, root + "debug.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage, root + "destroy_stage.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_0, root + "destroy_stage_0.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_1, root + "destroy_stage_1.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_2, root + "destroy_stage_2.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_3, root + "destroy_stage_3.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_4, root + "destroy_stage_4.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_5, root + "destroy_stage_5.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_6, root + "destroy_stage_6.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_7, root + "destroy_stage_7.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_8, root + "destroy_stage_8.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::destroy_stage_9, root + "destroy_stage_9.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::diamond_block, root + "diamond_block.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::diamond_ore, root + "diamond_ore.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::dirt, root + "dirt.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::dirt_path_side, root + "dirt_path_side.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::dirt_path_top, root + "dirt_path_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::dust_dot, root + "dust_dot.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::dust_line, root + "dust_line.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::enchanted_book, root + "enchanted_book.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::farmland, root + "farmland.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::farmland_moist, root + "farmland_moist.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::furnace_front, root + "furnace_front.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::furnace_front_on, root + "furnace_front_on.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::furnace_side, root + "furnace_side.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::furnace_top, root + "furnace_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::glass, root + "glass.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::glass_pane_top, root + "glass_pane_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::glowstone, root + "glowstone.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::grass, root + "grass.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::grass_block_side, root + "grass_block_side.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::grass_block_top, root + "grass_block_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::gravel, root + "gravel.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::gray_wool, root + "gray_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::green_wool, root + "green_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::iron_block, root + "iron_block.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::iron_ore, root + "iron_ore.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::item_frame, root + "item_frame.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::lectern_base, root + "lectern_base.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::lectern_front, root + "lectern_front.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::lectern_sides, root + "lectern_sides.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::lectern_top, root + "lectern_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::lever, root + "lever.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::light_blue_wool, root + "light_blue_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::light_gray_wool, root + "light_gray_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::lime_wool, root + "lime_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::magenta_wool, root + "magenta_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::netherrack, root + "netherrack.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::oak_door_bottom, root + "oak_door_bottom.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::oak_door_top, root + "oak_door_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::oak_leaves, root + "oak_leaves.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::oak_log, root + "oak_log.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::oak_log_spin, root + "oak_log_spin.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::oak_log_top, root + "oak_log_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::oak_planks, root + "oak_planks.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::oak_sapling, root + "oak_sapling.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::oak_sign, root + "oak_sign.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::oak_trapdoor, root + "oak_trapdoor.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::observer_back, root + "observer_back.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::observer_back_on, root + "observer_back_on.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::observer_front, root + "observer_front.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::observer_side, root + "observer_side.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::observer_top, root + "observer_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::obsidian, root + "obsidian.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::orange_wool, root + "orange_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::pink_tulip, root + "pink_tulip.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::pink_wool, root + "pink_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::piston_bottom, root + "piston_bottom.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::piston_inner, root + "piston_inner.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::piston_side, root + "piston_side.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::piston_top, root + "piston_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::piston_top_sticky, root + "piston_top_sticky.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::poppy, root + "poppy.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::purple_wool, root + "purple_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::quartz_block, root + "quartz_block.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::quartz_bricks, root + "quartz_bricks.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::quartz_pillar, root + "quartz_pillar.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::quartz_pillar_spin, root + "quartz_pillar_spin.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::quartz_pillar_top, root + "quartz_pillar_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::quartz_ore, root + "quartz_ore.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::red_wool, root + "red_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::redstone_block, root + "redstone_block.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::redstone_lamp, root + "redstone_lamp.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::redstone_lamp_on, root + "redstone_lamp_on.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::redstone_ore, root + "redstone_ore.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::redstone_torch, root + "redstone_torch.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::redstone_torch_off, root + "redstone_torch_off.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::repeater, root + "repeater.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::repeater_on, root + "repeater_on.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::sand, root + "sand.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::sandstone, root + "sandstone.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::sandstone_bottom, root + "sandstone_bottom.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::sandstone_top, root + "sandstone_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::smooth_stone, root + "smooth_stone.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::smooth_quartz_block, root + "smooth_quartz_block.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::stone, root + "stone.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::stone_bricks, root + "stone_bricks.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::sugar_cane, root + "sugar_cane.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::target_side, root + "target_side.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::target_top, root + "target_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tnt_bottom, root + "tnt_bottom.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tnt_side, root + "tnt_side.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::tnt_top, root + "tnt_top.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::torch, root + "torch.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage0, root + "wheat_stage0.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage1, root + "wheat_stage1.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage2, root + "wheat_stage2.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage3, root + "wheat_stage3.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage4, root + "wheat_stage4.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage5, root + "wheat_stage5.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage6, root + "wheat_stage6.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::wheat_stage7, root + "wheat_stage7.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::white_tulip, root + "white_tulip.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::white_wool, root + "white_wool.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::block::yellow_wool, root + "yellow_wool.png");

	root = Settings::Get()->getString(settings::strings::block_atlas) + "Items/";

	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::apple, root + "apple.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::arrow, root + "arrow.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::book, root + "book.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::book_and_quill, root + "book_and_quill.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::bow, root + "bow.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::bow_pulling_0, root + "bow_pulling_0.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::bow_pulling_1, root + "bow_pulling_1.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::bow_pulling_2, root + "bow_pulling_2.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::bread, root + "bread.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::bucket, root + "bucket.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::charcoal, root + "charcoal.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::coal, root + "coal.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::comparator, root + "comparator.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::diamond, root + "diamond.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::diamond_axe, root + "diamond_axe.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::diamond_hoe, root + "diamond_hoe.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::diamond_pickaxe, root + "diamond_pickaxe.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::diamond_shovel, root + "diamond_shovel.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::feather, root + "feather.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::flint, root + "flint.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::flint_and_steel, root + "flint_and_steel.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::glowstone_dust, root + "glowstone_dust.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::ink_sac, root + "ink_sac.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::iron_axe, root + "iron_axe.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::iron_hoe, root + "iron_hoe.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::iron_ingot, root + "iron_ingot.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::iron_pickaxe, root + "iron_pickaxe.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::iron_shovel, root + "iron_shovel.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::item_frame, root + "item_frame.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::leather, root + "leather.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::oak_door, root + "oak_door.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::oak_sign, root + "oak_sign.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::paper, root + "paper.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::quartz, root + "quartz.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::redstone, root + "redstone.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::repeater, root + "repeater.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::skeleton_egg, root + "skeleton_egg.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::stick, root + "stick.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::stone_axe, root + "stone_axe.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::stone_hoe, root + "stone_hoe.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::stone_pickaxe, root + "stone_pickaxe.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::stone_shovel, root + "stone_shovel.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::string, root + "string.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::sugar_cane, root + "sugar_cane.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::water_bucket, root + "water_bucket.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::wheat, root + "wheat.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::wheat_seeds, root + "wheat_seeds.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::wooden_axe, root + "wooden_axe.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::wooden_hoe, root + "wooden_hoe.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::wooden_pickaxe, root + "wooden_pickaxe.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::wooden_shovel, root + "wooden_shovel.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::written_book, root + "written_book.png");
	utils::shader::loadSubTextureArray(16, 16, settings::consts::shader::item::zombie_egg, root + "zombie_egg.png");
	glUniform1i(glGetUniformLocation(_shader.getProgram(), "textures"), 0);
	utils::shader::check_glstate("Successfully loaded img[0] texture array 2D", true);

	_skyShader.useProgram();
	utils::shader::loadTextureShader(7, _textures[5], "Resources/Textures/Blocks/glass.png");
	glUniform1i(glGetUniformLocation(_skyShader.getProgram(), "glass"), 7);

	utils::shader::loadTextureShader(4, _textures[1], Settings::Get()->getString(settings::strings::water_still));
	glUniform1i(glGetUniformLocation(_skyShader.getProgram(), "waterStill"), 4);

	utils::shader::loadTextureShader(5, _textures[2], Settings::Get()->getString(settings::strings::water_flow));
	glUniform1i(glGetUniformLocation(_skyShader.getProgram(), "waterFlow"), 5);

	_particleShader.useProgram();
	utils::shader::loadTextureShader(6, _textures[3], Settings::Get()->getString(settings::strings::particle_atlas));
	glUniform1i(glGetUniformLocation(_particleShader.getProgram(), "particleAtlas"), 6);

	_modelShader.useProgram();
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _textures[4]);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 64, 64, 3);
	utils::shader::loadSubTextureArray(64, 64, settings::consts::shader::texture::zombie, Settings::Get()->getString(settings::strings::tex_zombie));
	utils::shader::loadSubTextureArray(64, 64, settings::consts::shader::texture::skeleton, Settings::Get()->getString(settings::strings::tex_skeleton));
	utils::shader::loadSubTextureArray(64, 64, settings::consts::shader::texture::player, Settings::Get()->getString(settings::strings::tex_player));
	glUniform1i(glGetUniformLocation(_modelShader.getProgram(), "textures"), 3);
	utils::shader::check_glstate("Successfully loaded img[3] texture array 2D", true);
}

// ************************************************************************** //
//                                getTEx                                      //
// ************************************************************************** //

int Block::getTex( int dir, int value ) const {
	(void)dir;(void)value;
	return (texture);
}

int GrassBlock::getTex( int dir, int value ) const {
	(void)value;
	if (dir == face_dir::plus_z) {
		return (settings::consts::shader::block::grass_block_top);
	} else if (dir == face_dir::minus_z) {
		return (settings::consts::shader::block::dirt);
	}
	return (settings::consts::shader::block::grass_block_side);
}

int OakLog::getTex( int dir, int value ) const {
	int axis = (value >> offset::blocks::orientation) & 0x7;
	switch (dir) {
		case face_dir::minus_z:
		case face_dir::plus_z:
			return ((axis == face_dir::plus_z) ? settings::consts::shader::block::oak_log_top
				: (axis == face_dir::plus_x) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::oak_log);
		case face_dir::minus_x:
		case face_dir::plus_x:
			return ((axis == face_dir::plus_x) ? settings::consts::shader::block::oak_log_top
				: (axis == face_dir::plus_y) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::oak_log);
		case face_dir::minus_y:
		case face_dir::plus_y:
			return ((axis == face_dir::plus_y) ? settings::consts::shader::block::oak_log_top
				: (axis == face_dir::plus_x) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::oak_log);
	}
	return (settings::consts::shader::block::oak_log);
}

int Cactus::getTex( int dir, int value ) const {
	(void)value;
	if (dir == face_dir::plus_z) {
		return (settings::consts::shader::block::cactus_top);
	} else if (dir == face_dir::minus_z) {
		return (settings::consts::shader::block::cactus_bottom);
	}
	return (settings::consts::shader::block::cactus_side);
}

int Farmland::getTex( int dir, int value ) const {
	if (value == blocks::item) {
		return (settings::consts::shader::block::debug);
	} else if (dir == face_dir::plus_z) {
		return ((value & mask::farmland::wet) ? settings::consts::shader::block::farmland_moist
			: settings::consts::shader::block::farmland);
	}
	return (settings::consts::shader::block::dirt);
}

int DirtPath::getTex( int dir, int value ) const {
	if (value == blocks::item) {
		return (settings::consts::shader::block::debug);
	} else if (dir == face_dir::plus_z) {
		return (settings::consts::shader::block::dirt_path_top);
	} else if (dir == face_dir::minus_z) {
		return (settings::consts::shader::block::dirt);
	}
	return (settings::consts::shader::block::dirt_path_side);
}

int TNT::getTex( int dir, int value ) const {
	(void)value;
	if (dir == face_dir::plus_z) {
		return (settings::consts::shader::block::tnt_top);
	} else if (dir == face_dir::minus_z) {
		return (settings::consts::shader::block::tnt_bottom);
	}
	return (settings::consts::shader::block::tnt_side);
}

int Target::getTex( int dir, int value ) const {
	(void)value;
	return ((dir == face_dir::plus_z || dir == face_dir::minus_z)
		? settings::consts::shader::block::target_top : settings::consts::shader::block::target_side);
}

int CraftingTable::getTex( int dir, int value ) const {
	if (dir == face_dir::plus_z || dir == face_dir::minus_z) {
		return (settings::consts::shader::block::crafting_table_top);
	} else if (((value >> offset::blocks::orientation) & 0x7) == dir) {
		return (settings::consts::shader::block::crafting_table_front);
	}
	return (settings::consts::shader::block::crafting_table_side);
}

int Furnace::getTex( int dir, int value ) const {
	if (dir == face_dir::plus_z || dir == face_dir::minus_z) {
		return (settings::consts::shader::block::furnace_top);
	} else if (((value >> offset::blocks::orientation) & 0x7) == dir) {
		return (((value >> offset::redstone::activated) & 0x1)
			? settings::consts::shader::block::furnace_front_on : settings::consts::shader::block::furnace_front);
	}
	return (settings::consts::shader::block::furnace_side);
}

int OakDoor::getTex( int dir, int value ) const {
	(void)dir;
	if (value == blocks::item) {
		return (settings::consts::shader::item::oak_door);
	}
	switch (dir) {
		case face_dir::plus_z:
			return (settings::consts::shader::block::oak_door_top);
		case face_dir::minus_z:
			return (settings::consts::shader::block::oak_door_bottom);
		default:
			return (((value >> offset::blocks::bitfield) & door::upper_half)
				? settings::consts::shader::block::oak_door_top
				: settings::consts::shader::block::oak_door_bottom);
	}
}

int RedstoneLamp::getTex( int dir, int value ) const {
	(void)dir;
	return ((value & mask::redstone::activated) ? settings::consts::shader::block::redstone_lamp_on
		: settings::consts::shader::block::redstone_lamp);
}

int Piston::getTex( int dir, int value ) const {
	int orientation = (value >> offset::blocks::orientation) & 0x7;
	if (dir == orientation) {
		return ((value & mask::redstone::piston::moving) ? settings::consts::shader::block::piston_inner
			: (mined == blocks::sticky_piston) ? settings::consts::shader::block::piston_top_sticky
			: settings::consts::shader::block::piston_top);
	}
	if (dir == utils::block::opposite_dir(orientation)) {
		return (settings::consts::shader::block::piston_bottom);
	}
	return (settings::consts::shader::block::piston_side);
}

int Observer::getTex( int dir, int value ) const {
	int orientation = (value >> offset::blocks::orientation) & 0x7;
	if (dir == orientation) {
		return (settings::consts::shader::block::observer_front);
	}
	if (dir == utils::block::opposite_dir(orientation)) {
		return ((value & mask::redstone::activated) ? settings::consts::shader::block::observer_back_on
			: settings::consts::shader::block::observer_back);
	}
	if (orientation & 0x4) {
		return ((dir & 0x2) ? settings::consts::shader::block::observer_side
			: settings::consts::shader::block::observer_top);
	}
	return ((dir & 0x4) ? settings::consts::shader::block::observer_top
		: settings::consts::shader::block::observer_side);
}

int RedstoneTorch::getTex( int dir, int value ) const {
	(void)dir;
	if (value == blocks::item) {
		return (settings::consts::shader::block::redstone_torch);
	}
	return ((value & mask::redstone::powered) ? settings::consts::shader::block::redstone_torch_off
		: settings::consts::shader::block::redstone_torch);
}

int RedstoneDust::getTex( int dir, int value ) const {
	(void)dir;
	return ((value == blocks::item) ? settings::consts::shader::item::redstone
		: settings::consts::shader::block::dust_dot);
}

int Repeater::getTex( int dir, int value ) const {
	(void)dir;
	return ((value == blocks::item) ? settings::consts::shader::item::repeater
		: (value & mask::redstone::powered) ? settings::consts::shader::block::repeater_on
		: settings::consts::shader::block::repeater);
}

int Comparator::getTex( int dir, int value ) const {
	(void)dir;
	return ((value == blocks::item) ? settings::consts::shader::item::comparator
		: (value & mask::redstone::powered) ? settings::consts::shader::block::comparator_on
		: settings::consts::shader::block::comparator);
}

int WheatCrop::getTex( int dir, int value ) const {
	(void)dir;
	return (settings::consts::shader::block::wheat_stage0 + (value >> offset::blocks::bitfield));
}

int ItemFrame::getTex( int dir, int value ) const {
	(void)dir;(void)value;
	return (settings::consts::shader::item::item_frame);
}

int SandStone::getTex( int dir, int value ) const {
	(void)value;
	return ((dir == face_dir::plus_z) ? settings::consts::shader::block::sandstone_top
		: (dir == face_dir::minus_z) ? settings::consts::shader::block::sandstone_bottom
		: settings::consts::shader::block::sandstone);
}

int Bookshelf::getTex( int dir, int value ) const {
	(void)value;
	switch (dir) {
		case face_dir::plus_z:
		case face_dir::minus_z:
			return (settings::consts::shader::block::oak_planks);
	}
	return (settings::consts::shader::block::bookshelf);
}

int ChiseledQuartzBlock::getTex( int dir, int value ) const {
	int axis = (value >> offset::blocks::orientation) & 0x7;
	switch (dir) {
		case face_dir::minus_z:
		case face_dir::plus_z:
			return ((axis == face_dir::plus_z) ? settings::consts::shader::block::chiseled_quartz_block_top
				: (axis == face_dir::plus_x) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::chiseled_quartz_block);
		case face_dir::minus_x:
		case face_dir::plus_x:
			return ((axis == face_dir::plus_x) ? settings::consts::shader::block::chiseled_quartz_block_top
				: (axis == face_dir::plus_y) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::chiseled_quartz_block);
		case face_dir::minus_y:
		case face_dir::plus_y:
			return ((axis == face_dir::plus_y) ? settings::consts::shader::block::chiseled_quartz_block_top
				: (axis == face_dir::plus_x) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::chiseled_quartz_block);
	}
	return (settings::consts::shader::block::chiseled_quartz_block);
}

int QuartzPillar::getTex( int dir, int value ) const {
	int axis = (value >> offset::blocks::orientation) & 0x7;
	switch (dir) {
		case face_dir::minus_z:
		case face_dir::plus_z:
			return ((axis == face_dir::plus_z) ? settings::consts::shader::block::quartz_pillar_top
				: (axis == face_dir::plus_x) ? settings::consts::shader::block::quartz_pillar_spin : settings::consts::shader::block::quartz_pillar);
		case face_dir::minus_x:
		case face_dir::plus_x:
			return ((axis == face_dir::plus_x) ? settings::consts::shader::block::quartz_pillar_top
				: (axis == face_dir::plus_y) ? settings::consts::shader::block::quartz_pillar_spin : settings::consts::shader::block::quartz_pillar);
		case face_dir::minus_y:
		case face_dir::plus_y:
			return ((axis == face_dir::plus_y) ? settings::consts::shader::block::quartz_pillar_top
				: (axis == face_dir::plus_x) ? settings::consts::shader::block::quartz_pillar_spin : settings::consts::shader::block::quartz_pillar);
	}
	return (settings::consts::shader::block::quartz_pillar);
}
