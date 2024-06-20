#include "Chunk.hpp"

const std::array<std::unique_ptr<Block>, S_BLOCKS_SIZE> s_blocks = {
	std::make_unique<Air>(), std::make_unique<GrassBlock>(), std::make_unique<OakLog>(), std::make_unique<Cactus>(), std::make_unique<Farmland>(), std::make_unique<DirtPath>(), std::make_unique<TNT>(), std::make_unique<Target>(),
	std::make_unique<CraftingTable>(), std::make_unique<Furnace>(), std::make_unique<OakStairsBottom>(), std::make_unique<OakStairsTop>(), std::make_unique<OakDoor>(), std::make_unique<OakTrapdoor>(), std::make_unique<StoneStairsBottom>(), std::make_unique<StoneStairsTop>(),
	std::make_unique<SmoothStoneStairsBottom>(), std::make_unique<SmoothStoneStairsTop>(), std::make_unique<CobbleStoneStairsBottom>(), std::make_unique<CobbleStoneStairsTop>(), std::make_unique<StoneBricksStairsBottom>(), std::make_unique<StoneBricksStairsTop>(), std::make_unique<Lever>(), std::make_unique<OakSign>(),
	std::make_unique<Bedrock>(), std::make_unique<Dirt>(), std::make_unique<SmoothStone>(), std::make_unique<Stone>(), std::make_unique<Cobblestone>(), std::make_unique<StoneBrick>(), std::make_unique<CrackedStoneBrick>(), std::make_unique<Sand>(),
	std::make_unique<Gravel>(), std::make_unique<OakLeaves>(), std::make_unique<OakPlanks>(), std::make_unique<Glass>(), std::make_unique<GlassPane>(), std::make_unique<RedstoneLamp>(), std::make_unique<StoneButton>(), std::make_unique<OakButton>(),
	std::make_unique<CoalOre>(), std::make_unique<IronOre>(), std::make_unique<DiamondOre>(), std::make_unique<CoalBlock>(), std::make_unique<IronBlock>(), std::make_unique<DiamondBlock>(), std::make_unique<RedstoneOre>(), std::make_unique<RedstoneBlock>(),
	std::make_unique<OakSlabBottom>(), std::make_unique<OakSlabTop>(), std::make_unique<OakFence>(), std::make_unique<StoneSlabBottom>(), std::make_unique<StoneSlabTop>(), std::make_unique<SmoothStoneSlabBottom>(), std::make_unique<SmoothStoneSlabTop>(), std::make_unique<CobbleStoneSlabBottom>(),
	std::make_unique<CobbleStoneSlabTop>(), std::make_unique<StoneBricksSlabBottom>(), std::make_unique<StoneBricksSlabTop>(), std::make_unique<Piston>(), std::make_unique<StickyPiston>(), std::make_unique<PistonHead>(), std::make_unique<MovingPiston>(), std::make_unique<Observer>(),
	std::make_unique<Poppy>(), std::make_unique<Dandelion>(), std::make_unique<BlueOrchid>(), std::make_unique<Allium>(), std::make_unique<CornFlower>(), std::make_unique<PinkTulip>(), std::make_unique<Grass>(), std::make_unique<SugarCane>(),
	std::make_unique<DeadBush>(), std::make_unique<OakSapling>(), std::make_unique<Torch>(), std::make_unique<RedstoneTorch>(), std::make_unique<RedstoneDust>(), std::make_unique<Repeater>(), std::make_unique<Comparator>(), std::make_unique<Chest>(),
	std::make_unique<WheatCrop>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(),
	std::make_unique<Water>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(),
	std::make_unique<Stick>(), std::make_unique<WoodenShovel>(), std::make_unique<StoneShovel>(), std::make_unique<IronShovel>(), std::make_unique<DiamondShovel>(), std::make_unique<WoodenAxe>(), std::make_unique<StoneAxe>(), std::make_unique<IronAxe>(),
	std::make_unique<DiamondAxe>(), std::make_unique<WoodenPickaxe>(), std::make_unique<StonePickaxe>(), std::make_unique<IronPickaxe>(), std::make_unique<DiamondPickaxe>(), std::make_unique<Bow>(), std::make_unique<Arrow>(), std::make_unique<WorldEditWand>(),
	std::make_unique<Coal>(), std::make_unique<Charcoal>(), std::make_unique<IronIngot>(), std::make_unique<Diamond>(), std::make_unique<Bucket>(), std::make_unique<WaterBucket>(), std::make_unique<WoodenHoe>(), std::make_unique<StoneHoe>(),
	std::make_unique<IronHoe>(), std::make_unique<DiamondHoe>(), std::make_unique<WheatSeeds>(), std::make_unique<Wheat>(), std::make_unique<Bread>(), std::make_unique<Apple>(), std::make_unique<Flint>(), std::make_unique<FlintAndSteel>(),
	std::make_unique<ZombieEgg>(), std::make_unique<SkeletonEgg>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(),
	std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(), std::make_unique<TBD>(),
};

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
	std::array<int, 4> deltas;
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = (this->texX(face_dir::minus_x, value) << 4) + (this->texY(face_dir::minus_x, value) << 12) + (3 << 19);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		deltas[0] = (cornerLight << 24) + (shade << 22);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		deltas[1] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		deltas[2] = (cornerLight << 24) + (shade << 22) + (16 << 8) + (1 << 18);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		deltas[3] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17) + (16 << 8) + (1 << 18);
		utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, deltas, {0, 1, (1 << 8), 1 + (1 << 8)});
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = (this->texX(face_dir::plus_x, value) << 4) + (this->texY(face_dir::plus_x, value) << 12) + (4 << 19);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		deltas[0] = (cornerLight << 24) + (shade << 22);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		deltas[1] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		deltas[2] = (cornerLight << 24) + (shade << 22) + (16 << 8) + (1 << 18);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		deltas[3] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17) + (16 << 8) + (1 << 18);
		utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, deltas, {0, 1, (1 << 8), 1 + (1 << 8)});
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = (this->texX(face_dir::minus_y, value) << 4) + (this->texY(face_dir::minus_y, value) << 12) + (1 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		deltas[0] = (cornerLight << 24) + (shade << 22);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		deltas[1] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		deltas[2] = (cornerLight << 24) + (shade << 22) + (16 << 8) + (1 << 18);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		deltas[3] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17) + (16 << 8) + (1 << 18);
		utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, deltas, {0, 1, (1 << 8), 1 + (1 << 8)});
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = (this->texX(face_dir::plus_y, value) << 4) + (this->texY(face_dir::plus_y, value) << 12) + (2 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		deltas[0] = (cornerLight << 24) + (shade << 22);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		deltas[1] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		deltas[2] = (cornerLight << 24) + (shade << 22) + (16 << 8) + (1 << 18);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		deltas[3] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17) + (16 << 8) + (1 << 18);
		utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, deltas, {0, 1, (1 << 8), 1 + (1 << 8)});
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = (this->texX(face_dir::plus_z, value) << 4) + (this->texY(face_dir::plus_z, value) << 12);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		deltas[0] = (cornerLight << 24) + (shade << 22);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		deltas[1] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		deltas[2] = (cornerLight << 24) + (shade << 22) + (16 << 8) + (1 << 18);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		deltas[3] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17) + (16 << 8) + (1 << 18);
		utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, deltas, {0, 1, (1 << 8), 1 + (1 << 8)});
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = (this->texX(face_dir::minus_z, value) << 4) + (this->texY(face_dir::minus_z, value) << 12) + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		deltas[0] = (cornerLight << 24) + (shade << 22);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		deltas[1] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		deltas[2] = (cornerLight << 24) + (shade << 22) + (16 << 8) + (1 << 18);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		deltas[3] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17) + (16 << 8) + (1 << 18);
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, deltas, {0, 1, (1 << 8), 1 + (1 << 8)});
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

	int spec = (textureX << 4) + (textureY << 12) + (0 << 19);
	int faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	spec += (faceLight << 24);
	utils::shader::addQuads(vertices, {p0, p5, p2, p7}, spec, 16, 16, 0, 8); // recto
	utils::shader::addQuads(vertices, {p1, p4, p3, p6}, spec, 16, 16, 0, 8);
	utils::shader::addQuads(vertices, {p5, p0, p7, p2}, spec, 16, 16, 0, 8, true); // verso
	utils::shader::addQuads(vertices, {p4, p1, p6, p3}, spec, 16, 16, 0, 8, true);
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
	std::array<int, 4> deltas;
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = (this->texX(face_dir::minus_x, value) << 4) + (this->texY(face_dir::minus_x, value) << 12) + (3 << 19);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		deltas[0] = (cornerLight << 24) + (shade << 22);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		deltas[1] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		deltas[2] = (cornerLight << 24) + (shade << 22) + (15 << 8) + (1 << 18);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		deltas[3] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17) + (15 << 8) + (1 << 18);
		utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec + (1 << 8), deltas, {0, 1, (1 << 8), 1 + (1 << 8)});
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = (this->texX(face_dir::plus_x, value) << 4) + (this->texY(face_dir::plus_x, value) << 12) + (4 << 19);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		deltas[0] = (cornerLight << 24) + (shade << 22);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		deltas[1] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		deltas[2] = (cornerLight << 24) + (shade << 22) + (15 << 8) + (1 << 18);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		deltas[3] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17) + (15 << 8) + (1 << 18);
		utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec + (1 << 8), deltas, {0, 1, (1 << 8), 1 + (1 << 8)});
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = (this->texX(face_dir::minus_y, value) << 4) + (this->texY(face_dir::minus_y, value) << 12) + (1 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		deltas[0] = (cornerLight << 24) + (shade << 22);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		deltas[1] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		deltas[2] = (cornerLight << 24) + (shade << 22) + (15 << 8) + (1 << 18);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		deltas[3] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17) + (15 << 8) + (1 << 18);
		utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec + (1 << 8), deltas, {0, 1, (1 << 8), 1 + (1 << 8)});
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = (this->texX(face_dir::plus_y, value) << 4) + (this->texY(face_dir::plus_y, value) << 12) + (2 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		deltas[0] = (cornerLight << 24) + (shade << 22);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		deltas[1] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		deltas[2] = (cornerLight << 24) + (shade << 22) + (15 << 8) + (1 << 18);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		deltas[3] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17) + (15 << 8) + (1 << 18);
		utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec + (1 << 8), deltas, {0, 1, (1 << 8), 1 + (1 << 8)});
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = (this->texX(face_dir::plus_z, value) << 4) + (this->texY(face_dir::plus_z, value) << 12);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		deltas[0] = (cornerLight << 24) + (shade << 22);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		deltas[1] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		deltas[2] = (cornerLight << 24) + (shade << 22) + (16 << 8) + (1 << 18);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		deltas[3] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17) + (16 << 8) + (1 << 18);
		utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, deltas, {0, 1, (1 << 8), 1 + (1 << 8)});
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = (this->texX(face_dir::minus_z, value) << 4) + (this->texY(face_dir::minus_z, value) << 12) + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		deltas[0] = (cornerLight << 24) + (shade << 22);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		deltas[1] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		deltas[2] = (cornerLight << 24) + (shade << 22) + (16 << 8) + (1 << 18);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		deltas[3] = (cornerLight << 24) + (shade << 22) + 16 + (1 << 17) + (16 << 8) + (1 << 18);
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, deltas, {0, 1, (1 << 8), 1 + (1 << 8)});
	}
}

void SlabBottom::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0.5f);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0.5f);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0.5f);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0.5f);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int spec, faceLight;
	int baseSpec = (textureX << 4) + (textureY << 12);
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = baseSpec + (3 << 19);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec + (8 << 8), 16, 8, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = baseSpec + (4 << 19);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec + (8 << 8), 16, 8, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = baseSpec + (1 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec + (8 << 8), 16, 8, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = baseSpec + (2 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec + (8 << 8), 16, 8, 0, 8);
	}
	spec = baseSpec + (0 << 19);
	faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	spec += (faceLight << 24);
	utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, 16, 16, 0, 8);
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = baseSpec + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, 16, 16, 0, 8);
	}
}

