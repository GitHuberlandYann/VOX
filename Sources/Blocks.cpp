#include "Chunk.hpp"

const std::array<Block*, S_BLOCKS_SIZE> s_blocks = {
	new Air(), new GrassBlock(), new OakLog(), new Cactus(), new Farmland(), new DirtPath(), new TNT(), new Target(),
	new CraftingTable(), new Furnace(), new OakStairsBottom(), new OakStairsTop(), new OakDoor(), new OakTrapdoor(), new StoneStairsBottom(), new StoneStairsTop(),
	new SmoothStoneStairsBottom(), new SmoothStoneStairsTop(), new CobbleStoneStairsBottom(), new CobbleStoneStairsTop(), new StoneBricksStairsBottom(), new StoneBricksStairsTop(), new Lever(), new OakSign(),
	new Bedrock(), new Dirt(), new SmoothStone(), new Stone(), new Cobblestone(), new StoneBrick(), new CrackedStoneBrick(), new Sand(),
	new Gravel(), new OakLeaves(), new OakPlanks(), new Glass(), new GlassPane(), new RedstoneLamp(), new StoneButton(), new OakButton(),
	new CoalOre(), new IronOre(), new DiamondOre(), new CoalBlock(), new IronBlock(), new DiamondBlock(), new RedstoneOre(), new RedstoneBlock(),
	new OakSlabBottom(), new OakSlabTop(), new OakFence(), new StoneSlabBottom(), new StoneSlabTop(), new SmoothStoneSlabBottom(), new SmoothStoneSlabTop(), new CobbleStoneSlabBottom(),
	new CobbleStoneSlabTop(), new StoneBricksSlabBottom(), new StoneBricksSlabTop(), new Piston(), new StickyPiston(), new PistonHead(), new MovingPiston(), new Observer(),
	new Poppy(), new Dandelion(), new BlueOrchid(), new Allium(), new CornFlower(), new PinkTulip(), new Grass(), new SugarCane(),
	new DeadBush(), new OakSapling(), new Torch(), new RedstoneTorch(), new RedstoneDust(), new Repeater(), new Comparator(), new Chest(),
	new WheatCrop(), new TBD(), new TBD(), new TBD(), new TBD(), new TBD(), new TBD(), new TBD(),
	new Water(), new Water1(), new Water2(), new Water3(), new Water4(), new Water5(), new Water6(), new Water7(),
	new Stick(), new WoodenShovel(), new StoneShovel(), new IronShovel(), new DiamondShovel(), new WoodenAxe(), new StoneAxe(), new IronAxe(),
	new DiamondAxe(), new WoodenPickaxe(), new StonePickaxe(), new IronPickaxe(), new DiamondPickaxe(), new Bow(), new Arrow(), new WorldEditWand(),
	new Coal(), new Charcoal(), new IronIngot(), new Diamond(), new Bucket(), new WaterBucket(), new WoodenHoe(), new StoneHoe(),
	new IronHoe(), new DiamondHoe(), new WheatSeeds(), new Wheat(), new Bread(), new Apple(), new Flint(), new FlintAndSteel()
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

	t_shaderInput v0, v1, v2, v3;
	int spec, faceLight, cornerLight, shade;
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = (this->texX(face_dir::minus_x, value) << 4) + (this->texY(face_dir::minus_x, value) << 12) + (3 << 19);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		v0 = {spec + (cornerLight << 24) + (shade << 22), p4};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		v1 = {spec + (cornerLight << 24) + (shade << 22) + XTEX, p0};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p6};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p2};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = (this->texX(face_dir::plus_x, value) << 4) + (this->texY(face_dir::plus_x, value) << 12) + (4 << 19);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		v0 = {spec + (cornerLight << 24) + (shade << 22), p1};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		v1 = {spec + (cornerLight << 24) + (shade << 22) + XTEX, p5};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p3};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = (this->texX(face_dir::minus_y, value) << 4) + (this->texY(face_dir::minus_y, value) << 12) + (1 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		v0 = {spec + (cornerLight << 24) + (shade << 22), p0};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		v1 = {spec + (cornerLight << 24) + (shade << 22) + XTEX, p1};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p2};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p3};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = (this->texX(face_dir::plus_y, value) << 4) + (this->texY(face_dir::plus_y, value) << 12) + (2 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		v0 = {spec + (cornerLight << 24) + (shade << 22), p5};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		v1 = {spec + (cornerLight << 24) + (shade << 22) + XTEX, p4};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p7};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p6};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = (this->texX(face_dir::plus_z, value) << 4) + (this->texY(face_dir::plus_z, value) << 12);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		v0 = {spec + (cornerLight << 24) + (shade << 22), p4};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		v1 = {spec + (cornerLight << 24) + (shade << 22) + XTEX, p5};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p0};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p1};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = (this->texX(face_dir::minus_z, value) << 4) + (this->texY(face_dir::minus_z, value) << 12) + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		v0 = {spec + (cornerLight << 24) + (shade << 22), p2};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		v1 = {spec + (cornerLight << 24) + (shade << 22) + XTEX, p3};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p6};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3);
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
	t_shaderInput v0 = {spec, p0};
	t_shaderInput v1 = {spec + XTEX, p5};
	t_shaderInput v2 = {spec + YTEX, p2};
	t_shaderInput v3 = {spec + XTEX + YTEX, p7};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec, p1};
	v1 = {spec + XTEX, p4};
	v2 = {spec + YTEX, p3};
	v3 = {spec + XTEX + YTEX, p6};
	face_vertices(vertices, v0, v1, v2, v3);
	// also draw other face to still see them with face culling on
	v0 = {spec + XTEX, p5};
	v1 = {spec, p0};
	v2 = {spec + XTEX + YTEX, p7};
	v3 = {spec + YTEX, p2};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + XTEX, p4};
	v1 = {spec, p1};
	v2 = {spec + XTEX + YTEX, p6};
	v3 = {spec + YTEX, p3};
	face_vertices(vertices, v0, v1, v2, v3);
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

	t_shaderInput v0, v1, v2, v3;
	int spec, faceLight, cornerLight, shade;
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = (this->texX(face_dir::minus_x, value) << 4) + (this->texY(face_dir::minus_x, value) << 12) + (3 << 19);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		v0 = {spec + (1 << 8) + (cornerLight << 24) + (shade << 22), p4};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		v1 = {spec + (1 << 8) + (cornerLight << 24) + (shade << 22) + XTEX, p0};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p6};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x - 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p2};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = (this->texX(face_dir::plus_x, value) << 4) + (this->texY(face_dir::plus_x, value) << 12) + (4 << 19);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		v0 = {spec + (1 << 8) + (cornerLight << 24) + (shade << 22), p1};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		v1 = {spec + (1 << 8) + (cornerLight << 24) + (shade << 22) + XTEX, p5};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p3};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x + 1, pos.y, pos.z, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = (this->texX(face_dir::minus_y, value) << 4) + (this->texY(face_dir::minus_y, value) << 12) + (1 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		v0 = {spec + (1 << 8) + (cornerLight << 24) + (shade << 22), p0};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		v1 = {spec + (1 << 8) + (cornerLight << 24) + (shade << 22) + XTEX, p1};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p2};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y - 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p3};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = (this->texX(face_dir::plus_y, value) << 4) + (this->texY(face_dir::plus_y, value) << 12) + (2 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		v0 = {spec + (1 << 8) + (cornerLight << 24) + (shade << 22), p5};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		v1 = {spec + (1 << 8) + (cornerLight << 24) + (shade << 22) + XTEX, p4};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, -1, 0, 0, -1});
		v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p7};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		shade = chunk->computeShade(pos.x, pos.y + 1, pos.z, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
		v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p6};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = (this->texX(face_dir::plus_z, value) << 4) + (this->texY(face_dir::plus_z, value) << 12);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		v0 = {spec + (cornerLight << 24) + (shade << 22), p4};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		v1 = {spec + (cornerLight << 24) + (shade << 22) + XTEX, p5};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p0};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p1};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = (this->texX(face_dir::minus_z, value) << 4) + (this->texY(face_dir::minus_z, value) << 12) + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		v0 = {spec + (cornerLight << 24) + (shade << 22), p2};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
		v1 = {spec + (cornerLight << 24) + (shade << 22) + XTEX, p3};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p6};
		cornerLight = chunk->computeSmoothLight(faceLight, pos.x, pos.y, pos.z - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		shade = chunk->computeShade(pos.x, pos.y, pos.z - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
		v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3);
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

	t_shaderInput v0, v1, v2, v3;
	int spec, faceLight, shade;
	int baseSpec = (textureX << 4) + (textureY << 12);
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = baseSpec + (3 << 19);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		shade = 0;//computeShade(pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		spec += (faceLight << 24);
		v0 = {spec + (8 << 8) + (shade << 22), p4};
		v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p0};
		v2 = {spec + (shade << 22) + YTEX, p6};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = baseSpec + (4 << 19);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		shade = 0;//computeShade(pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		spec += (faceLight << 24);
		v0 = {spec + (8 << 8) + (shade << 22), p1};
		v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5};
		v2 = {spec + (shade << 22) + YTEX, p3};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = baseSpec + (1 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		shade = 0;//computeShade(pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		spec += (faceLight << 24);
		v0 = {spec + (8 << 8) + (shade << 22), p0};
		v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p1};
		v2 = {spec + (shade << 22) + YTEX, p2};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = baseSpec + (2 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		shade = 0;//computeShade(pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		spec += (faceLight << 24);
		v0 = {spec + (8 << 8) + (shade << 22), p5};
		v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p4};
		v2 = {spec + (shade << 22) + YTEX, p7};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	spec = baseSpec + (0 << 19);
	faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	shade = 0;//computeShade(pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
	spec += (faceLight << 24);
	// if (shade & 0xFF)std::cout << "shade is " << shade << std::endl;
	// if (shade & 0xFFFFFF00)std::cout << "problem" << std::endl;
	v0 = {spec + (shade << 22), p4};
	v1 = {spec + (shade << 22) + XTEX, p5};
	v2 = {spec + (shade << 22) + YTEX, p0};
	v3 = {spec + (shade << 22) + XTEX + YTEX, p1};
	face_vertices(vertices, v0, v1, v2, v3);
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = baseSpec + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		shade = 0;//computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p2};
		v1 = {spec + (shade << 22) + XTEX, p3};
		v2 = {spec + (shade << 22) + YTEX, p6};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3);
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

	t_shaderInput v0, v1, v2, v3;
	int spec, faceLight, shade;
	int baseSpec = (textureX << 4) + (textureY << 12);
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = baseSpec + (3 << 19);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		shade = 0;//computeShade(pos.x - 1, pos.y, pos.z, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p4};
		v1 = {spec + (shade << 22) + XTEX, p0};
		v2 = {spec - (8 << 8) + (shade << 22) + YTEX, p6};
		v3 = {spec - (8 << 8) + (shade << 22) + XTEX + YTEX, p2};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = baseSpec + (4 << 19);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		shade = 0;//computeShade(pos.x + 1, pos.y, pos.z, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p1};
		v1 = {spec + (shade << 22) + XTEX, p5};
		v2 = {spec - (8 << 8) + (shade << 22) + YTEX, p3};
		v3 = {spec - (8 << 8) + (shade << 22) + XTEX + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = baseSpec + (1 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		shade = 0;//computeShade(pos.x, pos.y - 1, pos.z, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p0};
		v1 = {spec + (shade << 22) + XTEX, p1};
		v2 = {spec - (8 << 8) + (shade << 22) + YTEX, p2};
		v3 = {spec - (8 << 8) + (shade << 22) + XTEX + YTEX, p3};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = baseSpec + (2 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		shade = 0;//computeShade(pos.x, pos.y + 1, pos.z, {1, 0, 0, 1, 0, 1, 0, 0, 1});
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p5};
		v1 = {spec + (shade << 22) + XTEX, p4};
		v2 = {spec - (8 << 8) + (shade << 22) + YTEX, p7};
		v3 = {spec - (8 << 8) + (shade << 22) + XTEX + YTEX, p6};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = baseSpec + (0 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		shade = 0;//computeShade(pos.x, pos.y, pos.z + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
		spec += (faceLight << 24);
		// if (shade & 0xFF)std::cout << "shade is " << shade << std::endl;
		// if (shade & 0xFFFFFF00)std::cout << "problem" << std::endl;
		v0 = {spec + (shade << 22), p4};
		v1 = {spec + (shade << 22) + XTEX, p5};
		v2 = {spec + (shade << 22) + YTEX, p0};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p1};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	spec = baseSpec + (5 << 19);
	faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	shade = 0;//computeShade(pos.x, pos.y, pos.z - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
	spec += (faceLight << 24);
	v0 = {spec + (shade << 22), p2};
	v1 = {spec + (shade << 22) + XTEX, p3};
	v2 = {spec + (shade << 22) + YTEX, p6};
	v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
	face_vertices(vertices, v0, v1, v2, v3);
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
	t_shaderInput v0 = {spec + 7 + (6 << 8), p4};
	t_shaderInput v1 = {spec - 7 + XTEX + (6 << 8), p0};
	t_shaderInput v2 = {spec + 7 + YTEX, p6};
	t_shaderInput v3 = {spec - 7 + XTEX + YTEX, p2};
	face_vertices(vertices, v0, v1, v2, v3); // -x
	v0 = {spec + 7 + (6 << 8), p1};
	v1 = {spec - 7 + XTEX + (6 << 8), p5};
	v2 = {spec + 7 + YTEX, p3};
	v3 = {spec - 7 + XTEX + YTEX, p7};
	face_vertices(vertices, v0, v1, v2, v3); // +x
	v0 = {spec + 7 + (6 << 8), p0};
	v1 = {spec - 7 + XTEX + (6 << 8), p1};
	v2 = {spec + 7 + YTEX, p2};
	v3 = {spec - 7 + XTEX + YTEX, p3};
	face_vertices(vertices, v0, v1, v2, v3); // -y
	v0 = {spec + 7 + (6 << 8), p5};
	v1 = {spec - 7 + XTEX + (6 << 8), p4};
	v2 = {spec + 7 + YTEX, p7};
	v3 = {spec - 7 + XTEX + YTEX, p6};
	face_vertices(vertices, v0, v1, v2, v3); // +y
	v0 = {spec + 7 + (6 << 8), p4};
	v1 = {spec - 7 + (6 << 8) + XTEX, p5};
	v2 = {spec + 7 + (8 << 8) + (1 << 18), p0};
	v3 = {spec - 7 + (8 << 8) + (1 << 18) + XTEX, p1};
	face_vertices(vertices, v0, v1, v2, v3); // +z
	if (((value >> offset::blocks::orientation) & 0x7) != face_dir::minus_z) {
		v0 = {spec + 7 + (14 << 8), p2};
		v1 = {spec - 7 + (14 << 8) + XTEX, p3};
		v2 = {spec + 7 + YTEX, p6};
		v3 = {spec - 7 + XTEX + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3); // -z
	}
}

void StairsBottom::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
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
	int spec, faceLight, shade;
	t_shaderInput v0, v1, v2, v3;
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = baseSpec + (3 << 19);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (corners::mm | corners::mp)) {
			case corners::mm | corners::mp:
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + XTEX, p0};
				v2 = {spec + (shade << 22) + YTEX, p6};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
				break ;
			case corners::mm:
				v0 = {spec + 8 + (shade << 22), p4 + glm::vec3(0, -0.5f, 0)};
				v1 = {spec + (shade << 22) + XTEX, p0};
				v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0, -0.5f, 0.5f)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0, 0, 0.5f)};
				face_vertices(vertices, v0, v1, v2, v3);
				v0 = {spec + (8 << 8) + (shade << 22), p4 - glm::vec3(0, 0, 0.5f)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p0 - glm::vec3(0, 0, 0.5f)};
				v2 = {spec + (shade << 22) + YTEX, p6};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
				break ;
			case corners::mp:
				v0 = {spec + (shade << 22), p4 + glm::vec3(0, 0, 0)};
				v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p0 + glm::vec3(0, 0.5f, 0)};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0, 0, 0.5f)};
				v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0, 0.5f, 0.5f)};
				face_vertices(vertices, v0, v1, v2, v3);
			default:
				v0 = {spec + (8 << 8) + (shade << 22), p4 - glm::vec3(0, 0, 0.5f)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p0 - glm::vec3(0, 0, 0.5f)};
				v2 = {spec + (shade << 22) + YTEX, p6};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
				break ;
		}
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = baseSpec + (4 << 19);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (corners::pm | corners::pp)) {
			case corners::pm | corners::pp:
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + XTEX, p5};
				v2 = {spec + (shade << 22) + YTEX, p3};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
				break ;
			case corners::pm:
				v0 = {spec + (shade << 22), p1 + glm::vec3(0, 0, 0)};
				v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p5 + glm::vec3(0, -0.5f, 0)};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p3 + glm::vec3(0, 0, 0.5f)};
				v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, -0.5f, 0.5f)};
				face_vertices(vertices, v0, v1, v2, v3);
				v0 = {spec + (8 << 8) + (shade << 22), p1 - glm::vec3(0, 0, 0.5f)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0, 0.5f)};
				v2 = {spec + (shade << 22) + YTEX, p3};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
				break ;
			case corners::pp:
				v0 = {spec + 8 + (shade << 22), p1 + glm::vec3(0, 0.5f, 0)};
				v1 = {spec + (shade << 22) + XTEX, p5 + glm::vec3(0, 0, 0)};
				v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p3 + glm::vec3(0, 0.5f, 0.5f)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, 0, 0.5f)};
				face_vertices(vertices, v0, v1, v2, v3);
			default:
				v0 = {spec + (8 << 8) + (shade << 22), p1 - glm::vec3(0, 0, 0.5f)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0, 0.5f)};
				v2 = {spec + (shade << 22) + YTEX, p3};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
				break ;
		}
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = baseSpec + (1 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (corners::mm | corners::pm)) {
			case corners::mm | corners::pm:
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + XTEX, p1};
				v2 = {spec + (shade << 22) + YTEX, p2};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
				break ;
			case corners::mm:
				v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0, 0)};
				v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p1 + glm::vec3(-0.5f, 0, 0)};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p2 + glm::vec3(0, 0, 0.5f)};
				v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(-0.5f, 0, 0.5f)};
				face_vertices(vertices, v0, v1, v2, v3);
				v0 = {spec + (8 << 8) + (shade << 22), p0 - glm::vec3(0, 0, 0.5f)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p1 - glm::vec3(0, 0, 0.5f)};
				v2 = {spec + (shade << 22) + YTEX, p2};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
				break ;
			case corners::pm:
				v0 = {spec + 8 + (shade << 22), p0 + glm::vec3(0.5f, 0, 0)};
				v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0, 0)};
				v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p2 + glm::vec3(0.5f, 0, 0.5f)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0, 0.5f)};
				face_vertices(vertices, v0, v1, v2, v3);
			default:
				v0 = {spec + (8 << 8) + (shade << 22), p0 - glm::vec3(0, 0, 0.5f)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p1 - glm::vec3(0, 0, 0.5f)};
				v2 = {spec + (shade << 22) + YTEX, p2};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
				break ;
		}
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = baseSpec + (2 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (corners::mp | corners::pp)) {
			case corners::mp | corners::pp:
				v0 = {spec + (shade << 22), p5};
				v1 = {spec + (shade << 22) + XTEX, p4};
				v2 = {spec + (shade << 22) + YTEX, p7};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
				break ;
			case corners::mp:
				v0 = {spec + 8 + (shade << 22), p5 + glm::vec3(-0.5f, 0, 0)};
				v1 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0, 0, 0)};
				v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p7 + glm::vec3(-0.5f, 0, 0.5f)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p6 + glm::vec3(0, 0, 0.5f)};
				face_vertices(vertices, v0, v1, v2, v3);
				v0 = {spec + (8 << 8) + (shade << 22), p5 - glm::vec3(0, 0, 0.5f)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 - glm::vec3(0, 0, 0.5f)};
				v2 = {spec + (shade << 22) + YTEX, p7};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
				break ;
			case corners::pp:
				v0 = {spec + (shade << 22), p5 + glm::vec3(0, 0, 0)};
				v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p4 + glm::vec3(0.5f, 0, 0)};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p7 + glm::vec3(0, 0, 0.5f)};
				v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p6 + glm::vec3(0.5f, 0, 0.5f)};
				face_vertices(vertices, v0, v1, v2, v3);
			default:
				v0 = {spec + (8 << 8) + (shade << 22), p5 - glm::vec3(0, 0, 0.5f)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 - glm::vec3(0, 0, 0.5f)};
				v2 = {spec + (shade << 22) + YTEX, p7};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
				break ;
		}
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = baseSpec + (0 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (corners::mp | corners::pp)) {
			case corners::mp:
				v0 = {spec + (shade << 22), p4 + glm::vec3(0, 0, 0)};
				v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, 0, 0)};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, 0)};
				v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(-0.5f, 0.5f, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case corners::pp:
				v0 = {spec + 8 + (shade << 22), p4 + glm::vec3(0.5f, 0, 0)};
				v1 = {spec + (shade << 22) + XTEX, p5};
				v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0.5f, 0.5f, 0)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case corners::mp | corners::pp:
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + XTEX, p5};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, 0)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
		}
		switch (corners & (corners::mm | corners::pm)) {
			case corners::mm:
				v0 = {spec + (8 << 8) + (shade << 22), p4 + glm::vec3(0, -0.5f, 0)};
				v1 = {spec - (1 << 4) + 8 + (8 << 8) + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, -0.5f, 0)};
				v2 = {spec + (shade << 22) + YTEX, p0 + glm::vec3(0, 0, 0)};
				v3 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(-0.5f, 0, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case corners::pm:
				v0 = {spec + 8 + (8 << 8) + (shade << 22), p4 + glm::vec3(0.5f, -0.5f, 0)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 + glm::vec3(0, -0.5f, 0)};
				v2 = {spec + 8 + (shade << 22) + YTEX, p0 + glm::vec3(0.5f, 0, 0)};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case corners::mm | corners::pm:
				v0 = {spec + (8 << 8) + (shade << 22), p4 + glm::vec3(0, -0.5f, 0)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 + glm::vec3(0, -0.5f, 0)};
				v2 = {spec + (shade << 22) + YTEX, p0};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p1};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
		}
	}
	// up of first step
	spec = baseSpec + (0 << 19);
	faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	shade = 0;
	spec += (faceLight << 24);
	switch (corners & (corners::mp | corners::pp)) {
		case corners::pp:
			v0 = {spec + (shade << 22), p4 + glm::vec3(0, 0, -0.5f)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, 0, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(-0.5f, 0.5f, -0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case corners::mp:
			v0 = {spec + 8 + (shade << 22), p4 + glm::vec3(0.5f, 0, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p5 + glm::vec3(0, 0, -0.5f)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0.5f, 0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case corners::mp | corners::pp:
			break ;
		default:
			v0 = {spec + (shade << 22), p4 + glm::vec3(0, 0, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p5 + glm::vec3(0, 0, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
	}
	switch (corners & (corners::mm | corners::pm)) {
		case corners::pm:
			v0 = {spec + (8 << 8) + (shade << 22), p4 + glm::vec3(0, -0.5f, -0.5f)};
			v1 = {spec - (1 << 4) + 8 + (8 << 8) + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, -0.5f, -0.5f)};
			v2 = {spec + (shade << 22) + YTEX, p0 + glm::vec3(0, 0, -0.5f)};
			v3 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(-0.5f, 0, -0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case corners::mm:
			v0 = {spec + 8 + (8 << 8) + (shade << 22), p4 + glm::vec3(0.5f, -0.5f, -0.5f)};
			v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 + glm::vec3(0, -0.5f, -0.5f)};
			v2 = {spec + 8 + (shade << 22) + YTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
			v3 = {spec + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0, -0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case corners::mm | corners::pm:
			break;
		default:
			v0 = {spec + (8 << 8) + (shade << 22), p4 + glm::vec3(0, -0.5f, -0.5f)};
			v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 + glm::vec3(0, -0.5f, -0.5f)};
			v2 = {spec + (shade << 22) + YTEX, p0 + glm::vec3(0, 0, -0.5f)};
			v3 = {spec + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0, -0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
	}
	spec = baseSpec + (faceLight << 24);
	// front minus_x of second step
	spec += (3 << 19);
	switch (corners) {
		case corners::pm | corners::pp: // full width
			v0 = {spec + (shade << 22), p4 + glm::vec3(0.5f, 0, 0)};
			v1 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, 0, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
			break ;
		case corners::pm: // only 'right'
		case corners::pm | corners::pp | corners::mp:
			v0 = {spec + 8 + (shade << 22), p4 + glm::vec3(0.5f, -0.5f, 0)};
			v1 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0, 0)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, -0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
			break ;
		case corners::pp: // only 'left'
		case corners::pm | corners::pp | corners::mm:
			v0 = {spec + (shade << 22), p4 + glm::vec3(0.5f, 0, 0)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0.5f, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, 0, -0.5f)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0.5f, -0.5f)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	// front plus_x of second step
	spec += (1 << 19);
	switch (corners) {
		case corners::mm | corners::mp: // full width
			v0 = {spec + (shade << 22), p1 + glm::vec3(-0.5f, 0, 0)};
			v1 = {spec + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, 0, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p1 + glm::vec3(-0.5f, 0, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p5 + glm::vec3(-0.5f, 0, -0.5f)};
			break ;
		case corners::mp: // only 'right'
		case corners::mm | corners::mp | corners::pm:
			v0 = {spec + 8 + (shade << 22), p1 + glm::vec3(-0.5f, 0.5f, 0)};
			v1 = {spec + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, 0, 0)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p1 + glm::vec3(-0.5f, 0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p5 + glm::vec3(-0.5f, 0, -0.5f)};
			break ;
		case corners::mm: // only 'left'
		case corners::mm | corners::mp | corners::pp:
			v0 = {spec + (shade << 22), p1 + glm::vec3(-0.5f, 0, 0)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, -0.5, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p1 + glm::vec3(-0.5f, 0, -0.5f)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p5 + glm::vec3(-0.5f, -0.5f, -0.5f)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	// front minus_y of second step
	spec -= (3 << 19);
	switch (corners) {
		case corners::mp | corners::pp: // full width
			v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.5f, 0)};
			v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.5f, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
			break ;
		case corners::pp: // only 'right'
		case corners::mp | corners::pp | corners::mm:
			v0 = {spec + 8 + (shade << 22), p0 + glm::vec3(0.5f, 0.5f, 0)};
			v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.5f, 0)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0.5f, 0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
			break ;
		case corners::mp: // only 'left'
		case corners::mp | corners::pp | corners::pm:
			v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.5f, 0)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p1 + glm::vec3(-0.5f, 0.5f, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(-0.5f, 0.5f, -0.5f)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	// front plus_y of second step
	spec += (1 << 19);
	switch (corners) {
		case corners::mm | corners::pm: // full width
			v0 = {spec + (shade << 22), p5 + glm::vec3(0, -0.5f, 0)};
			v1 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0, -0.5f, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p5 + glm::vec3(0, -0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0, -0.5f, -0.5f)};
			break ;
		case corners::pm: // only 'left'
		case corners::mm | corners::pm | corners::mp:
			v0 = {spec + (shade << 22), p5 + glm::vec3(0, -0.5f, 0)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p4 + glm::vec3(0.5f, -0.5f, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p5 + glm::vec3(0, -0.5f, -0.5f)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0.5f, -0.5f, -0.5f)};
			break ;
		case corners::mm: // only 'right'
		case corners::mm | corners::pm | corners::pp:
			v0 = {spec + 8 + (shade << 22), p5 + glm::vec3(-0.5f, -0.5f, 0)};
			v1 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0, -0.5f, 0)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p5 + glm::vec3(-0.5f, -0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0, -0.5f, -0.5f)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = baseSpec + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		shade = 0;
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p2};
		v1 = {spec + (shade << 22) + XTEX, p3};
		v2 = {spec + (shade << 22) + YTEX, p6};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3);
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
	int spec, faceLight, shade;
	t_shaderInput v0, v1, v2, v3;
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = baseSpec + (3 << 19);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (corners::mm | corners::mp)) {
			case corners::mm | corners::mp:
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + XTEX, p0};
				v2 = {spec + (shade << 22) + YTEX, p6};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
				break ;
			case corners::mm:
				v0 = {spec + 8 + (8 << 8) + (shade << 22), p4 + glm::vec3(0, -0.5f, -0.5f)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p0 + glm::vec3(0, 0, -0.5f)};
				v2 = {spec + 8 + (shade << 22) + YTEX, p6 + glm::vec3(0, -0.5f, 0)};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0, 0, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + XTEX, p0};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0, 0, 0.5f)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0, 0, 0.5f)};
				break ;
			case corners::mp:
				v0 = {spec + (8 << 8) + (shade << 22), p4 + glm::vec3(0, 0, -0.5f)};
				v1 = {spec - (1 << 4) + 8 + (8 << 8) + (shade << 22) + XTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
				v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0, 0, 0)};
				v3 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0, 0.5f, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
			default:
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + XTEX, p0};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0, 0, 0.5f)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0, 0, 0.5f)};
				break ;
		}
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = baseSpec + (4 << 19);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (corners::pm | corners::pp)) {
			case corners::pm | corners::pp:
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + XTEX, p5};
				v2 = {spec + (shade << 22) + YTEX, p3};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
				break ;
			case corners::pm:
				v0 = {spec + (8 << 8) + (shade << 22), p1 + glm::vec3(0, 0, -0.5f)};
				v1 = {spec - (1 << 4) + 8 + (8 << 8) + (shade << 22) + XTEX, p5 + glm::vec3(0, -0.5f, -0.5f)};
				v2 = {spec + (shade << 22) + YTEX, p3 + glm::vec3(0, 0, 0)};
				v3 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, -0.5f, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + XTEX, p5};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p3 + glm::vec3(0, 0, 0.5f)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, 0, 0.5f)};
				break ;
			case corners::pp:
				v0 = {spec + 8 + (8 << 8) + (shade << 22), p1 + glm::vec3(0, 0.5f, -0.5f)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 + glm::vec3(0, 0, -0.5f)};
				v2 = {spec + 8 + (shade << 22) + YTEX, p3 + glm::vec3(0, 0.5f, 0)};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, 0, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
			default:
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + XTEX, p5};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p3 + glm::vec3(0, 0, 0.5f)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, 0, 0.5f)};
				break ;
		}
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = baseSpec + (1 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (corners::mm | corners::pm)) {
			case corners::mm | corners::pm:
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + XTEX, p1};
				v2 = {spec + (shade << 22) + YTEX, p2};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
				break ;
			case corners::mm:
				v0 = {spec + (8 << 8) + (shade << 22), p0 + glm::vec3(0, 0, -0.5f)};
				v1 = {spec - (1 << 4) + 8 + (8 << 8) + (shade << 22) + XTEX, p1 + glm::vec3(-0.5f, 0, -0.5f)};
				v2 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0, 0, 0)};
				v3 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(-0.5f, 0, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + XTEX, p1};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p2 + glm::vec3(0, 0, 0.5f)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0, 0.5f)};
				break ;
			case corners::pm:
				v0 = {spec + 8 + (8 << 8) + (shade << 22), p0 + glm::vec3(0.5f, 0, -0.5f)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p1 + glm::vec3(0, 0, -0.5f)};
				v2 = {spec + 8 + (shade << 22) + YTEX, p2 + glm::vec3(0.5f, 0, 0)};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
			default:
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + XTEX, p1};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p2 + glm::vec3(0, 0, 0.5f)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0, 0.5f)};
				break ;
		}
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = baseSpec + (2 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (corners::mp | corners::pp)) {
			case corners::mp | corners::pp:
				v0 = {spec + (shade << 22), p5};
				v1 = {spec + (shade << 22) + XTEX, p4};
				v2 = {spec + (shade << 22) + YTEX, p7};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
				break ;
			case corners::mp:
				v0 = {spec + 8 + (8 << 8) + (shade << 22), p5 + glm::vec3(-0.5f, 0, -0.5f)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 + glm::vec3(0, 0, -0.5f)};
				v2 = {spec + 8 + (shade << 22) + YTEX, p7 + glm::vec3(-0.5f, 0, 0)};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p6 + glm::vec3(0, 0, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				v0 = {spec + (shade << 22), p5};
				v1 = {spec + (shade << 22) + XTEX, p4};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p7 + glm::vec3(0, 0, 0.5f)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p6 + glm::vec3(0, 0, 0.5f)};
				break ;
			case corners::pp:
				v0 = {spec + (8 << 8) + (shade << 22), p5 + glm::vec3(0, 0, -0.5f)};
				v1 = {spec - (1 << 4) + 8 + (8 << 8) + (shade << 22) + XTEX, p4 + glm::vec3(0.5f, 0, -0.5f)};
				v2 = {spec + (shade << 22) + YTEX, p7 + glm::vec3(0, 0, 0)};
				v3 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX + YTEX, p6 + glm::vec3(0.5f, 0, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
			default:
				v0 = {spec + (shade << 22), p5};
				v1 = {spec + (shade << 22) + XTEX, p4};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p7 + glm::vec3(0, 0, 0.5f)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p6 + glm::vec3(0, 0, 0.5f)};
				break ;
		}
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = baseSpec + (0 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		shade = 0;
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p4};
		v1 = {spec + (shade << 22) + XTEX, p5};
		v2 = {spec + (shade << 22) + YTEX, p0};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p1};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	// 'down' of first step
	spec = baseSpec + (5 << 19);
	faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	shade = 0;
	spec += (faceLight << 24);
	switch (corners & (corners::mp | corners::pp)) {
		case corners::pp:
			v0 = {spec + (shade << 22), p2 + glm::vec3(0, 0.5f, 0.5f)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p3 + glm::vec3(-0.5f, 0.5f, 0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0, 0, 0.5f)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(-0.5f, 0, 0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case corners::mp:
			v0 = {spec + 8 + (shade << 22), p2 + glm::vec3(0.5f, 0.5f, 0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p3 + glm::vec3(0, 0.5f, 0.5f)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0.5f, 0, 0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, 0, 0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case corners::mp | corners::pp:
			break ;
		default:
			v0 = {spec + (shade << 22), p2 + glm::vec3(0, 0.5f, 0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p3 + glm::vec3(0, 0.5f, 0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0, 0, 0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, 0, 0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
	}
	switch (corners & (corners::mm | corners::pm)) {
		case corners::pm:
			v0 = {spec + (8 << 8) + (shade << 22), p2 + glm::vec3(0, 0, 0.5f)};
			v1 = {spec - (1 << 4) + 8 + (8 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(-0.5f, 0, 0.5f)};
			v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0, -0.5f, 0.5f)};
			v3 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(-0.5f, -0.5f, 0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case corners::mm:
			v0 = {spec + 8 + (8 << 8) + (shade << 22), p2 + glm::vec3(0.5f, 0, 0.5f)};
			v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(0, 0, 0.5f)};
			v2 = {spec + 8 + (shade << 22) + YTEX, p6 + glm::vec3(0.5f, -0.5f, 0.5f)};
			v3 = {spec + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, -0.5f, 0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case corners::mm | corners::pm:
			break;
		default:
			v0 = {spec + (8 << 8) + (shade << 22), p2 + glm::vec3(0, 0, 0.5f)};
			v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(0, 0, 0.5f)};
			v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0, -0.5f, 0.5f)};
			v3 = {spec + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, -0.5f, 0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
	}
	spec = baseSpec + (faceLight << 24);
	// front minus_x of second step
	spec += (3 << 19);
	switch (corners) {
		case corners::pm | corners::pp: // full width
			v0 = {spec + (shade << 22), p4 + glm::vec3(0.5f, 0, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, 0, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0, -1)};
			break ;
		case corners::pm: // only 'right'
		case corners::pm | corners::pp | corners::mp:
			v0 = {spec + 8 + (shade << 22), p4 + glm::vec3(0.5f, -0.5f, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, -0.5f, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0, -1)};
			break ;
		case corners::pp: // only 'left'
		case corners::pm | corners::pp | corners::mm:
			v0 = {spec + (shade << 22), p4 + glm::vec3(0.5f, 0, -0.5f)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0.5f, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, 0, -1)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0.5f, -1)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	// front plus_x of second step
	spec += (1 << 19);
	switch (corners) {
		case corners::mm | corners::mp: // full width
			v0 = {spec + (shade << 22), p1 + glm::vec3(-0.5f, 0, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, 0, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p1 + glm::vec3(-0.5f, 0, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p5 + glm::vec3(-0.5f, 0, -1)};
			break ;
		case corners::mp: // only 'right'
		case corners::mm | corners::mp | corners::pm:
			v0 = {spec + 8 + (shade << 22), p1 + glm::vec3(-0.5f, 0.5f, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, 0, -0.5f)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p1 + glm::vec3(-0.5f, 0.5f, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p5 + glm::vec3(-0.5f, 0, -1)};
			break ;
		case corners::mm: // only 'left'
		case corners::mm | corners::mp | corners::pp:
			v0 = {spec + (shade << 22), p1 + glm::vec3(-0.5f, 0, -0.5f)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, -0.5, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p1 + glm::vec3(-0.5f, 0, -1)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p5 + glm::vec3(-0.5f, -0.5f, -1)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	// front minus_y of second step
	spec -= (3 << 19);
	switch (corners) {
		case corners::mp | corners::pp: // full width
			v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.5f, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -1)};
			break ;
		case corners::pp: // only 'right'
		case corners::mp | corners::pp | corners::mm:
			v0 = {spec + 8 + (shade << 22), p0 + glm::vec3(0.5f, 0.5f, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0.5f, 0.5f, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -1)};
			break ;
		case corners::mp: // only 'left'
		case corners::mp | corners::pp | corners::pm:
			v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.5f, -0.5f)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p1 + glm::vec3(-0.5f, 0.5f, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -1)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(-0.5f, 0.5f, -1)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	// front plus_y of second step
	spec += (1 << 19);
	switch (corners) {
		case corners::mm | corners::pm: // full width
			v0 = {spec + (shade << 22), p5 + glm::vec3(0, -0.5f, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0, -0.5f, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p5 + glm::vec3(0, -0.5f, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0, -0.5f, -1)};
			break ;
		case corners::pm: // only 'left'
		case corners::mm | corners::pm | corners::mp:
			v0 = {spec + (shade << 22), p5 + glm::vec3(0, -0.5f, -0.5f)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p4 + glm::vec3(0.5f, -0.5f, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p5 + glm::vec3(0, -0.5f, -1)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0.5f, -0.5f, -1)};
			break ;
		case corners::mm: // only 'right'
		case corners::mm | corners::pm | corners::pp:
			v0 = {spec + 8 + (shade << 22), p5 + glm::vec3(-0.5f, -0.5f, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0, -0.5f, -0.5f)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p5 + glm::vec3(-0.5f, -0.5f, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0, -0.5f, -1)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = baseSpec + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (corners::mp | corners::pp)) {
			case corners::mp:
				v0 = {spec + (shade << 22), p2 + glm::vec3(0, 0.5f, 0)};
				v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p3 + glm::vec3(-0.5f, 0.5f, 0)};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0, 0, 0)};
				v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(-0.5f, 0, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case corners::pp:
				v0 = {spec + 8 + (shade << 22), p2 + glm::vec3(0.5f, 0.5f, 0)};
				v1 = {spec + (shade << 22) + XTEX, p3 + glm::vec3(0, 0.5f, 0)};
				v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0.5f, 0, 0)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, 0, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case corners::mp | corners::pp:
				v0 = {spec + (shade << 22), p2 + glm::vec3(0, 0.5f, 0)};
				v1 = {spec + (shade << 22) + XTEX, p3 + glm::vec3(0, 0.5f, 0)};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
		}
		switch (corners & (corners::mm | corners::pm)) {
			case corners::mm:
				v0 = {spec + (8 << 8) + (shade << 22), p2 + glm::vec3(0, 0, 0)};
				v1 = {spec - (1 << 4) + 8 + (8 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(-0.5f, 0, 0)};
				v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0, -0.5f, 0)};
				v3 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(-0.5f, -0.5f, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case corners::pm:
				v0 = {spec + 8 + (8 << 8) + (shade << 22), p2 + glm::vec3(0.5f, 0, 0)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(0, 0, 0)};
				v2 = {spec + 8 + (shade << 22) + YTEX, p6 + glm::vec3(0.5f, -0.5f, 0)};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, -0.5f, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case corners::mm | corners::pm:
				v0 = {spec + (8 << 8) + (shade << 22), p2};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p3};
				v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0, -0.5f, 0)};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, -0.5f, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
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
	int xtex_l, xtex_r, spec, faceLight, shade = 0;
	t_shaderInput v0, v1, v2, v3;
	int bitfield = value >> offset::blocks::bitfield;
	bool open = !!(bitfield & door::open) ^ ((value >> offset::redstone::powered) & 0x1);
	switch ((value >> offset::blocks::orientation) & 0x7) {
		case face_dir::minus_x:
			if (!open) {
				orientation = face_dir::minus_x;
				xtex_l = (bitfield & door::right_hinge) ? 0 : XTEX;
				xtex_r = (bitfield & door::right_hinge) ? XTEX : 0;
			} else if (bitfield & door::right_hinge) {
				orientation = face_dir::plus_y;
				xtex_l = XTEX;
				xtex_r = 0;
			} else {
				orientation = face_dir::minus_y;
				xtex_l = 0;
				xtex_r = XTEX;
			}
			break ;
		case face_dir::plus_x:
			if (!open) {
				orientation = face_dir::plus_x;
				xtex_l = (bitfield & door::right_hinge) ? XTEX : 0;
				xtex_r = (bitfield & door::right_hinge) ? 0 : XTEX;
			} else if (bitfield & door::right_hinge) {
				orientation = face_dir::minus_y;
				xtex_l = 0;
				xtex_r = XTEX;
			} else {
				orientation = face_dir::plus_y;
				xtex_l = XTEX;
				xtex_r = 0;
			}
			break ;
		case face_dir::minus_y:
			if (!open) {
				orientation = face_dir::minus_y;
				xtex_l = (bitfield & door::right_hinge) ? 0 : XTEX;
				xtex_r = (bitfield & door::right_hinge) ? XTEX : 0;
			} else if (bitfield & door::right_hinge) {
				orientation = face_dir::minus_x;
				xtex_l = 0;
				xtex_r = XTEX;
			} else {
				orientation = face_dir::plus_x;
				xtex_l = XTEX;
				xtex_r = 0;
			}
			break ;
		case face_dir::plus_y:
			if (!open) {
				orientation = face_dir::plus_y;
				xtex_l = (bitfield & door::right_hinge) ? XTEX : 0;
				xtex_r = (bitfield & door::right_hinge) ? 0 : XTEX;
			} else if (bitfield & door::right_hinge) {
				orientation = face_dir::plus_x;
				xtex_l = XTEX;
				xtex_r = 0;
			} else {
				orientation = face_dir::minus_x;
				xtex_l = 0;
				xtex_r = XTEX;
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
			shade = 0;
			spec += (faceLight << 24);
			v0 = {spec + (shade << 22) + xtex_r, p4};
			v1 = {spec + (shade << 22) + xtex_l, p0};
			v2 = {spec + (shade << 22) + xtex_r + YTEX, p6};
			v3 = {spec + (shade << 22) + xtex_l + YTEX, p2};
			face_vertices(vertices, v0, v1, v2, v3);
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				spec = (this->texX(face_dir::plus_x, value) << 4) + (this->texY(face_dir::plus_x, value) << 12) + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22) + xtex_l, p1};
				v1 = {spec + (shade << 22) + xtex_r, p5};
				v2 = {spec + (shade << 22) + xtex_l + YTEX, p3};
				v3 = {spec + (shade << 22) + xtex_r + YTEX, p7};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				spec = (this->texX(face_dir::minus_y, value) << 4) + (this->texY(face_dir::minus_y, value) << 12) + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p1};
				v2 = {spec + (shade << 22) + YTEX, p2};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p3};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				spec = (this->texX(face_dir::plus_y, value) << 4) + (this->texY(face_dir::plus_y, value) << 12) + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p5};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p4};
				v2 = {spec + (shade << 22) + YTEX, p7};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p6};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if ((bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				spec = (this->texX(face_dir::plus_z) << 4) + (this->texY(face_dir::plus_z) << 12) + (0 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + XTEX, p4};
				v2 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + YTEX, p1};
				v3 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + XTEX + YTEX, p5};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (!(bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				spec = (this->texX(face_dir::minus_z, face_dir::minus_z) << 4) + (this->texY(face_dir::minus_z, face_dir::minus_z) << 12) + (5 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22) + (13 << 8), p6};
				v1 = {spec + (shade << 22) + (13 << 8) + XTEX, p2};
				v2 = {spec + (shade << 22) + YTEX, p7};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
				face_vertices(vertices, v0, v1, v2, v3);
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
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22) + xtex_r, p4};
				v1 = {spec + (shade << 22) + xtex_l, p0};
				v2 = {spec + (shade << 22) + xtex_r + YTEX, p6};
				v3 = {spec + (shade << 22) + xtex_l + YTEX, p2};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			spec = (this->texX(face_dir::plus_x, value) << 4) + (this->texY(face_dir::plus_x, value) << 12) + (4 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			spec += (faceLight << 24);
			v0 = {spec + (shade << 22) + xtex_l, p1};
			v1 = {spec + (shade << 22) + xtex_r, p5};
			v2 = {spec + (shade << 22) + xtex_l + YTEX, p3};
			v3 = {spec + (shade << 22) + xtex_r + YTEX, p7};
			face_vertices(vertices, v0, v1, v2, v3);
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				spec = (this->texX(face_dir::minus_y, value) << 4) + (this->texY(face_dir::minus_y, value) << 12) + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p1};
				v2 = {spec + (shade << 22) + YTEX, p2};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p3};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				spec = (this->texX(face_dir::plus_y, value) << 4) + (this->texY(face_dir::plus_y, value) << 12) + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p5};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p4};
				v2 = {spec + (shade << 22) + YTEX, p7};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p6};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if ((bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				spec = (this->texX(face_dir::plus_z) << 4) + (this->texY(face_dir::plus_z) << 12) + (0 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + XTEX, p4};
				v2 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + YTEX, p1};
				v3 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + XTEX + YTEX, p5};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (!(bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				spec = (this->texX(face_dir::minus_z, face_dir::minus_z) << 4) + (this->texY(face_dir::minus_z, face_dir::minus_z) << 12) + (5 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22) + (13 << 8), p6};
				v1 = {spec + (shade << 22) + (13 << 8) + XTEX, p2};
				v2 = {spec + (shade << 22) + YTEX, p7};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
				face_vertices(vertices, v0, v1, v2, v3);
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
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p0};
				v2 = {spec + (shade << 22) + YTEX, p6};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p2};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				spec = (this->texX(face_dir::plus_x, value) << 4) + (this->texY(face_dir::plus_x, value) << 12) + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p5};
				v2 = {spec + (shade << 22) + YTEX, p3};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p7};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			spec = (this->texX(face_dir::minus_y, value) << 4) + (this->texY(face_dir::minus_y, value) << 12) + (1 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			spec += (faceLight << 24);
			v0 = {spec + (shade << 22) + xtex_r, p0};
			v1 = {spec + (shade << 22) + xtex_l, p1};
			v2 = {spec + (shade << 22) + xtex_r + YTEX, p2};
			v3 = {spec + (shade << 22) + xtex_l + YTEX, p3};
			face_vertices(vertices, v0, v1, v2, v3);
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				spec = (this->texX(face_dir::plus_y, value) << 4) + (this->texY(face_dir::plus_y, value) << 12) + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22) + xtex_l, p5};
				v1 = {spec + (shade << 22) + xtex_r, p4};
				v2 = {spec + (shade << 22) + xtex_l + YTEX, p7};
				v3 = {spec + (shade << 22) + xtex_r + YTEX, p6};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if ((bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				spec = (this->texX(face_dir::plus_z) << 4) + (this->texY(face_dir::plus_z) << 12) + (0 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + XTEX, p0};
				v2 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + YTEX, p5};
				v3 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + XTEX + YTEX, p4};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (!(bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				spec = (this->texX(face_dir::minus_z, face_dir::minus_z) << 4) + (this->texY(face_dir::minus_z, face_dir::minus_z) << 12) + (5 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22) + (13 << 8), p7};
				v1 = {spec + (shade << 22) + (13 << 8) + XTEX, p6};
				v2 = {spec + (shade << 22) + YTEX, p3};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
				face_vertices(vertices, v0, v1, v2, v3);
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
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p0};
				v2 = {spec + (shade << 22) + YTEX, p6};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p2};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				spec = (this->texX(face_dir::plus_x, value) << 4) + (this->texY(face_dir::plus_x, value) << 12) + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p5};
				v2 = {spec + (shade << 22) + YTEX, p3};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p7};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				spec = (this->texX(face_dir::minus_y, value) << 4) + (this->texY(face_dir::minus_y, value) << 12) + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22) + xtex_r, p0};
				v1 = {spec + (shade << 22) + xtex_l, p1};
				v2 = {spec + (shade << 22) + xtex_r + YTEX, p2};
				v3 = {spec + (shade << 22) + xtex_l + YTEX, p3};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			spec = (this->texX(face_dir::plus_y, value) << 4) + (this->texY(face_dir::plus_y, value) << 12) + (2 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			spec += (faceLight << 24);
			v0 = {spec + (shade << 22) + xtex_l, p5};
			v1 = {spec + (shade << 22) + xtex_r, p4};
			v2 = {spec + (shade << 22) + xtex_l + YTEX, p7};
			v3 = {spec + (shade << 22) + xtex_r + YTEX, p6};
			face_vertices(vertices, v0, v1, v2, v3);
			if ((bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				spec = (this->texX(face_dir::plus_z) << 4) + (this->texY(face_dir::plus_z) << 12) + (0 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + XTEX, p0};
				v2 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + YTEX, p5};
				v3 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + XTEX + YTEX, p4};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (!(bitfield & door::upper_half) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				spec = (this->texX(face_dir::minus_z, face_dir::minus_z) << 4) + (this->texY(face_dir::minus_z, face_dir::minus_z) << 12) + (5 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22) + (13 << 8), p7};
				v1 = {spec + (shade << 22) + (13 << 8) + XTEX, p6};
				v2 = {spec + (shade << 22) + YTEX, p3};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
				face_vertices(vertices, v0, v1, v2, v3);
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

	int shade = 0;
	int bitfield = value >> offset::blocks::bitfield;
	int baseSpec = (textureX << 4) + (textureY << 12);
	int spec, faceLight;
	t_shaderInput v0, v1, v2, v3;
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
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + XTEX, p0};
				v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p6};
				v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p2};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				spec = baseSpec + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + XTEX, p5};
				v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p3};
				v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p7};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				spec = baseSpec + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + XTEX, p1};
				v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p2};
				v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p3};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				spec = baseSpec + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p5};
				v1 = {spec + (shade << 22) + XTEX, p4};
				v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p7};
				v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p6};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			spec = baseSpec + (0 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			spec += (faceLight << 24);
			v0 = {spec + (shade << 22), p4};
			v1 = {spec + (shade << 22) + XTEX, p5};
			v2 = {spec + (shade << 22) + YTEX, p0};
			v3 = {spec + (shade << 22) + XTEX + YTEX, p1};
			face_vertices(vertices, v0, v1, v2, v3);
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
				spec = baseSpec + (5 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p2};
				v1 = {spec + (shade << 22) + XTEX, p3};
				v2 = {spec + (shade << 22) + YTEX, p6};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
				face_vertices(vertices, v0, v1, v2, v3);
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
				v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p4};
				v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p0};
				v2 = {spec + (shade << 22), p6};
				v3 = {spec + (shade << 22) + XTEX, p2};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
				spec = baseSpec + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p1};
				v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p5};
				v2 = {spec + (shade << 22), p3};
				v3 = {spec + (shade << 22) + XTEX, p7};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
				spec = baseSpec + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p0};
				v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p1};
				v2 = {spec + (shade << 22), p2};
				v3 = {spec + (shade << 22) + XTEX, p3};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
				spec = baseSpec + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p5};
				v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p4};
				v2 = {spec + (shade << 22), p7};
				v3 = {spec + (shade << 22) + XTEX, p6};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
				spec = baseSpec + (0 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + XTEX, p5};
				v2 = {spec + (shade << 22) + YTEX, p0};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p1};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			spec = baseSpec + (5 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			spec += (faceLight << 24);
			v0 = {spec + (shade << 22), p2};
			v1 = {spec + (shade << 22) + XTEX, p3};
			v2 = {spec + (shade << 22) + YTEX, p6};
			v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
			face_vertices(vertices, v0, v1, v2, v3);
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
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + XTEX, p0};
				v2 = {spec + (shade << 22) + YTEX, p6};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
				face_vertices(vertices, v0, v1, v2, v3);
				if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
					spec = baseSpec + (4 << 19);
					faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p1};
					v1 = {spec + (shade << 22) + XTEX, p5};
					v2 = {spec + (shade << 22) + YTEX, p3};
					v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
					spec = baseSpec + (1 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
					spec += (faceLight << 24);
					v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p2};
					v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p0};
					v2 = {spec + (shade << 22) + XTEX, p3};
					v3 = {spec + (shade << 22), p1};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
					spec = baseSpec + (2 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p7};
					v1 = {spec + (shade << 22) + XTEX, p5};
					v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p6};
					v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p4};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
					spec = baseSpec + (0 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
					spec += (faceLight << 24);
					v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p0};
					v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p4};
					v2 = {spec + (shade << 22) + XTEX, p1};
					v3 = {spec + (shade << 22), p5};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
					spec = baseSpec + (5 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
					spec += (faceLight << 24);
					v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p6};
					v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p2};
					v2 = {spec + (shade << 22), p7};
					v3 = {spec + (shade << 22) + XTEX, p3};
					face_vertices(vertices, v0, v1, v2, v3);
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
					v0 = {spec + (shade << 22), p4};
					v1 = {spec + (shade << 22) + XTEX, p0};
					v2 = {spec + (shade << 22) + YTEX, p6};
					v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				spec = baseSpec + (4 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + XTEX, p5};
				v2 = {spec + (shade << 22) + YTEX, p3};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
				face_vertices(vertices, v0, v1, v2, v3);
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
					spec = baseSpec + (1 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p2};
					v1 = {spec + (shade << 22) + XTEX, p0};
					v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p3};
					v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p1};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
					spec = baseSpec + (2 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
					spec += (faceLight << 24);
					v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p7};
					v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p5};
					v2 = {spec + (shade << 22) + XTEX, p6};
					v3 = {spec + (shade << 22), p4};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
					spec = baseSpec + (0 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p0};
					v1 = {spec + (shade << 22) + XTEX, p4};
					v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p1};
					v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p5};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
					spec = baseSpec + (5 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p6};
					v1 = {spec + (shade << 22) + XTEX, p2};
					v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p7};
					v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p3};
					face_vertices(vertices, v0, v1, v2, v3);
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
					v0 = {spec + (shade << 22), p6};
					v1 = {spec + (shade << 22) + XTEX, p4};
					v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p2};
					v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p0};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
					spec = baseSpec + (4 << 19);
					faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
					spec += (faceLight << 24);
					v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p3};
					v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p1};
					v2 = {spec + (shade << 22) + XTEX, p7};
					v3 = {spec + (shade << 22), p5};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				spec = baseSpec + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + XTEX, p1};
				v2 = {spec + (shade << 22) + YTEX, p2};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
				face_vertices(vertices, v0, v1, v2, v3);
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
					spec = baseSpec + (2 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p5};
					v1 = {spec + (shade << 22) + XTEX, p4};
					v2 = {spec + (shade << 22) + YTEX, p7};
					v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
					spec = baseSpec + (0 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p4};
					v1 = {spec + (shade << 22) + XTEX, p5};
					v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p0};
					v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p1};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
					spec = baseSpec + (5 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
					spec += (faceLight << 24);
					v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p2};
					v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p3};
					v2 = {spec + (shade << 22) + XTEX, p6};
					v3 = {spec + (shade << 22), p7};
					face_vertices(vertices, v0, v1, v2, v3);
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
					v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p6};
					v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p4};
					v2 = {spec + (shade << 22) + XTEX, p2};
					v3 = {spec + (shade << 22), p0};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
					spec = baseSpec + (4 << 19);
					faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p3};
					v1 = {spec + (shade << 22) + XTEX, p1};
					v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p7};
					v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p5};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
					spec = baseSpec + (1 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p0};
					v1 = {spec + (shade << 22) + XTEX, p1};
					v2 = {spec + (shade << 22) + YTEX, p2};
					v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				spec = baseSpec + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p5};
				v1 = {spec + (shade << 22) + XTEX, p4};
				v2 = {spec + (shade << 22) + YTEX, p7};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
				face_vertices(vertices, v0, v1, v2, v3);
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
					spec = baseSpec + (0 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
					spec += (faceLight << 24);
					v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p4};
					v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p5};
					v2 = {spec + (shade << 22) + XTEX, p0};
					v3 = {spec + (shade << 22), p1};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
					spec = baseSpec + (5 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p2};
					v1 = {spec + (shade << 22) + XTEX, p3};
					v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p6};
					v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p7};
					face_vertices(vertices, v0, v1, v2, v3);
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
	int shade = 0;
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
	t_shaderInput v0 = {spec + 6 + (shade << 22), pp4};
	t_shaderInput v1 = {spec - 6 + (shade << 22) + XTEX, pp0};
	t_shaderInput v2 = {spec + 6 + (shade << 22) + YTEX, pp6};
	t_shaderInput v3 = {spec - 6 + (shade << 22) + XTEX + YTEX, pp2};
	face_vertices(vertices, v0, v1, v2, v3);
	spec = baseSpec + (4 << 19);
	spec += (faceLight << 24);
	v0 = {spec + 6 + (shade << 22), pp1};
	v1 = {spec - 6 + (shade << 22) + XTEX, pp5};
	v2 = {spec + 6 + (shade << 22) + YTEX, pp3};
	v3 = {spec - 6 + (shade << 22) + XTEX + YTEX, pp7};
	face_vertices(vertices, v0, v1, v2, v3);
	spec = baseSpec + (1 << 19);
	spec += (faceLight << 24);
	v0 = {spec + 6 + (shade << 22), pp0};
	v1 = {spec - 6 + (shade << 22) + XTEX, pp1};
	v2 = {spec + 6 + (shade << 22) + YTEX, pp2};
	v3 = {spec - 6 + (shade << 22) + XTEX + YTEX, pp3};
	face_vertices(vertices, v0, v1, v2, v3);
	spec = baseSpec + (2 << 19);
	spec += (faceLight << 24);
	v0 = {spec + 6 + (shade << 22), pp5};
	v1 = {spec - 6 + (shade << 22) + XTEX, pp4};
	v2 = {spec + 6 + (shade << 22) + YTEX, pp7};
	v3 = {spec - 6 + (shade << 22) + XTEX + YTEX, pp6};
	face_vertices(vertices, v0, v1, v2, v3);
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = baseSpec + (0 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		spec += (faceLight << 24);
		v0 = {spec + 6 + (6 << 8) + (shade << 22), pp4};
		v1 = {spec - 6 + (6 << 8) + (shade << 22) + XTEX, pp5};
		v2 = {spec + 6 - (6 << 8) + (shade << 22) + YTEX, pp0};
		v3 = {spec - 6 - (6 << 8) + (shade << 22) + XTEX + YTEX, pp1};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = baseSpec + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		spec += (faceLight << 24);
		v0 = {spec + 6 + (6 << 8) + (shade << 22), pp2};
		v1 = {spec - 6 + (6 << 8) + (shade << 22) + XTEX, pp3};
		v2 = {spec + 6 - (6 << 8) + (shade << 22) + YTEX, pp6};
		v3 = {spec - 6 - (6 << 8) + (shade << 22) + XTEX + YTEX, pp7};
		face_vertices(vertices, v0, v1, v2, v3);
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
			v0 = {spec + ((1 + 6*i) << 8) + (shade << 22), pp0};
			v1 = {spec - 10 + ((1 + 6*i) << 8) + (shade << 22) + XTEX, pp1};
			v2 = {spec - ((12 - 6*i) << 8) + (shade << 22) + YTEX, pp2};
			v3 = {spec - 10 - ((12 - 6*i) << 8) + (shade << 22) + XTEX + YTEX, pp3};
			face_vertices(vertices, v0, v1, v2, v3);
			spec = baseSpec + (2 << 19);
			spec += (faceLight << 24);
			v0 = {spec + 10 + ((1 + 6*i) << 8) + (shade << 22), pp5};
			v1 = {spec + ((1 + 6*i) << 8) + (shade << 22) + XTEX, pp4};
			v2 = {spec + 10 - ((12 - 6*i) << 8) + (shade << 22) + YTEX, pp7};
			v3 = {spec - ((12 - 6*i) << 8) + (shade << 22) + XTEX + YTEX, pp6};
			face_vertices(vertices, v0, v1, v2, v3);
			spec = baseSpec + (0 << 19);
			spec += (faceLight << 24);
			v0 = {spec + (7 << 8) + (shade << 22), pp4};
			v1 = {spec + (7 << 8) + (shade << 22) - 10 + XTEX, pp5};
			v2 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + YTEX, pp0};
			v3 = {spec - (1 << 12) + (9 << 8) + (shade << 22) - 10 + XTEX + YTEX, pp1};
			face_vertices(vertices, v0, v1, v2, v3);
			spec = baseSpec + (5 << 19);
			spec += (faceLight << 24);
			v0 = {spec + (7 << 8) + (shade << 22), pp2};
			v1 = {spec + (7 << 8) + (shade << 22) - 10 + XTEX, pp3};
			v2 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + YTEX, pp6};
			v3 = {spec - (1 << 12) + (9 << 8) + (shade << 22) - 10 + XTEX + YTEX, pp7};
			face_vertices(vertices, v0, v1, v2, v3);
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
			v0 = {spec + 10 + ((1 + 6*i) << 8) + (shade << 22), pp0};
			v1 = {spec + ((1 + 6*i) << 8) + (shade << 22) + XTEX, pp1};
			v2 = {spec + 10 - ((12 - 6*i) << 8) + (shade << 22) + YTEX, pp2};
			v3 = {spec - ((12 - 6*i) << 8) + (shade << 22) + XTEX + YTEX, pp3};
			face_vertices(vertices, v0, v1, v2, v3);
			spec = baseSpec + (2 << 19);
			spec += (faceLight << 24);
			v0 = {spec + ((1 + 6*i) << 8) + (shade << 22), pp5};
			v1 = {spec - 10 + ((1 + 6*i) << 8) + (shade << 22) + XTEX, pp4};
			v2 = {spec - ((12 - 6*i) << 8) + (shade << 22) + YTEX, pp7};
			v3 = {spec - 10 - ((12 - 6*i) << 8) + (shade << 22) + XTEX + YTEX, pp6};
			face_vertices(vertices, v0, v1, v2, v3);
			spec = baseSpec + (0 << 19);
			spec += (faceLight << 24);
			v0 = {spec + (7 << 8) + (shade << 22) + 10, pp4};
			v1 = {spec + (7 << 8) + (shade << 22) + XTEX, pp5};
			v2 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + 10 + YTEX, pp0};
			v3 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + XTEX + YTEX, pp1};
			face_vertices(vertices, v0, v1, v2, v3);
			spec = baseSpec + (5 << 19);
			spec += (faceLight << 24);
			v0 = {spec + (7 << 8) + (shade << 22) + 10, pp2};
			v1 = {spec + (7 << 8) + (shade << 22) + XTEX, pp3};
			v2 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + 10 + YTEX, pp6};
			v3 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + XTEX + YTEX, pp7};
			face_vertices(vertices, v0, v1, v2, v3);
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
			v0 = {spec + 10 + ((1 + 6*i) << 8) + (shade << 22), pp4};
			v1 = {spec + ((1 + 6*i) << 8) + (shade << 22) + XTEX, pp0};
			v2 = {spec + 10 - ((12 - 6*i) << 8) + (shade << 22) + YTEX, pp6};
			v3 = {spec - ((12 - 6*i) << 8) + (shade << 22) + XTEX + YTEX, pp2};
			face_vertices(vertices, v0, v1, v2, v3);
			spec = baseSpec + (4 << 19);
			spec += (faceLight << 24);
			v0 = {spec + ((1 + 6*i) << 8) + (shade << 22), pp1};
			v1 = {spec - 10 + ((1 + 6*i) << 8) + (shade << 22) + XTEX, pp5};
			v2 = {spec - ((12 - 6*i) << 8) + (shade << 22) + YTEX, pp3};
			v3 = {spec - 10 - ((12 - 6*i) << 8) + (shade << 22) + XTEX + YTEX, pp7};
			face_vertices(vertices, v0, v1, v2, v3);
			spec = baseSpec + (0 << 19);
			spec += (faceLight << 24);
			v0 = {spec + (7 << 8) + (shade << 22), pp5};
			v1 = {spec + (7 << 8) + (shade << 22) - 10 + XTEX, pp1};
			v2 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + YTEX, pp4};
			v3 = {spec - (1 << 12) + (9 << 8) + (shade << 22) - 10 + XTEX + YTEX, pp0};
			face_vertices(vertices, v0, v1, v2, v3);
			spec = baseSpec + (5 << 19);
			spec += (faceLight << 24);
			v0 = {spec + (7 << 8) + (shade << 22), pp3};
			v1 = {spec + (7 << 8) + (shade << 22) - 10 + XTEX, pp7};
			v2 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + YTEX, pp2};
			v3 = {spec - (1 << 12) + (9 << 8) + (shade << 22) - 10 + XTEX + YTEX, pp6};
			face_vertices(vertices, v0, v1, v2, v3);
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
			v0 = {spec + ((1 + 6*i) << 8) + (shade << 22), pp4};
			v1 = {spec - 10 + ((1 + 6*i) << 8) + (shade << 22) + XTEX, pp0};
			v2 = {spec - ((12 - 6*i) << 8) + (shade << 22) + YTEX, pp6};
			v3 = {spec - 10 - ((12 - 6*i) << 8) + (shade << 22) + XTEX + YTEX, pp2};
			face_vertices(vertices, v0, v1, v2, v3);
			spec = baseSpec + (4 << 19);
			spec += (faceLight << 24);
			v0 = {spec + 10 + ((1 + 6*i) << 8) + (shade << 22), pp1};
			v1 = {spec + ((1 + 6*i) << 8) + (shade << 22) + XTEX, pp5};
			v2 = {spec + 10 - ((12 - 6*i) << 8) + (shade << 22) + YTEX, pp3};
			v3 = {spec - ((12 - 6*i) << 8) + (shade << 22) + XTEX + YTEX, pp7};
			face_vertices(vertices, v0, v1, v2, v3);
			spec = baseSpec + (0 << 19);
			spec += (faceLight << 24);
			v0 = {spec + (7 << 8) + (shade << 22) + 10, pp5};
			v1 = {spec + (7 << 8) + (shade << 22) + XTEX, pp1};
			v2 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + 10 + YTEX, pp4};
			v3 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + XTEX + YTEX, pp0};
			face_vertices(vertices, v0, v1, v2, v3);
			spec = baseSpec + (5 << 19);
			spec += (faceLight << 24);
			v0 = {spec + (7 << 8) + (shade << 22) + 10, pp3};
			v1 = {spec + (7 << 8) + (shade << 22) + XTEX, pp7};
			v2 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + 10 + YTEX, pp2};
			v3 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + XTEX + YTEX, pp6};
			face_vertices(vertices, v0, v1, v2, v3);
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

	int shade = 0;
	int bitfield = (value >> offset::blocks::bitfield);
	int baseSpec = (this->texX(face_dir::plus_z, fence::arm_end) << 4) + (this->texY() << 12);
	int faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	int spec;
	t_shaderInput v0, v1, v2, v3;
	if (bitfield & fence::mx) {
		spec = baseSpec + (2 << 4) + (1 << 19);
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.4375f, 0)};
		v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.4375f, 0)};
		v2 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0, 0.4375f, 0)};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0.4375f, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
		spec += (1 << 19);
		v0 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.4375f, 0)};
		v1 = {spec + (shade << 22), p4 - glm::vec3(0, 0.4375f, 0)};
		v2 = {spec + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0, 0.4375f, 0)};
		v3 = {spec + (shade << 22) + YTEX, p6 - glm::vec3(0, 0.4375f, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
	} else {
		spec = baseSpec + (3 << 19) - (!!((value >> 12) & (fence::my | fence::py)) << 12);
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p4 + glm::vec3(0.4375f, 0, 0)};
		v1 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0.4375f, 0, 0)};
		v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0.4375f, 0, 0)};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0.4375f, 0, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (bitfield & fence::px) {
		spec = baseSpec + (3 << 4) + (1 << 19);
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.4375f, 0)};
		v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.4375f, 0)};
		v2 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0, 0.4375f, 0)};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0.4375f, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
		spec += (1 << 19);
		v0 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.4375f, 0)};
		v1 = {spec + (shade << 22), p4 - glm::vec3(0, 0.4375f, 0)};
		v2 = {spec + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0, 0.4375f, 0)};
		v3 = {spec + (shade << 22) + YTEX, p6 - glm::vec3(0, 0.4375f, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
	} else {
		spec = baseSpec + (4 << 19) - (!!((value >> 12) & (fence::my | fence::py)) << 12);
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p1 - glm::vec3(0.4375f, 0, 0)};
		v1 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0.4375f, 0, 0)};
		v2 = {spec + (shade << 22) + YTEX, p3 - glm::vec3(0.4375f, 0, 0)};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0.4375f, 0, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (bitfield & fence::my) {
		spec = baseSpec + (2 << 4) + (3 << 19);
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0.4375f, 0, 0)};
		v1 = {spec + (shade << 22), p0 + glm::vec3(0.4375f, 0, 0)};
		v2 = {spec + (shade << 22) + XTEX + YTEX, p6 + glm::vec3(0.4375f, 0, 0)};
		v3 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0.4375f, 0, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
		spec += (1 << 19);
		v0 = {spec + (shade << 22), p1 - glm::vec3(0.4375f, 0, 0)};
		v1 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0.4375f, 0, 0)};
		v2 = {spec + (shade << 22) + YTEX, p3 - glm::vec3(0.4375f, 0, 0)};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0.4375f, 0, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
	} else {
		spec = baseSpec + (1 << 19) - (!!((value >> 12) & (fence::mx | fence::px)) << 12);
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.4375f, 0)};
		v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.4375f, 0)};
		v2 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0, 0.4375f, 0)};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0.4375f, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (bitfield & fence::py) {
		spec = baseSpec + (3 << 4) + (3 << 19);
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0.4375f, 0, 0)};
		v1 = {spec + (shade << 22), p0 + glm::vec3(0.4375f, 0, 0)};
		v2 = {spec + (shade << 22) + XTEX + YTEX, p6 + glm::vec3(0.4375f, 0, 0)};
		v3 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0.4375f, 0, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
		spec += (1 << 19);
		v0 = {spec + (shade << 22), p1 - glm::vec3(0.4375f, 0, 0)};
		v1 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0.4375f, 0, 0)};
		v2 = {spec + (shade << 22) + YTEX, p3 - glm::vec3(0.4375f, 0, 0)};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0.4375f, 0, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
	} else {
		spec = baseSpec + (2 << 19) - (!!((value >> 12) & (fence::mx | fence::px)) << 12);
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p5 - glm::vec3(0, 0.4375f, 0)};
		v1 = {spec + (shade << 22) + XTEX, p4 - glm::vec3(0, 0.4375f, 0)};
		v2 = {spec + (shade << 22) + YTEX, p7 - glm::vec3(0, 0.4375f, 0)};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p6 - glm::vec3(0, 0.4375f, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1), face_dir::plus_z)) {
		spec = baseSpec + (0 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		spec += (faceLight << 24);
		if (bitfield & fence::py) {
			v0 = {spec + (shade << 22), p4};
			v1 = {spec + (shade << 22) + XTEX, p5};
		} else {
			v0 = {spec + (7 << 8) + (shade << 22), p4 - glm::vec3(0, 0.4375f, 0)};
			v1 = {spec + (7 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.4375f, 0)};
		}
		if (bitfield & fence::my) {
			v2 = {spec + (shade << 22) + YTEX, p0};
			v3 = {spec + (shade << 22) + XTEX + YTEX, p1};
		} else {
			v2 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.4375f, 0)};
			v3 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.4375f, 0)};
		}
		face_vertices(vertices, v0, v1, v2, v3);
		if (bitfield & fence::mx) {
			v0 = {spec + (shade << 22), p0};
			v1 = {spec + (shade << 22) + XTEX, p4};
			v2 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0.4375f, 0, 0)};
			v3 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0.4375f, 0, 0)};
			face_vertices(vertices, v0, v1, v2, v3);
		}
		if (bitfield & fence::px) {
			v0 = {spec + (shade << 22), p5};
			v1 = {spec + (shade << 22) + XTEX, p1};
			v2 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + YTEX, p5 - glm::vec3(0.4375f, 0, 0)};
			v3 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + XTEX + YTEX, p1 - glm::vec3(0.4375f, 0, 0)};
			face_vertices(vertices, v0, v1, v2, v3);
		}
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1), face_dir::minus_z)) {
		spec = baseSpec + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		spec += (faceLight << 24);
		if (bitfield & fence::my) {
			v0 = {spec + (shade << 22), p2};
			v1 = {spec + (shade << 22) + XTEX, p3};
		} else {
			v0 = {spec + (7 << 8) + (shade << 22), p2 + glm::vec3(0, 0.4375f, 0)};
			v1 = {spec + (7 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(0, 0.4375f, 0)};
		}
		if (bitfield & fence::py) {
			v2 = {spec + (shade << 22) + YTEX, p6};
			v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
		} else {
			v2 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + YTEX, p6 - glm::vec3(0, 0.4375f, 0)};
			v3 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0, 0.4375f, 0)};
		}
		face_vertices(vertices, v0, v1, v2, v3);
		if (bitfield & fence::mx) {
			v0 = {spec + (shade << 22), p6};
			v1 = {spec + (shade << 22) + XTEX, p2};
			v2 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0.4375f, 0, 0)};
			v3 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0.4375f, 0, 0)};
			face_vertices(vertices, v0, v1, v2, v3);
		}
		if (bitfield & fence::px) {
			v0 = {spec + (shade << 22), p7};
			v1 = {spec + (shade << 22) + XTEX, p3};
			v2 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + YTEX, p7 - glm::vec3(0.4375f, 0, 0)};
			v3 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + XTEX + YTEX, p3 - glm::vec3(0.4375f, 0, 0)};
			face_vertices(vertices, v0, v1, v2, v3);
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
	t_shaderInput v0 = {spec, {p4.x + 3.0f * one16th, p4.y, p4.z}};
	t_shaderInput v1 = {spec + XTEX, {p0.x + 3.0f * one16th, p0.y, p0.z}};
	t_shaderInput v2 = {spec + YTEX, {p6.x + 3.0f * one16th, p6.y, p6.z}};
	t_shaderInput v3 = {spec + XTEX + YTEX, {p2.x + 3.0f * one16th, p2.y, p2.z}};
	face_vertices(vertices, v0, v1, v2, v3); // -x
	v0 = {spec + XTEX, {p0.x + 13.0f * one16th, p0.y, p0.z}};
	v1 = {spec, {p4.x + 13.0f * one16th, p4.y, p4.z}};
	v2 = {spec + XTEX + YTEX, {p2.x + 13.0f * one16th, p2.y, p2.z}};
	v3 = {spec + YTEX, {p6.x + 13.0f * one16th, p6.y, p6.z}};
	face_vertices(vertices, v0, v1, v2, v3); // +x
	v0 = {spec, {p0.x, p0.y + 3.0f * one16th, p0.z}};
	v1 = {spec + XTEX, {p1.x, p1.y + 3.0f * one16th, p1.z}};
	v2 = {spec + YTEX, {p2.x, p2.y + 3.0f * one16th, p2.z}};
	v3 = {spec + XTEX + YTEX, {p3.x, p3.y + 3.0f * one16th, p3.z}};
	face_vertices(vertices, v0, v1, v2, v3); // -y
	v0 = {spec + XTEX, {p1.x, p1.y + 13.0f * one16th, p1.z}};
	v1 = {spec, {p0.x, p0.y + 13.0f * one16th, p0.z}};
	v2 = {spec + XTEX + YTEX, {p3.x, p3.y + 13.0f * one16th, p3.z}};
	v3 = {spec + YTEX, {p2.x, p2.y + 13.0f * one16th, p2.z}};
	face_vertices(vertices, v0, v1, v2, v3); // +y
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
	t_shaderInput v0, v1, v2, v3;
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
		v0 = {spec + x_texMX + (x_texMY << 8), p4};
		v1 = {spec - x_texPX + XTEX + (x_texMY << 8), p0};
		v2 = {spec + x_texMX - (x_texPY << 8) + YTEX, p6};
		v3 = {spec - x_texPX + XTEX - (x_texPY << 8) + YTEX, p2};
		face_vertices(vertices, v0, v1, v2, v3); // -x
	}
	if (placement != placement::wall || orientation != face_dir::minus_x) {
		v0 = {spec + x_texMX + (x_texMY << 8), p1};
		v1 = {spec - x_texPX + XTEX + (x_texMY << 8), p5};
		v2 = {spec + x_texMX - (x_texPY << 8) + YTEX, p3};
		v3 = {spec - x_texPX + XTEX - (x_texPY << 8) + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3); // +x
	}
	if (placement != placement::wall || orientation != face_dir::plus_y) {
		v0 = {spec + y_texMX + (y_texMY << 8), p0};
		v1 = {spec - y_texPX + XTEX + (y_texMY << 8), p1};
		v2 = {spec + y_texMX - (y_texPY << 8) + YTEX, p2};
		v3 = {spec - y_texPX + XTEX - (y_texPY << 8) + YTEX, p3};
		face_vertices(vertices, v0, v1, v2, v3); // -y
	}
	if (placement != placement::wall || orientation != face_dir::minus_y) {
		v0 = {spec + y_texMX + (y_texMY << 8), p5};
		v1 = {spec - y_texPX + XTEX + (y_texMY << 8), p4};
		v2 = {spec + y_texMX - (y_texPY << 8) + YTEX, p7};
		v3 = {spec - y_texPX + XTEX - (y_texPY << 8) + YTEX, p6};
		face_vertices(vertices, v0, v1, v2, v3); // +y
	}
	if (placement != placement::ceiling) {
		v0 = {spec + z_texMX + (z_texMY << 8), p4};
		v1 = {spec - z_texPX + (z_texMY << 8) + XTEX, p5};
		v2 = {spec + z_texMX - (z_texPY << 8) + YTEX, p0};
		v3 = {spec - z_texPX - (z_texPY << 8) + YTEX + XTEX, p1};
		face_vertices(vertices, v0, v1, v2, v3); // +z
	}
	if (placement != placement::floor) {
		v0 = {spec + z_texMX + (z_texMY << 8), p2};
		v1 = {spec - z_texPX + (z_texMY << 8) + XTEX, p3};
		v2 = {spec + z_texMX - (z_texPY << 8) + YTEX, p6};
		v3 = {spec - z_texPX - (z_texPY << 8) + YTEX + XTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3); // -z
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
	v0 = {spec + 7 + (6 << 8), p4};
	v1 = {spec - 7 + XTEX + (6 << 8), p0};
	v2 = {spec + 7 + YTEX, p6};
	v3 = {spec - 7 + XTEX + YTEX, p2};
	face_vertices(vertices, v0, v1, v2, v3); // -x
	v0 = {spec + 7 + (6 << 8), p1};
	v1 = {spec - 7 + XTEX + (6 << 8), p5};
	v2 = {spec + 7 + YTEX, p3};
	v3 = {spec - 7 + XTEX + YTEX, p7};
	face_vertices(vertices, v0, v1, v2, v3); // +x
	v0 = {spec + 7 + (6 << 8), p0};
	v1 = {spec - 7 + XTEX + (6 << 8), p1};
	v2 = {spec + 7 + YTEX, p2};
	v3 = {spec - 7 + XTEX + YTEX, p3};
	face_vertices(vertices, v0, v1, v2, v3); // -y
	v0 = {spec + 7 + (6 << 8), p5};
	v1 = {spec - 7 + XTEX + (6 << 8), p4};
	v2 = {spec + 7 + YTEX, p7};
	v3 = {spec - 7 + XTEX + YTEX, p6};
	face_vertices(vertices, v0, v1, v2, v3); // +y
	v0 = {spec + 7 + (6 << 8), p4};
	v1 = {spec - 7 + (6 << 8) + XTEX, p5};
	v2 = {spec + 7 + (8 << 8) + (1 << 18), p0};
	v3 = {spec - 7 + (8 << 8) + (1 << 18) + XTEX, p1};
	face_vertices(vertices, v0, v1, v2, v3); // +z
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
	t_shaderInput v0, v1, v2, v3;
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
	// drawing lever's base
	spec = (textureX << 4) + (textureY << 12) + (0 << 19);
	int faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	spec += (faceLight << 24);
	if (placement != placement::wall || orientation != face_dir::plus_x) {
		v0 = {spec + x_texMX + (x_texMY << 8), p4};
		v1 = {spec - x_texPX + XTEX + (x_texMY << 8), p0};
		v2 = {spec + x_texMX - (x_texPY << 8) + YTEX, p6};
		v3 = {spec - x_texPX + XTEX - (x_texPY << 8) + YTEX, p2};
		face_vertices(vertices, v0, v1, v2, v3); // -x
	}
	if (placement != placement::wall || orientation != face_dir::minus_x) {
		v0 = {spec + x_texMX + (x_texMY << 8), p1};
		v1 = {spec - x_texPX + XTEX + (x_texMY << 8), p5};
		v2 = {spec + x_texMX - (x_texPY << 8) + YTEX, p3};
		v3 = {spec - x_texPX + XTEX - (x_texPY << 8) + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3); // +x
	}
	if (placement != placement::wall || orientation != face_dir::plus_y) {
		v0 = {spec + y_texMX + (y_texMY << 8), p0};
		v1 = {spec - y_texPX + XTEX + (y_texMY << 8), p1};
		v2 = {spec + y_texMX - (y_texPY << 8) + YTEX, p2};
		v3 = {spec - y_texPX + XTEX - (y_texPY << 8) + YTEX, p3};
		face_vertices(vertices, v0, v1, v2, v3); // -y
	}
	if (placement != placement::wall || orientation != face_dir::minus_y) {
		v0 = {spec + y_texMX + (y_texMY << 8), p5};
		v1 = {spec - y_texPX + XTEX + (y_texMY << 8), p4};
		v2 = {spec + y_texMX - (y_texPY << 8) + YTEX, p7};
		v3 = {spec - y_texPX + XTEX - (y_texPY << 8) + YTEX, p6};
		face_vertices(vertices, v0, v1, v2, v3); // +y
	}
	if (placement != placement::ceiling) {
		v0 = {spec + z_texMX + (z_texMY << 8), p4};
		v1 = {spec - z_texPX + (z_texMY << 8) + XTEX, p5};
		v2 = {spec + z_texMX - (z_texPY << 8) + YTEX, p0};
		v3 = {spec - z_texPX - (z_texPY << 8) + YTEX + XTEX, p1};
		face_vertices(vertices, v0, v1, v2, v3); // +z
	}
	if (placement != placement::floor) {
		v0 = {spec + z_texMX + (z_texMY << 8), p2};
		v1 = {spec - z_texPX + (z_texMY << 8) + XTEX, p3};
		v2 = {spec + z_texMX - (z_texPY << 8) + YTEX, p6};
		v3 = {spec - z_texPX - (z_texPY << 8) + YTEX + XTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3); // -z
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
	t_shaderInput v0, v1, v2, v3;
	// (void)value; int color = pos.x;
	int color = (value >> offset::redstone::strength) & 0xF;
	spec += (color << 24);


	int mx = (value >> offset::redstone::dust::mx) & mask::redstone::dust::connect;
	int px = (value >> offset::redstone::dust::px) & mask::redstone::dust::connect;
	int my = (value >> offset::redstone::dust::my) & mask::redstone::dust::connect;
	int py = (value >> offset::redstone::dust::py) & mask::redstone::dust::connect;

	if ((mx && (my || py)) || (px && (my || py)) // display central dot if at least 2 perpendicular dir are on
		|| !(mx | py | my | py)) { // or if no dir is on
		v0 = {spec, p4 + glm::vec3(0, 0, -0.0005f)};
		v1 = {spec + XTEX, p5 + glm::vec3(0, 0, -0.0005f)};
		v2 = {spec + YTEX, p0 + glm::vec3(0, 0, -0.0005f)};
		v3 = {spec + XTEX + YTEX, p1 + glm::vec3(0, 0, -0.0005f)};
		face_vertices(vertices, v0, v1, v2, v3); // +z
	}

	spec -= (1 << 4); // dust line texture
	if (mx) {
		v0 = {spec + XTEX, p4};
		v1 = {spec + XTEX + YTEX - (8 << 8), p5 + glm::vec3(-0.5f, 0, 0)};
		v2 = {spec, p0};
		v3 = {spec + YTEX - (8 << 8), p1 + glm::vec3(-0.5f, 0, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
		if (mx & redstone::dust::up) {
			v0 = {spec, p0 + glm::vec3(0.002f, 0, 1)};
			v1 = {spec + XTEX, p4 + glm::vec3(0.002f, 0, 1)};
			v2 = {spec + YTEX, p0 + glm::vec3(0.002f, 0, 0)};
			v3 = {spec + XTEX + YTEX, p4 + glm::vec3(0.002f, 0, 0)};
			face_vertices(vertices, v0, v1, v2, v3);
		}
	}
	if (px) {
		v0 = {spec + (8 << 8) + XTEX, p4 + glm::vec3(0.5f, 0, 0)};
		v1 = {spec + XTEX + YTEX, p5};
		v2 = {spec + (8 << 8), p0 + glm::vec3(0.5f, 0, 0)};
		v3 = {spec + YTEX, p1};
		face_vertices(vertices, v0, v1, v2, v3);
		if (px & redstone::dust::up) {
			v0 = {spec, p5 + glm::vec3(-0.002f, 0, 1)};
			v1 = {spec + XTEX, p1 + glm::vec3(-0.002f, 0, 1)};
			v2 = {spec + YTEX, p5 + glm::vec3(-0.002f, 0, 0)};
			v3 = {spec + XTEX + YTEX, p1 + glm::vec3(-0.002f, 0, 0)};
			face_vertices(vertices, v0, v1, v2, v3);
		}
	}
	if (my) {
		v0 = {spec + XTEX + YTEX - (8 << 8), p4 + glm::vec3(0, -0.5f, 0)};
		v1 = {spec + YTEX - (8 << 8), p5 + glm::vec3(0, -0.5f, 0)};
		v2 = {spec + XTEX, p0};
		v3 = {spec, p1};
		face_vertices(vertices, v0, v1, v2, v3);
		if (my & redstone::dust::up) {
			v0 = {spec, p1 + glm::vec3(0, 0.002f, 1)};
			v1 = {spec + XTEX, p0 + glm::vec3(0, 0.002f, 1)};
			v2 = {spec + YTEX, p1 + glm::vec3(0, 0.002f, 0)};
			v3 = {spec + XTEX + YTEX, p0 + glm::vec3(0, 0.002f, 0)};
			face_vertices(vertices, v0, v1, v2, v3);
		}
	}
	if (py) {
		v0 = {spec + XTEX + YTEX, p4};
		v1 = {spec + YTEX, p5};
		v2 = {spec + XTEX + (8 << 8), p0 + glm::vec3(0, 0.5f, 0)};
		v3 = {spec + (8 << 8), p1 + glm::vec3(0, 0.5f, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
		if (py & redstone::dust::up) {
			v0 = {spec, p4 + glm::vec3(0, -0.002f, 1)};
			v1 = {spec + XTEX, p5 + glm::vec3(0, -0.002f, 1)};
			v2 = {spec + YTEX, p4 + glm::vec3(0, -0.002f, 0)};
			v3 = {spec + XTEX + YTEX, p5 + glm::vec3(0, -0.002f, 0)};
			face_vertices(vertices, v0, v1, v2, v3);
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

	t_shaderInput v0, v1, v2, v3;
	int baseSpec = (s_blocks[blocks::smooth_stone]->texX() << 4) + (s_blocks[blocks::smooth_stone]->texY() << 12);
	int spec, faceLight;
	// draw base
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = baseSpec + (3 << 19) + (chunk->computeLight(pos.x - 1, pos.y, pos.z) << 24);
		v0 = {spec + (14 << 8), p4 + glm::vec3(0, 0, -14 * one16th)};
		v1 = {spec + XTEX + (14 << 8), p0 + glm::vec3(0, 0, -14 * one16th)};
		v2 = {spec + YTEX, p6};
		v3 = {spec + XTEX + YTEX, p2};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = baseSpec + (4 << 19) + (chunk->computeLight(pos.x + 1, pos.y, pos.z) << 24);
		v0 = {spec + (14 << 8), p1 + glm::vec3(0, 0, -14 * one16th)};
		v1 = {spec + XTEX + (14 << 8), p5 + glm::vec3(0, 0, -14 * one16th)};
		v2 = {spec + YTEX, p3};
		v3 = {spec + XTEX + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = baseSpec + (1 << 19) + (chunk->computeLight(pos.x, pos.y - 1, pos.z) << 24);
		v0 = {spec + (14 << 8), p0 + glm::vec3(0, 0, -14 * one16th)};
		v1 = {spec + XTEX + (14 << 8), p1 + glm::vec3(0, 0, -14 * one16th)};
		v2 = {spec + YTEX, p2};
		v3 = {spec + XTEX + YTEX, p3};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = baseSpec + (2 << 19) + (chunk->computeLight(pos.x, pos.y + 1, pos.z) << 24);
		v0 = {spec + (14 << 8), p5 + glm::vec3(0, 0, -14 * one16th)};
		v1 = {spec + XTEX + (14 << 8), p4 + glm::vec3(0, 0, -14 * one16th)};
		v2 = {spec + YTEX, p7};
		v3 = {spec + XTEX + YTEX, p6};
		face_vertices(vertices, v0, v1, v2, v3);
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
	v0 = {spec, topLeft};
	v1 = {spec + XTEX, topLeft + right};
	v2 = {spec + YTEX, topLeft - front};
	v3 = {spec + XTEX + YTEX, topLeft + right - front};
	face_vertices(vertices, v0, v1, v2, v3);
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
	v0 = {spec + 6 + (5 << 8), p1 + right * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p0 - right * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p3 + right * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p2 - right * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 6 + (5 << 8), p4 - right * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p5 + right * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p6 - right * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p7 + right * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 6 + (5 << 8), p0 + front * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p4 - front * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p2 + front * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p6 - front * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 6 + (5 << 8), p5 - front * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p1 + front * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p7 - front * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p3 + front * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 7 + (6 << 8), p0 + glm::vec3(0, 0, -one16th)};
	v1 = {spec + XTEX - 7 + (6 << 8), p1 + glm::vec3(0, 0, -one16th)};
	v2 = {spec + 7 + YTEX - (8 << 8), p4 + glm::vec3(0, 0, -one16th)};
	v3 = {spec + XTEX - 7 + YTEX - (8 << 8), p5 + glm::vec3(0, 0, -one16th)};
	face_vertices(vertices, v0, v1, v2, v3);
	// draw back torch or lock bar
	float delta = 4.0f + 2.0f * ((value >> offset::redstone::repeater::ticks) & 0x3);
	p0 -= front * delta * one16th; p1 -= front * delta * one16th; p2 -= front * delta * one16th; p3 -= front * delta * one16th;
	p4 -= front * delta * one16th; p5 -= front * delta * one16th; p6 -= front * delta * one16th; p7 -= front * delta * one16th;
	if (!(value & mask::redstone::repeater::lock)) { // back torch
		v0 = {spec + 6 + (5 << 8), p1 + right * one16th};
		v1 = {spec + XTEX - 6 + (5 << 8), p0 - right * one16th};
		v2 = {spec + 6 + YTEX - (5 << 8), p3 + right * one16th};
		v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p2 - right * one16th};
		face_vertices(vertices, v0, v1, v2, v3);
		v0 = {spec + 6 + (5 << 8), p4 - right * one16th};
		v1 = {spec + XTEX - 6 + (5 << 8), p5 + right * one16th};
		v2 = {spec + 6 + YTEX - (5 << 8), p6 - right * one16th};
		v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p7 + right * one16th};
		face_vertices(vertices, v0, v1, v2, v3);
		v0 = {spec + 6 + (5 << 8), p0 + front * one16th};
		v1 = {spec + XTEX - 6 + (5 << 8), p4 - front * one16th};
		v2 = {spec + 6 + YTEX - (5 << 8), p2 + front * one16th};
		v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p6 - front * one16th};
		face_vertices(vertices, v0, v1, v2, v3);
		v0 = {spec + 6 + (5 << 8), p5 - front * one16th};
		v1 = {spec + XTEX - 6 + (5 << 8), p1 + front * one16th};
		v2 = {spec + 6 + YTEX - (5 << 8), p7 - front * one16th};
		v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p3 + front * one16th};
		face_vertices(vertices, v0, v1, v2, v3);
		v0 = {spec + 7 + (6 << 8), p0 + glm::vec3(0, 0, -one16th)};
		v1 = {spec + XTEX - 7 + (6 << 8), p1 + glm::vec3(0, 0, -one16th)};
		v2 = {spec + 7 + YTEX - (8 << 8), p4 + glm::vec3(0, 0, -one16th)};
		v3 = {spec + XTEX - 7 + YTEX - (8 << 8), p5 + glm::vec3(0, 0, -one16th)};
		face_vertices(vertices, v0, v1, v2, v3);
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
		v0 = {spec + 2 + (14 << 8), p1};
		v1 = {spec + XTEX - 2 + (14 << 8), p0};
		v2 = {spec + 2 + YTEX - (2 << 8), p3};
		v3 = {spec + XTEX - 2 + YTEX - (2 << 8), p2};
		face_vertices(vertices, v0, v1, v2, v3);
		v0 = {spec + 2 + (14 << 8), p4};
		v1 = {spec + XTEX - 2 + (14 << 8), p5};
		v2 = {spec + 2 + YTEX - (2 << 8), p6};
		v3 = {spec + XTEX - 2 + YTEX - (2 << 8), p7};
		face_vertices(vertices, v0, v1, v2, v3);
		v0 = {spec + 6 + (7 << 8), p0};
		v1 = {spec + XTEX - 8 + (7 << 8), p4};
		v2 = {spec + 6 + YTEX - (7 << 8), p2};
		v3 = {spec + XTEX - 8 + YTEX - (7 << 8), p6};
		face_vertices(vertices, v0, v1, v2, v3);
		v0 = {spec + 8 + (7 << 8), p5};
		v1 = {spec + XTEX - 6 + (7 << 8), p1};
		v2 = {spec + 8 + YTEX - (7 << 8), p7};
		v3 = {spec + XTEX - 6 + YTEX - (7 << 8), p3};
		face_vertices(vertices, v0, v1, v2, v3);
		v0 = {spec + 2 + (7 << 8), p0};
		v1 = {spec + XTEX - 2 + (7 << 8), p1};
		v2 = {spec + 2 + YTEX - (7 << 8), p4};
		v3 = {spec + XTEX - 2 + YTEX - (7 << 8), p5};
		face_vertices(vertices, v0, v1, v2, v3);
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

	t_shaderInput v0, v1, v2, v3;
	int baseSpec = (s_blocks[blocks::smooth_stone]->texX() << 4) + (s_blocks[blocks::smooth_stone]->texY() << 12);
	int spec, faceLight;
	// draw base
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z), face_dir::minus_x)) {
		spec = baseSpec + (3 << 19) + (chunk->computeLight(pos.x - 1, pos.y, pos.z) << 24);
		v0 = {spec + (14 << 8), p4 + glm::vec3(0, 0, -14 * one16th)};
		v1 = {spec + XTEX + (14 << 8), p0 + glm::vec3(0, 0, -14 * one16th)};
		v2 = {spec + YTEX, p6};
		v3 = {spec + XTEX + YTEX, p2};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z), face_dir::plus_x)) {
		spec = baseSpec + (4 << 19) + (chunk->computeLight(pos.x + 1, pos.y, pos.z) << 24);
		v0 = {spec + (14 << 8), p1 + glm::vec3(0, 0, -14 * one16th)};
		v1 = {spec + XTEX + (14 << 8), p5 + glm::vec3(0, 0, -14 * one16th)};
		v2 = {spec + YTEX, p3};
		v3 = {spec + XTEX + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z), face_dir::minus_y)) {
		spec = baseSpec + (1 << 19) + (chunk->computeLight(pos.x, pos.y - 1, pos.z) << 24);
		v0 = {spec + (14 << 8), p0 + glm::vec3(0, 0, -14 * one16th)};
		v1 = {spec + XTEX + (14 << 8), p1 + glm::vec3(0, 0, -14 * one16th)};
		v2 = {spec + YTEX, p2};
		v3 = {spec + XTEX + YTEX, p3};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z), face_dir::plus_y)) {
		spec = baseSpec + (2 << 19) + (chunk->computeLight(pos.x, pos.y + 1, pos.z) << 24);
		v0 = {spec + (14 << 8), p5 + glm::vec3(0, 0, -14 * one16th)};
		v1 = {spec + XTEX + (14 << 8), p4 + glm::vec3(0, 0, -14 * one16th)};
		v2 = {spec + YTEX, p7};
		v3 = {spec + XTEX + YTEX, p6};
		face_vertices(vertices, v0, v1, v2, v3);
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
	v0 = {spec, topLeft};
	v1 = {spec + XTEX, topLeft + right};
	v2 = {spec + YTEX, topLeft - front};
	v3 = {spec + XTEX + YTEX, topLeft + right - front};
	face_vertices(vertices, v0, v1, v2, v3);
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
	v0 = {spec + 6 + (5 << 8), p1 + right * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p0 - right * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p3 + right * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p2 - right * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 6 + (5 << 8), p4 - right * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p5 + right * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p6 - right * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p7 + right * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 6 + (5 << 8), p0 + front * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p4 - front * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p2 + front * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p6 - front * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 6 + (5 << 8), p5 - front * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p1 + front * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p7 - front * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p3 + front * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 7 + (6 << 8), p0 + glm::vec3(0, 0, -one16th)};
	v1 = {spec + XTEX - 7 + (6 << 8), p1 + glm::vec3(0, 0, -one16th)};
	v2 = {spec + 7 + YTEX - (8 << 8), p4 + glm::vec3(0, 0, -one16th)};
	v3 = {spec + XTEX - 7 + YTEX - (8 << 8), p5 + glm::vec3(0, 0, -one16th)};
	face_vertices(vertices, v0, v1, v2, v3);
	// back right
	p0 += right * 6.0f * one16th; p1 += right * 6.0f * one16th; p2 += right * 6.0f * one16th; p3 += right * 6.0f * one16th;
	p4 += right * 6.0f * one16th; p5 += right * 6.0f * one16th; p6 += right * 6.0f * one16th; p7 += right * 6.0f * one16th;
	v0 = {spec + 6 + (5 << 8), p1 + right * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p0 - right * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p3 + right * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p2 - right * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 6 + (5 << 8), p4 - right * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p5 + right * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p6 - right * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p7 + right * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 6 + (5 << 8), p0 + front * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p4 - front * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p2 + front * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p6 - front * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 6 + (5 << 8), p5 - front * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p1 + front * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p7 - front * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p3 + front * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 7 + (6 << 8), p0 + glm::vec3(0, 0, -one16th)};
	v1 = {spec + XTEX - 7 + (6 << 8), p1 + glm::vec3(0, 0, -one16th)};
	v2 = {spec + 7 + YTEX - (8 << 8), p4 + glm::vec3(0, 0, -one16th)};
	v3 = {spec + XTEX - 7 + YTEX - (8 << 8), p5 + glm::vec3(0, 0, -one16th)};
	face_vertices(vertices, v0, v1, v2, v3);
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
	v0 = {spec + 6 + (5 << 8), p1 + right * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p0 - right * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p3 + right * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p2 - right * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 6 + (5 << 8), p4 - right * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p5 + right * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p6 - right * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p7 + right * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 6 + (5 << 8), p0 + front * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p4 - front * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p2 + front * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p6 - front * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 6 + (5 << 8), p5 - front * one16th};
	v1 = {spec + XTEX - 6 + (5 << 8), p1 + front * one16th};
	v2 = {spec + 6 + YTEX - (5 << 8), p7 - front * one16th};
	v3 = {spec + XTEX - 6 + YTEX - (5 << 8), p3 + front * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 7 + (6 << 8), p0 + glm::vec3(0, 0, -one16th)};
	v1 = {spec + XTEX - 7 + (6 << 8), p1 + glm::vec3(0, 0, -one16th)};
	v2 = {spec + 7 + YTEX - (8 << 8), p4 + glm::vec3(0, 0, -one16th)};
	v3 = {spec + XTEX - 7 + YTEX - (8 << 8), p5 + glm::vec3(0, 0, -one16th)};
	face_vertices(vertices, v0, v1, v2, v3);
}

