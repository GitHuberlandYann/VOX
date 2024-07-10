#include "Chunk.hpp"
#include "logs.hpp"

const std::array<std::unique_ptr<Block>, S_BLOCKS_SIZE> s_blocks = {
	std::make_unique<Air>(), std::make_unique<GrassBlock>(), std::make_unique<OakLog>(), std::make_unique<Cactus>(), std::make_unique<Farmland>(), std::make_unique<DirtPath>(), std::make_unique<TNT>(), std::make_unique<Target>(),
	std::make_unique<CraftingTable>(), std::make_unique<Furnace>(), std::make_unique<OakStairs>(), std::make_unique<QuartzStairs>(), std::make_unique<OakDoor>(), std::make_unique<OakTrapdoor>(), std::make_unique<StoneStairs>(), std::make_unique<TBD>(),
	std::make_unique<SmoothStoneStairs>(), std::make_unique<TBD>(), std::make_unique<CobbleStoneStairs>(), std::make_unique<TBD>(), std::make_unique<StoneBricksStairs>(), std::make_unique<TBD>(), std::make_unique<Lever>(), std::make_unique<OakSign>(),
	std::make_unique<Bedrock>(), std::make_unique<Dirt>(), std::make_unique<SmoothStone>(), std::make_unique<Stone>(), std::make_unique<Cobblestone>(), std::make_unique<StoneBrick>(), std::make_unique<CrackedStoneBrick>(), std::make_unique<Sand>(),
	std::make_unique<Gravel>(), std::make_unique<OakLeaves>(), std::make_unique<OakPlanks>(), std::make_unique<Glass>(), std::make_unique<GlassPane>(), std::make_unique<RedstoneLamp>(), std::make_unique<StoneButton>(), std::make_unique<OakButton>(),
	std::make_unique<CoalOre>(), std::make_unique<IronOre>(), std::make_unique<DiamondOre>(), std::make_unique<CoalBlock>(), std::make_unique<IronBlock>(), std::make_unique<DiamondBlock>(), std::make_unique<RedstoneOre>(), std::make_unique<RedstoneBlock>(),
	std::make_unique<OakSlab>(), std::make_unique<QuartzSlab>(), std::make_unique<OakFence>(), std::make_unique<StoneSlab>(), std::make_unique<TBD>(), std::make_unique<SmoothStoneSlab>(), std::make_unique<TBD>(), std::make_unique<CobbleStoneSlab>(),
	std::make_unique<TBD>(), std::make_unique<StoneBricksSlab>(), std::make_unique<TBD>(), std::make_unique<Piston>(), std::make_unique<StickyPiston>(), std::make_unique<PistonHead>(), std::make_unique<MovingPiston>(), std::make_unique<Observer>(),
	std::make_unique<Poppy>(), std::make_unique<Dandelion>(), std::make_unique<BlueOrchid>(), std::make_unique<Allium>(), std::make_unique<CornFlower>(), std::make_unique<PinkTulip>(), std::make_unique<Grass>(), std::make_unique<SugarCane>(),
	std::make_unique<DeadBush>(), std::make_unique<OakSapling>(), std::make_unique<Torch>(), std::make_unique<RedstoneTorch>(), std::make_unique<RedstoneDust>(), std::make_unique<Repeater>(), std::make_unique<Comparator>(), std::make_unique<Chest>(),
	std::make_unique<WheatCrop>(), std::make_unique<ItemFrame>(), std::make_unique<BirchPlanks>(), std::make_unique<SandStone>(), std::make_unique<Glowstone>(), std::make_unique<Bookshelf>(), std::make_unique<Lectern>(), std::make_unique<Anvil>(),
	std::make_unique<Water>(), std::make_unique<TBD>(), std::make_unique<Obsidian>(), std::make_unique<TBD>(), std::make_unique<WorldEditBrush>(), std::make_unique<Feather>(), std::make_unique<Leather>(), std::make_unique<InkSac>(),
	std::make_unique<Stick>(), std::make_unique<WoodenShovel>(), std::make_unique<StoneShovel>(), std::make_unique<IronShovel>(), std::make_unique<DiamondShovel>(), std::make_unique<WoodenAxe>(), std::make_unique<StoneAxe>(), std::make_unique<IronAxe>(),
	std::make_unique<DiamondAxe>(), std::make_unique<WoodenPickaxe>(), std::make_unique<StonePickaxe>(), std::make_unique<IronPickaxe>(), std::make_unique<DiamondPickaxe>(), std::make_unique<Bow>(), std::make_unique<Arrow>(), std::make_unique<WorldEditWand>(),
	std::make_unique<Coal>(), std::make_unique<Charcoal>(), std::make_unique<IronIngot>(), std::make_unique<Diamond>(), std::make_unique<Bucket>(), std::make_unique<WaterBucket>(), std::make_unique<WoodenHoe>(), std::make_unique<StoneHoe>(),
	std::make_unique<IronHoe>(), std::make_unique<DiamondHoe>(), std::make_unique<WheatSeeds>(), std::make_unique<Wheat>(), std::make_unique<Bread>(), std::make_unique<Apple>(), std::make_unique<Flint>(), std::make_unique<FlintAndSteel>(),
	std::make_unique<ZombieEgg>(), std::make_unique<SkeletonEgg>(), std::make_unique<String>(), std::make_unique<GlowstoneDust>(), std::make_unique<Paper>(), std::make_unique<Book>(), std::make_unique<BookAndQuill>(), std::make_unique<WrittenBook>(),
	std::make_unique<WhiteWool>(), std::make_unique<PinkWool>(), std::make_unique<MagentaWool>(), std::make_unique<PurpleWool>(), std::make_unique<BlueWool>(), std::make_unique<LightBlueWool>(), std::make_unique<CyanWool>(), std::make_unique<GreenWool>(),
	std::make_unique<LimeWool>(), std::make_unique<YellowWool>(), std::make_unique<OrangeWool>(), std::make_unique<RedWool>(), std::make_unique<BrownWool>(), std::make_unique<BlackWool>(), std::make_unique<GrayWool>(), std::make_unique<LightGrayWool>(),
	std::make_unique<WhiteCarpet>(), std::make_unique<PinkCarpet>(), std::make_unique<MagentaCarpet>(), std::make_unique<PurpleCarpet>(), std::make_unique<BlueCarpet>(), std::make_unique<LightBlueCarpet>(), std::make_unique<CyanCarpet>(), std::make_unique<GreenCarpet>(),
	std::make_unique<LimeCarpet>(), std::make_unique<YellowCarpet>(), std::make_unique<OrangeCarpet>(), std::make_unique<RedCarpet>(), std::make_unique<BrownCarpet>(), std::make_unique<BlackCarpet>(), std::make_unique<GrayCarpet>(), std::make_unique<LightGrayCarpet>(),
	std::make_unique<Quartz>(), std::make_unique<Netherrack>(), std::make_unique<QuartzOre>(), std::make_unique<QuartzBlock>(), std::make_unique<SmoothQuartzBlock>(), std::make_unique<ChiseledQuartzBlock>(), std::make_unique<QuartzPillar>(), std::make_unique<QuartzBricks>(),
	std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>()
};

void Block::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	(void)chunk;
	(void)vertices;
	(void)start;
	(void)pos;
	(void)value;
}

void Cube::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int spec, faceLight, cornerLight, shade;
	std::array<int, 4> lights;
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = this->getTex(face_dir::minus_x, value);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		lights[0] = cornerLight + (3 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		lights[1] = cornerLight + (3 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		lights[2] = cornerLight + (3 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		lights[3] = cornerLight + (3 << 8) + (shade << 12);
		utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, lights, 16, 16);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = this->getTex(face_dir::plus_x, value);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		lights[0] = cornerLight + (4 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		lights[1] = cornerLight + (4 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		lights[2] = cornerLight + (4 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		lights[3] = cornerLight + (4 << 8) + (shade << 12);
		utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, lights, 16, 16);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = this->getTex(face_dir::minus_y, value);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		lights[0] = cornerLight + (1 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		lights[1] = cornerLight + (1 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		lights[2] = cornerLight + (1 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		lights[3] = cornerLight + (1 << 8) + (shade << 12);
		utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, lights, 16, 16);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = this->getTex(face_dir::plus_y, value);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		lights[0] = cornerLight + (2 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		lights[1] = cornerLight + (2 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		lights[2] = cornerLight + (2 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		lights[3] = cornerLight + (2 << 8) + (shade << 12);
		utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, lights, 16, 16);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = this->getTex(face_dir::plus_z, value);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		lights[0] = cornerLight + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		lights[1] = cornerLight + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		lights[2] = cornerLight + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		lights[3] = cornerLight + (shade << 12);
		utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, lights, 16, 16);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = this->getTex(face_dir::minus_z, value);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		lights[0] = cornerLight + (5 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		lights[1] = cornerLight + (5 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		lights[2] = cornerLight + (5 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		lights[3] = cornerLight + (5 << 8) + (shade << 12);
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, lights, 16, 16);
	}
}

void Cross::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	(void)value;
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int light = chunk->computeLight(pos.x, pos.y, pos.z);
	utils::shader::addQuads(vertices, {p0, p5, p2, p7}, texture, light, 16, 16); // recto
	utils::shader::addQuads(vertices, {p1, p4, p3, p6}, texture, light, 16, 16);
	utils::shader::addQuads(vertices, {p5, p0, p7, p2}, texture, light, 16, 16, true); // verso
	utils::shader::addQuads(vertices, {p4, p1, p6, p3}, texture, light, 16, 16, true);
}

void Cactus::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1 - one16th);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1 - one16th);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1 - one16th);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1 - one16th);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int spec = settings::consts::shader::block::cactus_side;
	int light = chunk->computeLight(pos.x, pos.y, pos.z);

	light += (3 << 8);
	glm::vec3 offset = {one16th, .0f, .0f};
	utils::shader::addQuads(vertices, {p4 + offset, p0 + offset, p6 + offset, p2 + offset}, spec, light, 16, 16);
	
	light += (1 << 8);
	utils::shader::addQuads(vertices, {p1 - offset, p5 - offset, p3 - offset, p7 - offset}, spec, light, 16, 16);

	light -= (3 << 8);
	offset = {.0f, one16th, .0f};
	utils::shader::addQuads(vertices, {p0 + offset, p1 + offset, p2 + offset, p3 + offset}, spec, light, 16, 16);

	light += (1 << 8);
	utils::shader::addQuads(vertices, {p5 - offset, p4 - offset, p7 - offset, p6 - offset}, spec, light, 16, 16);
		
	spec = settings::consts::shader::block::cactus_top;
	light -= (2 << 8);
	utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, light, 16, 16);
	
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = settings::consts::shader::block::cactus_bottom;
		light += (5 << 8);
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, light, 16, 16);
	}
}

void Farmland::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1 - one16th);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1 - one16th);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1 - one16th);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1 - one16th);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int spec, faceLight, cornerLight, shade;
	std::array<int, 4> lights;
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = this->getTex(face_dir::minus_x, value);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		lights[0] = cornerLight + (3 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		lights[1] = cornerLight + (3 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		lights[2] = cornerLight + (3 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		lights[3] = cornerLight + (3 << 8) + (shade << 12);
		utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec + (1 << 16), lights, 16, 15);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = this->getTex(face_dir::plus_x, value);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		lights[0] = cornerLight + (4 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		lights[1] = cornerLight + (4 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		lights[2] = cornerLight + (4 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		lights[3] = cornerLight + (4 << 8) + (shade << 12);
		utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec + (1 << 16), lights, 16, 15);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = this->getTex(face_dir::minus_y, value);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		lights[0] = cornerLight + (1 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		lights[1] = cornerLight + (1 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		lights[2] = cornerLight + (1 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		lights[3] = cornerLight + (1 << 8) + (shade << 12);
		utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec + (1 << 16), lights, 16, 15);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = this->getTex(face_dir::plus_y, value);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		lights[0] = cornerLight + (2 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		lights[1] = cornerLight + (2 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		lights[2] = cornerLight + (2 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		lights[3] = cornerLight + (2 << 8) + (shade << 12);
		utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec + (1 << 16), lights, 16, 15);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = this->getTex(face_dir::plus_z, value);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		lights[0] = cornerLight + (0 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		lights[1] = cornerLight + (0 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		lights[2] = cornerLight + (0 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		lights[3] = cornerLight + (0 << 8) + (shade << 12);
		utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, lights, 16, 16);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = this->getTex(face_dir::minus_z, value);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		lights[0] = cornerLight + (5 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		lights[1] = cornerLight + (5 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		lights[2] = cornerLight + (5 << 8) + (shade << 12);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		lights[3] = cornerLight + (5 << 8) + (shade << 12);
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, lights, 16, 16);
	}
}

void Slab::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	float zTop = (value & mask::slab::top) ? 1.f : .5f;
	float zBottom = (value & mask::slab::top) ? .5f : .0f;
	int zOffset = (value & mask::slab::top) ? 0 : (8 << 16);
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + zTop);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + zTop);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + zBottom);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + zBottom);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + zTop);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + zTop);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + zBottom);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + zBottom);

	int spec, light;
	int baseSpec = texture;
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = baseSpec;
		light = chunk->computeLight(pos.x - 1, pos.y, pos.z) + (3 << 8);
		utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec + zOffset, light, 16, 8);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = baseSpec;
		light = chunk->computeLight(pos.x + 1, pos.y, pos.z) + (4 << 8);
		utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec + zOffset, light, 16, 8);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = baseSpec;
		light = chunk->computeLight(pos.x, pos.y - 1, pos.z) + (1 << 8);
		utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec + zOffset, light, 16, 8);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = baseSpec;
		light = chunk->computeLight(pos.x, pos.y + 1, pos.z) + (2 << 8);
		utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec + zOffset, light, 16, 8);
	}
	if (!(value & mask::slab::top) || utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = baseSpec;
		light = chunk->computeLight(pos.x, pos.y, pos.z) + (0 << 8);
		utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, light, 16, 16);
	}
	if ((value & mask::slab::top) || utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = baseSpec;
		light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, light, 16, 16);
	}
}

void Torch::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	(void)chunk;
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int spec = getTex(face_dir::minus_x, value);
	int light = chunk->computeLight(pos.x, pos.y, pos.z);
	light = glm::max((light & 0xF), static_cast<int>(light_level)) + (light & 0xF0);
	switch ((value >> offset::blocks::orientation) & 0x7) { // orientation
		case face_dir::minus_z: // default
			p0 += glm::vec3( 7.0f * one16th,  7.0f * one16th, -6.0f * one16th);
			p1 += glm::vec3(-7.0f * one16th,  7.0f * one16th, -6.0f * one16th);
			p2 += glm::vec3( 7.0f * one16th,  7.0f * one16th, 0);
			p3 += glm::vec3(-7.0f * one16th,  7.0f * one16th, 0);

			p4 += glm::vec3( 7.0f * one16th, -7.0f * one16th, -6.0f * one16th);
			p5 += glm::vec3(-7.0f * one16th, -7.0f * one16th, -6.0f * one16th);
			p6 += glm::vec3( 7.0f * one16th, -7.0f * one16th, 0);
			p7 += glm::vec3(-7.0f * one16th, -7.0f * one16th, 0);
			break ;
		case face_dir::plus_x:
			p0 += glm::vec3( 11.0f * one16th,  7.0f * one16th, -3.0f * one16th);
			p1 += glm::vec3(-3.0f * one16th,   7.0f * one16th, -2.0f * one16th);
			p2 += glm::vec3( 15.0f * one16th,  7.0f * one16th, 3.0f * one16th);
			p3 += glm::vec3( 1.0f * one16th,   7.0f * one16th, 4.0f * one16th);

			p4 += glm::vec3( 11.0f * one16th, -7.0f * one16th, -3.0f * one16th);
			p5 += glm::vec3(-3.0f * one16th,  -7.0f * one16th, -2.0f * one16th);
			p6 += glm::vec3( 15.0f * one16th, -7.0f * one16th, 3.0f * one16th);
			p7 += glm::vec3( 1.0f * one16th,  -7.0f * one16th, 4.0f * one16th);
			break ;
		case face_dir::minus_x:
			p0 += glm::vec3( 3.0f * one16th,  7.0f * one16th, -2.0f * one16th);
			p1 += glm::vec3(-11.0f * one16th, 7.0f * one16th, -3.0f * one16th);
			p2 += glm::vec3(-1.0f * one16th,  7.0f * one16th, 4.0f * one16th);
			p3 += glm::vec3(-15.0f * one16th, 7.0f * one16th, 3.0f * one16th);

			p4 += glm::vec3( 3.0f * one16th, -7.0f * one16th, -2.0f * one16th);
			p5 += glm::vec3(-11.0f * one16th,-7.0f * one16th, -3.0f * one16th);
			p6 += glm::vec3(-1.0f * one16th, -7.0f * one16th, 4.0f * one16th);
			p7 += glm::vec3(-15.0f * one16th,-7.0f * one16th, 3.0f * one16th);
			break ;
		case face_dir::plus_y:
			p0 += glm::vec3( 7.0f * one16th, 11.0f * one16th, -3.0f * one16th);
			p1 += glm::vec3(-7.0f * one16th, 11.0f * one16th, -3.0f * one16th);
			p2 += glm::vec3( 7.0f * one16th, 15.0f * one16th,  3.0f * one16th);
			p3 += glm::vec3(-7.0f * one16th, 15.0f * one16th,  3.0f * one16th);

			p4 += glm::vec3( 7.0f * one16th, -3.0f * one16th, -2.0f * one16th);
			p5 += glm::vec3(-7.0f * one16th, -3.0f * one16th, -2.0f * one16th);
			p6 += glm::vec3( 7.0f * one16th,  1.0f * one16th,  4.0f * one16th);
			p7 += glm::vec3(-7.0f * one16th,  1.0f * one16th,  4.0f * one16th);
			break ;
		case face_dir::minus_y:
			p0 += glm::vec3( 7.0f * one16th,  3.0f * one16th, -2.0f * one16th);
			p1 += glm::vec3(-7.0f * one16th,  3.0f * one16th, -2.0f * one16th);
			p2 += glm::vec3( 7.0f * one16th, -1.0f * one16th,  4.0f * one16th);
			p3 += glm::vec3(-7.0f * one16th, -1.0f * one16th,  4.0f * one16th);

			p4 += glm::vec3( 7.0f * one16th, -11.0f * one16th, -3.0f * one16th);
			p5 += glm::vec3(-7.0f * one16th, -11.0f * one16th, -3.0f * one16th);
			p6 += glm::vec3( 7.0f * one16th, -15.0f * one16th,  3.0f * one16th);
			p7 += glm::vec3(-7.0f * one16th, -15.0f * one16th,  3.0f * one16th);
			break ;
	}
	spec += (7 << 12) + (6 << 16);
	utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, light, 2, 10); // -x
	utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, light, 2, 10); // +x
	utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, light, 2, 10); // -y
	utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, light, 2, 10); // +y
	utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, light, 2, 2);  // +z
	if (((value >> offset::blocks::orientation) & 0x7) != face_dir::minus_z) {
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec + (8 << 16), light, 2, 2); // -z
	}
}