void SlabTop::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0.5f);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0.5f);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0.5f);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0.5f);

	int spec, faceLight;
	int baseSpec = (textureX << 4) + (textureY << 12);
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = baseSpec + (3 << 19);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, 16, 8, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = baseSpec + (4 << 19);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, 16, 8, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = baseSpec + (1 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, 16, 8, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = baseSpec + (2 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, 16, 8, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = baseSpec + (0 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, 16, 16, 0, 8);
	}
	spec = baseSpec + (5 << 19);
	faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	spec += (faceLight << 24);
	utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, 16, 16, 0, 8);
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

	int spec = (textureX << 4) + (texY(face_dir::minus_x, !!(value & mask::redstone::powered)) << 12) + (0 << 19) + (15 << 24);
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
	spec += 7 + (6 << 8);
	utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, 2, 10, 0, 8); // -x
	utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, 2, 10, 0, 8); // +x
	utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, 2, 10, 0, 8); // -y
	utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, 2, 10, 0, 8); // +y
	utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, 2, 2, 0, 8);  // +z
	if (((value >> offset::blocks::orientation) & 0x7) != face_dir::minus_z) {
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec + (8 << 8), 2, 2, 0, 8); // -z
	}
}

void StairsBottom::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0.0f, start.y + pos.y + 0.0f, pos.z + 1.0f);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1.0f, start.y + pos.y + 0.0f, pos.z + 1.0f);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0.0f, start.y + pos.y + 0.0f, pos.z + 0.0f);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1.0f, start.y + pos.y + 0.0f, pos.z + 0.0f);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0.0f, start.y + pos.y + 1.0f, pos.z + 1.0f);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1.0f, start.y + pos.y + 1.0f, pos.z + 1.0f);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0.0f, start.y + pos.y + 1.0f, pos.z + 0.0f);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1.0f, start.y + pos.y + 1.0f, pos.z + 0.0f);

	int corners = (value >> offset::blocks::bitfield) & 0xF;
	int baseSpec = (textureX << 4) + (textureY << 12);
	int spec, faceLight;
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = baseSpec + (3 << 19);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		spec += (faceLight << 24);
		switch (corners & (corners::mm | corners::mp)) {
			case corners::mm | corners::mp:
				utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, 16, 16, 0, 8);
				break ;
			case corners::mm:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, -0.5f, 0.0f), p0, p6 + glm::vec3(0.0f, -0.5f, 0.5f), p2 + glm::vec3(0.0f, 0.0f, 0.5f)}, spec + 8, 8, 8, 0, 8);
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, 0.0f, -0.5f), p0 + glm::vec3(0.0f, 0.0f, -0.5f), p6, p2}, spec + (8 << 8), 16, 8, 0, 8);
				break ;
			case corners::mp:
				utils::shader::addQuads(vertices, {p4, p0 + glm::vec3(0.0f, 0.5f, 0.0f), p6 + glm::vec3(0.0f, 0.0f, 0.5f), p2 + glm::vec3(0.0f, 0.5f, 0.5f)}, spec, 8, 8, 0, 8);
			default:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, 0.0f, -0.5f), p0 + glm::vec3(0.0f, 0.0f, -0.5f), p6, p2}, spec + (8 << 8), 16, 8, 0, 8);
				break ;
		}
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = baseSpec + (4 << 19);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		spec += (faceLight << 24);
		switch (corners & (corners::pm | corners::pp)) {
			case corners::pm | corners::pp:
				utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, 16, 16, 0, 8);
				break ;
			case corners::pm:
				utils::shader::addQuads(vertices, {p1, p5 + glm::vec3(0.0f, -0.5f, 0.0f), p3 + glm::vec3(0.0f, 0.0f, 0.5f), p7 + glm::vec3(0.0f, -0.5f, 0.5f)}, spec, 8, 8, 0, 8);
				utils::shader::addQuads(vertices, {p1 + glm::vec3(0.0f, 0.0f, -0.5f), p5 + glm::vec3(0.0f, 0.0f, -0.5f), p3, p7}, spec + (8 << 8), 16, 8, 0, 8);
				break ;
			case corners::pp:
				utils::shader::addQuads(vertices, {p1 + glm::vec3(0.0f, 0.5f, 0.0f), p5, p3 + glm::vec3(0.0f, 0.5f, 0.5f), p7 + glm::vec3(0.0f, 0.0f, 0.5f)}, spec + 8, 8, 8, 0, 8);
			default:
				utils::shader::addQuads(vertices, {p1 + glm::vec3(0.0f, 0.0f, -0.5f), p5 + glm::vec3(0.0f, 0.0f, -0.5f), p3, p7}, spec + (8 << 8), 16, 8, 0, 8);
				break ;
		}
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = baseSpec + (1 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		spec += (faceLight << 24);
		switch (corners & (corners::mm | corners::pm)) {
			case corners::mm | corners::pm:
				utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, 16, 16, 0, 8);
				break ;
			case corners::mm:
				utils::shader::addQuads(vertices, {p0, p1 + glm::vec3(-0.5f, 0.0f, 0.0f), p2 + glm::vec3(0.0f, 0.0f, 0.5f), p3 + glm::vec3(-0.5f, 0.0f, 0.5f)}, spec, 8, 8, 0, 8);
				utils::shader::addQuads(vertices, {p0 + glm::vec3(0.0f, 0.0f, -0.5f), p1 + glm::vec3(0.0f, 0.0f, -0.5f), p2, p3}, spec + (8 << 8), 16, 8, 0, 8);
				break ;
			case corners::pm:
				utils::shader::addQuads(vertices, {p0 + glm::vec3(0.5f, 0.0f, 0.0f), p1, p2 + glm::vec3(0.5f, 0.0f, 0.5f), p3 + glm::vec3(0.0f, 0.0f, 0.5f)}, spec + 8, 8, 8, 0, 8);
			default:
				utils::shader::addQuads(vertices, {p0 + glm::vec3(0.0f, 0.0f, -0.5f), p1 + glm::vec3(0.0f, 0.0f, -0.5f), p2, p3}, spec + (8 << 8), 16, 8, 0, 8);
				break ;
		}
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = baseSpec + (2 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		spec += (faceLight << 24);
		switch (corners & (corners::mp | corners::pp)) {
			case corners::mp | corners::pp:
				utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, 16, 16, 0, 8);
				break ;
			case corners::mp:
				utils::shader::addQuads(vertices, {p5 + glm::vec3(-0.5f, 0.0f, 0.0f), p4, p7 + glm::vec3(-0.5f, 0.0f, 0.5f), p6 + glm::vec3(0.0f, 0.0f, 0.5f)}, spec + 8, 8, 8, 0, 8);
				utils::shader::addQuads(vertices, {p5 + glm::vec3(0.0f, 0.0f, -0.5f), p4 + glm::vec3(0.0f, 0.0f, -0.5f), p7, p6}, spec + (8 << 8), 16, 8, 0, 8);
				break ;
			case corners::pp:
				utils::shader::addQuads(vertices, {p5, p4 + glm::vec3(0.5f, 0.0f, 0.0f), p7 + glm::vec3(0.0f, 0.0f, 0.5f), p6 + glm::vec3(0.5f, 0.0f, 0.5f)}, spec, 8, 8, 0, 8);
			default:
				utils::shader::addQuads(vertices, {p5 + glm::vec3(0.0f, 0.0f, -0.5f), p4 + glm::vec3(0.0f, 0.0f, -0.5f), p7, p6}, spec + (8 << 8), 16, 8, 0, 8);
				break ;
		}
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = baseSpec + (0 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		spec += (faceLight << 24);
		switch (corners & (corners::mp | corners::pp)) {
			case corners::mp:
				utils::shader::addQuads(vertices, {p4, p5 + glm::vec3(-0.5f, 0.0f, 0.0f), p0 + glm::vec3(0.0f, 0.5f, 0.0f), p1 + glm::vec3(-0.5f, 0.5f, 0.0f)}, spec, 8, 8, 0, 8);
				break ;
			case corners::pp:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, 0.0f, 0.0f), p5, p0 + glm::vec3(0.5f, 0.5f, 0.0f), p1 + glm::vec3(0.0f, 0.5f, 0.0f)}, spec + 8, 8, 8, 0, 8);
				break ;
			case corners::mp | corners::pp:
				utils::shader::addQuads(vertices, {p4, p5, p0 + glm::vec3(0.0f, 0.5f, 0.0f), p1 + glm::vec3(0.0f, 0.5f, 0.0f)}, spec, 16, 8, 0, 8);
				break ;
		}
		switch (corners & (corners::mm | corners::pm)) {
			case corners::mm:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, -0.5f, 0.0f), p5 + glm::vec3(-0.5f, -0.5f, 0.0f), p0, p1 + glm::vec3(-0.5f, 0.0f, 0.0f)}, spec + (8 << 8), 8, 8, 0, 8);
				break ;
			case corners::pm:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, -0.5f, 0.0f), p5 + glm::vec3(0.0f, -0.5f, 0.0f), p0 + glm::vec3(0.5f, 0.0f, 0.0f), p1}, spec + 8 + (8 << 8), 8, 8, 0, 8);
				break ;
			case corners::mm | corners::pm:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, -0.5f, 0.0f), p5 + glm::vec3(0.0f, -0.5f, 0.0f), p0, p1}, spec + (8 << 8), 16, 8, 0, 8);
				break ;
		}
	}
	// up of first step
	spec = baseSpec + (0 << 19);
	faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	spec += (faceLight << 24);
	switch (corners & (corners::mp | corners::pp)) {
		case corners::pp:
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, 0.0f, -0.5f), p5 + glm::vec3(-0.5f, 0.0f, -0.5f), p0 + glm::vec3(0.0f, 0.5f, -0.5f), p1 + glm::vec3(-0.5f, 0.5f, -0.5f)}, spec, 8, 8, 0, 8);
			break ;
		case corners::mp:
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, 0.0f, -0.5f), p5 + glm::vec3(0.0f, 0.0f, -0.5f), p0 + glm::vec3(0.5f, 0.5f, -0.5f), p1 + glm::vec3(0.0f, 0.5f, -0.5f)}, spec + 8, 8, 8, 0, 8);
			break ;
		case corners::mp | corners::pp:
			break ;
		default:
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, 0.0f, -0.5f), p5 + glm::vec3(0.0f, 0.0f, -0.5f), p0 + glm::vec3(0.0f, 0.5f, -0.5f), p1 + glm::vec3(0.0f, 0.5f, -0.5f)}, spec, 16, 8, 0, 8);
			break ;
	}
	switch (corners & (corners::mm | corners::pm)) {
		case corners::pm:
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, -0.5f, -0.5f), p5 + glm::vec3(-0.5f, -0.5f, -0.5f), p0 + glm::vec3(0.0f, 0.0f, -0.5f), p1 + glm::vec3(-0.5f, 0.0f, -0.5f)}, spec + (8 << 8), 8, 8, 0, 8);
			break ;
		case corners::mm:
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, -0.5f, -0.5f), p5 + glm::vec3(0.0f, -0.5f, -0.5f), p0 + glm::vec3(0.5f, 0.0f, -0.5f), p1 + glm::vec3(0.0f, 0.0f, -0.5f)}, spec + 8 + (8 << 8), 8, 8, 0, 8);
			break ;
		case corners::mm | corners::pm:
			break;
		default:
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, -0.5f, -0.5f), p5 + glm::vec3(0.0f, -0.5f, -0.5f), p0 + glm::vec3(0.0f, 0.0f, -0.5f), p1 + glm::vec3(0.0f, 0.0f, -0.5f)}, spec + (8 << 8), 16, 8, 0, 8);
			break ;
	}
	spec = baseSpec + (faceLight << 24);
	// front minus_x of second step
	spec += (3 << 19);
	switch (corners) {
		case corners::pm | corners::pp: // full width
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, 0.0f, 0.0f), p0 + glm::vec3(0.5f, 0.0f, 0.0f), p4 + glm::vec3(0.5f, 0.0f, -0.5f), p0 + glm::vec3(0.5f, 0.0f, -0.5f)}, spec, 16, 8, 0, 8);
			break ;
		case corners::pm: // only 'right'
		case corners::pm | corners::pp | corners::mp:
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, -0.5f, 0.0f), p0 + glm::vec3(0.5f, 0.0f, 0.0f), p4 + glm::vec3(0.5f, -0.5f, -0.5f), p0 + glm::vec3(0.5f, 0.0f, -0.5f)}, spec + 8, 8, 8, 0, 8);
			break ;
		case corners::pp: // only 'left'
		case corners::pm | corners::pp | corners::mm:
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, 0.0f, 0.0f), p0 + glm::vec3(0.5f, 0.5f, 0.0f), p4 + glm::vec3(0.5f, 0.0f, -0.5f), p0 + glm::vec3(0.5f, 0.5f, -0.5f)}, spec, 16, 8, 0, 8);
			break ;
	}
	// front plus_x of second step
	spec += (1 << 19);
	switch (corners) {
		case corners::mm | corners::mp: // full width
			utils::shader::addQuads(vertices, {p1 + glm::vec3(-0.5f, 0.0f, 0.0f), p5 + glm::vec3(-0.5f, 0.0f, 0.0f), p1 + glm::vec3(-0.5f, 0.0f, -0.5f), p5 + glm::vec3(-0.5f, 0.0f, -0.5f)}, spec, 16, 8, 0, 8);
			break ;
		case corners::mp: // only 'right'
		case corners::mm | corners::mp | corners::pm:
			utils::shader::addQuads(vertices, {p1 + glm::vec3(-0.5f, 0.5f, 0.0f), p5 + glm::vec3(-0.5f, 0.0f, 0.0f), p1 + glm::vec3(-0.5f, 0.5f, -0.5f), p5 + glm::vec3(-0.5f, 0.0f, -0.5f)}, spec + 8, 8, 8, 0, 8);
			break ;
		case corners::mm: // only 'left'
		case corners::mm | corners::mp | corners::pp:
			utils::shader::addQuads(vertices, {p1 + glm::vec3(-0.5f, 0.0f, 0.0f), p5 + glm::vec3(-0.5f, -0.5f, 0.0f), p1 + glm::vec3(-0.5f, 0.0f, -0.5f), p5 + glm::vec3(-0.5f, -0.5f, -0.5f)}, spec, 8, 8, 0, 8);
			break ;
	}
	// front minus_y of second step
	spec -= (3 << 19);
	switch (corners) {
		case corners::mp | corners::pp: // full width
			utils::shader::addQuads(vertices, {p0 + glm::vec3(0.0f, 0.5f, 0.0f), p1 + glm::vec3(0.0f, 0.5f, 0.0f), p0 + glm::vec3(0.0f, 0.5f, -0.5f), p1 + glm::vec3(0.0f, 0.5f, -0.5f)}, spec, 16, 8, 0, 8);
			break ;
		case corners::pp: // only 'right'
		case corners::mp | corners::pp | corners::mm:
			utils::shader::addQuads(vertices, {p0 + glm::vec3(0.5f, 0.5f, 0.0f), p1 + glm::vec3(0.0f, 0.5f, 0.0f), p0 + glm::vec3(0.5f, 0.5f, -0.5f), p1 + glm::vec3(0.0f, 0.5f, -0.5f)}, spec + 8, 8, 8, 0, 8);
			break ;
		case corners::mp: // only 'left'
		case corners::mp | corners::pp | corners::pm:
			utils::shader::addQuads(vertices, {p0 + glm::vec3(0.0f, 0.5f, 0.0f), p1 + glm::vec3(-0.5f, 0.5f, 0.0f), p0 + glm::vec3(0.0f, 0.5f, -0.5f), p1 + glm::vec3(-0.5f, 0.5f, -0.5f)}, spec, 8, 8, 0, 8);
			break ;
	}
	// front plus_y of second step
	spec += (1 << 19);
	switch (corners) {
		case corners::mm | corners::pm: // full width
			utils::shader::addQuads(vertices, {p5 + glm::vec3(0.0f, -0.5f, 0.0f), p4 + glm::vec3(0.0f, -0.5f, 0.0f), p5 + glm::vec3(0.0f, -0.5f, -0.5f), p4 + glm::vec3(0.0f, -0.5f, -0.5f)}, spec, 16, 8, 0, 8);
			break ;
		case corners::pm: // only 'left'
		case corners::mm | corners::pm | corners::mp:
			utils::shader::addQuads(vertices, {p5 + glm::vec3(0.0f, -0.5f, 0.0f), p4 + glm::vec3(0.5f, -0.5f, 0.0f), p5 + glm::vec3(0.0f, -0.5f, -0.5f), p4 + glm::vec3(0.5f, -0.5f, -0.5f)}, spec, 8, 8, 0, 8);
			break ;
		case corners::mm: // only 'right'
		case corners::mm | corners::pm | corners::pp:
			utils::shader::addQuads(vertices, {p5 + glm::vec3(-0.5f, -0.5f, 0.0f), p4 + glm::vec3(0.0f, -0.5f, 0.0f), p5 + glm::vec3(-0.5f, -0.5f, -0.5f), p4 + glm::vec3(0.0f, -0.5f, -0.5f)}, spec + 8, 8, 8, 0, 8);
			break ;
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = baseSpec + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, 16, 16, 0, 8);
	}
}