void Piston::addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const
{
	float dxm = 0, dxp = 1, dym = 0, dyp = 1, dzm = 0, dzp = 1;
	int orientation = ((value >> offset::blocks::orientation) & 0x7), tyo = 0, ttyo;
	if (value & mask::redstone::piston::moving) {
		tyo = (4 << 8);
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

	t_shaderInput v0, v1, v2, v3;
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
		v0 = {spec + ttyo, pp0};
		v1 = {spec + XTEX + ttyo, pp1};
		v2 = {spec + YTEX, pp2};
		v3 = {spec + XTEX + YTEX, pp3};
		face_vertices(vertices, v0, v1, v2, v3);
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
		v0 = {spec + ttyo, pp0};
		v1 = {spec + XTEX + ttyo, pp1};
		v2 = {spec + YTEX, pp2};
		v3 = {spec + XTEX + YTEX, pp3};
		face_vertices(vertices, v0, v1, v2, v3);
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
		v0 = {spec + ttyo, pp0};
		v1 = {spec + XTEX + ttyo, pp1};
		v2 = {spec + YTEX, pp2};
		v3 = {spec + XTEX + YTEX, pp3};
		face_vertices(vertices, v0, v1, v2, v3);
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
		v0 = {spec + ttyo, pp0};
		v1 = {spec + XTEX + ttyo, pp1};
		v2 = {spec + YTEX, pp2};
		v3 = {spec + XTEX + YTEX, pp3};
		face_vertices(vertices, v0, v1, v2, v3);
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
		v0 = {spec + ttyo, pp0};
		v1 = {spec + XTEX + ttyo, pp1};
		v2 = {spec + YTEX, pp2};
		v3 = {spec + XTEX + YTEX, pp3};
		face_vertices(vertices, v0, v1, v2, v3);
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
		v0 = {spec + ttyo, pp0};
		v1 = {spec + XTEX + ttyo, pp1};
		v2 = {spec + YTEX, pp2};
		v3 = {spec + XTEX + YTEX, pp3};
		face_vertices(vertices, v0, v1, v2, v3);
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
	v0 = {spec, topLeft + right * 6.0f * one16th - front * 0.25f - up * 6.0f * one16th};
	v1 = {spec - 12 + XTEX, topLeft + right * 6.0f * one16th - up * 6.0f * one16th};
	v2 = {spec + YTEX - (12 << 8), topLeft + right * 10.0f * one16th - front * 0.25f - up * 6.0f * one16th};
	v3 = {spec - 12 + XTEX + YTEX - (12 << 8), topLeft + right * 10.0f * one16th - up * 6.0f * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec, topLeft + right * 10.0f * one16th - front * 0.25f - up * 6.0f * one16th};
	v1 = {spec - 12 + XTEX, topLeft + right * 10.0f * one16th - up * 6.0f * one16th};
	v2 = {spec + YTEX - (12 << 8), topLeft + right * 10.0f * one16th - front * 0.25f - up * 10.0f * one16th};
	v3 = {spec - 12 + XTEX + YTEX - (12 << 8), topLeft + right * 10.0f * one16th - up * 10.0f * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec, topLeft + right * 10.0f * one16th - front * 0.25f - up * 10.0f * one16th};
	v1 = {spec - 12 + XTEX, topLeft + right * 10.0f * one16th - up * 10.0f * one16th};
	v2 = {spec + YTEX - (12 << 8), topLeft + right * 6.0f * one16th - front * 0.25f - up * 10.0f * one16th};
	v3 = {spec - 12 + XTEX + YTEX - (12 << 8), topLeft + right * 6.0f * one16th - up * 10.0f * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec, topLeft + right * 6.0f * one16th - front * 0.25f - up * 10.0f * one16th};
	v1 = {spec - 12 + XTEX, topLeft + right * 6.0f * one16th - up * 10.0f * one16th};
	v2 = {spec + YTEX - (12 << 8), topLeft + right * 6.0f * one16th - front * 0.25f - up * 6.0f * one16th};
	v3 = {spec - 12 + XTEX + YTEX - (12 << 8), topLeft + right * 6.0f * one16th - up * 6.0f * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
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
	t_shaderInput v0 = {spec, topLeft + right};
	t_shaderInput v1 = {spec + XTEX, topLeft};
	t_shaderInput v2 = {spec + YTEX, topLeft + right - up};
	t_shaderInput v3 = {spec + XTEX + YTEX, topLeft - up};
	face_vertices(vertices, v0, v1, v2, v3);
	// back of front volume
	if (piston == blocks::sticky_piston) {
		spec = (s_blocks[blocks::piston]->texX(face_dir::minus_x, face_dir::minus_x << offset::blocks::orientation) << 4) + (s_blocks[blocks::piston]->texY(face_dir::minus_x, face_dir::minus_x << offset::blocks::orientation) << 12) + (0xF << 24);
	}
	v0 = {spec, topLeft - front * 0.25f};
	v1 = {spec + XTEX, topLeft + right - front * 0.25f};
	v2 = {spec + YTEX, topLeft - up - front * 0.25f};
	v3 = {spec + XTEX + YTEX, topLeft + right - up - front * 0.25f};
	face_vertices(vertices, v0, v1, v2, v3);
	// side faces
	spec = (s_blocks[piston]->texX(face_dir::minus_y, face_dir::minus_x << offset::blocks::orientation) << 4) + (s_blocks[piston]->texY(face_dir::minus_y, face_dir::minus_x << offset::blocks::orientation) << 12) + (0xF << 24);
	v0 = {spec, topLeft};
	v1 = {spec + XTEX, topLeft + right};
	v2 = {spec + YTEX - (12 << 8), topLeft - front * 0.25f};
	v3 = {spec + XTEX + YTEX - (12 << 8), topLeft + right - front * 0.25f};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec, topLeft - up};
	v1 = {spec + XTEX, topLeft};
	v2 = {spec + YTEX - (12 << 8), topLeft - up - front * 0.25f};
	v3 = {spec + XTEX + YTEX - (12 << 8), topLeft - front * 0.25f};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec, topLeft + right};
	v1 = {spec + XTEX, topLeft + right - up};
	v2 = {spec + YTEX - (12 << 8), topLeft + right - front * 0.25f};
	v3 = {spec + XTEX + YTEX - (12 << 8), topLeft + right - up - front * 0.25f};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec, topLeft + right - up};
	v1 = {spec + XTEX, topLeft - up};
	v2 = {spec + YTEX - (12 << 8), topLeft + right - up - front * 0.25f};
	v3 = {spec + XTEX + YTEX - (12 << 8), topLeft - up - front * 0.25f};
	face_vertices(vertices, v0, v1, v2, v3);
	// piston bar
	v0 = {spec + 4, topLeft + right * 6.0f * one16th - front * 1.0f - up * 6.0f * one16th};
	v1 = {spec + XTEX, topLeft + right * 6.0f * one16th - front * 0.25f - up * 6.0f * one16th};
	v2 = {spec + 4 + YTEX - (12 << 8), topLeft + right * 10.0f * one16th - front * 1.0f - up * 6.0f * one16th};
	v3 = {spec + XTEX + YTEX - (12 << 8), topLeft + right * 10.0f * one16th - front * 0.25f - up * 6.0f * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 4, topLeft + right * 10.0f * one16th - front * 1.0f - up * 6.0f * one16th};
	v1 = {spec + XTEX, topLeft + right * 10.0f * one16th - front * 0.25f - up * 6.0f * one16th};
	v2 = {spec + 4 + YTEX - (12 << 8), topLeft + right * 10.0f * one16th - front * 1.0f - up * 10.0f * one16th};
	v3 = {spec + XTEX + YTEX - (12 << 8), topLeft + right * 10.0f * one16th - front * 0.25f - up * 10.0f * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 4, topLeft + right * 10.0f * one16th - front * 1.0f - up * 10.0f * one16th};
	v1 = {spec + XTEX, topLeft + right * 10.0f * one16th - front * 0.25f - up * 10.0f * one16th};
	v2 = {spec + 4 + YTEX - (12 << 8), topLeft + right * 6.0f * one16th - front * 1.0f - up * 10.0f * one16th};
	v3 = {spec + XTEX + YTEX - (12 << 8), topLeft + right * 6.0f * one16th - front * 0.25f - up * 10.0f * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	v0 = {spec + 4, topLeft + right * 6.0f * one16th - front * 1.0f - up * 10.0f * one16th};
	v1 = {spec + XTEX, topLeft + right * 6.0f * one16th - front * 0.25f - up * 10.0f * one16th};
	v2 = {spec + 4 + YTEX - (12 << 8), topLeft + right * 6.0f * one16th - front * 1.0f - up * 6.0f * one16th};
	v3 = {spec + XTEX + YTEX - (12 << 8), topLeft + right * 6.0f * one16th - front * 0.25f - up * 6.0f * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
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
	// t_shaderInput v0 = {spec, topLeft + right};
	// t_shaderInput v1 = {spec + XTEX, topLeft};
	// t_shaderInput v2 = {spec + YTEX, topLeft + right - up};
	// t_shaderInput v3 = {spec + XTEX + YTEX, topLeft - up};
	// face_vertices(vertices, v0, v1, v2, v3);
	// player side
	t_shaderInput v0 = {spec,                              topLeft - front * 1.75f * one16th};
	t_shaderInput v1 = {spec + 8 + XTEX,                   topLeft + right - front * 1.75f * one16th};
	t_shaderInput v2 = {spec - (4 << 8) + YTEX,            topLeft - up * 0.5f - front * 1.75f * one16th};
	t_shaderInput v3 = {spec + 8 + XTEX - (4 << 8) + YTEX, topLeft + right - up * 0.5f - front * 1.75f * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	// side faces
	spec = 2 + (14 << 12) + (faceLight << 24);
	// top
	v0 = {spec,                               topLeft};
	v1 = {spec + 8 + XTEX,                    topLeft + right};
	v2 = {spec + YTEX - (14 << 8),            topLeft - front * 1.75f * one16th};
	v3 = {spec + 8 + XTEX + YTEX - (14 << 8), topLeft + right - front * 1.75f * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	// left
	spec = (14 << 12) + (2 << 8) + (faceLight << 24);
	v0 = {spec,                               topLeft};
	v1 = {spec - 14 + XTEX,                   topLeft - front * 1.75f * one16th};
	v2 = {spec + YTEX - (4 << 8),             topLeft - up * 0.5f};
	v3 = {spec - 14 + XTEX + YTEX - (4 << 8), topLeft - up * 0.5f - front * 1.75f * one16th};
	face_vertices(vertices, v0, v1, v2, v3);
	// right
	spec = 2 + (3 << 4) + (14 << 12) + (2 << 8) + (faceLight << 24);
	v0 = {spec,                               topLeft + right - front * 1.75f * one16th};
	v1 = {spec - 14 + XTEX,                   topLeft + right};
	v2 = {spec + YTEX - (4 << 8),             topLeft + right - front * 1.75f * one16th - up * 0.5f};
	v3 = {spec - 14 + XTEX + YTEX - (4 << 8), topLeft + right - up * 0.5f};
	face_vertices(vertices, v0, v1, v2, v3);
	// bottom
	spec = 10 + (1 << 4) + (14 << 12) + (faceLight << 24);
	v0 = {spec,                               topLeft - up * 0.5f - front * 1.75f * one16th};
	v1 = {spec + 8 + XTEX,                    topLeft + right - up * 0.5f - front * 1.75f * one16th};
	v2 = {spec + YTEX - (14 << 8),            topLeft - up * 0.5f};
	v3 = {spec + 8 + XTEX + YTEX - (14 << 8), topLeft + right - up * 0.5f};
	face_vertices(vertices, v0, v1, v2, v3);
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

	t_shaderInput v0, v1, v2, v3;
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
		v0 = {spec, pp0};
		v1 = {spec + XTEX, pp1};
		v2 = {spec + YTEX, pp2};
		v3 = {spec + XTEX + YTEX, pp3};
		face_vertices(vertices, v0, v1, v2, v3);
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
		v0 = {spec, pp0};
		v1 = {spec + XTEX, pp1};
		v2 = {spec + YTEX, pp2};
		v3 = {spec + XTEX + YTEX, pp3};
		face_vertices(vertices, v0, v1, v2, v3);
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
		v0 = {spec, pp0};
		v1 = {spec + XTEX, pp1};
		v2 = {spec + YTEX, pp2};
		v3 = {spec + XTEX + YTEX, pp3};
		face_vertices(vertices, v0, v1, v2, v3);
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
		v0 = {spec, pp0};
		v1 = {spec + XTEX, pp1};
		v2 = {spec + YTEX, pp2};
		v3 = {spec + XTEX + YTEX, pp3};
		face_vertices(vertices, v0, v1, v2, v3);
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
		v0 = {spec, pp0};
		v1 = {spec + XTEX, pp1};
		v2 = {spec + YTEX, pp2};
		v3 = {spec + XTEX + YTEX, pp3};
		face_vertices(vertices, v0, v1, v2, v3);
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
		v0 = {spec, pp0};
		v1 = {spec + XTEX, pp1};
		v2 = {spec + YTEX, pp2};
		v3 = {spec + XTEX + YTEX, pp3};
		face_vertices(vertices, v0, v1, v2, v3);
	}
}