void Stairs::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0.0f, start.y + pos.y + 0.0f, pos.z + 1.0f);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1.0f, start.y + pos.y + 0.0f, pos.z + 1.0f);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0.0f, start.y + pos.y + 0.0f, pos.z + 0.0f);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1.0f, start.y + pos.y + 0.0f, pos.z + 0.0f);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0.0f, start.y + pos.y + 1.0f, pos.z + 1.0f);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1.0f, start.y + pos.y + 1.0f, pos.z + 1.0f);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0.0f, start.y + pos.y + 1.0f, pos.z + 0.0f);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1.0f, start.y + pos.y + 1.0f, pos.z + 0.0f);

	if (!(value & mask::stairs::top)) { // TODO rework this fun entirely
		int corners = (value >> offset::blocks::bitfield) & 0xF;
		int spec = texture;
		int light;
		if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
			light = chunk->computeLight(pos.x - 1, pos.y, pos.z) + (3 << 8);
			switch (corners & (corners::mm | corners::mp)) {
				case corners::mm | corners::mp:
					utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, light, 16, 16);
					break ;
				case corners::mm:
					utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, -0.5f, 0.0f), p0, p6 + glm::vec3(0.0f, -0.5f, 0.5f), p2 + glm::vec3(0.0f, 0.0f, 0.5f)}, spec + (8 << 12), light, 8, 8);
					utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, 0.0f, -0.5f), p0 + glm::vec3(0.0f, 0.0f, -0.5f), p6, p2}, spec + (8 << 16), light, 16, 8);
					break ;
				case corners::mp:
					utils::shader::addQuads(vertices, {p4, p0 + glm::vec3(0.0f, 0.5f, 0.0f), p6 + glm::vec3(0.0f, 0.0f, 0.5f), p2 + glm::vec3(0.0f, 0.5f, 0.5f)}, spec, light, 8, 8);
				default:
					utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, 0.0f, -0.5f), p0 + glm::vec3(0.0f, 0.0f, -0.5f), p6, p2}, spec + (8 << 16), light, 16, 8);
					break ;
			}
		}
		if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
			light = chunk->computeLight(pos.x + 1, pos.y, pos.z) + (4 << 8);
			switch (corners & (corners::pm | corners::pp)) {
				case corners::pm | corners::pp:
					utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, light, 16, 16);
					break ;
				case corners::pm:
					utils::shader::addQuads(vertices, {p1, p5 + glm::vec3(0.0f, -0.5f, 0.0f), p3 + glm::vec3(0.0f, 0.0f, 0.5f), p7 + glm::vec3(0.0f, -0.5f, 0.5f)}, spec, light, 8, 8);
					utils::shader::addQuads(vertices, {p1 + glm::vec3(0.0f, 0.0f, -0.5f), p5 + glm::vec3(0.0f, 0.0f, -0.5f), p3, p7}, spec + (8 << 16), light, 16, 8);
					break ;
				case corners::pp:
					utils::shader::addQuads(vertices, {p1 + glm::vec3(0.0f, 0.5f, 0.0f), p5, p3 + glm::vec3(0.0f, 0.5f, 0.5f), p7 + glm::vec3(0.0f, 0.0f, 0.5f)}, spec + (8 << 12), light, 8, 8);
				default:
					utils::shader::addQuads(vertices, {p1 + glm::vec3(0.0f, 0.0f, -0.5f), p5 + glm::vec3(0.0f, 0.0f, -0.5f), p3, p7}, spec + (8 << 16), light, 16, 8);
					break ;
			}
		}
		if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
			light = chunk->computeLight(pos.x, pos.y - 1, pos.z) + (1 << 8);
			switch (corners & (corners::mm | corners::pm)) {
				case corners::mm | corners::pm:
					utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, light, 16, 16);
					break ;
				case corners::mm:
					utils::shader::addQuads(vertices, {p0, p1 + glm::vec3(-0.5f, 0.0f, 0.0f), p2 + glm::vec3(0.0f, 0.0f, 0.5f), p3 + glm::vec3(-0.5f, 0.0f, 0.5f)}, spec, light, 8, 8);
					utils::shader::addQuads(vertices, {p0 + glm::vec3(0.0f, 0.0f, -0.5f), p1 + glm::vec3(0.0f, 0.0f, -0.5f), p2, p3}, spec + (8 << 16), light, 16, 8);
					break ;
				case corners::pm:
					utils::shader::addQuads(vertices, {p0 + glm::vec3(0.5f, 0.0f, 0.0f), p1, p2 + glm::vec3(0.5f, 0.0f, 0.5f), p3 + glm::vec3(0.0f, 0.0f, 0.5f)}, spec + (8 << 12), light, 8, 8);
				default:
					utils::shader::addQuads(vertices, {p0 + glm::vec3(0.0f, 0.0f, -0.5f), p1 + glm::vec3(0.0f, 0.0f, -0.5f), p2, p3}, spec + (8 << 16), light, 16, 8);
					break ;
			}
		}
		if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
			light = chunk->computeLight(pos.x, pos.y + 1, pos.z) + (2 << 8);
			switch (corners & (corners::mp | corners::pp)) {
				case corners::mp | corners::pp:
					utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, light, 16, 16);
					break ;
				case corners::mp:
					utils::shader::addQuads(vertices, {p5 + glm::vec3(-0.5f, 0.0f, 0.0f), p4, p7 + glm::vec3(-0.5f, 0.0f, 0.5f), p6 + glm::vec3(0.0f, 0.0f, 0.5f)}, spec + (8 << 12), light, 8, 8);
					utils::shader::addQuads(vertices, {p5 + glm::vec3(0.0f, 0.0f, -0.5f), p4 + glm::vec3(0.0f, 0.0f, -0.5f), p7, p6}, spec + (8 << 16), light, 16, 8);
					break ;
				case corners::pp:
					utils::shader::addQuads(vertices, {p5, p4 + glm::vec3(0.5f, 0.0f, 0.0f), p7 + glm::vec3(0.0f, 0.0f, 0.5f), p6 + glm::vec3(0.5f, 0.0f, 0.5f)}, spec, light, 8, 8);
				default:
					utils::shader::addQuads(vertices, {p5 + glm::vec3(0.0f, 0.0f, -0.5f), p4 + glm::vec3(0.0f, 0.0f, -0.5f), p7, p6}, spec + (8 << 16), light, 16, 8);
					break ;
			}
		}
		if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
			light = chunk->computeLight(pos.x, pos.y, pos.z + 1) + (0 << 8);
			switch (corners & (corners::mp | corners::pp)) {
				case corners::mp:
					utils::shader::addQuads(vertices, {p4, p5 + glm::vec3(-0.5f, 0.0f, 0.0f), p0 + glm::vec3(0.0f, 0.5f, 0.0f), p1 + glm::vec3(-0.5f, 0.5f, 0.0f)}, spec, light, 8, 8);
					break ;
				case corners::pp:
					utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, 0.0f, 0.0f), p5, p0 + glm::vec3(0.5f, 0.5f, 0.0f), p1 + glm::vec3(0.0f, 0.5f, 0.0f)}, spec + (8 << 12), light, 8, 8);
					break ;
				case corners::mp | corners::pp:
					utils::shader::addQuads(vertices, {p4, p5, p0 + glm::vec3(0.0f, 0.5f, 0.0f), p1 + glm::vec3(0.0f, 0.5f, 0.0f)}, spec, light, 16, 8);
					break ;
			}
			switch (corners & (corners::mm | corners::pm)) {
				case corners::mm:
					utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, -0.5f, 0.0f), p5 + glm::vec3(-0.5f, -0.5f, 0.0f), p0, p1 + glm::vec3(-0.5f, 0.0f, 0.0f)}, spec + (8 << 16), light, 8, 8);
					break ;
				case corners::pm:
					utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, -0.5f, 0.0f), p5 + glm::vec3(0.0f, -0.5f, 0.0f), p0 + glm::vec3(0.5f, 0.0f, 0.0f), p1}, spec + (8 << 12) + (8 << 16), light, 8, 8);
					break ;
				case corners::mm | corners::pm:
					utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, -0.5f, 0.0f), p5 + glm::vec3(0.0f, -0.5f, 0.0f), p0, p1}, spec + (8 << 16), light, 16, 8);
					break ;
			}
		}
		// up of first step
		light = chunk->computeLight(pos.x, pos.y, pos.z) + (0 << 8);
		switch (corners & (corners::mp | corners::pp)) {
			case corners::pp:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, 0.0f, -0.5f), p5 + glm::vec3(-0.5f, 0.0f, -0.5f), p0 + glm::vec3(0.0f, 0.5f, -0.5f), p1 + glm::vec3(-0.5f, 0.5f, -0.5f)}, spec, light, 8, 8);
				break ;
			case corners::mp:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, 0.0f, -0.5f), p5 + glm::vec3(0.0f, 0.0f, -0.5f), p0 + glm::vec3(0.5f, 0.5f, -0.5f), p1 + glm::vec3(0.0f, 0.5f, -0.5f)}, spec + (8 << 12), light, 8, 8);
				break ;
			case corners::mp | corners::pp:
				break ;
			default:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, 0.0f, -0.5f), p5 + glm::vec3(0.0f, 0.0f, -0.5f), p0 + glm::vec3(0.0f, 0.5f, -0.5f), p1 + glm::vec3(0.0f, 0.5f, -0.5f)}, spec, light, 16, 8);
				break ;
		}
		switch (corners & (corners::mm | corners::pm)) {
			case corners::pm:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, -0.5f, -0.5f), p5 + glm::vec3(-0.5f, -0.5f, -0.5f), p0 + glm::vec3(0.0f, 0.0f, -0.5f), p1 + glm::vec3(-0.5f, 0.0f, -0.5f)}, spec + (8 << 16), light, 8, 8);
				break ;
			case corners::mm:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, -0.5f, -0.5f), p5 + glm::vec3(0.0f, -0.5f, -0.5f), p0 + glm::vec3(0.5f, 0.0f, -0.5f), p1 + glm::vec3(0.0f, 0.0f, -0.5f)}, spec + (8 << 12) + (8 << 16), light, 8, 8);
				break ;
			case corners::mm | corners::pm:
				break;
			default:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, -0.5f, -0.5f), p5 + glm::vec3(0.0f, -0.5f, -0.5f), p0 + glm::vec3(0.0f, 0.0f, -0.5f), p1 + glm::vec3(0.0f, 0.0f, -0.5f)}, spec + (8 << 16), light, 16, 8);
				break ;
		}
		// front minus_x of second step
		light += (3 << 8);
		switch (corners) {
			case corners::pm | corners::pp: // full width
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, 0.0f, 0.0f), p0 + glm::vec3(0.5f, 0.0f, 0.0f), p4 + glm::vec3(0.5f, 0.0f, -0.5f), p0 + glm::vec3(0.5f, 0.0f, -0.5f)}, spec, light, 16, 8);
				break ;
			case corners::pm: // only 'right'
			case corners::pm | corners::pp | corners::mp:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, -0.5f, 0.0f), p0 + glm::vec3(0.5f, 0.0f, 0.0f), p4 + glm::vec3(0.5f, -0.5f, -0.5f), p0 + glm::vec3(0.5f, 0.0f, -0.5f)}, spec + (8 << 12), light, 8, 8);
				break ;
			case corners::pp: // only 'left'
			case corners::pm | corners::pp | corners::mm:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, 0.0f, 0.0f), p0 + glm::vec3(0.5f, 0.5f, 0.0f), p4 + glm::vec3(0.5f, 0.0f, -0.5f), p0 + glm::vec3(0.5f, 0.5f, -0.5f)}, spec, light, 16, 8);
				break ;
		}
		// front plus_x of second step
		light += (1 << 8);
		switch (corners) {
			case corners::mm | corners::mp: // full width
				utils::shader::addQuads(vertices, {p1 + glm::vec3(-0.5f, 0.0f, 0.0f), p5 + glm::vec3(-0.5f, 0.0f, 0.0f), p1 + glm::vec3(-0.5f, 0.0f, -0.5f), p5 + glm::vec3(-0.5f, 0.0f, -0.5f)}, spec, light, 16, 8);
				break ;
			case corners::mp: // only 'right'
			case corners::mm | corners::mp | corners::pm:
				utils::shader::addQuads(vertices, {p1 + glm::vec3(-0.5f, 0.5f, 0.0f), p5 + glm::vec3(-0.5f, 0.0f, 0.0f), p1 + glm::vec3(-0.5f, 0.5f, -0.5f), p5 + glm::vec3(-0.5f, 0.0f, -0.5f)}, spec + (8 << 12), light, 8, 8);
				break ;
			case corners::mm: // only 'left'
			case corners::mm | corners::mp | corners::pp:
				utils::shader::addQuads(vertices, {p1 + glm::vec3(-0.5f, 0.0f, 0.0f), p5 + glm::vec3(-0.5f, -0.5f, 0.0f), p1 + glm::vec3(-0.5f, 0.0f, -0.5f), p5 + glm::vec3(-0.5f, -0.5f, -0.5f)}, spec, light, 8, 8);
				break ;
		}
		// front minus_y of second step
		light -= (3 << 8);
		switch (corners) {
			case corners::mp | corners::pp: // full width
				utils::shader::addQuads(vertices, {p0 + glm::vec3(0.0f, 0.5f, 0.0f), p1 + glm::vec3(0.0f, 0.5f, 0.0f), p0 + glm::vec3(0.0f, 0.5f, -0.5f), p1 + glm::vec3(0.0f, 0.5f, -0.5f)}, spec, light, 16, 8);
				break ;
			case corners::pp: // only 'right'
			case corners::mp | corners::pp | corners::mm:
				utils::shader::addQuads(vertices, {p0 + glm::vec3(0.5f, 0.5f, 0.0f), p1 + glm::vec3(0.0f, 0.5f, 0.0f), p0 + glm::vec3(0.5f, 0.5f, -0.5f), p1 + glm::vec3(0.0f, 0.5f, -0.5f)}, spec + (8 << 12), light, 8, 8);
				break ;
			case corners::mp: // only 'left'
			case corners::mp | corners::pp | corners::pm:
				utils::shader::addQuads(vertices, {p0 + glm::vec3(0.0f, 0.5f, 0.0f), p1 + glm::vec3(-0.5f, 0.5f, 0.0f), p0 + glm::vec3(0.0f, 0.5f, -0.5f), p1 + glm::vec3(-0.5f, 0.5f, -0.5f)}, spec, light, 8, 8);
				break ;
		}
		// front plus_y of second step
		light += (1 << 8);
		switch (corners) {
			case corners::mm | corners::pm: // full width
				utils::shader::addQuads(vertices, {p5 + glm::vec3(0.0f, -0.5f, 0.0f), p4 + glm::vec3(0.0f, -0.5f, 0.0f), p5 + glm::vec3(0.0f, -0.5f, -0.5f), p4 + glm::vec3(0.0f, -0.5f, -0.5f)}, spec, light, 16, 8);
				break ;
			case corners::pm: // only 'left'
			case corners::mm | corners::pm | corners::mp:
				utils::shader::addQuads(vertices, {p5 + glm::vec3(0.0f, -0.5f, 0.0f), p4 + glm::vec3(0.5f, -0.5f, 0.0f), p5 + glm::vec3(0.0f, -0.5f, -0.5f), p4 + glm::vec3(0.5f, -0.5f, -0.5f)}, spec, light, 8, 8);
				break ;
			case corners::mm: // only 'right'
			case corners::mm | corners::pm | corners::pp:
				utils::shader::addQuads(vertices, {p5 + glm::vec3(-0.5f, -0.5f, 0.0f), p4 + glm::vec3(0.0f, -0.5f, 0.0f), p5 + glm::vec3(-0.5f, -0.5f, -0.5f), p4 + glm::vec3(0.0f, -0.5f, -0.5f)}, spec + (8 << 12), light, 8, 8);
				break ;
		}
		if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
			light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
			utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, light, 16, 16);
		}
	} else { // upside down stairs
		int corners = (value >> offset::blocks::bitfield) & 0xF;
		int spec = texture;
		int light;
		if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
			light = chunk->computeLight(pos.x - 1, pos.y, pos.z) + (3 << 8);
			switch (corners & (corners::mm | corners::mp)) {
				case corners::mm | corners::mp:
					utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, light, 16, 16);
					break ;
				case corners::mm:
					utils::shader::addQuads(vertices, {p4 + glm::vec3(0.f, -.5f, -.5f), p0 + glm::vec3(0.f, 0.f, -.5f), p6 + glm::vec3(0.f, -.5f, 0.f), p2}, spec + (8 << 12) + (8 << 16), light, 8, 8);
					utils::shader::addQuads(vertices, {p4, p0, p6 + glm::vec3(0.f, 0.f, .5f), p2 + glm::vec3(0.f, 0.f, .5f)}, spec, light, 16, 8);
					break ;
				case corners::mp:
					utils::shader::addQuads(vertices, {p4 + glm::vec3(0.f, 0.f, -.5f), p0 + glm::vec3(0.f, 0.5f, -.5f), p6, p2 + glm::vec3(0.f, .5f, 0.f)}, spec + (8 << 16), light, 8, 8);
				default:
					utils::shader::addQuads(vertices, {p4, p0, p6 + glm::vec3(0.f, 0.f, .5f), p2 + glm::vec3(0.f, 0.f, .5f)}, spec, light, 16, 8);
					break ;
			}
		}
		if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
			light = chunk->computeLight(pos.x + 1, pos.y, pos.z) + (4 << 8);
			switch (corners & (corners::pm | corners::pp)) {
				case corners::pm | corners::pp:
					utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, light, 16, 16);
					break ;
				case corners::pm:
					utils::shader::addQuads(vertices, {p1 + glm::vec3(0.f, 0.f, -.5f), p5 + glm::vec3(0.f, -0.5f, -.5f), p3, p7 + glm::vec3(0.f, -.5f, 0.f)}, spec + (8 << 16), light, 8, 8);
					utils::shader::addQuads(vertices, {p1, p5, p3 + glm::vec3(0.f, 0.f, .5f), p7 + glm::vec3(0.f, 0.f, .5f)}, spec, light, 16, 8);
					break ;
				case corners::pp:
					utils::shader::addQuads(vertices, {p1 + glm::vec3(0.f, .5f, -.5f), p5 + glm::vec3(0.f, 0.f, -.5f), p3 + glm::vec3(0.f, .5f, 0.f), p7}, spec + (8 << 12) + (8 << 16), light, 8, 8);
				default:
					utils::shader::addQuads(vertices, {p1, p5, p3 + glm::vec3(0.f, 0.f, .5f), p7 + glm::vec3(0.f, 0.f, .5f)}, spec, light, 16, 8);
					break ;
			}
		}
		if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
			light = chunk->computeLight(pos.x, pos.y - 1, pos.z) + (1 << 8);
			switch (corners & (corners::mm | corners::pm)) {
				case corners::mm | corners::pm:
					utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, light, 16, 16);
					break ;
				case corners::mm:
					utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -.5f), p1 + glm::vec3(-.5f, .0f, -.5f), p2, p3 + glm::vec3(-.5f, .0f, 0.f)}, spec + (8 << 16), light, 8, 8);
					utils::shader::addQuads(vertices, {p0, p1, p2 + glm::vec3(0.f, 0.f, .5f), p3 + glm::vec3(0.f, 0.f, .5f)}, spec, light, 16, 8);
					break ;
				case corners::pm:
					utils::shader::addQuads(vertices, {p0 + glm::vec3(.5f, .0f, -.5f), p1 + glm::vec3(0.f, 0.f, -.5f), p2 + glm::vec3(0.5f, .0f, 0.f), p3}, spec + (8 << 12) + (8 << 16), light, 8, 8);
				default:
					utils::shader::addQuads(vertices, {p0, p1, p2 + glm::vec3(0.f, 0.f, .5f), p3 + glm::vec3(0.f, 0.f, .5f)}, spec, light, 16, 8);
					break ;
			}
		}
		if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
			light = chunk->computeLight(pos.x, pos.y + 1, pos.z) + (2 << 8);
			switch (corners & (corners::mp | corners::pp)) {
				case corners::mp | corners::pp:
					utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, light, 16, 16);
					break ;
				case corners::mp:
					utils::shader::addQuads(vertices, {p5 + glm::vec3(-.5f, .0f, -.5f), p4 + glm::vec3(0.f, 0.f, -.5f), p7 + glm::vec3(-0.5f, .0f, 0.f), p6}, spec + (8 << 12) + (8 << 16), light, 8, 8);
					utils::shader::addQuads(vertices, {p5, p4, p7 + glm::vec3(0.f, 0.f, .5f), p6 + glm::vec3(0.f, 0.f, .5f)}, spec, light, 16, 8);
					break ;
				case corners::pp:
					utils::shader::addQuads(vertices, {p5 + glm::vec3(0.f, 0.f, -.5f), p4 + glm::vec3(.5f, .0f, -.5f), p7, p6 + glm::vec3(.5f, .0f, 0.f)}, spec + (8 << 16), light, 8, 8);
				default:
					utils::shader::addQuads(vertices, {p5, p4, p7 + glm::vec3(0.f, 0.f, .5f), p6 + glm::vec3(0.f, 0.f, .5f)}, spec, light, 16, 8);
					break ;
			}
		}
		if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
			light = chunk->computeLight(pos.x, pos.y, pos.z + 1) + (0 << 8);
			utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, light, 16, 16);
		}
		// 'down' of first step
		light = chunk->computeLight(pos.x, pos.y, pos.z) + (5 << 8);
		switch (corners & (corners::mp | corners::pp)) {
			case corners::pp:
				utils::shader::addQuads(vertices, {p2 + glm::vec3(.0f, .5f, .5f), p3 + glm::vec3(-.5f, .5f, .5f), p6 + glm::vec3(.0f, .0f, .5f), p7 + glm::vec3(-.5f, .0f, .5f)}, spec, light, 8, 8);
				break ;
			case corners::mp:
				utils::shader::addQuads(vertices, {p2 + glm::vec3(.5f, .5f, .5f), p3 + glm::vec3(.0f, .5f, .5f), p6 + glm::vec3(.5f, .0f, .5f), p7 + glm::vec3(.0f, .0f, .5f)}, spec + (8 << 12), light, 8, 8);
				break ;
			case corners::mp | corners::pp:
				break ;
			default:
				utils::shader::addQuads(vertices, {p2 + glm::vec3(.0f, .5f, .5f), p3 + glm::vec3(.0f, .5f, .5f), p6 + glm::vec3(.0f, .0f, .5f), p7 + glm::vec3(.0f, .0f, .5f)}, spec, light, 16, 8);
				break ;
		}
		switch (corners & (corners::mm | corners::pm)) {
			case corners::pm:
				utils::shader::addQuads(vertices, {p2 + glm::vec3(.0f, .0f, .5f), p3 + glm::vec3(-.5f, .0f, .5f), p6 + glm::vec3(.0f, -.5f, .5f), p7 + glm::vec3(-.5f, -.5f, .5f)}, spec + (8 << 16), light, 8, 8);
				break ;
			case corners::mm:
				utils::shader::addQuads(vertices, {p2 + glm::vec3(.5f, .0f, .5f), p3 + glm::vec3(.0f, .0f, .5f), p6 + glm::vec3(.5f, -.5f, .5f), p7 + glm::vec3(.0f, -.5f, .5f)}, spec + (8 << 12) + (8 << 16), light, 8, 8);
				break ;
			case corners::mm | corners::pm:
				break;
			default:
				utils::shader::addQuads(vertices, {p2 + glm::vec3(.0f, .0f, .5f), p3 + glm::vec3(.0f, .0f, .5f), p6 + glm::vec3(.0f, -.5f, .5f), p7 + glm::vec3(.0f, -.5f, .5f)}, spec + (8 << 16), light, 16, 8);
				break ;
		}
		// front minus_x of second step
		light -= (2 << 8);
		switch (corners) {
			case corners::pm | corners::pp: // full width
				utils::shader::addQuads(vertices, {p4 + glm::vec3(.5f, .0f, -.5f), p0 + glm::vec3(.5f, .0f, -.5f), p4 + glm::vec3(.5f, .0f, -1.0f), p0 + glm::vec3(.5f, .0f, -1.0f)}, spec, light, 16, 8);
				break ;
			case corners::pm: // only 'right'
			case corners::pm | corners::pp | corners::mp:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(.5f, -.5f, -.5f), p0 + glm::vec3(.5f, .0f, -.5f), p4 + glm::vec3(.5f, -.5f, -1.0f), p0 + glm::vec3(.5f, .0f, -1.0f)}, spec + (8 << 12), light, 8, 8);
				break ;
			case corners::pp: // only 'left'
			case corners::pm | corners::pp | corners::mm:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(.5f, .0f, -.5f), p0 + glm::vec3(.5f, .5f, -.5f), p4 + glm::vec3(.5f, .0f, -1.0f), p0 + glm::vec3(.5f, .5f, -1.0f)}, spec, light, 8, 8);
				break ;
		}
		// front plus_x of second step
		light += (1 << 8);
		switch (corners) {
			case corners::mm | corners::mp: // full width
				utils::shader::addQuads(vertices, {p1 + glm::vec3(-.5f, .0f, -.5f), p5 + glm::vec3(-.5f, .0f, -.5f), p1 + glm::vec3(-.5f, .0f, -1.0f), p5 + glm::vec3(-.5f, .0f, -1.0f)}, spec, light, 16, 8);
				break ;
			case corners::mp: // only 'right'
			case corners::mm | corners::mp | corners::pm:
				utils::shader::addQuads(vertices, {p1 + glm::vec3(-.5f, .5f, -.5f), p5 + glm::vec3(-.5f, .0f, -.5f), p1 + glm::vec3(-.5f, .5f, -1.0f), p5 + glm::vec3(-.5f, .0f, -1.0f)}, spec + (8 << 12), light, 8, 8);
				break ;
			case corners::mm: // only 'left'
			case corners::mm | corners::mp | corners::pp:
				utils::shader::addQuads(vertices, {p1 + glm::vec3(-.5f, .0f, -.5f), p5 + glm::vec3(-.5f, -.5f, -.5f), p1 + glm::vec3(-.5f, .0f, -1.0f), p5 + glm::vec3(-.5f, -.5f, -1.0f)}, spec, light, 8, 8);
				break ;
		}
		// front minus_y of second step
		light -= (3 << 8);
		switch (corners) {
			case corners::mp | corners::pp: // full width
				utils::shader::addQuads(vertices, {p0 + glm::vec3(.0f, .5f, -.5f), p1 + glm::vec3(.0f, .5f, -.5f), p0 + glm::vec3(.0f, .5f, -1.0f), p1 + glm::vec3(.0f, .5f, -1.0f)}, spec, light, 16, 8);
				break ;
			case corners::pp: // only 'right'
			case corners::mp | corners::pp | corners::mm:
				utils::shader::addQuads(vertices, {p0 + glm::vec3(.5f, .5f, -.5f), p1 + glm::vec3(.0f, .5f, -.5f), p0 + glm::vec3(.5f, .5f, -1.0f), p1 + glm::vec3(.0f, .5f, -1.0f)}, spec + (8 << 12), light, 8, 8);
				break ;
			case corners::mp: // only 'left'
			case corners::mp | corners::pp | corners::pm:
				utils::shader::addQuads(vertices, {p0 + glm::vec3(.0f, .5f, -.5f), p1 + glm::vec3(-.5f, .5f, -.5f), p0 + glm::vec3(.0f, .5f, -1.0f), p1 + glm::vec3(-.5f, .5f, -1.0f)}, spec, light, 8, 8);
				break ;
		}
		// front plus_y of second step
		light += (1 << 8);
		switch (corners) {
			case corners::mm | corners::pm: // full width
				utils::shader::addQuads(vertices, {p5 + glm::vec3(.0f, -.5f, -.5f), p4 + glm::vec3(.0f, -.5f, -.5f), p5 + glm::vec3(.0f, -.5f, -1.0f), p4 + glm::vec3(.0f, -.5f, -1.0f)}, spec, light, 16, 8);
				break ;
			case corners::pm: // only 'left'
			case corners::mm | corners::pm | corners::mp:
				utils::shader::addQuads(vertices, {p5 + glm::vec3(.0f, -.5f, -.5f), p4 + glm::vec3(.5f, -.5f, -.5f), p5 + glm::vec3(.0f, -.5f, -1.0f), p4 + glm::vec3(.5f, -.5f, -1.0f)}, spec, light, 8, 8);
				break ;
			case corners::mm: // only 'right'
			case corners::mm | corners::pm | corners::pp:
				utils::shader::addQuads(vertices, {p5 + glm::vec3(-.5f, -.5f, -.5f), p4 + glm::vec3(.0f, -.5f, -.5f), p5 + glm::vec3(-.5f, -.5f, -1.0f), p4 + glm::vec3(.0f, -.5f, -1.0f)}, spec + (8 << 12), light, 8, 8);
				break ;
		}
		if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
			light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
			switch (corners & (corners::mp | corners::pp)) {
				case corners::mp:
					utils::shader::addQuads(vertices, {p2 + glm::vec3(.0f, .5f, .0f), p3 + glm::vec3(-.5f, .5f, .0f), p6, p7 + glm::vec3(-.5f, .0f, .0f)}, spec, light, 8, 8);
					break ;
				case corners::pp:
					utils::shader::addQuads(vertices, {p2 + glm::vec3(.5f, .5f, .0f), p3 + glm::vec3(.0f, .5f, .0f), p6 + glm::vec3(.5f, .0f, .0f), p7}, spec + (8 << 12), light, 8, 8);
					break ;
				case corners::mp | corners::pp:
					utils::shader::addQuads(vertices, {p2 + glm::vec3(.0f, .5f, .0f), p3 + glm::vec3(.0f, .5f, .0f), p6, p7}, spec, light, 16, 8);
					break ;
			}
			switch (corners & (corners::mm | corners::pm)) {
				case corners::mm:
					utils::shader::addQuads(vertices, {p2, p3 + glm::vec3(-.5f, .0f, .0f), p6 + glm::vec3(.0f, -.5f, .0f), p7 + glm::vec3(-.5f, -.5f, .0f)}, spec + (8 << 16), light, 8, 8);
					break ;
				case corners::pm:
					utils::shader::addQuads(vertices, {p2 + glm::vec3(.5f, .0f, .0f), p3, p6 + glm::vec3(.5f, -.5f, .0f), p7 + glm::vec3(.0f, -.5f, .0f)}, spec + (8 << 12) + (8 << 16), light, 8, 8);
					break ;
				case corners::mm | corners::pm:
					utils::shader::addQuads(vertices, {p2, p3, p6 + glm::vec3(.0f, -.5f, .0f), p7 + glm::vec3(.0f, -.5f, .0f)}, spec + (8 << 16), light, 16, 8);
					break ;
			}
		}
	}
}