void StairsTop::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int corners = (value >> offset::blocks::bitfield) & 0xF;
	int baseSpec = (textureX << 4) + (textureY << 12);
	int spec, faceLight;
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = baseSpec + (3 << 19);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		spec += (faceLight << 24);
		switch (corners & (corners::mm | corners::mp)) {
			case corners::mm | corners::mp:
				utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, 16, 16, 0, 8);
				break ;
			case corners::mm:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.f, -.5f, -.5f), p0 + glm::vec3(0.f, 0.f, -.5f), p6 + glm::vec3(0.f, -.5f, 0.f), p2}, spec + 8 + (8 << 8), 8, 8, 0, 8);
				utils::shader::addQuads(vertices, {p4, p0, p6 + glm::vec3(0.f, 0.f, .5f), p2 + glm::vec3(0.f, 0.f, .5f)}, spec, 16, 8, 0, 8);
				break ;
			case corners::mp:
				utils::shader::addQuads(vertices, {p4 + glm::vec3(0.f, 0.f, -.5f), p0 + glm::vec3(0.f, 0.5f, -.5f), p6, p2 + glm::vec3(0.f, .5f, 0.f)}, spec + (8 << 8), 8, 8, 0, 8);
			default:
				utils::shader::addQuads(vertices, {p4, p0, p6 + glm::vec3(0.f, 0.f, .5f), p2 + glm::vec3(0.f, 0.f, .5f)}, spec, 16, 8, 0, 8);
				break ;
		}
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = baseSpec + (4 << 19);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		spec += (faceLight << 24);
		switch (corners & (corners::pm | corners::pp)) {
			case corners::pm | corners::pp:
				utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, 16, 16, 0, 8);
				break ;
			case corners::pm:
				utils::shader::addQuads(vertices, {p1 + glm::vec3(0.f, 0.f, -.5f), p5 + glm::vec3(0.f, -0.5f, -.5f), p3, p7 + glm::vec3(0.f, -.5f, 0.f)}, spec + (8 << 8), 8, 8, 0, 8);
				utils::shader::addQuads(vertices, {p1, p5, p3 + glm::vec3(0.f, 0.f, .5f), p7 + glm::vec3(0.f, 0.f, .5f)}, spec, 16, 8, 0, 8);
				break ;
			case corners::pp:
				utils::shader::addQuads(vertices, {p1 + glm::vec3(0.f, .5f, -.5f), p5 + glm::vec3(0.f, 0.f, -.5f), p3 + glm::vec3(0.f, .5f, 0.f), p7}, spec + 8 + (8 << 8), 8, 8, 0, 8);
			default:
				utils::shader::addQuads(vertices, {p1, p5, p3 + glm::vec3(0.f, 0.f, .5f), p7 + glm::vec3(0.f, 0.f, .5f)}, spec, 16, 8, 0, 8);
				break ;
		}
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = baseSpec + (1 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		spec += (faceLight << 24);
		switch (corners & (corners::mm | corners::pm)) {
			case corners::mm | corners::pm:
				utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, 16, 16, 0, 8);
				break ;
			case corners::mm:
				utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -.5f), p1 + glm::vec3(-.5f, .0f, -.5f), p2, p3 + glm::vec3(-.5f, .0f, 0.f)}, spec + (8 << 8), 8, 8, 0, 8);
				utils::shader::addQuads(vertices, {p0, p1, p2 + glm::vec3(0.f, 0.f, .5f), p3 + glm::vec3(0.f, 0.f, .5f)}, spec, 16, 8, 0, 8);
				break ;
			case corners::pm:
				utils::shader::addQuads(vertices, {p0 + glm::vec3(.5f, .0f, -.5f), p1 + glm::vec3(0.f, 0.f, -.5f), p2 + glm::vec3(0.5f, .0f, 0.f), p3}, spec + 8 + (8 << 8), 8, 8, 0, 8);
			default:
				utils::shader::addQuads(vertices, {p0, p1, p2 + glm::vec3(0.f, 0.f, .5f), p3 + glm::vec3(0.f, 0.f, .5f)}, spec, 16, 8, 0, 8);
				break ;
		}
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = baseSpec + (2 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		spec += (faceLight << 24);
		switch (corners & (corners::mp | corners::pp)) {
			case corners::mp | corners::pp:
				utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, 16, 16, 0, 8);
				break ;
			case corners::mp:
				utils::shader::addQuads(vertices, {p5 + glm::vec3(-.5f, .0f, -.5f), p4 + glm::vec3(0.f, 0.f, -.5f), p7 + glm::vec3(-0.5f, .0f, 0.f), p6}, spec + 8 + (8 << 8), 8, 8, 0, 8);
				utils::shader::addQuads(vertices, {p5, p4, p7 + glm::vec3(0.f, 0.f, .5f), p6 + glm::vec3(0.f, 0.f, .5f)}, spec, 16, 8, 0, 8);
				break ;
			case corners::pp:
				utils::shader::addQuads(vertices, {p5 + glm::vec3(0.f, 0.f, -.5f), p4 + glm::vec3(.5f, .0f, -.5f), p7, p6 + glm::vec3(.5f, .0f, 0.f)}, spec + (8 << 8), 8, 8, 0, 8);
			default:
				utils::shader::addQuads(vertices, {p5, p4, p7 + glm::vec3(0.f, 0.f, .5f), p6 + glm::vec3(0.f, 0.f, .5f)}, spec, 16, 8, 0, 8);
				break ;
		}
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = baseSpec + (0 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, 16, 16, 0, 8);
	}
	// 'down' of first step
	spec = baseSpec + (5 << 19);
	faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	spec += (faceLight << 24);
	switch (corners & (corners::mp | corners::pp)) {
		case corners::pp:
			utils::shader::addQuads(vertices, {p2 + glm::vec3(.0f, .5f, .5f), p3 + glm::vec3(-.5f, .5f, .5f), p6 + glm::vec3(.0f, .0f, .5f), p7 + glm::vec3(-.5f, .0f, .5f)}, spec, 8, 8, 0, 8);
			break ;
		case corners::mp:
			utils::shader::addQuads(vertices, {p2 + glm::vec3(.5f, .5f, .5f), p3 + glm::vec3(.0f, .5f, .5f), p6 + glm::vec3(.5f, .0f, .5f), p7 + glm::vec3(.0f, .0f, .5f)}, spec + 8, 8, 8, 0, 8);
			break ;
		case corners::mp | corners::pp:
			break ;
		default:
			utils::shader::addQuads(vertices, {p2 + glm::vec3(.0f, .5f, .5f), p3 + glm::vec3(.0f, .5f, .5f), p6 + glm::vec3(.0f, .0f, .5f), p7 + glm::vec3(.0f, .0f, .5f)}, spec, 16, 8, 0, 8);
			break ;
	}
	switch (corners & (corners::mm | corners::pm)) {
		case corners::pm:
			utils::shader::addQuads(vertices, {p2 + glm::vec3(.0f, .0f, .5f), p3 + glm::vec3(-.5f, .0f, .5f), p6 + glm::vec3(.0f, -.5f, .5f), p7 + glm::vec3(-.5f, -.5f, .5f)}, spec + (8 << 8), 8, 8, 0, 8);
			break ;
		case corners::mm:
			utils::shader::addQuads(vertices, {p2 + glm::vec3(.5f, .0f, .5f), p3 + glm::vec3(.0f, .0f, .5f), p6 + glm::vec3(.5f, -.5f, .5f), p7 + glm::vec3(.0f, -.5f, .5f)}, spec + 8 + (8 << 8), 8, 8, 0, 8);
			break ;
		case corners::mm | corners::pm:
			break;
		default:
			utils::shader::addQuads(vertices, {p2 + glm::vec3(.0f, .0f, .5f), p3 + glm::vec3(.0f, .0f, .5f), p6 + glm::vec3(.0f, -.5f, .5f), p7 + glm::vec3(.0f, -.5f, .5f)}, spec + (8 << 8), 16, 8, 0, 8);
			break ;
	}
	spec = baseSpec + (faceLight << 24);
	// front minus_x of second step
	spec += (3 << 19);
	switch (corners) {
		case corners::pm | corners::pp: // full width
			utils::shader::addQuads(vertices, {p4 + glm::vec3(.5f, .0f, -.5f), p0 + glm::vec3(.5f, .0f, -.5f), p4 + glm::vec3(.5f, .0f, -1.0f), p0 + glm::vec3(.5f, .0f, -1.0f)}, spec, 16, 8, 0, 8);
			break ;
		case corners::pm: // only 'right'
		case corners::pm | corners::pp | corners::mp:
			utils::shader::addQuads(vertices, {p4 + glm::vec3(.5f, -.5f, -.5f), p0 + glm::vec3(.5f, .0f, -.5f), p4 + glm::vec3(.5f, -.5f, -1.0f), p0 + glm::vec3(.5f, .0f, -1.0f)}, spec + 8, 8, 8, 0, 8);
			break ;
		case corners::pp: // only 'left'
		case corners::pm | corners::pp | corners::mm:
			utils::shader::addQuads(vertices, {p4 + glm::vec3(.5f, .0f, -.5f), p0 + glm::vec3(.5f, .5f, -.5f), p4 + glm::vec3(.5f, .0f, -1.0f), p0 + glm::vec3(.5f, .5f, -1.0f)}, spec, 8, 8, 0, 8);
			break ;
	}
	// front plus_x of second step
	spec += (1 << 19);
	switch (corners) {
		case corners::mm | corners::mp: // full width
			utils::shader::addQuads(vertices, {p1 + glm::vec3(-.5f, .0f, -.5f), p5 + glm::vec3(-.5f, .0f, -.5f), p1 + glm::vec3(-.5f, .0f, -1.0f), p5 + glm::vec3(-.5f, .0f, -1.0f)}, spec, 16, 8, 0, 8);
			break ;
		case corners::mp: // only 'right'
		case corners::mm | corners::mp | corners::pm:
			utils::shader::addQuads(vertices, {p1 + glm::vec3(-.5f, .5f, -.5f), p5 + glm::vec3(-.5f, .0f, -.5f), p1 + glm::vec3(-.5f, .5f, -1.0f), p5 + glm::vec3(-.5f, .0f, -1.0f)}, spec + 8, 8, 8, 0, 8);
			break ;
		case corners::mm: // only 'left'
		case corners::mm | corners::mp | corners::pp:
			utils::shader::addQuads(vertices, {p1 + glm::vec3(-.5f, .0f, -.5f), p5 + glm::vec3(-.5f, -.5f, -.5f), p1 + glm::vec3(-.5f, .0f, -1.0f), p5 + glm::vec3(-.5f, -.5f, -1.0f)}, spec, 8, 8, 0, 8);
			break ;
	}
	// front minus_y of second step
	spec -= (3 << 19);
	switch (corners) {
		case corners::mp | corners::pp: // full width
			utils::shader::addQuads(vertices, {p0 + glm::vec3(.0f, .5f, -.5f), p1 + glm::vec3(.0f, .5f, -.5f), p0 + glm::vec3(.0f, .5f, -1.0f), p1 + glm::vec3(.0f, .5f, -1.0f)}, spec, 16, 8, 0, 8);
			break ;
		case corners::pp: // only 'right'
		case corners::mp | corners::pp | corners::mm:
			utils::shader::addQuads(vertices, {p0 + glm::vec3(.5f, .5f, -.5f), p1 + glm::vec3(.0f, .5f, -.5f), p0 + glm::vec3(.5f, .5f, -1.0f), p1 + glm::vec3(.0f, .5f, -1.0f)}, spec + 8, 8, 8, 0, 8);
			break ;
		case corners::mp: // only 'left'
		case corners::mp | corners::pp | corners::pm:
			utils::shader::addQuads(vertices, {p0 + glm::vec3(.0f, .5f, -.5f), p1 + glm::vec3(-.5f, .5f, -.5f), p0 + glm::vec3(.0f, .5f, -1.0f), p1 + glm::vec3(-.5f, .5f, -1.0f)}, spec, 8, 8, 0, 8);
			break ;
	}
	// front plus_y of second step
	spec += (1 << 19);
	switch (corners) {
		case corners::mm | corners::pm: // full width
			utils::shader::addQuads(vertices, {p5 + glm::vec3(.0f, -.5f, -.5f), p4 + glm::vec3(.0f, -.5f, -.5f), p5 + glm::vec3(.0f, -.5f, -1.0f), p4 + glm::vec3(.0f, -.5f, -1.0f)}, spec, 16, 8, 0, 8);
			break ;
		case corners::pm: // only 'left'
		case corners::mm | corners::pm | corners::mp:
			utils::shader::addQuads(vertices, {p5 + glm::vec3(.0f, -.5f, -.5f), p4 + glm::vec3(.5f, -.5f, -.5f), p5 + glm::vec3(.0f, -.5f, -1.0f), p4 + glm::vec3(.5f, -.5f, -1.0f)}, spec, 8, 8, 0, 8);
			break ;
		case corners::mm: // only 'right'
		case corners::mm | corners::pm | corners::pp:
			utils::shader::addQuads(vertices, {p5 + glm::vec3(-.5f, -.5f, -.5f), p4 + glm::vec3(.0f, -.5f, -.5f), p5 + glm::vec3(-.5f, -.5f, -1.0f), p4 + glm::vec3(.0f, -.5f, -1.0f)}, spec + 8, 8, 8, 0, 8);
			break ;
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = baseSpec + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		spec += (faceLight << 24);
		switch (corners & (corners::mp | corners::pp)) {
			case corners::mp:
				utils::shader::addQuads(vertices, {p2 + glm::vec3(.0f, .5f, .0f), p3 + glm::vec3(-.5f, .5f, .0f), p6, p7 + glm::vec3(-.5f, .0f, .0f)}, spec, 8, 8, 0, 8);
				break ;
			case corners::pp:
				utils::shader::addQuads(vertices, {p2 + glm::vec3(.5f, .5f, .0f), p3 + glm::vec3(.0f, .5f, .0f), p6 + glm::vec3(.5f, .0f, .0f), p7}, spec + 8, 8, 8, 0, 8);
				break ;
			case corners::mp | corners::pp:
				utils::shader::addQuads(vertices, {p2 + glm::vec3(.0f, .5f, .0f), p3 + glm::vec3(.0f, .5f, .0f), p6, p7}, spec, 16, 8, 0, 8);
				break ;
		}
		switch (corners & (corners::mm | corners::pm)) {
			case corners::mm:
				utils::shader::addQuads(vertices, {p2, p3 + glm::vec3(-.5f, .0f, .0f), p6 + glm::vec3(.0f, -.5f, .0f), p7 + glm::vec3(-.5f, -.5f, .0f)}, spec + (8 << 8), 8, 8, 0, 8);
				break ;
			case corners::pm:
				utils::shader::addQuads(vertices, {p2 + glm::vec3(.5f, .0f, .0f), p3, p6 + glm::vec3(.5f, -.5f, .0f), p7 + glm::vec3(.0f, -.5f, .0f)}, spec + 8 + (8 << 8), 8, 8, 0, 8);
				break ;
			case corners::mm | corners::pm:
				utils::shader::addQuads(vertices, {p2, p3, p6 + glm::vec3(.0f, -.5f, .0f), p7 + glm::vec3(.0f, -.5f, .0f)}, spec + (8 << 8), 16, 8, 0, 8);
				break ;
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
	int spec, faceLight;
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
			spec = (this->texX(face_dir::minus_x, value) << 4) + (this->texY(face_dir::minus_x, value) << 12) + (3 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, 16, 16, 0, 8, !flip);
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				spec = (this->texX(face_dir::plus_x, value) << 4) + (this->texY(face_dir::plus_x, value) << 12) + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, 16, 16, 0, 8, flip);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				spec = (this->texX(face_dir::minus_y, value) << 4) + (this->texY(face_dir::minus_y, value) << 12) + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, 3, 16, 0, 8);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				spec = (this->texX(face_dir::plus_y, value) << 4) + (this->texY(face_dir::plus_y, value) << 12) + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, 3, 16, 0, 8);
			}
			if ((bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				spec = (this->texX(face_dir::plus_z) << 4) + (this->texY(face_dir::plus_z) << 12) + (0 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p0, p4, p1, p5}, spec, 16, 3, 0, 8);
			}
			if (!(bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				spec = (this->texX(face_dir::minus_z, face_dir::minus_z) << 4) + (this->texY(face_dir::minus_z, face_dir::minus_z) << 12) + (5 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p6, p2, p7, p3}, spec + (13 << 8), 16, 3, 0, 8);
			}
			break ;
		case face_dir::plus_x:
			p1.x -= 0.8125f;
			p3.x -= 0.8125f;
			p5.x -= 0.8125f;
			p7.x -= 0.8125f;
			if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
				spec = (this->texX(face_dir::minus_x, value) << 4) + (this->texY(face_dir::minus_x, value) << 12) + (3 << 19);
				faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, 16, 16, 0, 8, !flip);
			}
			spec = (this->texX(face_dir::plus_x, value) << 4) + (this->texY(face_dir::plus_x, value) << 12) + (4 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, 16, 16, 0, 8, flip);
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				spec = (this->texX(face_dir::minus_y, value) << 4) + (this->texY(face_dir::minus_y, value) << 12) + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, 3, 16, 0, 8);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				spec = (this->texX(face_dir::plus_y, value) << 4) + (this->texY(face_dir::plus_y, value) << 12) + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, 3, 16, 0, 8);
			}
			if ((bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				spec = (this->texX(face_dir::plus_z) << 4) + (this->texY(face_dir::plus_z) << 12) + (0 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p0, p4, p1, p5}, spec, 16, 3, 0, 8);
			}
			if (!(bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				spec = (this->texX(face_dir::minus_z, face_dir::minus_z) << 4) + (this->texY(face_dir::minus_z, face_dir::minus_z) << 12) + (5 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p6, p2, p7, p3}, spec + (13 << 8), 16, 3, 0, 8);
			}
			break ;
		case face_dir::minus_y:
			p0.y += 0.8125f;
			p1.y += 0.8125f;
			p2.y += 0.8125f;
			p3.y += 0.8125f;
			if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
				spec = (this->texX(face_dir::minus_x, value) << 4) + (this->texY(face_dir::minus_x, value) << 12) + (3 << 19);
				faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, 3, 16, 0, 8);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				spec = (this->texX(face_dir::plus_x, value) << 4) + (this->texY(face_dir::plus_x, value) << 12) + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, 3, 16, 0, 8);
			}
			spec = (this->texX(face_dir::minus_y, value) << 4) + (this->texY(face_dir::minus_y, value) << 12) + (1 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, 16, 16, 0, 8, !flip);
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				spec = (this->texX(face_dir::plus_y, value) << 4) + (this->texY(face_dir::plus_y, value) << 12) + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, 16, 16, 0, 8, flip);
			}
			if ((bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				spec = (this->texX(face_dir::plus_z) << 4) + (this->texY(face_dir::plus_z) << 12) + (0 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p1, p0, p5, p4}, spec, 16, 3, 0, 8);
			}
			if (!(bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				spec = (this->texX(face_dir::minus_z, face_dir::minus_z) << 4) + (this->texY(face_dir::minus_z, face_dir::minus_z) << 12) + (5 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p7, p6, p3, p2}, spec + (13 << 8), 16, 3, 0, 8);
			}
			break ;
		case face_dir::plus_y:
			p4.y -= 0.8125f;
			p5.y -= 0.8125f;
			p6.y -= 0.8125f;
			p7.y -= 0.8125f;
			if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
				spec = (this->texX(face_dir::minus_x, value) << 4) + (this->texY(face_dir::minus_x, value) << 12) + (3 << 19);
				faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, 3, 16, 0, 8);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				spec = (this->texX(face_dir::plus_x, value) << 4) + (this->texY(face_dir::plus_x, value) << 12) + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, 3, 16, 0, 8);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				spec = (this->texX(face_dir::minus_y, value) << 4) + (this->texY(face_dir::minus_y, value) << 12) + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, 16, 16, 0, 8, !flip);
			}
			spec = (this->texX(face_dir::plus_y, value) << 4) + (this->texY(face_dir::plus_y, value) << 12) + (2 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, 16, 16, 0, 8, flip);
			if ((bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				spec = (this->texX(face_dir::plus_z) << 4) + (this->texY(face_dir::plus_z) << 12) + (0 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p1, p0, p5, p4}, spec, 16, 3, 0, 8);
			}
			if (!(bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				spec = (this->texX(face_dir::minus_z, face_dir::minus_z) << 4) + (this->texY(face_dir::minus_z, face_dir::minus_z) << 12) + (5 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p7, p6, p3, p2}, spec + (13 << 8), 16, 3, 0, 8);
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
	int baseSpec = (textureX << 4) + (textureY << 12);
	int spec, faceLight;
	bool open = !!(bitfield & door::open) ^ ((value >> offset::redstone::powered) & 0x1);
	if (!open) {
		if (!(bitfield & door::upper_half)) {
			p0.z -= 0.8125f;
			p1.z -= 0.8125f;
			p4.z -= 0.8125f;
			p5.z -= 0.8125f;
			if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
				spec = baseSpec + (3 << 19);
				faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, 16, 3, 0, 8);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				spec = baseSpec + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, 16, 3, 0, 8);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				spec = baseSpec + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, 16, 3, 0, 8);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				spec = baseSpec + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, 16, 3, 0, 8);
			}
			spec = baseSpec + (0 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, 16, 16, 0, 8);
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				spec = baseSpec + (5 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, 16, 16, 0, 8);
			}
		} else {
			p2.z += 0.8125f;
			p3.z += 0.8125f;
			p6.z += 0.8125f;
			p7.z += 0.8125f;
			if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
				spec = baseSpec + (3 << 19);
				faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, 16, 3, 0, 8, false, true);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				spec = baseSpec + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, 16, 3, 0, 8, false, true);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				spec = baseSpec + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, 16, 3, 0, 8, false, true);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				spec = baseSpec + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, 16, 3, 0, 8, false, true);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				spec = baseSpec + (0 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, 16, 16, 0, 8);
			}
			spec = baseSpec + (5 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, 16, 16, 0, 8);
		}
	} else { // open trapdoor
		switch ((value >> offset::blocks::orientation) & 0x7) {
			case face_dir::minus_x:
				p0 += glm::vec3(0.8125f, 0, 0);
				p2 += glm::vec3(0.8125f, 0, 0);
				p4 += glm::vec3(0.8125f, 0, 0);
				p6 += glm::vec3(0.8125f, 0, 0);
				spec = baseSpec + (3 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, 16, 16, 0, 8);
				if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
					spec = baseSpec + (4 << 19);
					faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, 16, 16, 0, 8);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
					spec = baseSpec + (1 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p2, p0, p3, p1}, spec, 16, 3, 0, 8, true, true);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
					spec = baseSpec + (2 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p7, p5, p6, p4}, spec, 16, 3, 0, 8);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
					spec = baseSpec + (0 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p0, p4, p1, p5}, spec, 16, 3, 0, 8, true, true);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
					spec = baseSpec + (5 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p6, p2, p7, p3}, spec, 16, 3, 0, 8, true, true);
				}
				break ;
			case face_dir::plus_x:
				p1 -= glm::vec3(0.8125f, 0, 0);
				p3 -= glm::vec3(0.8125f, 0, 0);
				p5 -= glm::vec3(0.8125f, 0, 0);
				p7 -= glm::vec3(0.8125f, 0, 0);
				if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
					spec = baseSpec + (3 << 19);
					faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec, 16, 16, 0, 8);
				}
				spec = baseSpec + (4 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec, 16, 16, 0, 8);
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
					spec = baseSpec + (1 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p2, p0, p3, p1}, spec, 16, 3, 0, 8);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
					spec = baseSpec + (2 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p7, p5, p6, p4}, spec, 16, 3, 0, 8, true, true);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
					spec = baseSpec + (0 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p0, p4, p1, p5}, spec, 16, 3, 0, 8);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
					spec = baseSpec + (5 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p6, p2, p7, p3}, spec, 16, 3, 0, 8);
				}
				break ;
			case face_dir::minus_y:
				p0 += glm::vec3(0, 0.8125f, 0);
				p1 += glm::vec3(0, 0.8125f, 0);
				p2 += glm::vec3(0, 0.8125f, 0);
				p3 += glm::vec3(0, 0.8125f, 0);
				if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
					spec = baseSpec + (3 << 19);
					faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p6, p4, p2, p0}, spec, 16, 3, 0, 8);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
					spec = baseSpec + (4 << 19);
					faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p3, p1, p7, p5}, spec, 16, 3, 0, 8, true, true);
				}
				spec = baseSpec + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, 16, 16, 0, 8);
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
					spec = baseSpec + (2 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, 16, 16, 0, 8);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
					spec = baseSpec + (0 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, 16, 3, 0, 8);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
					spec = baseSpec + (5 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, 16, 3, 0, 8, true, true);
				}
				break ;
			case face_dir::plus_y:
				p4 -= glm::vec3(0, 0.8125f, 0);
				p5 -= glm::vec3(0, 0.8125f, 0);
				p6 -= glm::vec3(0, 0.8125f, 0);
				p7 -= glm::vec3(0, 0.8125f, 0);
				if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
					spec = baseSpec + (3 << 19);
					faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p6, p4, p2, p0}, spec, 16, 3, 0, 8, true, true);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
					spec = baseSpec + (4 << 19);
					faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p3, p1, p7, p5}, spec, 16, 3, 0, 8);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
					spec = baseSpec + (1 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec, 16, 16, 0, 8);
				}
				spec = baseSpec + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
				spec += (faceLight << 24);
				utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec, 16, 16, 0, 8);
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
					spec = baseSpec + (0 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, 16, 3, 0, 8, true, true);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
					spec = baseSpec + (5 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
					spec += (faceLight << 24);
					utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, 16, 3, 0, 8);
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
	int baseSpec = (textureX << 4) + (textureY << 12);
	int faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	int spec = baseSpec + (3 << 19);
	spec += (faceLight << 24);
	utils::shader::addQuads(vertices, {pp4, pp0, pp6, pp2}, spec + 6, 4, 16, 0, 8);
	spec = baseSpec + (4 << 19);
	spec += (faceLight << 24);
	utils::shader::addQuads(vertices, {pp1, pp5, pp3, pp7}, spec + 6, 4, 16, 0, 8);
	spec = baseSpec + (1 << 19);
	spec += (faceLight << 24);
	utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + 6, 4, 16, 0, 8);
	spec = baseSpec + (2 << 19);
	spec += (faceLight << 24);
	utils::shader::addQuads(vertices, {pp5, pp4, pp7, pp6}, spec + 6, 4, 16, 0, 8);
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = baseSpec + (0 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {pp4, pp5, pp0, pp1}, spec + 6 + (6 << 8), 4, 4, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = baseSpec + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {pp2, pp3, pp6, pp7}, spec + 6 + (6 << 8), 4, 4, 0, 8);
	}
	// arms
	faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
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
			spec = baseSpec + (1 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + ((1 + 6*i) << 8), 6, 3, 0, 8);
			spec = baseSpec + (2 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp5, pp4, pp7, pp6}, spec + 10 + ((1 + 6*i) << 8), 6, 3, 0, 8);
			spec = baseSpec + (0 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp4, pp5, pp0, pp1}, spec + (7 << 8), 6, 2, 0, 8);
			spec = baseSpec + (5 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp2, pp3, pp6, pp7}, spec + (7 << 8), 6, 2, 0, 8);
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
			spec = baseSpec + (1 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + 10 + ((1 + 6*i) << 8), 6, 3, 0, 8);
			spec = baseSpec + (2 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp5, pp4, pp7, pp6}, spec + ((1 + 6*i) << 8), 6, 3, 0, 8);
			spec = baseSpec + (0 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp4, pp5, pp0, pp1}, spec + (7 << 8), 6, 2, 0, 8);
			spec = baseSpec + (5 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp2, pp3, pp6, pp7}, spec + (7 << 8), 6, 2, 0, 8);
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
			spec = baseSpec + (3 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp4, pp0, pp6, pp2}, spec + 10 + ((1 + 6*i) << 8), 6, 3, 0, 8);
			spec = baseSpec + (4 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp1, pp5, pp3, pp7}, spec + ((1 + 6*i) << 8), 6, 3, 0, 8);
			spec = baseSpec + (0 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp5, pp1, pp4, pp0}, spec + (7 << 8), 6, 2, 0, 8);
			spec = baseSpec + (5 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp3, pp7, pp2, pp6}, spec + (7 << 8), 6, 2, 0, 8);
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
			spec = baseSpec + (3 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp4, pp0, pp6, pp2}, spec + ((1 + 6*i) << 8), 6, 3, 0, 8);
			spec = baseSpec + (4 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp1, pp5, pp3, pp7}, spec + 10 + ((1 + 6*i) << 8), 6, 3, 0, 8);
			spec = baseSpec + (0 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp5, pp1, pp4, pp0}, spec + (7 << 8), 6, 2, 0, 8);
			spec = baseSpec + (5 << 19);
			spec += (faceLight << 24);
			utils::shader::addQuads(vertices, {pp3, pp7, pp2, pp6}, spec + (7 << 8), 6, 2, 0, 8);
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
	int baseSpec = (this->texX(face_dir::plus_z, fence::arm_end) << 4) + (this->texY() << 12);
	int faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	int spec;
	if (bitfield & fence::mx) {
		spec = baseSpec + (2 << 4) + (1 << 19);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.4375f, 0.f), p1 + glm::vec3(0.f, 0.4375f, 0.f), p2 + glm::vec3(0.f, 0.4375f, 0.f), p3 + glm::vec3(0.f, 0.4375f, 0.f)}, spec, 16, 16, 0, 8);
		spec += (1 << 19);
		utils::shader::addQuads(vertices, {p5 + glm::vec3(0.f, -0.4375f, 0.f), p4 + glm::vec3(0.f, -0.4375f, 0.f), p7 + glm::vec3(0.f, -0.4375f, 0.f), p6 + glm::vec3(0.f, -0.4375f, 0.f)}, spec, 16, 16, 0, 8, true);
	} else {
		spec = baseSpec + (3 << 19) - (!!((value >> 12) & (fence::my | fence::py)) << 12);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p4 + glm::vec3(0.4375f, 0.f, 0.f), p0 + glm::vec3(0.4375f, 0.f, 0.f), p6 + glm::vec3(0.4375f, 0.f, 0.f), p2 + glm::vec3(0.4375f, 0.f, 0.f)}, spec, 16, 16, 0, 8);
	}
	if (bitfield & fence::px) {
		spec = baseSpec + (3 << 4) + (1 << 19);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.4375f, 0.f), p1 + glm::vec3(0.f, 0.4375f, 0.f), p2 + glm::vec3(0.f, 0.4375f, 0.f), p3 + glm::vec3(0.f, 0.4375f, 0.f)}, spec, 16, 16, 0, 8);
		spec += (1 << 19);
		utils::shader::addQuads(vertices, {p5 + glm::vec3(0.f, -0.4375f, 0.f), p4 + glm::vec3(0.f, -0.4375f, 0.f), p7 + glm::vec3(0.f, -0.4375f, 0.f), p6 + glm::vec3(0.f, -0.4375f, 0.f)}, spec, 16, 16, 0, 8, true);
	} else {
		spec = baseSpec + (4 << 19) - (!!((value >> 12) & (fence::my | fence::py)) << 12);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p1 + glm::vec3(-0.4375f, 0.f, 0.f), p5 + glm::vec3(-0.4375f, 0.f, 0.f), p3 + glm::vec3(-0.4375f, 0.f, 0.f), p7 + glm::vec3(-0.4375f, 0.f, 0.f)}, spec, 16, 16, 0, 8);
	}
	if (bitfield & fence::my) {
		spec = baseSpec + (2 << 4) + (3 << 19);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p4 + glm::vec3(0.4375f, 0.f, 0.f), p0 + glm::vec3(0.4375f, 0.f, 0.f), p6 + glm::vec3(0.4375f, 0.f, 0.f), p2 + glm::vec3(0.4375f, 0.f, 0.f)}, spec, 16, 16, 0, 8, true);
		spec += (1 << 19);
		utils::shader::addQuads(vertices, {p1 + glm::vec3(-0.4375f, 0.f, 0.f), p5 + glm::vec3(-0.4375f, 0.f, 0.f), p3 + glm::vec3(-0.4375f, 0.f, 0.f), p7 + glm::vec3(-0.4375f, 0.f, 0.f)}, spec, 16, 16, 0, 8);
	} else {
		spec = baseSpec + (1 << 19) - (!!((value >> 12) & (fence::mx | fence::px)) << 12);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.4375f, 0.f), p1 + glm::vec3(0.f, 0.4375f, 0.f), p2 + glm::vec3(0.f, 0.4375f, 0.f), p3 + glm::vec3(0.f, 0.4375f, 0.f)}, spec, 16, 16, 0, 8);
	}
	if (bitfield & fence::py) {
		spec = baseSpec + (3 << 4) + (3 << 19);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p4 + glm::vec3(0.4375f, 0.f, 0.f), p0 + glm::vec3(0.4375f, 0.f, 0.f), p6 + glm::vec3(0.4375f, 0.f, 0.f), p2 + glm::vec3(0.4375f, 0.f, 0.f)}, spec, 16, 16, 0, 8, true);
		spec += (1 << 19);
		utils::shader::addQuads(vertices, {p1 + glm::vec3(-0.4375f, 0.f, 0.f), p5 + glm::vec3(-0.4375f, 0.f, 0.f), p3 + glm::vec3(-0.4375f, 0.f, 0.f), p7 + glm::vec3(-0.4375f, 0.f, 0.f)}, spec, 16, 16, 0, 8);
	} else {
		spec = baseSpec + (2 << 19) - (!!((value >> 12) & (fence::mx | fence::px)) << 12);
		spec += (faceLight << 24);
		utils::shader::addQuads(vertices, {p5 + glm::vec3(0.f, -0.4375f, 0.f), p4 + glm::vec3(0.f, -0.4375f, 0.f), p7 + glm::vec3(0.f, -0.4375f, 0.f), p6 + glm::vec3(0.f, -0.4375f, 0.f)}, spec, 16, 16, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = baseSpec + (0 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		spec += (faceLight << 24);
		if ((bitfield & fence::py) && (bitfield & fence::my)) {
			utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec, 16, 16, 0, 8);
		} else if (bitfield & fence::py) {
			utils::shader::addQuads(vertices, {p4, p5, p0 + glm::vec3(0.f, 0.4375f, 0.f), p1 + glm::vec3(0.f, 0.4375f, 0.f)}, spec, 16, 9, 0, 8);
		} else if (bitfield & fence::my) {
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.f, -0.4375f, 0.f), p5 + glm::vec3(0.f, -0.4375f, 0.f), p0, p1}, spec + (7 << 8), 16, 9, 0, 8);
		} else {
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.f, -0.4375f, 0.f), p5 + glm::vec3(0.f, -0.4375f, 0.f), p0 + glm::vec3(0.f, 0.4375f, 0.f), p1 + glm::vec3(0.f, 0.4375f, 0.f)}, spec + (7 << 8), 16, 2, 0, 8);
		}
		if (bitfield & fence::mx) {
			utils::shader::addQuads(vertices, {p0, p4, p0 + glm::vec3(0.4375f, 0.f, 0.f), p4 + glm::vec3(0.4375f, 0.f, 0.f)}, spec, 16, 7, 0, 8);
		}
		if (bitfield & fence::px) {
			utils::shader::addQuads(vertices, {p5, p1, p5 + glm::vec3(-0.4375f, 0.f, 0.f), p1 + glm::vec3(-0.4375f, 0.f, 0.f)}, spec, 16, 7, 0, 8);
		}
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = baseSpec + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		spec += (faceLight << 24);
		if ((bitfield & fence::my) && (bitfield & fence::py)) {
			utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec, 16, 16, 0, 8);
		} else if (bitfield & fence::my) {
			utils::shader::addQuads(vertices, {p2, p3, p6 + glm::vec3(0.f, -0.4375f, 0.f), p7 + glm::vec3(0.f, -0.4375f, 0.f)}, spec, 16, 9, 0, 8);
		} else if (bitfield & fence::py) {
			utils::shader::addQuads(vertices, {p2 + glm::vec3(0.f, 0.4375f, 0.f), p3 + glm::vec3(0.f, 0.4375f, 0.f), p6, p7}, spec + (7 << 8), 16, 9, 0, 8);
		} else {
			utils::shader::addQuads(vertices, {p2 + glm::vec3(0.f, 0.4375f, 0.f), p3 + glm::vec3(0.f, 0.4375f, 0.f), p6 + glm::vec3(0.f, -0.4375f, 0.f), p7 + glm::vec3(0.f, -0.4375f, 0.f)}, spec + (7 << 8), 16, 2, 0, 8);
		}
		if (bitfield & fence::mx) {
			utils::shader::addQuads(vertices, {p6, p2, p6 + glm::vec3(0.4375f, 0.f, 0.f), p2 + glm::vec3(0.4375f, 0.f, 0.f)}, spec, 16, 7, 0, 8);
		}
		if (bitfield & fence::px) {
			utils::shader::addQuads(vertices, {p7, p3, p7 + glm::vec3(-0.4375f, 0.f, 0.f), p3 + glm::vec3(-0.4375f, 0.f, 0.f)}, spec, 16, 7, 0, 8);
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

	int spec = (textureX << 4) + (texY(face_dir::plus_z, value) << 12) + (0 << 19) + (chunk->computeLight(pos.x, pos.y, pos.z) << 24);
	utils::shader::addQuads(vertices, {p4 + glm::vec3(3.0f * one16th, 0.0f, 0.0f), {p0.x + 3.0f * one16th, p0.y, p0.z}, {p6.x + 3.0f * one16th, p6.y, p6.z}, {p2.x + 3.0f * one16th, p2.y, p2.z}}, spec, 16, 16, 0, 8); // -x
	utils::shader::addQuads(vertices, {p0 + glm::vec3(13.0f * one16th, 0.0f, 0.0f), {p4.x + 13.0f * one16th, p4.y, p4.z}, {p2.x + 13.0f * one16th, p2.y, p2.z}, {p6.x + 13.0f * one16th, p6.y, p6.z}}, spec, 16, 16, 0, 8); // +x
	utils::shader::addQuads(vertices, {p0 + glm::vec3(0.0f, 3.0f * one16th, 0.0f), {p1.x, p1.y + 3.0f * one16th, p1.z}, {p2.x, p2.y + 3.0f * one16th, p2.z}, {p3.x, p3.y + 3.0f * one16th, p3.z}}, spec, 16, 16, 0, 8); // -y
	utils::shader::addQuads(vertices, {p1 + glm::vec3(0.0f, 13.0f * one16th, 0.0f), {p4.x, p4.y + 13.0f * one16th, p4.z}, {p2.x, p2.y + 13.0f * one16th, p2.z}, {p6.x, p6.y + 13.0f * one16th, p6.z}}, spec, 16, 16, 0, 8); // +y
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
	spec = (s_blocks[blocks::cobblestone]->texX() << 4) + (s_blocks[blocks::cobblestone]->texY() << 12) + (0 << 19);
	int faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	spec += (faceLight << 24);
	if (placement != placement::wall || orientation != face_dir::plus_x) {
		utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec + x_texMX + (x_texMY << 8), 16 - x_texMX - x_texPX, 16 - x_texMY - x_texPY, 0, 8); // -x
	}
	if (placement != placement::wall || orientation != face_dir::minus_x) {
		utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec + x_texMX + (x_texMY << 8), 16 - x_texMX - x_texPX, 16 - x_texMY - x_texPY, 0, 8); // +x
	}
	if (placement != placement::wall || orientation != face_dir::plus_y) {
		utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec + y_texMX + (y_texMY << 8), 16 - y_texMX - y_texPX, 16 - y_texMY - y_texPY, 0, 8); // -y
	}
	if (placement != placement::wall || orientation != face_dir::minus_y) {
		utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec + y_texMX + (y_texMY << 8), 16 - y_texMX - y_texPX, 16 - y_texMY - y_texPY, 0, 8); // +y
	}
	if (placement != placement::ceiling) {
		utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec + z_texMX + (z_texMY << 8), 16 - z_texMX - z_texPX, 16 - z_texMY - z_texPY, 0, 8); // +z
	}
	if (placement != placement::floor) {
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec + z_texMX + (z_texMY << 8), 16 - z_texMX - z_texPX, 16 - z_texMY - z_texPY, 0, 8); // -z
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

	spec = (textureX << 4) + (textureY << 12);
	spec += (faceLight << 24);
	utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec + 7 + (6 << 8), 2, 10, 0, 8); // -x
	utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec + 7 + (6 << 8), 2, 10, 0, 8); // +x
	utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec + 7 + (6 << 8), 2, 10, 0, 8); // -y
	utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec + 7 + (6 << 8), 2, 10, 0, 8); // +y
	utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec + 7 + (6 << 8), 2, 2, 0, 8); // +z
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
	spec = (textureX << 4) + (textureY << 12) + (0 << 19);
	int faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	spec += (faceLight << 24);
	if (placement != placement::wall || orientation != face_dir::plus_x) {
		utils::shader::addQuads(vertices, {p4, p0, p6, p2}, spec + x_texMX + (x_texMY << 8), 16 - x_texMX - x_texPX, 16 - x_texMY - x_texPY, 0, 8); // -x
	}
	if (placement != placement::wall || orientation != face_dir::minus_x) {
		utils::shader::addQuads(vertices, {p1, p5, p3, p7}, spec + x_texMX + (x_texMY << 8), 16 - x_texMX - x_texPX, 16 - x_texMY - x_texPY, 0, 8); // +x
	}
	if (placement != placement::wall || orientation != face_dir::plus_y) {
		utils::shader::addQuads(vertices, {p0, p1, p2, p3}, spec + y_texMX + (y_texMY << 8), 16 - y_texMX - y_texPX, 16 - y_texMY - y_texPY, 0, 8); // -y
	}
	if (placement != placement::wall || orientation != face_dir::minus_y) {
		utils::shader::addQuads(vertices, {p5, p4, p7, p6}, spec + y_texMX + (y_texMY << 8), 16 - y_texMX - y_texPX, 16 - y_texMY - y_texPY, 0, 8); // +y
	}
	if (placement != placement::ceiling) {
		utils::shader::addQuads(vertices, {p4, p5, p0, p1}, spec + z_texMX + (z_texMY << 8), 16 - z_texMX - z_texPX, 16 - z_texMY - z_texPY, 0, 8); // +z
	}
	if (placement != placement::floor) {
		utils::shader::addQuads(vertices, {p2, p3, p6, p7}, spec + z_texMX + (z_texMY << 8), 16 - z_texMX - z_texPX, 16 - z_texMY - z_texPY, 0, 8); // -z
	}
}