void Door::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int orientation = 0;
	int spec, light;
	int bitfield = value >> offset::blocks::bitfield;
	bool open = !!(bitfield & door::open) ^ ((value >> offset::redstone::powered) & 0x1);
	bool flip;
	switch ((value >> offset::blocks::orientation) & 0x7) {
		case face_dir::minus_x:
			if (!open) {
				orientation = face_dir::minus_x;
				flip = !(bitfield & door::right_hinge);
			} else if (bitfield & door::right_hinge) {
				orientation = face_dir::plus_y;
				flip = true;
			} else {
				orientation = face_dir::minus_y;
				flip = false;
			}
			break ;
		case face_dir::plus_x:
			if (!open) {
				orientation = face_dir::plus_x;
				flip = (bitfield & door::right_hinge);
			} else if (bitfield & door::right_hinge) {
				orientation = face_dir::minus_y;
				flip = false;
			} else {
				orientation = face_dir::plus_y;
				flip = true;
			}
			break ;
		case face_dir::minus_y:
			if (!open) {
				orientation = face_dir::minus_y;
				flip = !(bitfield & door::right_hinge);
			} else if (bitfield & door::right_hinge) {
				orientation = face_dir::minus_x;
				flip = false;
			} else {
				orientation = face_dir::plus_x;
				flip = true;
			}
			break ;
		case face_dir::plus_y:
			if (!open) {
				orientation = face_dir::plus_y;
				flip = (bitfield & door::right_hinge);
			} else if (bitfield & door::right_hinge) {
				orientation = face_dir::plus_x;
				flip = true;
			} else {
				orientation = face_dir::minus_x;
				flip = false;
			}
			break ;
	}
	switch (orientation) {
		case face_dir::minus_x:
			p0.x += 0.8125f;
			p2.x += 0.8125f;
			p4.x += 0.8125f;
			p6.x += 0.8125f;
			spec = this->getTex(face_dir::minus_x, value);
			light = chunk->computeLight(pos.x, pos.y, pos.z) + (3 << 8);
			utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, light, 16, 16, !flip);
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				spec = this->getTex(face_dir::plus_x, value);
				light = chunk->computeLight(pos.x + 1, pos.y, pos.z) + (4 << 8);
				utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, light, 16, 16, flip);
			}
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				spec = this->getTex(face_dir::minus_y, value);
				light = chunk->computeLight(pos.x, pos.y - 1, pos.z) + (1 << 8);
				utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, light, 3, 16);
			}
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				spec = this->getTex(face_dir::plus_y, value);
				light = chunk->computeLight(pos.x, pos.y + 1, pos.z) + (2 << 8);
				utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, light, 3, 16);
			}
			if ((bitfield & door::upper_half) && utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				spec = this->getTex(face_dir::plus_z);
				light = chunk->computeLight(pos.x, pos.y, pos.z + 1) + (0 << 8);
				utils::shader::addQuads(vertices, {p0, p4, p1, p5}, spec, light, 16, 3);
			}
			if (!(bitfield & door::upper_half) && utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				spec = this->getTex(face_dir::minus_z, face_dir::minus_z);
				light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
				utils::shader::addQuads(vertices, {p6, p2, p7, p3}, spec + (13 << 16), light, 16, 3);
			}
			break ;
		case face_dir::plus_x:
			p1.x -= 0.8125f;
			p3.x -= 0.8125f;
			p5.x -= 0.8125f;
			p7.x -= 0.8125f;
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
				spec = this->getTex(face_dir::minus_x, value);
				light = chunk->computeLight(pos.x - 1, pos.y, pos.z) + (3 << 8);
				utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, light, 16, 16, !flip);
			}
			spec = this->getTex(face_dir::plus_x, value);
			light = chunk->computeLight(pos.x, pos.y, pos.z) + (4 << 8);
			utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, light, 16, 16, flip);
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				spec = this->getTex(face_dir::minus_y, value);
				light = chunk->computeLight(pos.x, pos.y - 1, pos.z) + (1 << 8);
				utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, light, 3, 16);
			}
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				spec = this->getTex(face_dir::plus_y, value);
				light = chunk->computeLight(pos.x, pos.y + 1, pos.z) + (2 << 8);
				utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, light, 3, 16);
			}
			if ((bitfield & door::upper_half) && utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				spec = this->getTex(face_dir::plus_z);
				light = chunk->computeLight(pos.x, pos.y, pos.z + 1) + (0 << 8);
				utils::shader::addQuads(vertices, {p0, p4, p1, p5}, spec, light, 16, 3);
			}
			if (!(bitfield & door::upper_half) && utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				spec = this->getTex(face_dir::minus_z, face_dir::minus_z);
				light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
				utils::shader::addQuads(vertices, {p6, p2, p7, p3}, spec + (13 << 16), light, 16, 3);
			}
			break ;
		case face_dir::minus_y:
			p0.y += 0.8125f;
			p1.y += 0.8125f;
			p2.y += 0.8125f;
			p3.y += 0.8125f;
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
				spec = this->getTex(face_dir::minus_x, value);
				light = chunk->computeLight(pos.x - 1, pos.y, pos.z) + (3 << 8);
				utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, light, 3, 16);
			}
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				spec = this->getTex(face_dir::plus_x, value);
				light = chunk->computeLight(pos.x + 1, pos.y, pos.z) + (4 << 8);
				utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, light, 3, 16);
			}
			spec = this->getTex(face_dir::minus_y, value);
			light = chunk->computeLight(pos.x, pos.y, pos.z) + (1 << 8);
			utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, light, 16, 16, !flip);
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				spec = this->getTex(face_dir::plus_y, value);
				light = chunk->computeLight(pos.x, pos.y + 1, pos.z) + (2 << 8);
				utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, light, 16, 16, flip);
			}
			if ((bitfield & door::upper_half) && utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				spec = this->getTex(face_dir::plus_z);
				light = chunk->computeLight(pos.x, pos.y, pos.z + 1) + (0 << 8);
				utils::shader::addQuads(vertices, {p1, p0, p5, p4}, spec, light, 16, 3);
			}
			if (!(bitfield & door::upper_half) && utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				spec = this->getTex(face_dir::minus_z, face_dir::minus_z);
				light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
				utils::shader::addQuads(vertices, {p7, p6, p3, p2}, spec + (13 << 16), light, 16, 3);
			}
			break ;
		case face_dir::plus_y:
			p4.y -= 0.8125f;
			p5.y -= 0.8125f;
			p6.y -= 0.8125f;
			p7.y -= 0.8125f;
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
				spec = this->getTex(face_dir::minus_x, value);
				light = chunk->computeLight(pos.x - 1, pos.y, pos.z) + (3 << 8);
				utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, light, 3, 16);
			}
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				spec = this->getTex(face_dir::plus_x, value);
				light = chunk->computeLight(pos.x + 1, pos.y, pos.z) + (4 << 8);
				utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, light, 3, 16);
			}
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				spec = this->getTex(face_dir::minus_y, value);
				light = chunk->computeLight(pos.x, pos.y - 1, pos.z) + (1 << 8);
				utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, light, 16, 16, !flip);
			}
			spec = this->getTex(face_dir::plus_y, value);
			light = chunk->computeLight(pos.x, pos.y, pos.z) + (2 << 8);
			utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, light, 16, 16, flip);
			if ((bitfield & door::upper_half) && utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				spec = this->getTex(face_dir::plus_z);
				light = chunk->computeLight(pos.x, pos.y, pos.z + 1) + (0 << 8);
				utils::shader::addQuads(vertices, {p1, p0, p5, p4}, spec, light, 16, 3);
			}
			if (!(bitfield & door::upper_half) && utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				spec = this->getTex(face_dir::minus_z, face_dir::minus_z);
				light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
				utils::shader::addQuads(vertices, {p7, p6, p3, p2}, spec + (13 << 16), light, 16, 3);
			}
			break ;
	}
}

void Trapdoor::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int bitfield = value >> offset::blocks::bitfield;
	int spec = texture;
	int light;
	bool open = !!(bitfield & door::open) ^ ((value >> offset::redstone::powered) & 0x1);
	if (!open) {
		if (!(bitfield & door::upper_half)) {
			p0.z -= 0.8125f;
			p1.z -= 0.8125f;
			p4.z -= 0.8125f;
			p5.z -= 0.8125f;
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
				light = chunk->computeLight(pos.x - 1, pos.y, pos.z) + (3 << 8);
				utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, light, 16, 3);
			}
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				light = chunk->computeLight(pos.x + 1, pos.y, pos.z) + (4 << 8);
				utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, light, 16, 3);
			}
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				light = chunk->computeLight(pos.x, pos.y - 1, pos.z) + (1 << 8);
				utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, light, 16, 3);
			}
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				light = chunk->computeLight(pos.x, pos.y + 1, pos.z) + (2 << 8);
				utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, light, 16, 3);
			}
			light = chunk->computeLight(pos.x, pos.y, pos.z) + (0 << 8);
			utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, light, 16, 16);
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
				utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, light, 16, 16);
			}
		} else {
			p2.z += 0.8125f;
			p3.z += 0.8125f;
			p6.z += 0.8125f;
			p7.z += 0.8125f;
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
				light = chunk->computeLight(pos.x - 1, pos.y, pos.z) + (3 << 8);
				utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, light, 16, 3, false, true);
			}
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				light = chunk->computeLight(pos.x + 1, pos.y, pos.z) + (4 << 8);
				utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, light, 16, 3, false, true);
			}
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				light = chunk->computeLight(pos.x, pos.y - 1, pos.z) + (1 << 8);
				utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, light, 16, 3, false, true);
			}
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				light = chunk->computeLight(pos.x, pos.y + 1, pos.z) + (2 << 8);
				utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, light, 16, 3, false, true);
			}
			if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				light = chunk->computeLight(pos.x, pos.y, pos.z + 1) + (0 << 8);
				utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, light, 16, 16);
			}
			light = chunk->computeLight(pos.x, pos.y, pos.z) + (5 << 8);
			utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, light, 16, 16);
		}
	} else { // open trapdoor
		switch ((value >> offset::blocks::orientation) & 0x7) {
			case face_dir::minus_x:
				p0 += glm::vec3(0.8125f, 0, 0);
				p2 += glm::vec3(0.8125f, 0, 0);
				p4 += glm::vec3(0.8125f, 0, 0);
				p6 += glm::vec3(0.8125f, 0, 0);
				light = chunk->computeLight(pos.x, pos.y, pos.z) + (3 << 8);
				utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, light, 16, 16);
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
					light = chunk->computeLight(pos.x + 1, pos.y, pos.z) + (4 << 8);
					utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, light, 16, 16);
				}
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
					light = chunk->computeLight(pos.x, pos.y - 1, pos.z) + (1 << 8);
					utils::shader::addQuads(vertices, {p2, p0, p3, p1}, spec, light, 16, 3, true, true);
				}
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
					light = chunk->computeLight(pos.x, pos.y + 1, pos.z) + (2 << 8);
					utils::shader::addQuads(vertices, {p7, p5, p6, p4}, spec, light, 16, 3);
				}
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
					light = chunk->computeLight(pos.x, pos.y, pos.z + 1) + (0 << 8);
					utils::shader::addQuads(vertices, {p0, p4, p1, p5}, spec, light, 16, 3, true, true);
				}
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
					light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
					utils::shader::addQuads(vertices, {p6, p2, p7, p3}, spec, light, 16, 3, true, true);
				}
				break ;
			case face_dir::plus_x:
				p1 -= glm::vec3(0.8125f, 0, 0);
				p3 -= glm::vec3(0.8125f, 0, 0);
				p5 -= glm::vec3(0.8125f, 0, 0);
				p7 -= glm::vec3(0.8125f, 0, 0);
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
					light = chunk->computeLight(pos.x - 1, pos.y, pos.z) + (3 << 8);
					utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, light, 16, 16);
				}
				light = chunk->computeLight(pos.x, pos.y, pos.z) + (4 << 8);
				utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, light, 16, 16);
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
					light = chunk->computeLight(pos.x, pos.y - 1, pos.z) + (1 << 8);
					utils::shader::addQuads(vertices, {p2, p0, p3, p1}, spec, light, 16, 3);
				}
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
					light = chunk->computeLight(pos.x, pos.y + 1, pos.z) + (2 << 8);
					utils::shader::addQuads(vertices, {p7, p5, p6, p4}, spec, light, 16, 3, true, true);
				}
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
					light = chunk->computeLight(pos.x, pos.y, pos.z + 1) + (0 << 8);
					utils::shader::addQuads(vertices, {p0, p4, p1, p5}, spec, light, 16, 3);
				}
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
					light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
					utils::shader::addQuads(vertices, {p6, p2, p7, p3}, spec, light, 16, 3);
				}
				break ;
			case face_dir::minus_y:
				p0 += glm::vec3(0, 0.8125f, 0);
				p1 += glm::vec3(0, 0.8125f, 0);
				p2 += glm::vec3(0, 0.8125f, 0);
				p3 += glm::vec3(0, 0.8125f, 0);
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
					light = chunk->computeLight(pos.x - 1, pos.y, pos.z) + (3 << 8);
					utils::shader::addQuads(vertices, {p6, p4, p2, p0}, spec, light, 16, 3);
				}
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
					light = chunk->computeLight(pos.x + 1, pos.y, pos.z) + (4 << 8);
					utils::shader::addQuads(vertices, {p3, p1, p7, p5}, spec, light, 16, 3, true, true);
				}
				light = chunk->computeLight(pos.x, pos.y, pos.z) + (1 << 8);
				utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, light, 16, 16);
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
					light = chunk->computeLight(pos.x, pos.y + 1, pos.z) + (2 << 8);
					utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, light, 16, 16);
				}
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
					light = chunk->computeLight(pos.x, pos.y, pos.z + 1) + (0 << 8);
					utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, light, 16, 3);
				}
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
					light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
					utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, light, 16, 3, true, true);
				}
				break ;
			case face_dir::plus_y:
				p4 -= glm::vec3(0, 0.8125f, 0);
				p5 -= glm::vec3(0, 0.8125f, 0);
				p6 -= glm::vec3(0, 0.8125f, 0);
				p7 -= glm::vec3(0, 0.8125f, 0);
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
					light = chunk->computeLight(pos.x - 1, pos.y, pos.z) + (3 << 8);
					utils::shader::addQuads(vertices, {p6, p4, p2, p0}, spec, light, 16, 3, true, true);
				}
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
					light = chunk->computeLight(pos.x + 1, pos.y, pos.z) + (4 << 8);
					utils::shader::addQuads(vertices, {p3, p1, p7, p5}, spec, light, 16, 3);
				}
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
					light = chunk->computeLight(pos.x, pos.y - 1, pos.z) + (1 << 8);
					utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, light, 16, 16);
				}
				light = chunk->computeLight(pos.x, pos.y, pos.z) + (2 << 8);
				utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, light, 16, 16);
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
					light = chunk->computeLight(pos.x, pos.y, pos.z + 1) + (0 << 8);
					utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, light, 16, 3, true, true);
				}
				if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
					light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
					utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, light, 16, 3);
				}
				break ;
		}
	}
}

void Fence::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	glm::vec3 pp0 = p0 + glm::vec3( 0.375f,  0.375f, 0);
	glm::vec3 pp1 = p1 + glm::vec3(-0.375f,  0.375f, 0);
	glm::vec3 pp2 = p2 + glm::vec3( 0.375f,  0.375f, 0);
	glm::vec3 pp3 = p3 + glm::vec3(-0.375f,  0.375f, 0);
	glm::vec3 pp4 = p4 + glm::vec3( 0.375f, -0.375f, 0);
	glm::vec3 pp5 = p5 + glm::vec3(-0.375f, -0.375f, 0);
	glm::vec3 pp6 = p6 + glm::vec3( 0.375f, -0.375f, 0);
	glm::vec3 pp7 = p7 + glm::vec3(-0.375f, -0.375f, 0);

	// central pole
	int spec = texture;
	int baseLight = chunk->computeLight(pos.x, pos.y, pos.z);
	int light = baseLight + (3 << 8);
	utils::shader::addQuads(vertices, {pp4, pp0, pp6, pp2}, spec + (6 << 12), light, 4, 16);
	light = baseLight + (4 << 8);
	utils::shader::addQuads(vertices, {pp1, pp5, pp3, pp7}, spec + (6 << 12), light, 4, 16);
	light = baseLight + (1 << 8);
	utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + (6 << 12), light, 4, 16);
	light = baseLight + (2 << 8);
	utils::shader::addQuads(vertices, {pp5, pp4, pp7, pp6}, spec + (6 << 12), light, 4, 16);
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		light = chunk->computeLight(pos.x, pos.y, pos.z + 1) + (0 << 8);
		utils::shader::addQuads(vertices, {pp4, pp5, pp0, pp1}, spec + (6 << 12) + (6 << 16), light, 4, 4);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
		utils::shader::addQuads(vertices, {pp2, pp3, pp6, pp7}, spec + (6 << 12) + (6 << 16), light, 4, 4);
	}
	// arms
	baseLight = chunk->computeLight(pos.x, pos.y, pos.z);
	if ((value >> offset::blocks::bitfield) & fence::mx) {
		pp0 = p0 + glm::vec3( 0,                7.0f * one16th, -one16th);
		pp1 = p1 + glm::vec3(-10.0f * one16th,  7.0f * one16th, -one16th);
		pp2 = p2 + glm::vec3( 0,                7.0f * one16th, 12.0f * one16th);
		pp3 = p3 + glm::vec3(-10.0f * one16th,  7.0f * one16th, 12.0f * one16th);
		pp4 = p4 + glm::vec3( 0,               -7.0f * one16th, -one16th);
		pp5 = p5 + glm::vec3(-10.0f * one16th, -7.0f * one16th, -one16th);
		pp6 = p6 + glm::vec3( 0,               -7.0f * one16th, 12.0f * one16th);
		pp7 = p7 + glm::vec3(-10.0f * one16th, -7.0f * one16th, 12.0f * one16th);
		for (int i = 0; i < 2; ++i) {
			light = baseLight + (1 << 8);
			utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + ((1 + 6*i) << 16), light, 6, 3);
			light = baseLight + (2 << 8);
			utils::shader::addQuads(vertices, {pp5, pp4, pp7, pp6}, spec + (10 << 12) + ((1 + 6*i) << 16), light, 6, 3);
			light = baseLight + (0 << 8);
			utils::shader::addQuads(vertices, {pp4, pp5, pp0, pp1}, spec + (7 << 16), light, 6, 2);
			light = baseLight + (5 << 8);
			utils::shader::addQuads(vertices, {pp2, pp3, pp6, pp7}, spec + (7 << 16), light, 6, 2);
			// bottom arm is drawn on second loop
			pp0.z -= 6.0f * one16th;
			pp1.z -= 6.0f * one16th;
			pp2.z -= 6.0f * one16th;
			pp3.z -= 6.0f * one16th;
			pp4.z -= 6.0f * one16th;
			pp5.z -= 6.0f * one16th;
			pp6.z -= 6.0f * one16th;
			pp7.z -= 6.0f * one16th;
		}
	}
	if ((value >> offset::blocks::bitfield) & fence::px) {
		pp0 = p0 + glm::vec3( 10.0f * one16th,  7.0f * one16th, -one16th);
		pp1 = p1 + glm::vec3( 0,                7.0f * one16th, -one16th);
		pp2 = p2 + glm::vec3( 10.0f * one16th,  7.0f * one16th, 12.0f * one16th);
		pp3 = p3 + glm::vec3( 0,                7.0f * one16th, 12.0f * one16th);
		pp4 = p4 + glm::vec3( 10.0f * one16th, -7.0f * one16th, -one16th);
		pp5 = p5 + glm::vec3( 0,               -7.0f * one16th, -one16th);
		pp6 = p6 + glm::vec3( 10.0f * one16th, -7.0f * one16th, 12.0f * one16th);
		pp7 = p7 + glm::vec3( 0,               -7.0f * one16th, 12.0f * one16th);
		for (int i = 0; i < 2; ++i) {
			light = baseLight + (1 << 8);
			utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + (10 << 12) + ((1 + 6*i) << 16), light, 6, 3);
			light = baseLight + (2 << 8);
			utils::shader::addQuads(vertices, {pp5, pp4, pp7, pp6}, spec + ((1 + 6*i) << 16), light, 6, 3);
			light = baseLight + (0 << 8);
			utils::shader::addQuads(vertices, {pp4, pp5, pp0, pp1}, spec + (7 << 16), light, 6, 2);
			light = baseLight + (5 << 8);
			utils::shader::addQuads(vertices, {pp2, pp3, pp6, pp7}, spec + (7 << 16), light, 6, 2);
			// bottom arm is drawn on second loop
			pp0.z -= 6.0f * one16th;
			pp1.z -= 6.0f * one16th;
			pp2.z -= 6.0f * one16th;
			pp3.z -= 6.0f * one16th;
			pp4.z -= 6.0f * one16th;
			pp5.z -= 6.0f * one16th;
			pp6.z -= 6.0f * one16th;
			pp7.z -= 6.0f * one16th;
		}
	}
	if ((value >> offset::blocks::bitfield) & fence::my) {
		pp0 = p0 + glm::vec3( 7.0f * one16th,  0,               -one16th);
		pp1 = p1 + glm::vec3(-7.0f * one16th,  0,               -one16th);
		pp2 = p2 + glm::vec3( 7.0f * one16th,  0,               12.0f * one16th);
		pp3 = p3 + glm::vec3(-7.0f * one16th,  0,               12.0f * one16th);
		pp4 = p4 + glm::vec3( 7.0f * one16th, -10.0f * one16th, -one16th);
		pp5 = p5 + glm::vec3(-7.0f * one16th, -10.0f * one16th, -one16th);
		pp6 = p6 + glm::vec3( 7.0f * one16th, -10.0f * one16th, 12.0f * one16th);
		pp7 = p7 + glm::vec3(-7.0f * one16th, -10.0f * one16th, 12.0f * one16th);
		for (int i = 0; i < 2; ++i) {
			light = baseLight + (3 << 8);
			utils::shader::addQuads(vertices, {pp4, pp0, pp6, pp2}, spec + (10 << 12) + ((1 + 6*i) << 16), light, 6, 3);
			light = baseLight + (4 << 8);
			utils::shader::addQuads(vertices, {pp1, pp5, pp3, pp7}, spec + ((1 + 6*i) << 16), light, 6, 3);
			light = baseLight + (0 << 8);
			utils::shader::addQuads(vertices, {pp5, pp1, pp4, pp0}, spec + (7 << 16), light, 6, 2);
			light = baseLight + (5 << 8);
			utils::shader::addQuads(vertices, {pp3, pp7, pp2, pp6}, spec + (7 << 16), light, 6, 2);
			// bottom arm is drawn on second loop
			pp0.z -= 6.0f * one16th;
			pp1.z -= 6.0f * one16th;
			pp2.z -= 6.0f * one16th;
			pp3.z -= 6.0f * one16th;
			pp4.z -= 6.0f * one16th;
			pp5.z -= 6.0f * one16th;
			pp6.z -= 6.0f * one16th;
			pp7.z -= 6.0f * one16th;
		}
	}
	if ((value >> offset::blocks::bitfield) & fence::py) {
		pp0 = p0 + glm::vec3( 7.0f * one16th, 10.0f * one16th, -one16th);
		pp1 = p1 + glm::vec3(-7.0f * one16th, 10.0f * one16th, -one16th);
		pp2 = p2 + glm::vec3( 7.0f * one16th, 10.0f * one16th, 12.0f * one16th);
		pp3 = p3 + glm::vec3(-7.0f * one16th, 10.0f * one16th, 12.0f * one16th);
		pp4 = p4 + glm::vec3( 7.0f * one16th, 0,               -one16th);
		pp5 = p5 + glm::vec3(-7.0f * one16th, 0,               -one16th);
		pp6 = p6 + glm::vec3( 7.0f * one16th, 0,               12.0f * one16th);
		pp7 = p7 + glm::vec3(-7.0f * one16th, 0,               12.0f * one16th);
		for (int i = 0; i < 2; ++i) {
			light = baseLight + (3 << 8);
			utils::shader::addQuads(vertices, {pp4, pp0, pp6, pp2}, spec + ((1 + 6*i) << 16), light, 6, 3);
			light = baseLight + (4 << 8);
			utils::shader::addQuads(vertices, {pp1, pp5, pp3, pp7}, spec + (10 << 12) + ((1 + 6*i) << 16), light, 6, 3);
			light = baseLight + (0 << 8);
			utils::shader::addQuads(vertices, {pp5, pp1, pp4, pp0}, spec + (7 << 16), light, 6, 2);
			light = baseLight + (5 << 8);
			utils::shader::addQuads(vertices, {pp3, pp7, pp2, pp6}, spec + (7 << 16), light, 6, 2);
			// bottom arm is drawn on second loop
			pp0.z -= 6.0f * one16th;
			pp1.z -= 6.0f * one16th;
			pp2.z -= 6.0f * one16th;
			pp3.z -= 6.0f * one16th;
			pp4.z -= 6.0f * one16th;
			pp5.z -= 6.0f * one16th;
			pp6.z -= 6.0f * one16th;
			pp7.z -= 6.0f * one16th;
		}
	}
}