void RedstoneDust::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	(void)chunk;
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0.002f);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0.002f);
	// glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	// glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0.002f);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0.002f);
	// glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	// glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int spec = (texX() << 4) + (texY() << 12); // dust dot texture
	// (void)value; int color = pos.x;
	int color = (value >> offset::redstone::strength) & 0xF;
	spec += (color << 24);


	int mx = (value >> offset::redstone::dust::mx) & mask::redstone::dust::connect;
	int px = (value >> offset::redstone::dust::px) & mask::redstone::dust::connect;
	int my = (value >> offset::redstone::dust::my) & mask::redstone::dust::connect;
	int py = (value >> offset::redstone::dust::py) & mask::redstone::dust::connect;

	if ((mx && (my || py)) || (px && (my || py)) // display central dot if at least 2 perpendicular dir are on
		|| !(mx | py | my | py)) { // or if no dir is on
		utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, 0.0f, -0.0005f), p5 + glm::vec3(0.0f, 0.0f, -0.0005f), p0 + glm::vec3(0.0f, 0.0f, -0.0005f), p1 + glm::vec3(0.0f, 0.0f, -0.0005f)}, spec, 16, 16, 0, 8);
	}

	spec -= (1 << 4); // dust line texture
	if (mx) {
		utils::shader::addQuads(vertices, {p4, p5 + glm::vec3(-0.5f, 0.0f, 0.0f), p0, p1 + glm::vec3(-0.5f, 0.0f, 0.0f)}, spec, {16 + (1 << 17), 16 + (1 << 17) + (8 << 8) + (1 << 18), 0, (8 << 8) + (1 << 18)}, {1, 1 + (1 << 8), 0, (1 << 8)});
		if (mx & redstone::dust::up) {
			utils::shader::addQuads(vertices, {p0 + glm::vec3(0.002f, 0.0f, 1.0f), p4 + glm::vec3(0.002f, 0.0f, 1.0f), p0 + glm::vec3(0.002f, 0.0f, 0.0f), p4 + glm::vec3(0.002f, 0.0f, 0.0f)}, spec, 16, 16, 0, 8);
		}
	}
	if (px) {
		utils::shader::addQuads(vertices, {p4 + glm::vec3(0.5f, 0.0f, 0.0f), p5, p0 + glm::vec3(0.5f, 0.0f, 0.0f), p1}, spec + (8 << 8), {16 + (1 << 17), 16 + (1 << 17) + (8 << 8) + (1 << 18), 0, (8 << 8) + (1 << 18)}, {1, 1 + (1 << 8), 0, (1 << 8)});
		if (px & redstone::dust::up) {
			utils::shader::addQuads(vertices, {p5 + glm::vec3(-0.002f, 0.0f, 1.0f), p1 + glm::vec3(-0.002f, 0.0f, 1.0f), p5 + glm::vec3(-0.002f, 0.0f, 0.0f), p1 + glm::vec3(-0.002f, 0.0f, 0.0f)}, spec, 16, 16, 0, 8);
		}
	}
	if (my) {
		utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, -0.5f, 0.0f), p5 + glm::vec3(0.0f, -0.5f, 0.0f), p0, p1}, spec, {16 + (1 << 17) + (8 << 8) + (1 << 18), (8 << 8) + (1 << 18), 16 + (1 << 17), 0}, {1 + (1 << 8), (1 << 8), 1, 0});
		if (my & redstone::dust::up) {
			utils::shader::addQuads(vertices, {p1 + glm::vec3(0.0f, 0.002f, 1.0f), p0 + glm::vec3(0.0f, 0.002f, 1.0f), p1 + glm::vec3(0.0f, 0.002f, 0.0f), p0 + glm::vec3(0.0f, 0.002f, 0.0f)}, spec, 16, 16, 0, 8);
		}
	}
	if (py) {
		utils::shader::addQuads(vertices, {p4, p5, p0 + glm::vec3(0.0f, 0.5f, 0.0f), p1 + glm::vec3(0.0f, 0.5f, 0.0f)}, spec + (8 << 8), {16 + (1 << 17) + (8 << 8) + (1 << 18), (8 << 8) + (1 << 18), 16 + (1 << 17), 0}, {1 + (1 << 8), (1 << 8), 1, 0});
		if (py & redstone::dust::up) {
			utils::shader::addQuads(vertices, {p4 + glm::vec3(0.0f, -0.002f, 1.0f), p5 + glm::vec3(0.0f, -0.002f, 1.0f), p4 + glm::vec3(0.0f, -0.002f, 0.0f), p5 + glm::vec3(0.0f, -0.002f, 0.0f)}, spec, 16, 16, 0, 8);
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

	int baseSpec = (s_blocks[blocks::smooth_stone]->texX() << 4) + (s_blocks[blocks::smooth_stone]->texY() << 12);
	int spec, faceLight;
	// draw base
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = baseSpec + (3 << 19) + (chunk->computeLight(pos.x - 1, pos.y, pos.z) << 24);
		utils::shader::addQuads(vertices, {p4 + glm::vec3(0.f, 0.f, -14.f * one16th), p0 + glm::vec3(0.f, 0.f, -14.f * one16th), p6, p2}, spec + (14 << 8), 16, 2, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = baseSpec + (4 << 19) + (chunk->computeLight(pos.x + 1, pos.y, pos.z) << 24);
		utils::shader::addQuads(vertices, {p1 + glm::vec3(0.f, 0.f, -14.f * one16th), p5 + glm::vec3(0.f, 0.f, -14.f * one16th), p3, p7}, spec + (14 << 8), 16, 2, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = baseSpec + (1 << 19) + (chunk->computeLight(pos.x, pos.y - 1, pos.z) << 24);
		utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -14.f * one16th), p1 + glm::vec3(0.f, 0.f, -14.f * one16th), p2, p3}, spec + (14 << 8), 16, 2, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = baseSpec + (2 << 19) + (chunk->computeLight(pos.x, pos.y + 1, pos.z) << 24);
		utils::shader::addQuads(vertices, {p5 + glm::vec3(0.f, 0.f, -14.f * one16th), p4 + glm::vec3(0.f, 0.f, -14.f * one16th), p7, p6}, spec + (14 << 8), 16, 2, 0, 8);
	}
	// draw top
	faceLight = (chunk->computeLight(pos.x, pos.y, pos.z) << 24);
	spec = (this->texX(face_dir::minus_x, !!(value & mask::redstone::powered)) << 4) + (textureY << 12) + faceLight;
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
	utils::shader::addQuads(vertices, {topLeft, topLeft + right, topLeft - front, topLeft + right - front}, spec, 16, 16, 0, 8);
	// draw front torch
	spec = (s_blocks[blocks::redstone_torch]->texX() << 4) + (s_blocks[blocks::redstone_torch]->texY(face_dir::minus_x, !(value & mask::redstone::powered)) << 12);
	spec += faceLight;
	p2 = topLeft + right * 7.0f * one16th - front * 2.0f * one16th;
	p3 = topLeft + right * 9.0f * one16th - front * 2.0f * one16th;
	p6 = topLeft + right * 7.0f * one16th - front * 4.0f * one16th;
	p7 = topLeft + right * 9.0f * one16th - front * 4.0f * one16th;
	topLeft.z += 6.0f * one16th;
	p0 = topLeft + right * 7.0f * one16th - front * 2.0f * one16th;
	p1 = topLeft + right * 9.0f * one16th - front * 2.0f * one16th;
	p4 = topLeft + right * 7.0f * one16th - front * 4.0f * one16th;
	p5 = topLeft + right * 9.0f * one16th - front * 4.0f * one16th;
	utils::shader::addQuads(vertices, {p1 + right * one16th, p0 - right * one16th, p3 + right * one16th, p2 - right * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p4 - right * one16th, p5 + right * one16th, p6 - right * one16th, p7 + right * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p0 + front * one16th, p4 - front * one16th, p2 + front * one16th, p6 - front * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p5 - front * one16th, p1 + front * one16th, p7 - front * one16th, p3 + front * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -one16th), p1 + glm::vec3(0.f, 0.f, -one16th), p4 + glm::vec3(0.f, 0.f, -one16th), p5 + glm::vec3(0.f, 0.f, -one16th)}, spec + 7 + (6 << 8), 2, 2, 0, 8);
	// draw back torch or lock bar
	float delta = 4.0f + 2.0f * ((value >> offset::redstone::repeater::ticks) & 0x3);
	p0 -= front * delta * one16th; p1 -= front * delta * one16th; p2 -= front * delta * one16th; p3 -= front * delta * one16th;
	p4 -= front * delta * one16th; p5 -= front * delta * one16th; p6 -= front * delta * one16th; p7 -= front * delta * one16th;
	if (!(value & mask::redstone::repeater::lock)) { // back torch
		utils::shader::addQuads(vertices, {p1 + right * one16th, p0 - right * one16th, p3 + right * one16th, p2 - right * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
		utils::shader::addQuads(vertices, {p4 - right * one16th, p5 + right * one16th, p6 - right * one16th, p7 + right * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
		utils::shader::addQuads(vertices, {p0 + front * one16th, p4 - front * one16th, p2 + front * one16th, p6 - front * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
		utils::shader::addQuads(vertices, {p5 - front * one16th, p1 + front * one16th, p7 - front * one16th, p3 + front * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
		utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -one16th), p1 + glm::vec3(0.f, 0.f, -one16th), p4 + glm::vec3(0.f, 0.f, -one16th), p5 + glm::vec3(0.f, 0.f, -one16th)}, spec + 7 + (6 << 8), 2, 2, 0, 8);
	} else { // lock bar
		spec = (s_blocks[blocks::bedrock]->texX() << 4) + (s_blocks[blocks::bedrock]->texY() << 12);
		spec += faceLight;
		p2 += -right * 5.0f * one16th;
		p3 +=  right * 5.0f * one16th;
		p6 += -right * 5.0f * one16th;
		p7 +=  right * 5.0f * one16th;
		const glm::vec3 up = {0, 0, 1};
		p0 += -right * 5.0f * one16th - up * 4.0f * one16th;
		p1 +=  right * 5.0f * one16th - up * 4.0f * one16th;
		p4 += -right * 5.0f * one16th - up * 4.0f * one16th;
		p5 +=  right * 5.0f * one16th - up * 4.0f * one16th;
		utils::shader::addQuads(vertices, {p1, p0, p3, p2}, spec + 2 + (14 << 8), 12, 2, 0, 8);
		utils::shader::addQuads(vertices, {p4, p5, p6, p7}, spec + 2 + (14 << 8), 12, 2, 0, 8);
		utils::shader::addQuads(vertices, {p0, p4, p2, p6}, spec + 6 + (7 << 8), 2, 2, 0, 8);
		utils::shader::addQuads(vertices, {p5, p1, p7, p3}, spec + 8 + (7 << 8), 2, 2, 0, 8);
		utils::shader::addQuads(vertices, {p0, p1, p4, p5}, spec + 2 + (7 << 8), 12, 2, 0, 8);
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

	int baseSpec = (s_blocks[blocks::smooth_stone]->texX() << 4) + (s_blocks[blocks::smooth_stone]->texY() << 12);
	int spec, faceLight;
	// draw base
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = baseSpec + (3 << 19) + (chunk->computeLight(pos.x - 1, pos.y, pos.z) << 24);
		utils::shader::addQuads(vertices, {p4 + glm::vec3(0.f, 0.f, -14.f * one16th), p0 + glm::vec3(0.f, 0.f, -14.f * one16th), p6, p2}, spec + (14 << 8), 16, 2, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = baseSpec + (4 << 19) + (chunk->computeLight(pos.x + 1, pos.y, pos.z) << 24);
		utils::shader::addQuads(vertices, {p1 + glm::vec3(0.f, 0.f, -14.f * one16th), p5 + glm::vec3(0.f, 0.f, -14.f * one16th), p3, p7}, spec + (14 << 8), 16, 2, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = baseSpec + (1 << 19) + (chunk->computeLight(pos.x, pos.y - 1, pos.z) << 24);
		utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -14.f * one16th), p1 + glm::vec3(0.f, 0.f, -14.f * one16th), p2, p3}, spec + (14 << 8), 16, 2, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = baseSpec + (2 << 19) + (chunk->computeLight(pos.x, pos.y + 1, pos.z) << 24);
		utils::shader::addQuads(vertices, {p5 + glm::vec3(0.f, 0.f, -14.f * one16th), p4 + glm::vec3(0.f, 0.f, -14.f * one16th), p7, p6}, spec + (14 << 8), 16, 2, 0, 8);
	}
	// draw top
	faceLight = (chunk->computeLight(pos.x, pos.y, pos.z) << 24);
	spec = (this->texX(face_dir::minus_x, !!(value & mask::redstone::powered)) << 4) + (textureY << 12) + faceLight;
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
	utils::shader::addQuads(vertices, {topLeft, topLeft + right, topLeft - front, topLeft + right - front}, spec, 16, 16, 0, 8);
	// draw back torches
	spec = (s_blocks[blocks::redstone_torch]->texX() << 4) + (s_blocks[blocks::redstone_torch]->texY(face_dir::minus_x, !(value & mask::redstone::powered)) << 12);
	spec += faceLight;
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
	utils::shader::addQuads(vertices, {p1 + right * one16th, p0 - right * one16th, p3 + right * one16th, p2 - right * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p4 - right * one16th, p5 + right * one16th, p6 - right * one16th, p7 + right * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p0 + front * one16th, p4 - front * one16th, p2 + front * one16th, p6 - front * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p5 - front * one16th, p1 + front * one16th, p7 - front * one16th, p3 + front * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -one16th), p1 + glm::vec3(0.f, 0.f, -one16th), p4 + glm::vec3(0.f, 0.f, -one16th), p5 + glm::vec3(0.f, 0.f, -one16th)}, spec + 7 + (6 << 8), 2, 2, 0, 8);
	// back right
	p0 += right * 6.0f * one16th; p1 += right * 6.0f * one16th; p2 += right * 6.0f * one16th; p3 += right * 6.0f * one16th;
	p4 += right * 6.0f * one16th; p5 += right * 6.0f * one16th; p6 += right * 6.0f * one16th; p7 += right * 6.0f * one16th;
	utils::shader::addQuads(vertices, {p1 + right * one16th, p0 - right * one16th, p3 + right * one16th, p2 - right * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p4 - right * one16th, p5 + right * one16th, p6 - right * one16th, p7 + right * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p0 + front * one16th, p4 - front * one16th, p2 + front * one16th, p6 - front * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p5 - front * one16th, p1 + front * one16th, p7 - front * one16th, p3 + front * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -one16th), p1 + glm::vec3(0.f, 0.f, -one16th), p4 + glm::vec3(0.f, 0.f, -one16th), p5 + glm::vec3(0.f, 0.f, -one16th)}, spec + 7 + (6 << 8), 2, 2, 0, 8);
	// draw front torch
	spec = (s_blocks[blocks::redstone_torch]->texX() << 4) + (s_blocks[blocks::redstone_torch]->texY(face_dir::minus_x, !(value & mask::redstone::comparator::mode)) << 12);
	spec += faceLight;
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
	utils::shader::addQuads(vertices, {p1 + right * one16th, p0 - right * one16th, p3 + right * one16th, p2 - right * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p4 - right * one16th, p5 + right * one16th, p6 - right * one16th, p7 + right * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p0 + front * one16th, p4 - front * one16th, p2 + front * one16th, p6 - front * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p5 - front * one16th, p1 + front * one16th, p7 - front * one16th, p3 + front * one16th}, spec + 6 + (5 << 8), 4, 6, 0, 8);
	utils::shader::addQuads(vertices, {p0 + glm::vec3(0.f, 0.f, -one16th), p1 + glm::vec3(0.f, 0.f, -one16th), p4 + glm::vec3(0.f, 0.f, -one16th), p5 + glm::vec3(0.f, 0.f, -one16th)}, spec + 7 + (6 << 8), 2, 2, 0, 8);
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

	int spec;

	glm::vec3 pp0, pp1, pp2, pp3;
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
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
		spec = (this->texX(face_dir::minus_x, value) << 4) + (this->texY(face_dir::minus_x, value) << 12) + (3 << 19);
		spec += (chunk->computeLight(pos.x - 1, pos.y, pos.z) << 24);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + (ttyo << 8), 16, 16 - ttyo, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
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
		spec = (this->texX(face_dir::plus_x, value) << 4) + (this->texY(face_dir::plus_x, value) << 12) + (4 << 19);
		spec += (chunk->computeLight(pos.x + 1, pos.y, pos.z) << 24);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + (ttyo << 8), 16, 16 - ttyo, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
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
		spec = (this->texX(face_dir::minus_y, value) << 4) + (this->texY(face_dir::minus_y, value) << 12) + (1 << 19);
		spec += (chunk->computeLight(pos.x, pos.y - 1, pos.z) << 24);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + (ttyo << 8), 16, 16 - ttyo, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
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
		spec = (this->texX(face_dir::plus_y, value) << 4) + (this->texY(face_dir::plus_y, value) << 12) + (2 << 19);
		spec += (chunk->computeLight(pos.x, pos.y + 1, pos.z) << 24);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + (ttyo << 8), 16, 16 - ttyo, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
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
		spec = (this->texX(face_dir::plus_z, value) << 4) + (this->texY(face_dir::plus_z, value) << 12);
		spec += (chunk->computeLight(pos.x, pos.y, pos.z + 1) << 24);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + (ttyo << 8), 16, 16 - ttyo, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
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
		spec = (this->texX(face_dir::minus_z, value) << 4) + (this->texY(face_dir::minus_z, value) << 12) + (5 << 19);
		spec += (chunk->computeLight(pos.x, pos.y, pos.z - 1) << 24);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec + (ttyo << 8), 16, 16 - ttyo, 0, 8);
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
	spec &= 0xFFFF0000;
	spec += (this->texX(face_dir::minus_x, face_dir::minus_y << offset::blocks::orientation) << 4) + (this->texY(face_dir::minus_x, face_dir::minus_y << offset::blocks::orientation) << 12);
	glm::vec3 topLeft = glm::vec3(start, 0) + pos + glm::vec3(0.5f, 0.5f, 0.5f) + (-right + front + up) * 0.5f;
	// piston bar
	utils::shader::addQuads(vertices, {topLeft + (right - up) * 6.0f * one16th - front * 0.25f,
									topLeft + (right - up) * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 0.25f - up * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - up * 6.0f * one16th}, spec, 4, 4, 0, 8);
	utils::shader::addQuads(vertices, {topLeft + right * 10.0f * one16th - front * 0.25f - up * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - up * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 0.25f - up * 10.0f * one16th,
									topLeft + right * 10.0f * one16th - up * 10.0f * one16th}, spec, 4, 4, 0, 8);
	utils::shader::addQuads(vertices, {topLeft + right * 10.0f * one16th - front * 0.25f - up * 10.0f * one16th,
									topLeft + right * 10.0f * one16th - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 0.25f - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - up * 10.0f * one16th}, spec, 4, 4, 0, 8);
	utils::shader::addQuads(vertices, {topLeft + right * 6.0f * one16th - front * 0.25f - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 0.25f - up * 6.0f * one16th,
									topLeft + right * 6.0f * one16th - up * 6.0f * one16th}, spec, 4, 4, 0, 8);
}

void PistonHead::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	(void)chunk;

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
	glm::vec3 topLeft = glm::vec3(start, 0) + pos + glm::vec3(0.5f, 0.5f, 0.5f) + (-right + front + up) * 0.5f;
	int piston = (value & mask::redstone::piston::sticky) ? blocks::sticky_piston : blocks::piston;
	// front face
	int spec = (s_blocks[piston]->texX(face_dir::minus_x, face_dir::minus_x << offset::blocks::orientation) << 4) + (s_blocks[piston]->texY(face_dir::minus_x, face_dir::minus_x << offset::blocks::orientation) << 12) + (0xF << 24);
	utils::shader::addQuads(vertices, {topLeft + right, topLeft, topLeft + right - up, topLeft - up}, spec, 16, 16, 0, 8);
	// back of front volume
	if (piston == blocks::sticky_piston) {
		spec = (s_blocks[blocks::piston]->texX(face_dir::minus_x, face_dir::minus_x << offset::blocks::orientation) << 4) + (s_blocks[blocks::piston]->texY(face_dir::minus_x, face_dir::minus_x << offset::blocks::orientation) << 12) + (0xF << 24);
	}
	utils::shader::addQuads(vertices, {topLeft - front * 0.25f, topLeft + right - front * 0.25f, topLeft - up - front * 0.25f, topLeft + right - up - front * 0.25f}, spec, 16, 16, 0, 8);
	// side faces
	spec = (s_blocks[piston]->texX(face_dir::minus_y, face_dir::minus_x << offset::blocks::orientation) << 4) + (s_blocks[piston]->texY(face_dir::minus_y, face_dir::minus_x << offset::blocks::orientation) << 12) + (0xF << 24);
	utils::shader::addQuads(vertices, {topLeft, topLeft + right, topLeft - front * 0.25f, topLeft + right - front * 0.25f}, spec, 16, 4, 0, 8);
	utils::shader::addQuads(vertices, {topLeft - up, topLeft, topLeft - up - front * 0.25f, topLeft - front * 0.25f}, spec, 16, 4, 0, 8);
	utils::shader::addQuads(vertices, {topLeft + right, topLeft + right - up, topLeft + right - front * 0.25f, topLeft + right - up - front * 0.25f}, spec, 16, 4, 0, 8);
	utils::shader::addQuads(vertices, {topLeft + right - up, topLeft - up, topLeft + right - up - front * 0.25f, topLeft - up - front * 0.25f}, spec, 16, 4, 0, 8);
	// piston bar
	utils::shader::addQuads(vertices, {topLeft + right * 6.0f * one16th - front * 1.0f - up * 6.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 0.25f - up * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 1.0f - up * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 0.25f - up * 6.0f * one16th}, spec + 4, 12, 4, 0, 8);
	utils::shader::addQuads(vertices, {topLeft + right * 10.0f * one16th - front * 1.0f - up * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 0.25f - up * 6.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 1.0f - up * 10.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 0.25f - up * 10.0f * one16th}, spec + 4, 12, 4, 0, 8);
	utils::shader::addQuads(vertices, {topLeft + right * 10.0f * one16th - front * 1.0f - up * 10.0f * one16th,
									topLeft + right * 10.0f * one16th - front * 0.25f - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 1.0f - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 0.25f - up * 10.0f * one16th}, spec + 4, 12, 4, 0, 8);
	utils::shader::addQuads(vertices, {topLeft + right * 6.0f * one16th - front * 1.0f - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 0.25f - up * 10.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 1.0f - up * 6.0f * one16th,
									topLeft + right * 6.0f * one16th - front * 0.25f - up * 6.0f * one16th,}, spec + 4, 12, 4, 0, 8);
}