void GlassPane::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	(void)chunk;(void)vertices;(void)start;(void)pos;(void)value;
	// TODO move this to sky_vertex
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int bitfield = (value >> offset::blocks::bitfield);
	int spec = settings::consts::shader::block::glass;
	int light = chunk->computeLight(pos.x, pos.y, pos.z);
	if (bitfield & fence::mx) {
		utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.4375f, 0.f), p1 + glm::vec3(-.5f, 0.4375f, 0.f), p2 + glm::vec3(0.f, 0.4375f, 0.f), p3 + glm::vec3(-.5f, 0.4375f, 0.f)}, spec, light + (1 << 8), 8, 16);
		utils::shader::addQuads(vertices, {p5 + glm::vec3(-.5f, -0.4375f, 0.f), p4 + glm::vec3(0.f, -0.4375f, 0.f), p7 + glm::vec3(-.5f, -0.4375f, 0.f), p6 + glm::vec3(0.f, -0.4375f, 0.f)}, spec, light + (2 << 8), 8, 16, true);
	} else {
		int sspec = (bitfield & (fence::my | fence::py)) ? spec : settings::consts::shader::block::glass_pane_top;
		utils::shader::addQuads(vertices, {p4 + glm::vec3(0.4375f, -.4375f, 0.f), p0 + glm::vec3(0.4375f, .4375f, 0.f), p6 + glm::vec3(0.4375f, -.4375f, 0.f), p2 + glm::vec3(0.4375f, .4375f, 0.f)}, sspec + (7 << 12), light + (3 << 8), 2, 16);
	}
	if (bitfield & fence::px) {
		utils::shader::addQuads(vertices, {p0 + glm::vec3(.5f, 0.4375f, 0.f), p1 + glm::vec3(0.f, 0.4375f, 0.f), p2 + glm::vec3(.5f, 0.4375f, 0.f), p3 + glm::vec3(0.f, 0.4375f, 0.f)}, spec + (8 << 12), light + (1 << 8), 8, 16);
		utils::shader::addQuads(vertices, {p5 + glm::vec3(0.f, -0.4375f, 0.f), p4 + glm::vec3(.5f, -0.4375f, 0.f), p7 + glm::vec3(0.f, -0.4375f, 0.f), p6 + glm::vec3(.5f, -0.4375f, 0.f)}, spec + (8 << 12), light + (2 << 8), 8, 16, true);
	} else {
		int sspec = (bitfield & (fence::my | fence::py)) ? spec : settings::consts::shader::block::glass_pane_top;
		utils::shader::addQuads(vertices, {p1 + glm::vec3(-0.4375f, .4375f, 0.f), p5 + glm::vec3(-0.4375f, -.4375f, 0.f), p3 + glm::vec3(-0.4375f, .4375f, 0.f), p7 + glm::vec3(-0.4375f, -.4375f, 0.f)}, sspec + (7 << 12), light + (4 << 8), 2, 16);
	}
	if (bitfield & fence::my) {
		utils::shader::addQuads(vertices, {p4 + glm::vec3(0.4375f, -.5f, 0.f), p0 + glm::vec3(0.4375f, 0.f, 0.f), p6 + glm::vec3(0.4375f, -.5f, 0.f), p2 + glm::vec3(0.4375f, 0.f, 0.f)}, spec, light + (3 << 8), 8, 16, true);
		utils::shader::addQuads(vertices, {p1 + glm::vec3(-0.4375f, 0.f, 0.f), p5 + glm::vec3(-0.4375f, -.5f, 0.f), p3 + glm::vec3(-0.4375f, 0.f, 0.f), p7 + glm::vec3(-0.4375f, -.5f, 0.f)}, spec, light + (4 << 8), 8, 16);
	} else {
		int sspec = (bitfield & (fence::mx | fence::px)) ? spec : settings::consts::shader::block::glass_pane_top;
		utils::shader::addQuads(vertices, {p0 + glm::vec3(.4375f, 0.4375f, 0.f), p1 + glm::vec3(-.4375f, 0.4375f, 0.f), p2 + glm::vec3(.4375f, 0.4375f, 0.f), p3 + glm::vec3(-.4375f, 0.4375f, 0.f)}, sspec + (7 << 12), light + (1 << 8), 2, 16);
	}
	if (bitfield & fence::py) {
		utils::shader::addQuads(vertices, {p4 + glm::vec3(0.4375f, 0.f, 0.f), p0 + glm::vec3(0.4375f, .5f, 0.f), p6 + glm::vec3(0.4375f, 0.f, 0.f), p2 + glm::vec3(0.4375f, .5f, 0.f)}, spec + (8 << 12), light + (3 << 8), 8, 16, true);
		utils::shader::addQuads(vertices, {p1 + glm::vec3(-0.4375f, .5f, 0.f), p5 + glm::vec3(-0.4375f, 0.f, 0.f), p3 + glm::vec3(-0.4375f, .5f, 0.f), p7 + glm::vec3(-0.4375f, 0.f, 0.f)}, spec + (8 << 12), light + (4 << 8), 8, 16);
	} else {
		int sspec = (bitfield & (fence::mx | fence::px)) ? spec : settings::consts::shader::block::glass_pane_top;
		utils::shader::addQuads(vertices, {p5 + glm::vec3(-.4375f, -0.4375f, 0.f), p4 + glm::vec3(.4375f, -0.4375f, 0.f), p7 + glm::vec3(-.4375f, -0.4375f, 0.f), p6 + glm::vec3(.4375f, -0.4375f, 0.f)}, sspec + (7 << 12), light + (2 << 8), 2, 16);
	}
	spec = settings::consts::shader::block::glass_pane_top;
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		light = chunk->computeLight(pos.x, pos.y, pos.z + 1) + (0 << 8);
		if ((bitfield & fence::py) && (bitfield & fence::my)) {
			utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, light, 16, 16);
		} else if (bitfield & fence::py) {
			utils::shader::addQuads(vertices, {p4, p5, p0 + glm::vec3(0.f, 0.4375f, 0.f), p1 + glm::vec3(0.f, 0.4375f, 0.f)}, spec, light, 16, 9);
		} else if (bitfield & fence::my) {
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.f, -0.4375f, 0.f), p5 + glm::vec3(0.f, -0.4375f, 0.f), p0, p1}, spec + (7 << 16), light, 16, 9);
		} else {
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.f, -0.4375f, 0.f), p5 + glm::vec3(0.f, -0.4375f, 0.f), p0 + glm::vec3(0.f, 0.4375f, 0.f), p1 + glm::vec3(0.f, 0.4375f, 0.f)}, spec + (7 << 16), light, 16, 2);
		}
		if (bitfield & fence::mx) {
			utils::shader::addQuads(vertices, {p0, p4, p0 + glm::vec3(0.4375f, 0.f, 0.f), p4 + glm::vec3(0.4375f, 0.f, 0.f)}, spec, light, 16, 7);
		}
		if (bitfield & fence::px) {
			utils::shader::addQuads(vertices, {p5, p1, p5 + glm::vec3(-0.4375f, 0.f, 0.f), p1 + glm::vec3(-0.4375f, 0.f, 0.f)}, spec, light, 16, 7);
		}
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
		if ((bitfield & fence::my) && (bitfield & fence::py)) {
			utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, light, 16, 16);
		} else if (bitfield & fence::my) {
			utils::shader::addQuads(vertices, {p2, p3, p6 + glm::vec3(0.f, -0.4375f, 0.f), p7 + glm::vec3(0.f, -0.4375f, 0.f)}, spec, light, 16, 9);
		} else if (bitfield & fence::py) {
			utils::shader::addQuads(vertices, {p2 + glm::vec3(0.f, 0.4375f, 0.f), p3 + glm::vec3(0.f, 0.4375f, 0.f), p6, p7}, spec + (7 << 16), light, 16, 9);
		} else {
			utils::shader::addQuads(vertices, {p2 + glm::vec3(0.f, 0.4375f, 0.f), p3 + glm::vec3(0.f, 0.4375f, 0.f), p6 + glm::vec3(0.f, -0.4375f, 0.f), p7 + glm::vec3(0.f, -0.4375f, 0.f)}, spec + (7 << 16), light, 16, 2);
		}
		if (bitfield & fence::mx) {
			utils::shader::addQuads(vertices, {p6, p2, p6 + glm::vec3(0.4375f, 0.f, 0.f), p2 + glm::vec3(0.4375f, 0.f, 0.f)}, spec, light, 16, 7);
		}
		if (bitfield & fence::px) {
			utils::shader::addQuads(vertices, {p7, p3, p7 + glm::vec3(-0.4375f, 0.f, 0.f), p3 + glm::vec3(-0.4375f, 0.f, 0.f)}, spec, light, 16, 7);
		}
	}
}

void Crop::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	(void)value;
	glm::vec3 p0 = {start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1 - one16th};
	glm::vec3 p1 = {start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1 - one16th};
	glm::vec3 p2 = {start.x + pos.x + 0, start.y + pos.y + 0, pos.z - one16th};
	glm::vec3 p3 = {start.x + pos.x + 1, start.y + pos.y + 0, pos.z - one16th};

	glm::vec3 p4 = {start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1 - one16th};
	glm::vec3 p5 = {start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1 - one16th};
	glm::vec3 p6 = {start.x + pos.x + 0, start.y + pos.y + 1, pos.z - one16th};
	glm::vec3 p7 = {start.x + pos.x + 1, start.y + pos.y + 1, pos.z - one16th};

	int spec = getTex(face_dir::plus_z, value);
	int light = chunk->computeLight(pos.x, pos.y, pos.z);
	utils::shader::addQuads(vertices, {p4 + glm::vec3(3.0f * one16th, 0.0f, 0.0f), {p0.x + 3.0f * one16th, p0.y, p0.z}, {p6.x + 3.0f * one16th, p6.y, p6.z}, {p2.x + 3.0f * one16th, p2.y, p2.z}}, spec, light, 16, 16); // -x
	utils::shader::addQuads(vertices, {p0 + glm::vec3(13.0f * one16th, 0.0f, 0.0f), {p4.x + 13.0f * one16th, p4.y, p4.z}, {p2.x + 13.0f * one16th, p2.y, p2.z}, {p6.x + 13.0f * one16th, p6.y, p6.z}}, spec, light, 16, 16); // +x
	utils::shader::addQuads(vertices, {p0 + glm::vec3(0.0f, 3.0f * one16th, 0.0f), {p1.x, p1.y + 3.0f * one16th, p1.z}, {p2.x, p2.y + 3.0f * one16th, p2.z}, {p3.x, p3.y + 3.0f * one16th, p3.z}}, spec, light, 16, 16); // -y
	utils::shader::addQuads(vertices, {p1 + glm::vec3(0.0f, 13.0f * one16th, 0.0f), {p0.x, p0.y + 13.0f * one16th, p0.z}, {p3.x, p3.y + 13.0f * one16th, p3.z}, {p2.x, p2.y + 13.0f * one16th, p2.z}}, spec, light, 16, 16); // +y
}

void ItemFrame::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	if (value & mask::frame::notVisible) {
		return ;
	}

	glm::vec3 front, right, up;
	pos += glm::vec3(start.x + .5f, start.y + .5f, .5f);

	int placement = (value >> offset::blocks::bitfield) & 0x3;
	int orientation = (value >> offset::blocks::orientation) & 0x7;
	switch (placement) {
		case placement::wall:
			up = settings::consts::math::world_up;
			switch (orientation) {
				case face_dir::minus_x:
					front = { 1.f, .0f, .0f};
					right = { .0f,-1.f, .0f};
					break ;
				case face_dir::plus_x:
					front = {-1.f, .0f, .0f};
					right = { .0f, 1.f, .0f};
					break ;
				case face_dir::minus_y:
					front = { 0.f, 1.f, .0f};
					right = { 1.f, .0f, .0f};
					break ;
				case face_dir::plus_y:
					front = { .0f, -1.f, .0f};
					right = { -1.f, .0f, .0f};
					break ;
			}
			break ;
		case placement::floor:
			front = { .0f, .0f, -1.f};
			right = { 1.f, .0f, .0f};
			up    = { .0f, 1.f, .0f};
			break ;
		case placement::ceiling:
			front = { .0f, .0f, 1.f};
			right = { 1.f, .0f, .0f};
			up    = { .0f,-1.f, .0f};
			break ;
	}
	int faceLight = chunk->computeLight(pos.x - start.x, pos.y - start.y, pos.z);
	pos += front * 7.f * one16th - (right + up) * .5f;
	addMeshItem(vertices, faceLight, pos, front, right, up, 1.f);
}

void Lectern::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 front, right;
	pos += glm::vec3(start.x + .5f, start.y + .5f, 0.f);

	int orientation = (value >> offset::blocks::orientation) & 0x7;
	switch (orientation) {
		case face_dir::minus_x:
			front = { 1.f, .0f, .0f};
			right = { .0f,-1.f, .0f};
			break ;
		case face_dir::plus_x:
			front = {-1.f, .0f, .0f};
			right = { .0f, 1.f, .0f};
			break ;
		case face_dir::minus_y:
			front = { 0.f, 1.f, .0f};
			right = { 1.f, .0f, .0f};
			break ;
		case face_dir::plus_y:
			front = { .0f, -1.f, .0f};
			right = { -1.f, .0f, .0f};
			break ;
	}
	pos -= (front + right) * .5f;
	int faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	addMeshItem(vertices, faceLight, pos, front, right, settings::consts::math::world_up, 1.f);
}

void Anvil::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 front, right;
	pos += glm::vec3(start.x + .5f, start.y + .5f, 0.f);

	int orientation = (value >> offset::blocks::orientation) & 0x7;
	switch (orientation) {
		case face_dir::minus_x:
		case face_dir::plus_x:
			front = { 1.f, .0f, .0f};
			right = { .0f,-1.f, .0f};
			break ;
		case face_dir::minus_y:
		case face_dir::plus_y:
			front = { 0.f, 1.f, .0f};
			right = { 1.f, .0f, .0f};
			break ;
	}
	pos -= (front + right) * .5f;
	int faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	addMeshItem(vertices, faceLight, pos, front, right, settings::consts::math::world_up, 1.f);
}

void Lever::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int spec, x_texMX, x_texPX, x_texMY, x_texPY, y_texMX, y_texPX, y_texMY, y_texPY, z_texMX, z_texPX, z_texMY,  z_texPY, ztop, zbot;
	int placement = (value >> offset::blocks::bitfield) & 0x3;
	int orientation = (value >> offset::blocks::orientation) & 0x7;
	switch (placement) {
		case placement::wall:
			switch (orientation) {
				case face_dir::minus_x:
					p0 += glm::vec3( 13.0f * one16th,  5.0f * one16th, -4.0f * one16th);
					p1 += glm::vec3( 0,                5.0f * one16th, -4.0f * one16th);
					p2 += glm::vec3( 13.0f * one16th,  5.0f * one16th,  4.0f * one16th);
					p3 += glm::vec3( 0,                5.0f * one16th,  4.0f * one16th);

					p4 += glm::vec3( 13.0f * one16th, -5.0f * one16th, -4.0f * one16th);
					p5 += glm::vec3( 0,               -5.0f * one16th, -4.0f * one16th);
					p6 += glm::vec3( 13.0f * one16th, -5.0f * one16th,  4.0f * one16th);
					p7 += glm::vec3( 0,               -5.0f * one16th,  4.0f * one16th);

					x_texMX = 5; x_texPX = 5;  x_texMY = 4; x_texPY = 4;
					y_texMX = 0; y_texPX = 13; y_texMY = 4; y_texPY = 4;
					z_texMX = 13; z_texPX = 0;  z_texMY = 5;  z_texPY = 5;
					break ;
				case face_dir::plus_x:
					p0 += glm::vec3( 0,               5.0f * one16th, -4.0f * one16th);
					p1 += glm::vec3(-13.0f * one16th, 5.0f * one16th, -4.0f * one16th);
					p2 += glm::vec3( 0,               5.0f * one16th,  4.0f * one16th);
					p3 += glm::vec3(-13.0f * one16th, 5.0f * one16th,  4.0f * one16th);

					p4 += glm::vec3( 0,              -5.0f * one16th, -4.0f * one16th);
					p5 += glm::vec3(-13.0f * one16th,-5.0f * one16th, -4.0f * one16th);
					p6 += glm::vec3( 0,              -5.0f * one16th,  4.0f * one16th);
					p7 += glm::vec3(-13.0f * one16th,-5.0f * one16th,  4.0f * one16th);

					x_texMX = 5; x_texPX = 5;  x_texMY = 4; x_texPY = 4;
					y_texMX = 0; y_texPX = 13; y_texMY = 4; y_texPY = 4;
					z_texMX = 0; z_texPX = 13;  z_texMY = 5;  z_texPY = 5;
					break ;
				case face_dir::minus_y:
					p0 += glm::vec3( 5.0f * one16th, 13.0f * one16th, -4.0f * one16th);
					p1 += glm::vec3(-5.0f * one16th, 13.0f * one16th, -4.0f * one16th);
					p2 += glm::vec3( 5.0f * one16th, 13.0f * one16th,  4.0f * one16th);
					p3 += glm::vec3(-5.0f * one16th, 13.0f * one16th,  4.0f * one16th);

					p4 += glm::vec3( 5.0f * one16th, 0,               -4.0f * one16th);
					p5 += glm::vec3(-5.0f * one16th, 0,               -4.0f * one16th);
					p6 += glm::vec3( 5.0f * one16th, 0,                4.0f * one16th);
					p7 += glm::vec3(-5.0f * one16th, 0,                4.0f * one16th);

					x_texMX = 0; x_texPX = 13; x_texMY = 4; x_texPY = 4;
					y_texMX = 5; y_texPX = 5;  y_texMY = 4; y_texPY = 4;
					z_texMX = 5; z_texPX = 5;  z_texMY = 13;  z_texPY = 0;
					break ;
				case face_dir::plus_y:
					p0 += glm::vec3( 5.0f * one16th, 0,               -4.0f * one16th);
					p1 += glm::vec3(-5.0f * one16th, 0,               -4.0f * one16th);
					p2 += glm::vec3( 5.0f * one16th, 0,                4.0f * one16th);
					p3 += glm::vec3(-5.0f * one16th, 0,                4.0f * one16th);

					p4 += glm::vec3( 5.0f * one16th, -13.0f * one16th, -4.0f * one16th);
					p5 += glm::vec3(-5.0f * one16th, -13.0f * one16th, -4.0f * one16th);
					p6 += glm::vec3( 5.0f * one16th, -13.0f * one16th,  4.0f * one16th);
					p7 += glm::vec3(-5.0f * one16th, -13.0f * one16th,  4.0f * one16th);

					x_texMX = 0; x_texPX = 13; x_texMY = 4; x_texPY = 4;
					y_texMX = 5; y_texPX = 5;  y_texMY = 4; y_texPY = 4;
					z_texMX = 5; z_texPX = 5;  z_texMY = 0;  z_texPY = 13;
					break ;
			}
			break ;
		case placement::floor:
		case placement::ceiling:
			ztop = (placement == placement::floor) ? 13 : 0;
			zbot = (placement == placement::floor) ? 0 : 13;
			switch (orientation) {
				case face_dir::minus_x:
				case face_dir::plus_x:
					p0 += glm::vec3( 4.0f * one16th, 5.0f * one16th, -ztop * one16th);
					p1 += glm::vec3(-4.0f * one16th, 5.0f * one16th, -ztop * one16th);
					p2 += glm::vec3( 4.0f * one16th, 5.0f * one16th,  zbot * one16th);
					p3 += glm::vec3(-4.0f * one16th, 5.0f * one16th,  zbot * one16th);

					p4 += glm::vec3( 4.0f * one16th,-5.0f * one16th, -ztop * one16th);
					p5 += glm::vec3(-4.0f * one16th,-5.0f * one16th, -ztop * one16th);
					p6 += glm::vec3( 4.0f * one16th,-5.0f * one16th,  zbot * one16th);
					p7 += glm::vec3(-4.0f * one16th,-5.0f * one16th,  zbot * one16th);

					x_texMX = 5; x_texPX = 5; x_texMY = ztop; x_texPY = zbot;
					y_texMX = 4; y_texPX = 4; y_texMY = ztop; y_texPY = zbot;
					z_texMX = 4; z_texPX = 4; z_texMY = 5;  z_texPY = 5;
					break ;
				case face_dir::minus_y:
				case face_dir::plus_y:
					p0 += glm::vec3( 5.0f * one16th, 4.0f * one16th, -ztop * one16th);
					p1 += glm::vec3(-5.0f * one16th, 4.0f * one16th, -ztop * one16th);
					p2 += glm::vec3( 5.0f * one16th, 4.0f * one16th,  zbot * one16th);
					p3 += glm::vec3(-5.0f * one16th, 4.0f * one16th,  zbot * one16th);

					p4 += glm::vec3( 5.0f * one16th,-4.0f * one16th, -ztop * one16th);
					p5 += glm::vec3(-5.0f * one16th,-4.0f * one16th, -ztop * one16th);
					p6 += glm::vec3( 5.0f * one16th,-4.0f * one16th,  zbot * one16th);
					p7 += glm::vec3(-5.0f * one16th,-4.0f * one16th,  zbot * one16th);

					x_texMX = 4; x_texPX = 4; x_texMY = ztop; x_texPY = zbot;
					y_texMX = 5; y_texPX = 5; y_texMY = ztop; y_texPY = zbot;
					z_texMX = 4; z_texPX = 4; z_texMY = 5;  z_texPY = 5;
					break ;
			}
			break ;
	}
	// drawing lever's base
	spec = s_blocks[blocks::cobblestone]->texture;
	int light = chunk->computeLight(pos.x, pos.y, pos.z);
	if (placement != placement::wall || orientation != face_dir::plus_x) {
		utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec + (x_texMX << 12) + (x_texMY << 16), light, 16 - x_texMX - x_texPX, 16 - x_texMY - x_texPY); // -x
	}
	if (placement != placement::wall || orientation != face_dir::minus_x) {
		utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec + (x_texMX << 12) + (x_texMY << 16), light, 16 - x_texMX - x_texPX, 16 - x_texMY - x_texPY); // +x
	}
	if (placement != placement::wall || orientation != face_dir::plus_y) {
		utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec + (y_texMX << 12) + (y_texMY << 16), light, 16 - y_texMX - y_texPX, 16 - y_texMY - y_texPY); // -y
	}
	if (placement != placement::wall || orientation != face_dir::minus_y) {
		utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec + (y_texMX << 12) + (y_texMY << 16), light, 16 - y_texMX - y_texPX, 16 - y_texMY - y_texPY); // +y
	}
	if (placement != placement::ceiling) {
		utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec + (z_texMX << 12) + (z_texMY << 16), light, 16 - z_texMX - z_texPX, 16 - z_texMY - z_texPY); // +z
	}
	if (placement != placement::floor) {
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec + (z_texMX << 12) + (z_texMY << 16), light, 16 - z_texMX - z_texPX, 16 - z_texMY - z_texPY); // -z
	}
	// it's lever time
	glm::vec3 top, right, front;
	bool powered = (value >> offset::redstone::powered) & 0x1;
	switch (placement) {
	case placement::wall:
		switch (orientation) {
			case face_dir::minus_x:
				top = glm::vec3(-1, 0, (powered) ? -1 : 1);
				break ;
			case face_dir::plus_x:
				top = glm::vec3( 1, 0, (powered) ? -1 : 1);
				break ;
			case face_dir::minus_y:
				top = glm::vec3(0, -1, (powered) ? -1 : 1);
				break ;
			case face_dir::plus_y:
				top = glm::vec3(0,  1, (powered) ? -1 : 1);
				break ;
		}
		break ;
	case placement::floor:
	case placement::ceiling:
		switch (orientation) {
			case face_dir::minus_x:
				top = glm::vec3((powered) ? -1 : 1, 0, (placement == placement::floor) ? 1 : -1);
				break ;
			case face_dir::plus_x:
				top = glm::vec3((powered) ? 1 : -1, 0, (placement == placement::floor) ? 1 : -1);
				break ;
			case face_dir::minus_y:
				top = glm::vec3(0, (powered) ? -1 : 1, (placement == placement::floor) ? 1 : -1);
				break ;
			case face_dir::plus_y:
				top = glm::vec3(0, (powered) ? 1 : -1, (placement == placement::floor) ? 1 : -1);
				break ;
		}
		break ;
	}
	top   = glm::normalize(top);
	right = glm::normalize(glm::cross(top, glm::vec3(0, 0, 1)));
	front = glm::normalize(glm::cross(top, right));
	glm::vec3 baseCenter = (p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7) / 8.0f;

	p0 = baseCenter - right * one16th - front * one16th + top * 10.0f * one16th;
	p1 = baseCenter + right * one16th - front * one16th + top * 10.0f * one16th;
	p2 = baseCenter - right * one16th - front * one16th;
	p3 = baseCenter + right * one16th - front * one16th;

	p4 = baseCenter - right * one16th + front * one16th + top * 10.0f * one16th;
	p5 = baseCenter + right * one16th + front * one16th + top * 10.0f * one16th;
	p6 = baseCenter - right * one16th + front * one16th;
	p7 = baseCenter + right * one16th + front * one16th;

	spec = texture;
	utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec + (7 << 12) + (6 << 16), light, 2, 10); // -x
	utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec + (7 << 12) + (6 << 16), light, 2, 10); // +x
	utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec + (7 << 12) + (6 << 16), light, 2, 10); // -y
	utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec + (7 << 12) + (6 << 16), light, 2, 10); // +y
	utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec + (7 << 12) + (6 << 16), light, 2, 2); // +z
}

void Button::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int spec, x_texMX, x_texPX, x_texMY, x_texPY, y_texMX, y_texPX, y_texMY, y_texPY, z_texMX, z_texPX, z_texMY,  z_texPY, ztop, zbot;
	int placement = (value >> offset::blocks::bitfield) & 0x3;
	int orientation = (value >> offset::blocks::orientation) & 0x7;
	float delta = (value & mask::redstone::powered) ? 15.0f : 14.0f;
	switch (placement) {
		case placement::wall:
			switch (orientation) {
				case face_dir::minus_x:
					p0 += glm::vec3( delta * one16th,  5.0f * one16th, -6.0f * one16th);
					p1 += glm::vec3( 0,                5.0f * one16th, -6.0f * one16th);
					p2 += glm::vec3( delta * one16th,  5.0f * one16th,  6.0f * one16th);
					p3 += glm::vec3( 0,                5.0f * one16th,  6.0f * one16th);

					p4 += glm::vec3( delta * one16th, -5.0f * one16th, -6.0f * one16th);
					p5 += glm::vec3( 0,               -5.0f * one16th, -6.0f * one16th);
					p6 += glm::vec3( delta * one16th, -5.0f * one16th,  6.0f * one16th);
					p7 += glm::vec3( 0,               -5.0f * one16th,  6.0f * one16th);

					x_texMX = 5; x_texPX = 5;  x_texMY = 6; x_texPY = 6;
					y_texMX = 0; y_texPX = delta; y_texMY = 6; y_texPY = 6;
					z_texMX = delta; z_texPX = 0;  z_texMY = 5;  z_texPY = 5;
					break ;
				case face_dir::plus_x:
					p0 += glm::vec3( 0,               5.0f * one16th, -6.0f * one16th);
					p1 += glm::vec3(-delta * one16th, 5.0f * one16th, -6.0f * one16th);
					p2 += glm::vec3( 0,               5.0f * one16th,  6.0f * one16th);
					p3 += glm::vec3(-delta * one16th, 5.0f * one16th,  6.0f * one16th);

					p4 += glm::vec3( 0,              -5.0f * one16th, -6.0f * one16th);
					p5 += glm::vec3(-delta * one16th,-5.0f * one16th, -6.0f * one16th);
					p6 += glm::vec3( 0,              -5.0f * one16th,  6.0f * one16th);
					p7 += glm::vec3(-delta * one16th,-5.0f * one16th,  6.0f * one16th);

					x_texMX = 5; x_texPX = 5;  x_texMY = 6; x_texPY = 6;
					y_texMX = 0; y_texPX = delta; y_texMY = 6; y_texPY = 6;
					z_texMX = 0; z_texPX = delta;  z_texMY = 5;  z_texPY = 5;
					break ;
				case face_dir::minus_y:
					p0 += glm::vec3( 5.0f * one16th, delta * one16th, -6.0f * one16th);
					p1 += glm::vec3(-5.0f * one16th, delta * one16th, -6.0f * one16th);
					p2 += glm::vec3( 5.0f * one16th, delta * one16th,  6.0f * one16th);
					p3 += glm::vec3(-5.0f * one16th, delta * one16th,  6.0f * one16th);

					p4 += glm::vec3( 5.0f * one16th, 0,               -6.0f * one16th);
					p5 += glm::vec3(-5.0f * one16th, 0,               -6.0f * one16th);
					p6 += glm::vec3( 5.0f * one16th, 0,                6.0f * one16th);
					p7 += glm::vec3(-5.0f * one16th, 0,                6.0f * one16th);

					x_texMX = 0; x_texPX = delta; x_texMY = 6; x_texPY = 6;
					y_texMX = 5; y_texPX = 5;  y_texMY = 6; y_texPY = 6;
					z_texMX = 5; z_texPX = 5;  z_texMY = delta;  z_texPY = 0;
					break ;
				case face_dir::plus_y:
					p0 += glm::vec3( 5.0f * one16th, 0,               -6.0f * one16th);
					p1 += glm::vec3(-5.0f * one16th, 0,               -6.0f * one16th);
					p2 += glm::vec3( 5.0f * one16th, 0,                6.0f * one16th);
					p3 += glm::vec3(-5.0f * one16th, 0,                6.0f * one16th);

					p4 += glm::vec3( 5.0f * one16th, -delta * one16th, -6.0f * one16th);
					p5 += glm::vec3(-5.0f * one16th, -delta * one16th, -6.0f * one16th);
					p6 += glm::vec3( 5.0f * one16th, -delta * one16th,  6.0f * one16th);
					p7 += glm::vec3(-5.0f * one16th, -delta * one16th,  6.0f * one16th);

					x_texMX = 0; x_texPX = delta; x_texMY = 6; x_texPY = 6;
					y_texMX = 5; y_texPX = 5;  y_texMY = 6; y_texPY = 6;
					z_texMX = 5; z_texPX = 5;  z_texMY = 0;  z_texPY = delta;
					break ;
			}
			break ;
		case placement::floor:
		case placement::ceiling:
			ztop = (placement == placement::floor) ? delta : 0;
			zbot = (placement == placement::floor) ? 0 : delta;
			switch (orientation) {
				case face_dir::minus_y:
				case face_dir::plus_y:
					p0 += glm::vec3( 5.0f * one16th, 6.0f * one16th, -ztop * one16th);
					p1 += glm::vec3(-5.0f * one16th, 6.0f * one16th, -ztop * one16th);
					p2 += glm::vec3( 5.0f * one16th, 6.0f * one16th,  zbot * one16th);
					p3 += glm::vec3(-5.0f * one16th, 6.0f * one16th,  zbot * one16th);

					p4 += glm::vec3( 5.0f * one16th,-6.0f * one16th, -ztop * one16th);
					p5 += glm::vec3(-5.0f * one16th,-6.0f * one16th, -ztop * one16th);
					p6 += glm::vec3( 5.0f * one16th,-6.0f * one16th,  zbot * one16th);
					p7 += glm::vec3(-5.0f * one16th,-6.0f * one16th,  zbot * one16th);

					x_texMX = 6; x_texPX = 6; x_texMY = ztop; x_texPY = zbot;
					y_texMX = 5; y_texPX = 5; y_texMY = ztop; y_texPY = zbot;
					z_texMX = 5; z_texPX = 5; z_texMY = 6;  z_texPY = 6;
					break ;
				case face_dir::minus_x:
				case face_dir::plus_x:
					p0 += glm::vec3( 6.0f * one16th, 5.0f * one16th, -ztop * one16th);
					p1 += glm::vec3(-6.0f * one16th, 5.0f * one16th, -ztop * one16th);
					p2 += glm::vec3( 6.0f * one16th, 5.0f * one16th,  zbot * one16th);
					p3 += glm::vec3(-6.0f * one16th, 5.0f * one16th,  zbot * one16th);

					p4 += glm::vec3( 6.0f * one16th,-5.0f * one16th, -ztop * one16th);
					p5 += glm::vec3(-6.0f * one16th,-5.0f * one16th, -ztop * one16th);
					p6 += glm::vec3( 6.0f * one16th,-5.0f * one16th,  zbot * one16th);
					p7 += glm::vec3(-6.0f * one16th,-5.0f * one16th,  zbot * one16th);

					x_texMX = 5; x_texPX = 5; x_texMY = ztop; x_texPY = zbot;
					y_texMX = 6; y_texPX = 6; y_texMY = ztop; y_texPY = zbot;
					z_texMX = 5; z_texPX = 5; z_texMY = 6;  z_texPY = 6;
					break ;
			}
			break ;
	}
	spec = texture;
	int light = chunk->computeLight(pos.x, pos.y, pos.z);
	if (placement != placement::wall || orientation != face_dir::plus_x) {
		utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec + (x_texMX << 12) + (x_texMY << 16), light, 16 - x_texMX - x_texPX, 16 - x_texMY - x_texPY); // -x
	}
	if (placement != placement::wall || orientation != face_dir::minus_x) {
		utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec + (x_texMX << 12) + (x_texMY << 16), light, 16 - x_texMX - x_texPX, 16 - x_texMY - x_texPY); // +x
	}
	if (placement != placement::wall || orientation != face_dir::plus_y) {
		utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec + (y_texMX << 12) + (y_texMY << 16), light, 16 - y_texMX - y_texPX, 16 - y_texMY - y_texPY); // -y
	}
	if (placement != placement::wall || orientation != face_dir::minus_y) {
		utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec + (y_texMX << 12) + (y_texMY << 16), light, 16 - y_texMX - y_texPX, 16 - y_texMY - y_texPY); // +y
	}
	if (placement != placement::ceiling) {
		utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec + (z_texMX << 12) + (z_texMY << 16), light, 16 - z_texMX - z_texPX, 16 - z_texMY - z_texPY); // +z
	}
	if (placement != placement::floor) {
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec + (z_texMX << 12) + (z_texMY << 16), light, 16 - z_texMX - z_texPX, 16 - z_texMY - z_texPY); // -z
	}
}

void RedstoneDust::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	(void)chunk;
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0.002f);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0.002f);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0.002f);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0.002f);

	int spec = settings::consts::shader::block::dust_dot;
	int color = (value >> offset::redstone::strength) & 0xF;

	int mx = (value >> offset::redstone::dust::mx) & mask::redstone::dust::connect;
	int px = (value >> offset::redstone::dust::px) & mask::redstone::dust::connect;
	int my = (value >> offset::redstone::dust::my) & mask::redstone::dust::connect;
	int py = (value >> offset::redstone::dust::py) & mask::redstone::dust::connect;

	if ((mx && (my || py)) || (px && (my || py)) // display central dot if at least 2 perpendicular dir are on
		|| !(mx | py | my | py)) { // or if no dir is on
		utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, 0.0f, -0.0005f), p5 + glm::vec3(0.0f, 0.0f, -0.0005f), p0 + glm::vec3(0.0f, 0.0f, -0.0005f), p1 + glm::vec3(0.0f, 0.0f, -0.0005f)}, spec, color, 16, 16);;
	}

	spec = settings::consts::shader::block::dust_line;
	if (mx) {
		utils::shader::addQuads(vertices, {p0, p4, p1 + glm::vec3(-0.5f, 0.0f, 0.0f), p5 + glm::vec3(-0.5f, 0.0f, 0.0f)}, spec, color, 16, 8);
		if (mx & redstone::dust::up) {
			utils::shader::addQuads(vertices, {p0 + glm::vec3(0.002f, 0.0f, 1.0f), p4 + glm::vec3(0.002f, 0.0f, 1.0f), p0 + glm::vec3(0.002f, 0.0f, 0.0f), p4 + glm::vec3(0.002f, 0.0f, 0.0f)}, spec, color, 16, 16);;
		}
	}
	if (px) {
		utils::shader::addQuads(vertices, {p0 + glm::vec3(0.5f, 0.0f, 0.0f), p4 + glm::vec3(0.5f, 0.0f, 0.0f), p1, p5}, spec + (8 << 16), color, 16, 8);
		if (px & redstone::dust::up) {
			utils::shader::addQuads(vertices, {p5 + glm::vec3(-0.002f, 0.0f, 1.0f), p1 + glm::vec3(-0.002f, 0.0f, 1.0f), p5 + glm::vec3(-0.002f, 0.0f, 0.0f), p1 + glm::vec3(-0.002f, 0.0f, 0.0f)}, spec, color, 16, 16);;
		}
	}
	if (my) {
		utils::shader::addQuads(vertices, {p1, p0, p5 + glm::vec3(0.0f, -0.5f, 0.0f), p4 + glm::vec3(0.0f, -0.5f, 0.0f)}, spec, color, 16, 8);
		if (my & redstone::dust::up) {
			utils::shader::addQuads(vertices, {p1 + glm::vec3(0.0f, 0.002f, 1.0f), p0 + glm::vec3(0.0f, 0.002f, 1.0f), p1 + glm::vec3(0.0f, 0.002f, 0.0f), p0 + glm::vec3(0.0f, 0.002f, 0.0f)}, spec, color, 16, 16);;
		}
	}
	if (py) {
		utils::shader::addQuads(vertices, {p1 + glm::vec3(0.0f, 0.5f, 0.0f), p0 + glm::vec3(0.0f, 0.5f, 0.0f), p5, p4}, spec + (8 << 16), color, 16, 8);
		if (py & redstone::dust::up) {
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, -0.002f, 1.0f), p5 + glm::vec3(0.0f, -0.002f, 1.0f), p4 + glm::vec3(0.0f, -0.002f, 0.0f), p5 + glm::vec3(0.0f, -0.002f, 0.0f)}, spec, color, 16, 16);;
		}
	}
}