void OakSign::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	(void)chunk;
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
			std::cout << "ERROR oakSign::addMesh orientation" << std::endl;
			return ;
	}
	glm::vec3 topLeft = glm::vec3(start, 0) + pos + glm::vec3(0.5f, 0.5f, 0.5f) + (-right + front) * 0.5f + up * 0.25f;

	// front face
	int faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	int spec = 2 + (14 << 12) + (2 << 8) + (faceLight << 24);
	// player side
	utils::shader::addQuads(vertices, {topLeft - front * 1.75f * one16th,
								topLeft + right - front * 1.75f * one16th,
								topLeft - up * 0.5f - front * 1.75f * one16th,
								topLeft + right - up * 0.5f - front * 1.75f * one16th}, spec, 24, 12, 0, 8);
	// side faces
	spec = 2 + (14 << 12) + (faceLight << 24);
	// top
	utils::shader::addQuads(vertices, {topLeft,
								topLeft + right,
								topLeft - front * 1.75f * one16th,
								topLeft + right - front * 1.75f * one16th}, spec, 24, 2, 0, 8);
	// left
	spec = (14 << 12) + (2 << 8) + (faceLight << 24);
	utils::shader::addQuads(vertices, {topLeft,
								topLeft - front * 1.75f * one16th,
								topLeft - up * 0.5f,
								topLeft - up * 0.5f - front * 1.75f * one16th}, spec, 2, 12, 0, 8);
	// right
	spec = 2 + (3 << 4) + (14 << 12) + (2 << 8) + (faceLight << 24);
	utils::shader::addQuads(vertices, {topLeft + right - front * 1.75f * one16th,
								topLeft + right,
								topLeft + right - front * 1.75f * one16th - up * 0.5f,
								topLeft + right - up * 0.5f}, spec, 2, 12, 0, 8);
	// bottom
	spec = 10 + (1 << 4) + (14 << 12) + (faceLight << 24);
	utils::shader::addQuads(vertices, {topLeft - up * 0.5f - front * 1.75f * one16th,
								topLeft + right - up * 0.5f - front * 1.75f * one16th,
								topLeft - up * 0.5f,
								topLeft + right - up * 0.5f}, spec, 24, 2, 0, 8);
}