void Repeater::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int spec = s_blocks[blocks::smooth_stone]->texture;
	int light;
	// draw base
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		light = (3 << 8) + chunk->computeLight(pos.x - 1, pos.y, pos.z);
		utils::shader::addQuads(vertices, {p4 + glm::vec3(0.f, 0.f, -14.f * one16th), p0 + glm::vec3(0.f, 0.f, -14.f * one16th), p6, p2}, spec + (14 << 16), light, 16, 2);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		light = (4 << 8) + chunk->computeLight(pos.x + 1, pos.y, pos.z);
		utils::shader::addQuads(vertices, {p1 + glm::vec3(0.f, 0.f, -14.f * one16th), p5 + glm::vec3(0.f, 0.f, -14.f * one16th), p3, p7}, spec + (14 << 16), light, 16, 2);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		light = (1 << 8) + chunk->computeLight(pos.x, pos.y - 1, pos.z);
		utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -14.f * one16th), p1 + glm::vec3(0.f, 0.f, -14.f * one16th), p2, p3}, spec + (14 << 16), light, 16, 2);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		light = (2 << 8) + chunk->computeLight(pos.x, pos.y + 1, pos.z);
		utils::shader::addQuads(vertices, {p5 + glm::vec3(0.f, 0.f, -14.f * one16th), p4 + glm::vec3(0.f, 0.f, -14.f * one16th), p7, p6}, spec + (14 << 16), light, 16, 2);
	}
	// draw top
	light = chunk->computeLight(pos.x, pos.y, pos.z);
	spec = this->getTex(face_dir::minus_x, value);
	glm::vec3 right, front;
	switch ((value >> offset::blocks::orientation) & 0x7) {
		case (face_dir::minus_x):
			right = glm::vec3(0, 1, 0);
			front = glm::vec3(-1, 0, 0);
			break ;
		case (face_dir::plus_x):
			right = glm::vec3(0, -1, 0);
			front = glm::vec3(1, 0, 0);
			break ;
		case (face_dir::minus_y):
			right = glm::vec3(-1, 0, 0);
			front = glm::vec3(0, -1, 0);
			break ;
		case (face_dir::plus_y):
			right = glm::vec3(1, 0, 0);
			front = glm::vec3(0, 1, 0);
			break ;
	}
	glm::vec3 topLeft = p4 + glm::vec3(0.5f, -0.5f, -14 * one16th) - right * 0.5f + front * 0.5f;
	utils::shader::addQuads(vertices, {topLeft, topLeft + right, topLeft - front, topLeft + right - front}, spec, light, 16, 16);
	// draw front torch
	spec = s_blocks[blocks::redstone_torch]->getTex(face_dir::minus_x, (value & mask::redstone::powered) ^ mask::redstone::powered);
	p2 = topLeft + right * 7.0f * one16th - front * 2.0f * one16th;
	p3 = topLeft + right * 9.0f * one16th - front * 2.0f * one16th;
	p6 = topLeft + right * 7.0f * one16th - front * 4.0f * one16th;
	p7 = topLeft + right * 9.0f * one16th - front * 4.0f * one16th;
	topLeft.z += 6.0f * one16th;
	p0 = topLeft + right * 7.0f * one16th - front * 2.0f * one16th;
	p1 = topLeft + right * 9.0f * one16th - front * 2.0f * one16th;
	p4 = topLeft + right * 7.0f * one16th - front * 4.0f * one16th;
	p5 = topLeft + right * 9.0f * one16th - front * 4.0f * one16th;
	utils::shader::addQuads(vertices, {p1 + right * one16th, p0 - right * one16th, p3 + right * one16th, p2 - right * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p4 - right * one16th, p5 + right * one16th, p6 - right * one16th, p7 + right * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p0 + front * one16th, p4 - front * one16th, p2 + front * one16th, p6 - front * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p5 - front * one16th, p1 + front * one16th, p7 - front * one16th, p3 + front * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -one16th), p1 + glm::vec3(0.f, 0.f, -one16th), p4 + glm::vec3(0.f, 0.f, -one16th), p5 + glm::vec3(0.f, 0.f, -one16th)}, spec + (7 << 12) + (6 << 16), light, 2, 2);
	// draw back torch or lock bar
	float delta = 4.0f + 2.0f * ((value >> offset::redstone::repeater::ticks) & 0x3);
	p0 -= front * delta * one16th; p1 -= front * delta * one16th; p2 -= front * delta * one16th; p3 -= front * delta * one16th;
	p4 -= front * delta * one16th; p5 -= front * delta * one16th; p6 -= front * delta * one16th; p7 -= front * delta * one16th;
	if (!(value & mask::redstone::repeater::lock)) { // back torch
		utils::shader::addQuads(vertices, {p1 + right * one16th, p0 - right * one16th, p3 + right * one16th, p2 - right * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
		utils::shader::addQuads(vertices, {p4 - right * one16th, p5 + right * one16th, p6 - right * one16th, p7 + right * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
		utils::shader::addQuads(vertices, {p0 + front * one16th, p4 - front * one16th, p2 + front * one16th, p6 - front * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
		utils::shader::addQuads(vertices, {p5 - front * one16th, p1 + front * one16th, p7 - front * one16th, p3 + front * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
		utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -one16th), p1 + glm::vec3(0.f, 0.f, -one16th), p4 + glm::vec3(0.f, 0.f, -one16th), p5 + glm::vec3(0.f, 0.f, -one16th)}, spec + (7 << 12) + (6 << 16), light, 2, 2);
	} else { // lock bar
		spec = s_blocks[blocks::bedrock]->texture;
		p2 += -right * 5.0f * one16th;
		p3 +=  right * 5.0f * one16th;
		p6 += -right * 5.0f * one16th;
		p7 +=  right * 5.0f * one16th;
		const glm::vec3 up = {0, 0, 1};
		p0 += -right * 5.0f * one16th - up * 4.0f * one16th;
		p1 +=  right * 5.0f * one16th - up * 4.0f * one16th;
		p4 += -right * 5.0f * one16th - up * 4.0f * one16th;
		p5 +=  right * 5.0f * one16th - up * 4.0f * one16th;
		utils::shader::addQuads(vertices, {p1, p0, p3, p2}, spec + 2 + (14 << 8), light, 12, 2);
		utils::shader::addQuads(vertices, {p4, p5, p6, p7}, spec + 2 + (14 << 8), light, 12, 2);
		utils::shader::addQuads(vertices, {p0, p4, p2, p6}, spec + 6 + (7 << 8), light, 2, 2);
		utils::shader::addQuads(vertices, {p5, p1, p7, p3}, spec + (8 << 12) + (7 << 8), light, 2, 2);
		utils::shader::addQuads(vertices, {p0, p1, p4, p5}, spec + 2 + (7 << 8), light, 12, 2);
	}
}

void Comparator::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int spec = s_blocks[blocks::smooth_stone]->texture;
	int light;
	// draw base
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		light = (3 << 8) + chunk->computeLight(pos.x - 1, pos.y, pos.z);
		utils::shader::addQuads(vertices, {p4 + glm::vec3(0.f, 0.f, -14.f * one16th), p0 + glm::vec3(0.f, 0.f, -14.f * one16th), p6, p2}, spec + (14 << 16), light, 16, 2);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		light = (4 << 8) + chunk->computeLight(pos.x + 1, pos.y, pos.z);
		utils::shader::addQuads(vertices, {p1 + glm::vec3(0.f, 0.f, -14.f * one16th), p5 + glm::vec3(0.f, 0.f, -14.f * one16th), p3, p7}, spec + (14 << 16), light, 16, 2);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		light = (1 << 8) + chunk->computeLight(pos.x, pos.y - 1, pos.z);
		utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -14.f * one16th), p1 + glm::vec3(0.f, 0.f, -14.f * one16th), p2, p3}, spec + (14 << 16), light, 16, 2);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		light = (2 << 8) + chunk->computeLight(pos.x, pos.y + 1, pos.z);
		utils::shader::addQuads(vertices, {p5 + glm::vec3(0.f, 0.f, -14.f * one16th), p4 + glm::vec3(0.f, 0.f, -14.f * one16th), p7, p6}, spec + (14 << 16), light, 16, 2);
	}
	// draw top
	light = chunk->computeLight(pos.x, pos.y, pos.z);
	spec = this->getTex(face_dir::minus_x, value);
	glm::vec3 right, front;
	switch ((value >> offset::blocks::orientation) & 0x7) {
		case (face_dir::minus_x):
			right = glm::vec3(0, 1, 0);
			front = glm::vec3(-1, 0, 0);
			break ;
		case (face_dir::plus_x):
			right = glm::vec3(0, -1, 0);
			front = glm::vec3(1, 0, 0);
			break ;
		case (face_dir::minus_y):
			right = glm::vec3(-1, 0, 0);
			front = glm::vec3(0, -1, 0);
			break ;
		case (face_dir::plus_y):
			right = glm::vec3(1, 0, 0);
			front = glm::vec3(0, 1, 0);
			break ;
	}
	glm::vec3 topLeft = p4 + glm::vec3(0.5f, -0.5f, -14 * one16th) - right * 0.5f + front * 0.5f;
	utils::shader::addQuads(vertices, {topLeft, topLeft + right, topLeft - front, topLeft + right - front}, spec, light, 16, 16);
	// draw back torches
	spec = s_blocks[blocks::redstone_torch]->getTex(face_dir::minus_x, (value & mask::redstone::powered) ^ mask::redstone::powered);
	// back left
	p2 = topLeft + right * 4.0f * one16th - front * 11.0f * one16th;
	p3 = topLeft + right * 6.0f * one16th - front * 11.0f * one16th;
	p6 = topLeft + right * 4.0f * one16th - front * 13.0f * one16th;
	p7 = topLeft + right * 6.0f * one16th - front * 13.0f * one16th;
	topLeft.z += 6.0f * one16th;
	p0 = topLeft + right * 4.0f * one16th - front * 11.0f * one16th;
	p1 = topLeft + right * 6.0f * one16th - front * 11.0f * one16th;
	p4 = topLeft + right * 4.0f * one16th - front * 13.0f * one16th;
	p5 = topLeft + right * 6.0f * one16th - front * 13.0f * one16th;
	utils::shader::addQuads(vertices, {p1 + right * one16th, p0 - right * one16th, p3 + right * one16th, p2 - right * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p4 - right * one16th, p5 + right * one16th, p6 - right * one16th, p7 + right * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p0 + front * one16th, p4 - front * one16th, p2 + front * one16th, p6 - front * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p5 - front * one16th, p1 + front * one16th, p7 - front * one16th, p3 + front * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -one16th), p1 + glm::vec3(0.f, 0.f, -one16th), p4 + glm::vec3(0.f, 0.f, -one16th), p5 + glm::vec3(0.f, 0.f, -one16th)}, spec + (7 << 12) + (6 << 16), light, 2, 2);
	// back right
	p0 += right * 6.0f * one16th; p1 += right * 6.0f * one16th; p2 += right * 6.0f * one16th; p3 += right * 6.0f * one16th;
	p4 += right * 6.0f * one16th; p5 += right * 6.0f * one16th; p6 += right * 6.0f * one16th; p7 += right * 6.0f * one16th;
	utils::shader::addQuads(vertices, {p1 + right * one16th, p0 - right * one16th, p3 + right * one16th, p2 - right * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p4 - right * one16th, p5 + right * one16th, p6 - right * one16th, p7 + right * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p0 + front * one16th, p4 - front * one16th, p2 + front * one16th, p6 - front * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p5 - front * one16th, p1 + front * one16th, p7 - front * one16th, p3 + front * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -one16th), p1 + glm::vec3(0.f, 0.f, -one16th), p4 + glm::vec3(0.f, 0.f, -one16th), p5 + glm::vec3(0.f, 0.f, -one16th)}, spec + (7 << 12) + (6 << 16), light, 2, 2);
	// draw front torch
	spec = s_blocks[blocks::redstone_torch]->getTex(face_dir::minus_x, (!(value & mask::redstone::comparator::mode)) << offset::redstone::powered);
	float delta = (((value & mask::redstone::comparator::mode) == redstone::comparator::compare) ? 3.0f : 2.0f) * one16th;
	const glm::vec3 up = {0, 0, 1};
	p0 += front * 9.0f * one16th - right * 3.0f * one16th - up * delta;
	p1 += front * 9.0f * one16th - right * 3.0f * one16th - up * delta;
	p2 += front * 9.0f * one16th - right * 3.0f * one16th - up * delta;
	p3 += front * 9.0f * one16th - right * 3.0f * one16th - up * delta;
	p4 += front * 9.0f * one16th - right * 3.0f * one16th - up * delta;
	p5 += front * 9.0f * one16th - right * 3.0f * one16th - up * delta;
	p6 += front * 9.0f * one16th - right * 3.0f * one16th - up * delta;
	p7 += front * 9.0f * one16th - right * 3.0f * one16th - up * delta;
	utils::shader::addQuads(vertices, {p1 + right * one16th, p0 - right * one16th, p3 + right * one16th, p2 - right * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p4 - right * one16th, p5 + right * one16th, p6 - right * one16th, p7 + right * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p0 + front * one16th, p4 - front * one16th, p2 + front * one16th, p6 - front * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p5 - front * one16th, p1 + front * one16th, p7 - front * one16th, p3 + front * one16th}, spec + (6 << 12) + (5 << 16), light, 4, 6);
	utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -one16th), p1 + glm::vec3(0.f, 0.f, -one16th), p4 + glm::vec3(0.f, 0.f, -one16th), p5 + glm::vec3(0.f, 0.f, -one16th)}, spec + (7 << 12) + (6 << 16), light, 2, 2);
}

void Piston::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	float dxm = 0, dxp = 1, dym = 0, dyp = 1, dzm = 0, dzp = 1;
	int orientation = ((value >> offset::blocks::orientation) & 0x7), tyo = 0, ttyo;
	if (value & mask::redstone::piston::moving) {
		tyo = 4;
		switch (orientation) {
			case face_dir::minus_x:
				dxm = 0.25f;
				break ;
			case face_dir::plus_x:
				dxp = 0.75f;
				break ;
			case face_dir::minus_y:
				dym = 0.25f;
				break ;
			case face_dir::plus_y:
				dyp = 0.75f;
				break ;
			case face_dir::minus_z:
				dzm = 0.25f;
				break ;
			case face_dir::plus_z:
				dzp = 0.75f;
				break ;
		}
	}
	glm::vec3 p0 = glm::vec3(start.x + pos.x + dxm, start.y + pos.y + dym, pos.z + dzp);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + dxp, start.y + pos.y + dym, pos.z + dzp);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + dxm, start.y + pos.y + dym, pos.z + dzm);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + dxp, start.y + pos.y + dym, pos.z + dzm);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + dxm, start.y + pos.y + dyp, pos.z + dzp);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + dxp, start.y + pos.y + dyp, pos.z + dzp);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + dxm, start.y + pos.y + dyp, pos.z + dzm);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + dxp, start.y + pos.y + dyp, pos.z + dzm);

	int spec, light;

	glm::vec3 pp0, pp1, pp2, pp3;
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		ttyo = tyo;
		switch (orientation) {
			case (face_dir::minus_x):
				pp0 = p4; pp1 = p0; pp2 = p6; pp3 = p2;
				ttyo = 0;
				break ;
			case (face_dir::plus_x):
				pp0 = p4; pp1 = p0; pp2 = p6; pp3 = p2;
				ttyo = 0;
				break ;
			case (face_dir::minus_y):
				pp0 = p0; pp1 = p2; pp2 = p4; pp3 = p6;
				break ;
			case (face_dir::plus_y):
				pp0 = p6; pp1 = p4; pp2 = p2; pp3 = p0;
				break ;
			case (face_dir::minus_z):
				pp0 = p2; pp1 = p6; pp2 = p0; pp3 = p4;
				break ;
			case (face_dir::plus_z):
				pp0 = p4; pp1 = p0; pp2 = p6; pp3 = p2;
				break ;
		}
		spec = getTex(face_dir::minus_x, value);
		light = chunk->computeLight(pos.x - 1, pos.y, pos.z) + (3 << 8);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + (ttyo << 16), light, 16, 16 - ttyo);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		ttyo = tyo;
		switch (orientation) {
			case (face_dir::minus_x):
				pp0 = p1; pp1 = p5; pp2 = p3; pp3 = p7;
				ttyo = 0;
				break ;
			case (face_dir::plus_x):
				pp0 = p1; pp1 = p5; pp2 = p3; pp3 = p7;
				ttyo = 0;
				break ;
			case (face_dir::minus_y):
				pp0 = p3; pp1 = p1; pp2 = p7; pp3 = p5;
				break ;
			case (face_dir::plus_y):
				pp0 = p5; pp1 = p7; pp2 = p1; pp3 = p3;
				break ;
			case (face_dir::minus_z):
				pp0 = p7; pp1 = p3; pp2 = p5; pp3 = p1;
				break ;
			case (face_dir::plus_z):
				pp0 = p1; pp1 = p5; pp2 = p3; pp3 = p7;
				break ;
		}
		spec = getTex(face_dir::plus_x, value);
		light = chunk->computeLight(pos.x + 1, pos.y, pos.z) + (4 << 8);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + (ttyo << 16), light, 16, 16 - ttyo);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		ttyo = tyo;
		switch (orientation) {
			case (face_dir::minus_x):
				pp0 = p2; pp1 = p0; pp2 = p3; pp3 = p1;
				break ;
			case (face_dir::plus_x):
				pp0 = p1; pp1 = p3; pp2 = p0; pp3 = p2;
				break ;
			case (face_dir::minus_y):
				pp0 = p0; pp1 = p1; pp2 = p2; pp3 = p3;
				ttyo = 0;
				break ;
			case (face_dir::plus_y):
				pp0 = p0; pp1 = p1; pp2 = p2; pp3 = p3;
				ttyo = 0;
				break ;
			case (face_dir::minus_z):
				pp0 = p3; pp1 = p2; pp2 = p1; pp3 = p0;
				break ;
			case (face_dir::plus_z):
				pp0 = p0; pp1 = p1; pp2 = p2; pp3 = p3;
				break ;
		}
		spec = getTex(face_dir::minus_y, value);
		light = chunk->computeLight(pos.x, pos.y - 1, pos.z) + (1 << 8);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + (ttyo << 16), light, 16, 16 - ttyo);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		ttyo = tyo;
		switch (orientation) {
			case (face_dir::minus_x):
				pp0 = p4; pp1 = p6; pp2 = p5; pp3 = p7;
				break ;
			case (face_dir::plus_x):
				pp0 = p7; pp1 = p5; pp2 = p6; pp3 = p4;
				break ;
			case (face_dir::minus_y):
				pp0 = p5; pp1 = p4; pp2 = p7; pp3 = p6;
				ttyo = 0;
				break ;
			case (face_dir::plus_y):
				pp0 = p5; pp1 = p4; pp2 = p7; pp3 = p6;
				ttyo = 0;
				break ;
			case (face_dir::minus_z):
				pp0 = p6; pp1 = p7; pp2 = p4; pp3 = p5;
				break ;
			case (face_dir::plus_z):
				pp0 = p5; pp1 = p4; pp2 = p7; pp3 = p6;
				break ;
		}
		spec = getTex(face_dir::plus_y, value);
		light = chunk->computeLight(pos.x, pos.y + 1, pos.z) + (2 << 8);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + (ttyo << 16), light, 16, 16 - ttyo);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		ttyo = tyo;
		switch (orientation) {
			case (face_dir::minus_x):
				pp0 = p0; pp1 = p4; pp2 = p1; pp3 = p5;
				break ;
			case (face_dir::plus_x):
				pp0 = p5; pp1 = p1; pp2 = p4; pp3 = p0;
				break ;
			case (face_dir::minus_y):
				pp0 = p1; pp1 = p0; pp2 = p5; pp3 = p4;
				break ;
			case (face_dir::plus_y):
				pp0 = p4; pp1 = p5; pp2 = p0; pp3 = p1;
				break ;
			case (face_dir::minus_z):
				pp0 = p4; pp1 = p5; pp2 = p0; pp3 = p1;
				ttyo = 0;
				break ;
			case (face_dir::plus_z):
				pp0 = p4; pp1 = p5; pp2 = p0; pp3 = p1;
				ttyo = 0;
				break ;
		}
		spec = getTex(face_dir::plus_z, value);
		light = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + (ttyo << 16), light, 16, 16 - ttyo);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		ttyo = tyo;
		switch (orientation) {
			case (face_dir::minus_x):
				pp0 = p6; pp1 = p2; pp2 = p7; pp3 = p3;
				break ;
			case (face_dir::plus_x):
				pp0 = p3; pp1 = p7; pp2 = p2; pp3 = p6;
				break ;
			case (face_dir::minus_y):
				pp0 = p2; pp1 = p3; pp2 = p6; pp3 = p7;
				break ;
			case (face_dir::plus_y):
				pp0 = p7; pp1 = p6; pp2 = p3; pp3 = p2;
				break ;
			case (face_dir::minus_z):
				pp0 = p2; pp1 = p3; pp2 = p6; pp3 = p7;
				ttyo = 0;
				break ;
			case (face_dir::plus_z):
				pp0 = p2; pp1 = p3; pp2 = p6; pp3 = p7;
				ttyo = 0;
				break ;
		}
		spec = getTex(face_dir::minus_z, value);
		light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 8);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + (ttyo << 16), light, 16, 16 - ttyo);
	}
	if (!(value & mask::redstone::piston::moving)) {
		return ;
	}
	glm::vec3 right, front, up;
	switch ((value >> offset::blocks::orientation) & 0x7) {
		case (face_dir::minus_x):
			right = glm::vec3( 0, 1, 0);
			front = glm::vec3(-1, 0, 0);
			up    = glm::vec3( 0, 0, 1);
			break ;
		case (face_dir::plus_x):
			right = glm::vec3(0, -1, 0);
			front = glm::vec3(1,  0, 0);
			up    = glm::vec3(0,  0, 1);
			break ;
		case (face_dir::minus_y):
			right = glm::vec3(-1,  0, 0);
			front = glm::vec3( 0, -1, 0);
			up    = glm::vec3( 0,  0, 1);
			break ;
		case (face_dir::plus_y):
			right = glm::vec3(1, 0, 0);
			front = glm::vec3(0, 1, 0);
			up    = glm::vec3(0, 0, 1);
			break ;
		case (face_dir::minus_z):
			right = glm::vec3(1, 0, 0);
			front = glm::vec3(0, 0, -1);
			up    = glm::vec3(0, 1, 0);
			break ;
		case (face_dir::plus_z):
			right = glm::vec3(1,  0, 0);
			front = glm::vec3(0,  0, 1);
			up    = glm::vec3(0, -1, 0);
			break ;
	}
	light = chunk->computeLight(pos.x, pos.y, pos.z);
	spec &= 0xFFFF0000;
	spec += getTex(face_dir::minus_x, face_dir::minus_y << offset::blocks::orientation);
	glm::vec3 topLeft = glm::vec3(start, 0) + pos + glm::vec3(0.5f, 0.5f, 0.5f) + (-right + front + up) * 0.5f;
	// piston bar
	utils::shader::addQuads(vertices, {topLeft + (right - up) * 6.0f * one16th - front * 0.25f,
									topLeft + (right - up) * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 0.25f - up * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - up * 6.0f * one16th}, spec, light, 4, 4);
	utils::shader::addQuads(vertices, {topLeft + right * 10.0f * one16th - front * 0.25f - up * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - up * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 0.25f - up * 10.0f * one16th,
									topLeft + right * 10.0f * one16th - up * 10.0f * one16th}, spec, light, 4, 4);
	utils::shader::addQuads(vertices, {topLeft + right * 10.0f * one16th - front * 0.25f - up * 10.0f * one16th,
									topLeft + right * 10.0f * one16th - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 0.25f - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - up * 10.0f * one16th}, spec, light, 4, 4);
	utils::shader::addQuads(vertices, {topLeft + right * 6.0f * one16th - front * 0.25f - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 0.25f - up * 6.0f * one16th,
									topLeft + right * 6.0f * one16th - up * 6.0f * one16th}, spec, light, 4, 4);
}

void PistonHead::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 right, front, up;
	switch ((value >> offset::blocks::orientation) & 0x7) {
		case (face_dir::minus_x):
			right = glm::vec3( 0, 1, 0);
			front = glm::vec3(-1, 0, 0);
			up    = glm::vec3( 0, 0, 1);
			break ;
		case (face_dir::plus_x):
			right = glm::vec3(0, -1, 0);
			front = glm::vec3(1,  0, 0);
			up    = glm::vec3(0,  0, 1);
			break ;
		case (face_dir::minus_y):
			right = glm::vec3(-1,  0, 0);
			front = glm::vec3( 0, -1, 0);
			up    = glm::vec3( 0,  0, 1);
			break ;
		case (face_dir::plus_y):
			right = glm::vec3(1, 0, 0);
			front = glm::vec3(0, 1, 0);
			up    = glm::vec3(0, 0, 1);
			break ;
		case (face_dir::minus_z):
			right = glm::vec3(1, 0, 0);
			front = glm::vec3(0, 0, -1);
			up    = glm::vec3(0, 1, 0);
			break ;
		case (face_dir::plus_z):
			right = glm::vec3(1,  0, 0);
			front = glm::vec3(0,  0, 1);
			up    = glm::vec3(0, -1, 0);
			break ;
	}
	int light = chunk->computeLight(pos.x - front.x, pos.y - front.y, pos.z - front.z);
	glm::vec3 topLeft = glm::vec3(start, 0) + pos + glm::vec3(0.5f, 0.5f, 0.5f) + (-right + front + up) * 0.5f;
	int piston = (value & mask::redstone::piston::sticky) ? blocks::sticky_piston : blocks::piston;
	// front face
	int spec = s_blocks[piston]->getTex(face_dir::minus_x, face_dir::minus_x << offset::blocks::orientation);
	utils::shader::addQuads(vertices, {topLeft + right, topLeft, topLeft + right - up, topLeft - up}, spec, light, 16, 16);
	// back of front volume
	if (piston == blocks::sticky_piston) {
		spec = s_blocks[blocks::piston]->getTex(face_dir::minus_x, face_dir::minus_x << offset::blocks::orientation);
	}
	utils::shader::addQuads(vertices, {topLeft - front * 0.25f, topLeft + right - front * 0.25f, topLeft - up - front * 0.25f, topLeft + right - up - front * 0.25f}, spec, light, 16, 16);
	// side faces
	spec = s_blocks[piston]->getTex(face_dir::minus_y, face_dir::minus_x << offset::blocks::orientation);
	utils::shader::addQuads(vertices, {topLeft, topLeft + right, topLeft - front * 0.25f, topLeft + right - front * 0.25f}, spec, light, 16, 4);
	utils::shader::addQuads(vertices, {topLeft - up, topLeft, topLeft - up - front * 0.25f, topLeft - front * 0.25f}, spec, light, 16, 4);
	utils::shader::addQuads(vertices, {topLeft + right, topLeft + right - up, topLeft + right - front * 0.25f, topLeft + right - up - front * 0.25f}, spec, light, 16, 4);
	utils::shader::addQuads(vertices, {topLeft + right - up, topLeft - up, topLeft + right - up - front * 0.25f, topLeft - up - front * 0.25f}, spec, light, 16, 4);
	// piston bar
	utils::shader::addQuads(vertices, {topLeft + right * 6.0f * one16th - front * 1.0f - up * 6.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 0.25f - up * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 1.0f - up * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 0.25f - up * 6.0f * one16th}, spec + (4 << 12), light, 12, 4);
	utils::shader::addQuads(vertices, {topLeft + right * 10.0f * one16th - front * 1.0f - up * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 0.25f - up * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 1.0f - up * 10.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 0.25f - up * 10.0f * one16th}, spec + (4 << 12), light, 12, 4);
	utils::shader::addQuads(vertices, {topLeft + right * 10.0f * one16th - front * 1.0f - up * 10.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 0.25f - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 1.0f - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 0.25f - up * 10.0f * one16th}, spec + (4 << 12), light, 12, 4);
	utils::shader::addQuads(vertices, {topLeft + right * 6.0f * one16th - front * 1.0f - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 0.25f - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 1.0f - up * 6.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 0.25f - up * 6.0f * one16th,}, spec + (4 << 12), light, 12, 4);
}

void OakSign::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 right, front, up;
	switch ((value >> offset::blocks::orientation) & 0x7) {
		case (face_dir::minus_x):
			right = glm::vec3( 0, 1, 0);
			front = glm::vec3(-1, 0, 0);
			up    = glm::vec3( 0, 0, 1);
			break ;
		case (face_dir::plus_x):
			right = glm::vec3(0, -1, 0);
			front = glm::vec3(1,  0, 0);
			up    = glm::vec3(0,  0, 1);
			break ;
		case (face_dir::minus_y):
			right = glm::vec3(-1,  0, 0);
			front = glm::vec3( 0, -1, 0);
			up    = glm::vec3( 0,  0, 1);
			break ;
		case (face_dir::plus_y):
			right = glm::vec3(1, 0, 0);
			front = glm::vec3(0, 1, 0);
			up    = glm::vec3(0, 0, 1);
			break ;
		default:
			LOGERROR("oakSign::addMesh orientation");
			return ;
	}
	glm::vec3 topLeft = glm::vec3(start, 0) + pos + glm::vec3(0.5f, 0.5f, 0.5f) + (-right + front) * 0.5f + up * 0.25f;

	// front face
	int light = chunk->computeLight(pos.x, pos.y, pos.z);
	int spec = settings::consts::shader::block::oak_sign;
	// player side
	utils::shader::addQuads(vertices, {topLeft - front * 1.75f * one16th,
								topLeft + right * .66f - front * 1.75f * one16th,
								topLeft - up * 0.5f - front * 1.75f * one16th,
								topLeft + right * .66f - up * 0.5f - front * 1.75f * one16th}, spec + (2 << 16), light + (1 << 8), 16, 12);
	utils::shader::addQuads(vertices, {topLeft + right * .66f - front * 1.75f * one16th,
								topLeft + right - front * 1.75f * one16th,
								topLeft + right * .66f - up * 0.5f - front * 1.75f * one16th,
								topLeft + right - up * 0.5f - front * 1.75f * one16th}, spec + (2 << 16), light + (1 << 8), 8, 12);
	// back face
	utils::shader::addQuads(vertices, {topLeft + right,
								topLeft + right * .33f,
								topLeft + right - up * 0.5f,
								topLeft + right * .33f - up * 0.5f}, spec + (2 << 16), light + (2 << 8), 16, 12);
	utils::shader::addQuads(vertices, {topLeft + right * .33f,
								topLeft,
								topLeft + right * .33f - up * 0.5f,
								topLeft - up * 0.5f}, spec + (2 << 16), light + (2 << 8), 8, 12);
	// side faces
	// top
	utils::shader::addQuads(vertices, {topLeft,
								topLeft + right * .66f,
								topLeft - front * 1.75f * one16th,
								topLeft + right * .66f - front * 1.75f * one16th}, spec, light + (0 << 8), 16, 2);
	utils::shader::addQuads(vertices, {topLeft + right * .66f,
								topLeft + right,
								topLeft + right * .66f - front * 1.75f * one16th,
								topLeft + right - front * 1.75f * one16th}, spec, light + (0 << 8), 8, 2);
	// left
	utils::shader::addQuads(vertices, {topLeft,
								topLeft - front * 1.75f * one16th,
								topLeft - up * 0.5f,
								topLeft - up * 0.5f - front * 1.75f * one16th}, spec + (8 << 12), light + (3 << 8), 2, 12);
	// right
	utils::shader::addQuads(vertices, {topLeft + right - front * 1.75f * one16th,
								topLeft + right,
								topLeft + right - front * 1.75f * one16th - up * 0.5f,
								topLeft + right - up * 0.5f}, spec + (10 << 12), light + (4 << 8), 2, 12);
	// bottom
	utils::shader::addQuads(vertices, {topLeft - up * 0.5f - front * 1.75f * one16th,
								topLeft + right * .66f - up * 0.5f - front * 1.75f * one16th,
								topLeft - up * 0.5f,
								topLeft + right * .66f - up * 0.5f}, spec, light + (5 << 8), 16, 2);
	utils::shader::addQuads(vertices, {topLeft + right * .66f - up * 0.5f - front * 1.75f * one16th,
								topLeft + right - up * 0.5f - front * 1.75f * one16th,
								topLeft + right * .66f - up * 0.5f,
								topLeft + right - up * 0.5f}, spec, light + (5 << 8), 8, 2);
}

void Observer::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	int orientation = utils::block::opposite_dir((value >> offset::blocks::orientation) & 0x7);

	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int spec, light;

	glm::vec3 pp0, pp1, pp2, pp3;
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		switch (orientation) {
			case (face_dir::minus_x):
			case (face_dir::plus_x):
			case (face_dir::minus_y):
			case (face_dir::plus_y):
				pp0 = p4; pp1 = p0; pp2 = p6; pp3 = p2;
				break ;
			case (face_dir::minus_z):
			case (face_dir::plus_z):
				pp0 = p6; pp1 = p4; pp2 = p2; pp3 = p0;
				break ;
		}
		spec = getTex(face_dir::minus_x, value);
		light = chunk->computeLight(pos.x - 1, pos.y, pos.z) + (3 << 19);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec, light, 16, 16);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		switch (orientation) {
			case (face_dir::minus_x):
			case (face_dir::plus_x):
			case (face_dir::minus_y):
			case (face_dir::plus_y):
				pp0 = p1; pp1 = p5; pp2 = p3; pp3 = p7;
				break ;
			case (face_dir::minus_z):
			case (face_dir::plus_z):
				pp0 = p5; pp1 = p7; pp2 = p1; pp3 = p3;
				break ;
		}
		spec = getTex(face_dir::plus_x, value);
		light = chunk->computeLight(pos.x + 1, pos.y, pos.z) + (4 << 19);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec, light, 16, 16);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		switch (orientation) {
			case (face_dir::minus_x):
			case (face_dir::plus_x):
			case (face_dir::minus_y):
			case (face_dir::plus_y):
				pp0 = p0; pp1 = p1; pp2 = p2; pp3 = p3;
				break ;
			case (face_dir::minus_z):
				pp0 = p3; pp1 = p2; pp2 = p1; pp3 = p0;
				break ;
			case (face_dir::plus_z):
				pp0 = p0; pp1 = p1; pp2 = p2; pp3 = p3;
				break ;
		}
		spec = getTex(face_dir::minus_y, value);
		light = chunk->computeLight(pos.x, pos.y - 1, pos.z) + (1 << 19);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec, light, 16, 16);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		switch (orientation) {
			case (face_dir::minus_x):
			case (face_dir::plus_x):
			case (face_dir::minus_y):
			case (face_dir::plus_y):
				pp0 = p5; pp1 = p4; pp2 = p7; pp3 = p6;
				break ;
			case (face_dir::minus_z):
				pp0 = p6; pp1 = p7; pp2 = p4; pp3 = p5;
				break ;
			case (face_dir::plus_z):
				pp0 = p5; pp1 = p4; pp2 = p7; pp3 = p6;
				break ;
		}
		spec = getTex(face_dir::plus_y, value);
		light = chunk->computeLight(pos.x, pos.y + 1, pos.z) + (2 << 19);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec, light, 16, 16);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		switch (orientation) {
			case (face_dir::minus_x):
				pp0 = p0; pp1 = p4; pp2 = p1; pp3 = p5;
				break ;
			case (face_dir::plus_x):
				pp0 = p5; pp1 = p1; pp2 = p4; pp3 = p0;
				break ;
			case (face_dir::minus_y):
				pp0 = p1; pp1 = p0; pp2 = p5; pp3 = p4;
				break ;
			case (face_dir::plus_y):
				pp0 = p4; pp1 = p5; pp2 = p0; pp3 = p1;
				break ;
			case (face_dir::minus_z):
				pp0 = p4; pp1 = p5; pp2 = p0; pp3 = p1;
				break ;
			case (face_dir::plus_z):
				pp0 = p4; pp1 = p5; pp2 = p0; pp3 = p1;
				break ;
		}
		spec = getTex(face_dir::plus_z, value);
		light = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec, light, 16, 16);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		switch (orientation) {
			case (face_dir::minus_x):
				pp0 = p6; pp1 = p2; pp2 = p7; pp3 = p3;
				break ;
			case (face_dir::plus_x):
				pp0 = p3; pp1 = p7; pp2 = p2; pp3 = p6;
				break ;
			case (face_dir::minus_y):
				pp0 = p2; pp1 = p3; pp2 = p6; pp3 = p7;
				break ;
			case (face_dir::plus_y):
				pp0 = p7; pp1 = p6; pp2 = p3; pp3 = p2;
				break ;
			case (face_dir::minus_z):
				pp0 = p2; pp1 = p3; pp2 = p6; pp3 = p7;
				break ;
			case (face_dir::plus_z):
				pp0 = p2; pp1 = p3; pp2 = p6; pp3 = p7;
				break ;
		}
		spec = getTex(face_dir::minus_z, value);
		light = chunk->computeLight(pos.x, pos.y, pos.z - 1) + (5 << 19);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec, light, 16, 16);
	}
}

void Carpet::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + one16th);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + one16th);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + one16th);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + one16th);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int spec = texture;
	int light, cornerLight, shade;
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		light = chunk->computeLight(pos.x - 1, pos.y, pos.z) + (3 << 19);
		utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec + (15 << 16), light, 16, 1);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		light = chunk->computeLight(pos.x + 1, pos.y, pos.z) + (4 << 19);
		utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec + (15 << 16), light, 16, 1);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		light = chunk->computeLight(pos.x, pos.y - 1, pos.z) + (1 << 19);
		utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec + (15 << 16), light, 16, 1);
	}
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		light = chunk->computeLight(pos.x, pos.y + 1, pos.z) + (2 << 19);
		utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec + (15 << 16), light, 16, 1);
	}
	std::array<int, 4> lights;
	light = chunk->computeLight(pos.x, pos.y, pos.z);
	cornerLight = chunk->computeSmoothLight(light, pos.x, pos.y, pos.z, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
	shade = chunk->computeShade(pos.x, pos.y, pos.z, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
	lights[0] = cornerLight + (shade << 12);
	cornerLight = chunk->computeSmoothLight(light, pos.x, pos.y, pos.z, {0, 1, 0, 1, 1, 0, 1, 0, 0});
	shade = chunk->computeShade(pos.x, pos.y, pos.z, {0, 1, 0, 1, 1, 0, 1, 0, 0});
	lights[1] = cornerLight + (shade << 12);
	cornerLight = chunk->computeSmoothLight(light, pos.x, pos.y, pos.z, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
	shade = chunk->computeShade(pos.x, pos.y, pos.z, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
	lights[2] = cornerLight + (shade << 12);
	cornerLight = chunk->computeSmoothLight(light, pos.x, pos.y, pos.z, {0, -1, 0, 1, -1, 0, 1, 0, 0});
	shade = chunk->computeShade(pos.x, pos.y, pos.z, {0, -1, 0, 1, -1, 0, 1, 0, 0});
	lights[3] = cornerLight + (shade << 12);
	utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, lights, 16, 16);
	if (utils::block::visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		light = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		cornerLight = chunk->computeSmoothLight(light, pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		lights[0] = cornerLight + (shade << 12) + (5 << 8);
		cornerLight = chunk->computeSmoothLight(light, pos.x, pos.y, pos.z - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		lights[1] = cornerLight + (shade << 12) + (5 << 8);
		cornerLight = chunk->computeSmoothLight(light, pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		lights[2] = cornerLight + (shade << 12) + (5 << 8);
		cornerLight = chunk->computeSmoothLight(light, pos.x, pos.y, pos.z - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		lights[3] = cornerLight + (shade << 12) + (5 << 8);
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, lights, 16, 16);
	}
}