void Observer::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	int orientation = opposite_dir((value >> offset::blocks::orientation) & 0x7);

	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int spec;

	glm::vec3 pp0, pp1, pp2, pp3;
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
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
		spec = (this->texX(face_dir::minus_x, value) << 4) + (this->texY(face_dir::minus_x, value) << 12) + (3 << 19);
		spec += (chunk->computeLight(pos.x - 1, pos.y, pos.z) << 24);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec, 16, 16, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
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
		spec = (this->texX(face_dir::plus_x, value) << 4) + (this->texY(face_dir::plus_x, value) << 12) + (4 << 19);
		spec += (chunk->computeLight(pos.x + 1, pos.y, pos.z) << 24);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec, 16, 16, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
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
		spec = (this->texX(face_dir::minus_y, value) << 4) + (this->texY(face_dir::minus_y, value) << 12) + (1 << 19);
		spec += (chunk->computeLight(pos.x, pos.y - 1, pos.z) << 24);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec, 16, 16, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
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
		spec = (this->texX(face_dir::plus_y, value) << 4) + (this->texY(face_dir::plus_y, value) << 12) + (2 << 19);
		spec += (chunk->computeLight(pos.x, pos.y + 1, pos.z) << 24);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec, 16, 16, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
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
		spec = (this->texX(face_dir::plus_z, value) << 4) + (this->texY(face_dir::plus_z, value) << 12);
		spec += (chunk->computeLight(pos.x, pos.y, pos.z + 1) << 24);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec, 16, 16, 0, 8);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
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
		spec = (this->texX(face_dir::minus_z, value) << 4) + (this->texY(face_dir::minus_z, value) << 12) + (5 << 19);
		spec += (chunk->computeLight(pos.x, pos.y, pos.z - 1) << 24);
		utils::shader::addQuads(vertices, {pp0, pp1, pp2, pp3}, spec, 16, 16, 0, 8);
	}
}
