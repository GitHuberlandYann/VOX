#include "Chunk.hpp"

const Block *s_blocks[S_BLOCKS_SIZE] = {
	new Air(), new GrassBlock(), new OakLog(), new Cactus(), new Farmland(), new DirtPath(), new TNT(), new TBD(),
	new CraftingTable(), new Furnace(), new OakStairsBottom(), new OakStairsTop(), new OakDoor(), new OakTrapdoor(), new StoneStairsBottom(), new StoneStairsTop(),
	new SmoothStoneStairsBottom(), new SmoothStoneStairsTop(), new CobbleStoneStairsBottom(), new CobbleStoneStairsTop(), new StoneBricksStairsBottom(), new StoneBricksStairsTop(), new Lever(), new TBD(),
	new Bedrock(), new Dirt(), new SmoothStone(), new Stone(), new Cobblestone(), new StoneBrick(), new CrackedStoneBrick(), new Sand(),
	new Gravel(), new OakLeaves(), new OakPlanks(), new Glass(), new GlassPane(), new RedstoneLamp(), new TBD(), new TBD(),
	new CoalOre(), new IronOre(), new DiamondOre(), new CoalBlock(), new IronBlock(), new DiamondBlock(), new RedstoneOre(), new RedstoneBlock(),
	new OakSlabBottom(), new OakSlabTop(), new OakFence(), new StoneSlabBottom(), new StoneSlabTop(), new SmoothStoneSlabBottom(), new SmoothStoneSlabTop(), new CobbleStoneSlabBottom(),
	new CobbleStoneSlabTop(), new StoneBricksSlabBottom(), new StoneBricksSlabTop(), new TBD(), new TBD(), new TBD(), new TBD(), new TBD(),
	new Poppy(), new Dandelion(), new BlueOrchid(), new Allium(), new CornFlower(), new PinkTulip(), new Grass(), new SugarCane(),
	new DeadBush(), new OakSapling(), new Torch(), new TBD(), new TBD(), new TBD(), new TBD(), new Chest(),
	new WheatCrop(), new WheatCrop1(), new WheatCrop2(), new WheatCrop3(), new WheatCrop4(), new WheatCrop5(), new WheatCrop6(), new WheatCrop7(),
	new Water(), new Water1(), new Water2(), new Water3(), new Water4(), new Water5(), new Water6(), new Water7(),
	new Stick(), new WoodenShovel(), new StoneShovel(), new IronShovel(), new DiamondShovel(), new WoodenAxe(), new StoneAxe(), new IronAxe(),
	new DiamondAxe(), new WoodenPickaxe(), new StonePickaxe(), new IronPickaxe(), new DiamondPickaxe(), new Bow(), new Arrow(), new TBD(),
	new Coal(), new Charcoal(), new IronIngot(), new Diamond(), new Bucket(), new WaterBucket(), new WoodenHoe(), new StoneHoe(),
	new IronHoe(), new DiamondHoe(), new WheatSeeds(), new Wheat(), new Bread(), new Apple(), new Flint(), new FlintAndSteel()
};

void Cube::addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::ivec3 pos, int value ) const
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
	int offset = 0, orientation = -1, litFurnace = 0;
	if (oriented) {
		orientation = (value >> 9) & 0x7;
		litFurnace = (value >> 12) & 0x1;
		offset = orientation + (litFurnace << 4);
	} else if ((value & 0xFF) == blocks::OAK_LOG) {
		offset = (value >> 9) & 0x3;
	} else if (mined == blocks::REDSTONE_LAMP) { // TODO change this so offset given to texX and texY is just 'value'
		offset = (value >> REDSTONE::ACTIVATED_OFFSET) & 0x1;
	}
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z, true), face_dir::MINUSX)) {
		spec = (this->texX(face_dir::MINUSX, offset) << 4) + (this->texY(face_dir::MINUSX, offset) << 12) + (3 << 19);
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
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z, true), face_dir::PLUSX)) {
		spec = (this->texX(face_dir::PLUSX, offset) << 4) + (this->texY(face_dir::PLUSX, offset) << 12) + (4 << 19);
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z, true), face_dir::MINUSY)) {
		spec = (this->texX(face_dir::MINUSY, offset) << 4) + (this->texY(face_dir::MINUSY, offset) << 12) + (1 << 19);
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z, true), face_dir::PLUSY)) {
		spec = (this->texX(face_dir::PLUSY, offset) << 4) + (this->texY(face_dir::PLUSY, offset) << 12) + (2 << 19);
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
		spec = (this->texX(face_dir::PLUSZ, offset) << 4) + (this->texY(face_dir::PLUSZ, offset) << 12);
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
		spec = (this->texX(face_dir::MINUSZ, offset) << 4) + (this->texY(face_dir::MINUSZ, offset) << 12) + (5 << 19);
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

void Cross::addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::ivec3 pos, int value ) const
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

void Farmland::addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::ivec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1 - ONE16TH);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1 - ONE16TH);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1 - ONE16TH);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1 - ONE16TH);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	t_shaderInput v0, v1, v2, v3;
	int spec, faceLight, cornerLight, shade;
	int offset = (value & blocks::WET_FARMLAND);
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z, true), face_dir::MINUSX)) {
		spec = (this->texX(face_dir::MINUSX, offset) << 4) + (this->texY(face_dir::MINUSX, offset) << 12) + (3 << 19);
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
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z, true), face_dir::PLUSX)) {
		spec = (this->texX(face_dir::PLUSX, offset) << 4) + (this->texY(face_dir::PLUSX, offset) << 12) + (4 << 19);
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z, true), face_dir::MINUSY)) {
		spec = (this->texX(face_dir::MINUSY, offset) << 4) + (this->texY(face_dir::MINUSY, offset) << 12) + (1 << 19);
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z, true), face_dir::PLUSY)) {
		spec = (this->texX(face_dir::PLUSY, offset) << 4) + (this->texY(face_dir::PLUSY, offset) << 12) + (2 << 19);
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
		spec = (this->texX(face_dir::PLUSZ, offset) << 4) + (this->texY(face_dir::PLUSZ, offset) << 12);
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
		spec = (this->texX(face_dir::MINUSZ, offset) << 4) + (this->texY(face_dir::MINUSZ, offset) << 12) + (5 << 19);
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

void SlabBottom::addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::ivec3 pos, int value ) const
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
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z, true), face_dir::MINUSX)) {
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
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z, true), face_dir::PLUSX)) {
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z, true), face_dir::MINUSY)) {
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z, true), face_dir::PLUSY)) {
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
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

void SlabTop::addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::ivec3 pos, int value ) const
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
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z, true), face_dir::MINUSX)) {
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
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z, true), face_dir::PLUSX)) {
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z, true), face_dir::MINUSY)) {
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z, true), face_dir::PLUSY)) {
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
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

void Torch::addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::ivec3 pos, int value ) const
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

	int spec = (textureX << 4) + (textureY << 12) + (0 << 19) + (15 << 24);
	switch ((value >> 9) & 0x7) { // orientation
		case face_dir::MINUSZ: // default
			p0 += glm::vec3( 7.0f * ONE16TH,  7.0f * ONE16TH, -6.0f * ONE16TH);
			p1 += glm::vec3(-7.0f * ONE16TH,  7.0f * ONE16TH, -6.0f * ONE16TH);
			p2 += glm::vec3( 7.0f * ONE16TH,  7.0f * ONE16TH, 0);
			p3 += glm::vec3(-7.0f * ONE16TH,  7.0f * ONE16TH, 0);

			p4 += glm::vec3( 7.0f * ONE16TH, -7.0f * ONE16TH, -6.0f * ONE16TH);
			p5 += glm::vec3(-7.0f * ONE16TH, -7.0f * ONE16TH, -6.0f * ONE16TH);
			p6 += glm::vec3( 7.0f * ONE16TH, -7.0f * ONE16TH, 0);
			p7 += glm::vec3(-7.0f * ONE16TH, -7.0f * ONE16TH, 0);
			break ;
		case face_dir::PLUSX:
			p0 += glm::vec3( 11.0f * ONE16TH,  7.0f * ONE16TH, -3.0f * ONE16TH);
			p1 += glm::vec3(-3.0f * ONE16TH,   7.0f * ONE16TH, -2.0f * ONE16TH);
			p2 += glm::vec3( 15.0f * ONE16TH,  7.0f * ONE16TH, 3.0f * ONE16TH);
			p3 += glm::vec3( 1.0f * ONE16TH,   7.0f * ONE16TH, 4.0f * ONE16TH);

			p4 += glm::vec3( 11.0f * ONE16TH, -7.0f * ONE16TH, -3.0f * ONE16TH);
			p5 += glm::vec3(-3.0f * ONE16TH,  -7.0f * ONE16TH, -2.0f * ONE16TH);
			p6 += glm::vec3( 15.0f * ONE16TH, -7.0f * ONE16TH, 3.0f * ONE16TH);
			p7 += glm::vec3( 1.0f * ONE16TH,  -7.0f * ONE16TH, 4.0f * ONE16TH);
			break ;
		case face_dir::MINUSX:
			p0 += glm::vec3( 3.0f * ONE16TH,  7.0f * ONE16TH, -2.0f * ONE16TH);
			p1 += glm::vec3(-11.0f * ONE16TH, 7.0f * ONE16TH, -3.0f * ONE16TH);
			p2 += glm::vec3(-1.0f * ONE16TH,  7.0f * ONE16TH, 4.0f * ONE16TH);
			p3 += glm::vec3(-15.0f * ONE16TH, 7.0f * ONE16TH, 3.0f * ONE16TH);

			p4 += glm::vec3( 3.0f * ONE16TH, -7.0f * ONE16TH, -2.0f * ONE16TH);
			p5 += glm::vec3(-11.0f * ONE16TH,-7.0f * ONE16TH, -3.0f * ONE16TH);
			p6 += glm::vec3(-1.0f * ONE16TH, -7.0f * ONE16TH, 4.0f * ONE16TH);
			p7 += glm::vec3(-15.0f * ONE16TH,-7.0f * ONE16TH, 3.0f * ONE16TH);
			break ;
		case face_dir::PLUSY:
			p0 += glm::vec3( 7.0f * ONE16TH, 11.0f * ONE16TH, -3.0f * ONE16TH);
			p1 += glm::vec3(-7.0f * ONE16TH, 11.0f * ONE16TH, -3.0f * ONE16TH);
			p2 += glm::vec3( 7.0f * ONE16TH, 15.0f * ONE16TH,  3.0f * ONE16TH);
			p3 += glm::vec3(-7.0f * ONE16TH, 15.0f * ONE16TH,  3.0f * ONE16TH);

			p4 += glm::vec3( 7.0f * ONE16TH, -3.0f * ONE16TH, -2.0f * ONE16TH);
			p5 += glm::vec3(-7.0f * ONE16TH, -3.0f * ONE16TH, -2.0f * ONE16TH);
			p6 += glm::vec3( 7.0f * ONE16TH,  1.0f * ONE16TH,  4.0f * ONE16TH);
			p7 += glm::vec3(-7.0f * ONE16TH,  1.0f * ONE16TH,  4.0f * ONE16TH);
			break ;
		case face_dir::MINUSY:
			p0 += glm::vec3( 7.0f * ONE16TH,  3.0f * ONE16TH, -2.0f * ONE16TH);
			p1 += glm::vec3(-7.0f * ONE16TH,  3.0f * ONE16TH, -2.0f * ONE16TH);
			p2 += glm::vec3( 7.0f * ONE16TH, -1.0f * ONE16TH,  4.0f * ONE16TH);
			p3 += glm::vec3(-7.0f * ONE16TH, -1.0f * ONE16TH,  4.0f * ONE16TH);

			p4 += glm::vec3( 7.0f * ONE16TH, -11.0f * ONE16TH, -3.0f * ONE16TH);
			p5 += glm::vec3(-7.0f * ONE16TH, -11.0f * ONE16TH, -3.0f * ONE16TH);
			p6 += glm::vec3( 7.0f * ONE16TH, -15.0f * ONE16TH,  3.0f * ONE16TH);
			p7 += glm::vec3(-7.0f * ONE16TH, -15.0f * ONE16TH,  3.0f * ONE16TH);
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
	if (((value >> 9) & 0x7) != face_dir::MINUSZ) {
		v0 = {spec + 7 + (14 << 8), p2};
		v1 = {spec - 7 + (14 << 8) + XTEX, p3};
		v2 = {spec + 7 + YTEX, p6};
		v3 = {spec - 7 + XTEX + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3); // -z
	}
}

void StairsBottom::addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::ivec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int corners = (value >> 12) & 0xF;
	int baseSpec = (textureX << 4) + (textureY << 12);
	int spec, faceLight, shade;
	t_shaderInput v0, v1, v2, v3;
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z, true), face_dir::MINUSX)) {
		spec = baseSpec + (3 << 19);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (CORNERS::MM | CORNERS::MP)) {
			case CORNERS::MM | CORNERS::MP:
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + XTEX, p0};
				v2 = {spec + (shade << 22) + YTEX, p6};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
				break ;
			case CORNERS::MM:
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
			case CORNERS::MP:
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
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z, true), face_dir::PLUSX)) {
		spec = baseSpec + (4 << 19);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (CORNERS::PM | CORNERS::PP)) {
			case CORNERS::PM | CORNERS::PP:
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + XTEX, p5};
				v2 = {spec + (shade << 22) + YTEX, p3};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
				break ;
			case CORNERS::PM:
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
			case CORNERS::PP:
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z, true), face_dir::MINUSY)) {
		spec = baseSpec + (1 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (CORNERS::MM | CORNERS::PM)) {
			case CORNERS::MM | CORNERS::PM:
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + XTEX, p1};
				v2 = {spec + (shade << 22) + YTEX, p2};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
				break ;
			case CORNERS::MM:
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
			case CORNERS::PM:
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z, true), face_dir::PLUSY)) {
		spec = baseSpec + (2 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (CORNERS::MP | CORNERS::PP)) {
			case CORNERS::MP | CORNERS::PP:
				v0 = {spec + (shade << 22), p5};
				v1 = {spec + (shade << 22) + XTEX, p4};
				v2 = {spec + (shade << 22) + YTEX, p7};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
				break ;
			case CORNERS::MP:
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
			case CORNERS::PP:
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
		spec = baseSpec + (0 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (CORNERS::MP | CORNERS::PP)) {
			case CORNERS::MP:
				v0 = {spec + (shade << 22), p4 + glm::vec3(0, 0, 0)};
				v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, 0, 0)};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, 0)};
				v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(-0.5f, 0.5f, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case CORNERS::PP:
				v0 = {spec + 8 + (shade << 22), p4 + glm::vec3(0.5f, 0, 0)};
				v1 = {spec + (shade << 22) + XTEX, p5};
				v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0.5f, 0.5f, 0)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case CORNERS::MP | CORNERS::PP:
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + XTEX, p5};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, 0)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
		}
		switch (corners & (CORNERS::MM | CORNERS::PM)) {
			case CORNERS::MM:
				v0 = {spec + (8 << 8) + (shade << 22), p4 + glm::vec3(0, -0.5f, 0)};
				v1 = {spec - (1 << 4) + 8 + (8 << 8) + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, -0.5f, 0)};
				v2 = {spec + (shade << 22) + YTEX, p0 + glm::vec3(0, 0, 0)};
				v3 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(-0.5f, 0, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case CORNERS::PM:
				v0 = {spec + 8 + (8 << 8) + (shade << 22), p4 + glm::vec3(0.5f, -0.5f, 0)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 + glm::vec3(0, -0.5f, 0)};
				v2 = {spec + 8 + (shade << 22) + YTEX, p0 + glm::vec3(0.5f, 0, 0)};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case CORNERS::MM | CORNERS::PM:
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
	switch (corners & (CORNERS::MP | CORNERS::PP)) {
		case CORNERS::PP:
			v0 = {spec + (shade << 22), p4 + glm::vec3(0, 0, -0.5f)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, 0, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(-0.5f, 0.5f, -0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case CORNERS::MP:
			v0 = {spec + 8 + (shade << 22), p4 + glm::vec3(0.5f, 0, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p5 + glm::vec3(0, 0, -0.5f)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0.5f, 0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case CORNERS::MP | CORNERS::PP:
			break ;
		default:
			v0 = {spec + (shade << 22), p4 + glm::vec3(0, 0, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p5 + glm::vec3(0, 0, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
	}
	switch (corners & (CORNERS::MM | CORNERS::PM)) {
		case CORNERS::PM:
			v0 = {spec + (8 << 8) + (shade << 22), p4 + glm::vec3(0, -0.5f, -0.5f)};
			v1 = {spec - (1 << 4) + 8 + (8 << 8) + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, -0.5f, -0.5f)};
			v2 = {spec + (shade << 22) + YTEX, p0 + glm::vec3(0, 0, -0.5f)};
			v3 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(-0.5f, 0, -0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case CORNERS::MM:
			v0 = {spec + 8 + (8 << 8) + (shade << 22), p4 + glm::vec3(0.5f, -0.5f, -0.5f)};
			v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 + glm::vec3(0, -0.5f, -0.5f)};
			v2 = {spec + 8 + (shade << 22) + YTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
			v3 = {spec + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0, -0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case CORNERS::MM | CORNERS::PM:
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
	// front MINUSX of second step
	spec += (3 << 19);
	switch (corners) {
		case CORNERS::PM | CORNERS::PP: // full width
			v0 = {spec + (shade << 22), p4 + glm::vec3(0.5f, 0, 0)};
			v1 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, 0, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
			break ;
		case CORNERS::PM: // only 'right'
		case CORNERS::PM | CORNERS::PP | CORNERS::MP:
			v0 = {spec + 8 + (shade << 22), p4 + glm::vec3(0.5f, -0.5f, 0)};
			v1 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0, 0)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, -0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
			break ;
		case CORNERS::PP: // only 'left'
		case CORNERS::PM | CORNERS::PP | CORNERS::MM:
			v0 = {spec + (shade << 22), p4 + glm::vec3(0.5f, 0, 0)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0.5f, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, 0, -0.5f)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0.5f, -0.5f)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	// front PLUSX of second step
	spec += (1 << 19);
	switch (corners) {
		case CORNERS::MM | CORNERS::MP: // full width
			v0 = {spec + (shade << 22), p1 + glm::vec3(-0.5f, 0, 0)};
			v1 = {spec + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, 0, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p1 + glm::vec3(-0.5f, 0, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p5 + glm::vec3(-0.5f, 0, -0.5f)};
			break ;
		case CORNERS::MP: // only 'right'
		case CORNERS::MM | CORNERS::MP | CORNERS::PM:
			v0 = {spec + 8 + (shade << 22), p1 + glm::vec3(-0.5f, 0.5f, 0)};
			v1 = {spec + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, 0, 0)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p1 + glm::vec3(-0.5f, 0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p5 + glm::vec3(-0.5f, 0, -0.5f)};
			break ;
		case CORNERS::MM: // only 'left'
		case CORNERS::MM | CORNERS::MP | CORNERS::PP:
			v0 = {spec + (shade << 22), p1 + glm::vec3(-0.5f, 0, 0)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, -0.5, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p1 + glm::vec3(-0.5f, 0, -0.5f)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p5 + glm::vec3(-0.5f, -0.5f, -0.5f)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	// front MINUSY of second step
	spec -= (3 << 19);
	switch (corners) {
		case CORNERS::MP | CORNERS::PP: // full width
			v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.5f, 0)};
			v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.5f, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
			break ;
		case CORNERS::PP: // only 'right'
		case CORNERS::MP | CORNERS::PP | CORNERS::MM:
			v0 = {spec + 8 + (shade << 22), p0 + glm::vec3(0.5f, 0.5f, 0)};
			v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.5f, 0)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0.5f, 0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
			break ;
		case CORNERS::MP: // only 'left'
		case CORNERS::MP | CORNERS::PP | CORNERS::PM:
			v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.5f, 0)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p1 + glm::vec3(-0.5f, 0.5f, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(-0.5f, 0.5f, -0.5f)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	// front PLUSY of second step
	spec += (1 << 19);
	switch (corners) {
		case CORNERS::MM | CORNERS::PM: // full width
			v0 = {spec + (shade << 22), p5 + glm::vec3(0, -0.5f, 0)};
			v1 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0, -0.5f, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p5 + glm::vec3(0, -0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0, -0.5f, -0.5f)};
			break ;
		case CORNERS::PM: // only 'left'
		case CORNERS::MM | CORNERS::PM | CORNERS::MP:
			v0 = {spec + (shade << 22), p5 + glm::vec3(0, -0.5f, 0)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p4 + glm::vec3(0.5f, -0.5f, 0)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p5 + glm::vec3(0, -0.5f, -0.5f)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0.5f, -0.5f, -0.5f)};
			break ;
		case CORNERS::MM: // only 'right'
		case CORNERS::MM | CORNERS::PM | CORNERS::PP:
			v0 = {spec + 8 + (shade << 22), p5 + glm::vec3(-0.5f, -0.5f, 0)};
			v1 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0, -0.5f, 0)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p5 + glm::vec3(-0.5f, -0.5f, -0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0, -0.5f, -0.5f)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
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

void StairsTop::addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::ivec3 pos, int value ) const
{
	glm::vec3 p0 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p1 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1);
	glm::vec3 p2 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 0);
	glm::vec3 p3 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 0);

	glm::vec3 p4 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p5 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1);
	glm::vec3 p6 = glm::vec3(start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 0);
	glm::vec3 p7 = glm::vec3(start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 0);

	int corners = (value >> 12) & 0xF;
	int baseSpec = (textureX << 4) + (textureY << 12);
	int spec, faceLight, shade;
	t_shaderInput v0, v1, v2, v3;
	if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z, true), face_dir::MINUSX)) {
		spec = baseSpec + (3 << 19);
		faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (CORNERS::MM | CORNERS::MP)) {
			case CORNERS::MM | CORNERS::MP:
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + XTEX, p0};
				v2 = {spec + (shade << 22) + YTEX, p6};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
				break ;
			case CORNERS::MM:
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
			case CORNERS::MP:
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
	if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z, true), face_dir::PLUSX)) {
		spec = baseSpec + (4 << 19);
		faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (CORNERS::PM | CORNERS::PP)) {
			case CORNERS::PM | CORNERS::PP:
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + XTEX, p5};
				v2 = {spec + (shade << 22) + YTEX, p3};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
				break ;
			case CORNERS::PM:
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
			case CORNERS::PP:
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z, true), face_dir::MINUSY)) {
		spec = baseSpec + (1 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (CORNERS::MM | CORNERS::PM)) {
			case CORNERS::MM | CORNERS::PM:
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + XTEX, p1};
				v2 = {spec + (shade << 22) + YTEX, p2};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
				break ;
			case CORNERS::MM:
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
			case CORNERS::PM:
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z, true), face_dir::PLUSY)) {
		spec = baseSpec + (2 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (CORNERS::MP | CORNERS::PP)) {
			case CORNERS::MP | CORNERS::PP:
				v0 = {spec + (shade << 22), p5};
				v1 = {spec + (shade << 22) + XTEX, p4};
				v2 = {spec + (shade << 22) + YTEX, p7};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
				break ;
			case CORNERS::MP:
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
			case CORNERS::PP:
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
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
	switch (corners & (CORNERS::MP | CORNERS::PP)) {
		case CORNERS::PP:
			v0 = {spec + (shade << 22), p2 + glm::vec3(0, 0.5f, 0.5f)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p3 + glm::vec3(-0.5f, 0.5f, 0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0, 0, 0.5f)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(-0.5f, 0, 0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case CORNERS::MP:
			v0 = {spec + 8 + (shade << 22), p2 + glm::vec3(0.5f, 0.5f, 0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p3 + glm::vec3(0, 0.5f, 0.5f)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0.5f, 0, 0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, 0, 0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case CORNERS::MP | CORNERS::PP:
			break ;
		default:
			v0 = {spec + (shade << 22), p2 + glm::vec3(0, 0.5f, 0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p3 + glm::vec3(0, 0.5f, 0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0, 0, 0.5f)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, 0, 0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
	}
	switch (corners & (CORNERS::MM | CORNERS::PM)) {
		case CORNERS::PM:
			v0 = {spec + (8 << 8) + (shade << 22), p2 + glm::vec3(0, 0, 0.5f)};
			v1 = {spec - (1 << 4) + 8 + (8 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(-0.5f, 0, 0.5f)};
			v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0, -0.5f, 0.5f)};
			v3 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(-0.5f, -0.5f, 0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case CORNERS::MM:
			v0 = {spec + 8 + (8 << 8) + (shade << 22), p2 + glm::vec3(0.5f, 0, 0.5f)};
			v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(0, 0, 0.5f)};
			v2 = {spec + 8 + (shade << 22) + YTEX, p6 + glm::vec3(0.5f, -0.5f, 0.5f)};
			v3 = {spec + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, -0.5f, 0.5f)};
			face_vertices(vertices, v0, v1, v2, v3);
			break ;
		case CORNERS::MM | CORNERS::PM:
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
	// front MINUSX of second step
	spec += (3 << 19);
	switch (corners) {
		case CORNERS::PM | CORNERS::PP: // full width
			v0 = {spec + (shade << 22), p4 + glm::vec3(0.5f, 0, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, 0, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0, -1)};
			break ;
		case CORNERS::PM: // only 'right'
		case CORNERS::PM | CORNERS::PP | CORNERS::MP:
			v0 = {spec + 8 + (shade << 22), p4 + glm::vec3(0.5f, -0.5f, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, -0.5f, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0, -1)};
			break ;
		case CORNERS::PP: // only 'left'
		case CORNERS::PM | CORNERS::PP | CORNERS::MM:
			v0 = {spec + (shade << 22), p4 + glm::vec3(0.5f, 0, -0.5f)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0.5f, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, 0, -1)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0.5f, -1)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	// front PLUSX of second step
	spec += (1 << 19);
	switch (corners) {
		case CORNERS::MM | CORNERS::MP: // full width
			v0 = {spec + (shade << 22), p1 + glm::vec3(-0.5f, 0, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, 0, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p1 + glm::vec3(-0.5f, 0, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p5 + glm::vec3(-0.5f, 0, -1)};
			break ;
		case CORNERS::MP: // only 'right'
		case CORNERS::MM | CORNERS::MP | CORNERS::PM:
			v0 = {spec + 8 + (shade << 22), p1 + glm::vec3(-0.5f, 0.5f, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, 0, -0.5f)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p1 + glm::vec3(-0.5f, 0.5f, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p5 + glm::vec3(-0.5f, 0, -1)};
			break ;
		case CORNERS::MM: // only 'left'
		case CORNERS::MM | CORNERS::MP | CORNERS::PP:
			v0 = {spec + (shade << 22), p1 + glm::vec3(-0.5f, 0, -0.5f)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p5 + glm::vec3(-0.5f, -0.5, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p1 + glm::vec3(-0.5f, 0, -1)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p5 + glm::vec3(-0.5f, -0.5f, -1)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	// front MINUSY of second step
	spec -= (3 << 19);
	switch (corners) {
		case CORNERS::MP | CORNERS::PP: // full width
			v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.5f, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -1)};
			break ;
		case CORNERS::PP: // only 'right'
		case CORNERS::MP | CORNERS::PP | CORNERS::MM:
			v0 = {spec + 8 + (shade << 22), p0 + glm::vec3(0.5f, 0.5f, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0.5f, 0.5f, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -1)};
			break ;
		case CORNERS::MP: // only 'left'
		case CORNERS::MP | CORNERS::PP | CORNERS::PM:
			v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.5f, -0.5f)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p1 + glm::vec3(-0.5f, 0.5f, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -1)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(-0.5f, 0.5f, -1)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	// front PLUSY of second step
	spec += (1 << 19);
	switch (corners) {
		case CORNERS::MM | CORNERS::PM: // full width
			v0 = {spec + (shade << 22), p5 + glm::vec3(0, -0.5f, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0, -0.5f, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p5 + glm::vec3(0, -0.5f, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0, -0.5f, -1)};
			break ;
		case CORNERS::PM: // only 'left'
		case CORNERS::MM | CORNERS::PM | CORNERS::MP:
			v0 = {spec + (shade << 22), p5 + glm::vec3(0, -0.5f, -0.5f)};
			v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p4 + glm::vec3(0.5f, -0.5f, -0.5f)};
			v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p5 + glm::vec3(0, -0.5f, -1)};
			v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0.5f, -0.5f, -1)};
			break ;
		case CORNERS::MM: // only 'right'
		case CORNERS::MM | CORNERS::PM | CORNERS::PP:
			v0 = {spec + 8 + (shade << 22), p5 + glm::vec3(-0.5f, -0.5f, -0.5f)};
			v1 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0, -0.5f, -0.5f)};
			v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p5 + glm::vec3(-0.5f, -0.5f, -1)};
			v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0, -0.5f, -1)};
			break ;
	}
	face_vertices(vertices, v0, v1, v2, v3);
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
		spec = baseSpec + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		shade = 0;
		spec += (faceLight << 24);
		switch (corners & (CORNERS::MP | CORNERS::PP)) {
			case CORNERS::MP:
				v0 = {spec + (shade << 22), p2 + glm::vec3(0, 0.5f, 0)};
				v1 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX, p3 + glm::vec3(-0.5f, 0.5f, 0)};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0, 0, 0)};
				v3 = {spec - (1 << 4) + 8 - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(-0.5f, 0, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case CORNERS::PP:
				v0 = {spec + 8 + (shade << 22), p2 + glm::vec3(0.5f, 0.5f, 0)};
				v1 = {spec + (shade << 22) + XTEX, p3 + glm::vec3(0, 0.5f, 0)};
				v2 = {spec + 8 - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0.5f, 0, 0)};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, 0, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case CORNERS::MP | CORNERS::PP:
				v0 = {spec + (shade << 22), p2 + glm::vec3(0, 0.5f, 0)};
				v1 = {spec + (shade << 22) + XTEX, p3 + glm::vec3(0, 0.5f, 0)};
				v2 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + YTEX, p6};
				v3 = {spec - (1 << 12) + (8 << 8) + (shade << 22) + XTEX + YTEX, p7};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
		}
		switch (corners & (CORNERS::MM | CORNERS::PM)) {
			case CORNERS::MM:
				v0 = {spec + (8 << 8) + (shade << 22), p2 + glm::vec3(0, 0, 0)};
				v1 = {spec - (1 << 4) + 8 + (8 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(-0.5f, 0, 0)};
				v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0, -0.5f, 0)};
				v3 = {spec - (1 << 4) + 8 + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(-0.5f, -0.5f, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case CORNERS::PM:
				v0 = {spec + 8 + (8 << 8) + (shade << 22), p2 + glm::vec3(0.5f, 0, 0)};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(0, 0, 0)};
				v2 = {spec + 8 + (shade << 22) + YTEX, p6 + glm::vec3(0.5f, -0.5f, 0)};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, -0.5f, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
			case CORNERS::MM | CORNERS::PM:
				v0 = {spec + (8 << 8) + (shade << 22), p2};
				v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p3};
				v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0, -0.5f, 0)};
				v3 = {spec + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, -0.5f, 0)};
				face_vertices(vertices, v0, v1, v2, v3);
				break ;
		}
	}
}

void Door::addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::ivec3 pos, int value ) const
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
	int bitfield = value >> 12;
	bool open = !!(bitfield & DOOR::OPEN) ^ ((value >> REDSTONE::POWERED_OFFSET) & 0x1);
	switch ((value >> 9) & 0x7) {
		case face_dir::MINUSX:
			if (!open) {
				orientation = face_dir::MINUSX;
				xtex_l = (bitfield & DOOR::RIGHT_HINGE) ? 0 : XTEX;
				xtex_r = (bitfield & DOOR::RIGHT_HINGE) ? XTEX : 0;
			} else if (bitfield & DOOR::RIGHT_HINGE) {
				orientation = face_dir::PLUSY;
				xtex_l = XTEX;
				xtex_r = 0;
			} else {
				orientation = face_dir::MINUSY;
				xtex_l = 0;
				xtex_r = XTEX;
			}
			break ;
		case face_dir::PLUSX:
			if (!open) {
				orientation = face_dir::PLUSX;
				xtex_l = (bitfield & DOOR::RIGHT_HINGE) ? XTEX : 0;
				xtex_r = (bitfield & DOOR::RIGHT_HINGE) ? 0 : XTEX;
			} else if (bitfield & DOOR::RIGHT_HINGE) {
				orientation = face_dir::MINUSY;
				xtex_l = 0;
				xtex_r = XTEX;
			} else {
				orientation = face_dir::PLUSY;
				xtex_l = XTEX;
				xtex_r = 0;
			}
			break ;
		case face_dir::MINUSY:
			if (!open) {
				orientation = face_dir::MINUSY;
				xtex_l = (bitfield & DOOR::RIGHT_HINGE) ? 0 : XTEX;
				xtex_r = (bitfield & DOOR::RIGHT_HINGE) ? XTEX : 0;
			} else if (bitfield & DOOR::RIGHT_HINGE) {
				orientation = face_dir::MINUSX;
				xtex_l = 0;
				xtex_r = XTEX;
			} else {
				orientation = face_dir::PLUSX;
				xtex_l = XTEX;
				xtex_r = 0;
			}
			break ;
		case face_dir::PLUSY:
			if (!open) {
				orientation = face_dir::PLUSY;
				xtex_l = (bitfield & DOOR::RIGHT_HINGE) ? XTEX : 0;
				xtex_r = (bitfield & DOOR::RIGHT_HINGE) ? 0 : XTEX;
			} else if (bitfield & DOOR::RIGHT_HINGE) {
				orientation = face_dir::PLUSX;
				xtex_l = XTEX;
				xtex_r = 0;
			} else {
				orientation = face_dir::MINUSX;
				xtex_l = 0;
				xtex_r = XTEX;
			}
			break ;
	}
	switch (orientation) {
		case face_dir::MINUSX:
			p0.x += 0.8125f;
			p2.x += 0.8125f;
			p4.x += 0.8125f;
			p6.x += 0.8125f;
			spec = (this->texX(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) << 12) + (3 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			shade = 0;
			spec += (faceLight << 24);
			v0 = {spec + (shade << 22) + xtex_r, p4};
			v1 = {spec + (shade << 22) + xtex_l, p0};
			v2 = {spec + (shade << 22) + xtex_r + YTEX, p6};
			v3 = {spec + (shade << 22) + xtex_l + YTEX, p2};
			face_vertices(vertices, v0, v1, v2, v3);
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z, true), face_dir::PLUSX)) {
				spec = (this->texX(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) << 12) + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22) + xtex_l, p1};
				v1 = {spec + (shade << 22) + xtex_r, p5};
				v2 = {spec + (shade << 22) + xtex_l + YTEX, p3};
				v3 = {spec + (shade << 22) + xtex_r + YTEX, p7};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z, true), face_dir::MINUSY)) {
				spec = (this->texX(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) << 12) + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p1};
				v2 = {spec + (shade << 22) + YTEX, p2};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p3};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z, true), face_dir::PLUSY)) {
				spec = (this->texX(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) << 12) + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p5};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p4};
				v2 = {spec + (shade << 22) + YTEX, p7};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p6};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if ((bitfield & DOOR::UPPER_HALF) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
				spec = (this->texX(face_dir::PLUSZ) << 4) + (this->texY(face_dir::PLUSZ) << 12) + (0 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + XTEX, p4};
				v2 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + YTEX, p1};
				v3 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + XTEX + YTEX, p5};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (!(bitfield & DOOR::UPPER_HALF) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
				spec = (this->texX(face_dir::MINUSZ, face_dir::MINUSZ) << 4) + (this->texY(face_dir::MINUSZ, face_dir::MINUSZ) << 12) + (5 << 19);
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
		case face_dir::PLUSX:
			p1.x -= 0.8125f;
			p3.x -= 0.8125f;
			p5.x -= 0.8125f;
			p7.x -= 0.8125f;
			if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z, true), face_dir::MINUSX)) {
				spec = (this->texX(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) << 12) + (3 << 19);
				faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22) + xtex_r, p4};
				v1 = {spec + (shade << 22) + xtex_l, p0};
				v2 = {spec + (shade << 22) + xtex_r + YTEX, p6};
				v3 = {spec + (shade << 22) + xtex_l + YTEX, p2};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			spec = (this->texX(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) << 12) + (4 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			spec += (faceLight << 24);
			v0 = {spec + (shade << 22) + xtex_l, p1};
			v1 = {spec + (shade << 22) + xtex_r, p5};
			v2 = {spec + (shade << 22) + xtex_l + YTEX, p3};
			v3 = {spec + (shade << 22) + xtex_r + YTEX, p7};
			face_vertices(vertices, v0, v1, v2, v3);
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z, true), face_dir::MINUSY)) {
				spec = (this->texX(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) << 12) + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p1};
				v2 = {spec + (shade << 22) + YTEX, p2};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p3};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z, true), face_dir::PLUSY)) {
				spec = (this->texX(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) << 12) + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p5};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p4};
				v2 = {spec + (shade << 22) + YTEX, p7};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p6};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if ((bitfield & DOOR::UPPER_HALF) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
				spec = (this->texX(face_dir::PLUSZ) << 4) + (this->texY(face_dir::PLUSZ) << 12) + (0 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + XTEX, p4};
				v2 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + YTEX, p1};
				v3 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + XTEX + YTEX, p5};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (!(bitfield & DOOR::UPPER_HALF) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
				spec = (this->texX(face_dir::MINUSZ, face_dir::MINUSZ) << 4) + (this->texY(face_dir::MINUSZ, face_dir::MINUSZ) << 12) + (5 << 19);
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
		case face_dir::MINUSY:
			p0.y += 0.8125f;
			p1.y += 0.8125f;
			p2.y += 0.8125f;
			p3.y += 0.8125f;
			if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z, true), face_dir::MINUSX)) {
				spec = (this->texX(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) << 12) + (3 << 19);
				faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p0};
				v2 = {spec + (shade << 22) + YTEX, p6};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p2};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z, true), face_dir::PLUSX)) {
				spec = (this->texX(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) << 12) + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p5};
				v2 = {spec + (shade << 22) + YTEX, p3};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p7};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			spec = (this->texX(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) << 12) + (1 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			spec += (faceLight << 24);
			v0 = {spec + (shade << 22) + xtex_r, p0};
			v1 = {spec + (shade << 22) + xtex_l, p1};
			v2 = {spec + (shade << 22) + xtex_r + YTEX, p2};
			v3 = {spec + (shade << 22) + xtex_l + YTEX, p3};
			face_vertices(vertices, v0, v1, v2, v3);
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z, true), face_dir::PLUSY)) {
				spec = (this->texX(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) << 12) + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22) + xtex_l, p5};
				v1 = {spec + (shade << 22) + xtex_r, p4};
				v2 = {spec + (shade << 22) + xtex_l + YTEX, p7};
				v3 = {spec + (shade << 22) + xtex_r + YTEX, p6};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if ((bitfield & DOOR::UPPER_HALF) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
				spec = (this->texX(face_dir::PLUSZ) << 4) + (this->texY(face_dir::PLUSZ) << 12) + (0 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + XTEX, p0};
				v2 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + YTEX, p5};
				v3 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + XTEX + YTEX, p4};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (!(bitfield & DOOR::UPPER_HALF) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
				spec = (this->texX(face_dir::MINUSZ, face_dir::MINUSZ) << 4) + (this->texY(face_dir::MINUSZ, face_dir::MINUSZ) << 12) + (5 << 19);
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
		case face_dir::PLUSY:
			p4.y -= 0.8125f;
			p5.y -= 0.8125f;
			p6.y -= 0.8125f;
			p7.y -= 0.8125f;
			if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z, true), face_dir::MINUSX)) {
				spec = (this->texX(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) << 12) + (3 << 19);
				faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p0};
				v2 = {spec + (shade << 22) + YTEX, p6};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p2};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z, true), face_dir::PLUSX)) {
				spec = (this->texX(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) << 12) + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + 3 + (1 << 17), p5};
				v2 = {spec + (shade << 22) + YTEX, p3};
				v3 = {spec + (shade << 22) + 3 + (1 << 17) + YTEX, p7};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z, true), face_dir::MINUSY)) {
				spec = (this->texX(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) << 12) + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22) + xtex_r, p0};
				v1 = {spec + (shade << 22) + xtex_l, p1};
				v2 = {spec + (shade << 22) + xtex_r + YTEX, p2};
				v3 = {spec + (shade << 22) + xtex_l + YTEX, p3};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			spec = (this->texX(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) << 4) + (this->texY(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) << 12) + (2 << 19);
			faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
			spec += (faceLight << 24);
			v0 = {spec + (shade << 22) + xtex_l, p5};
			v1 = {spec + (shade << 22) + xtex_r, p4};
			v2 = {spec + (shade << 22) + xtex_l + YTEX, p7};
			v3 = {spec + (shade << 22) + xtex_r + YTEX, p6};
			face_vertices(vertices, v0, v1, v2, v3);
			if ((bitfield & DOOR::UPPER_HALF) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
				spec = (this->texX(face_dir::PLUSZ) << 4) + (this->texY(face_dir::PLUSZ) << 12) + (0 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
				shade = 0;
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + XTEX, p0};
				v2 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + YTEX, p5};
				v3 = {spec + (shade << 22) - (1 << 12) + (3 << 8) + XTEX + YTEX, p4};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (!(bitfield & DOOR::UPPER_HALF) && visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
				spec = (this->texX(face_dir::MINUSZ, face_dir::MINUSZ) << 4) + (this->texY(face_dir::MINUSZ, face_dir::MINUSZ) << 12) + (5 << 19);
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

void Trapdoor::addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::ivec3 pos, int value ) const
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
	int bitfield = value >> 12;
	int baseSpec = (textureX << 4) + (textureY << 12);
	int spec, faceLight;
	t_shaderInput v0, v1, v2, v3;
	bool open = !!(bitfield & DOOR::OPEN) ^ ((value >> REDSTONE::POWERED_OFFSET) & 0x1);
	if (!open) {
		if (!(bitfield & DOOR::UPPER_HALF)) {
			p0.z -= 0.8125f;
			p1.z -= 0.8125f;
			p4.z -= 0.8125f;
			p5.z -= 0.8125f;
			if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z, true), face_dir::MINUSX)) {
				spec = baseSpec + (3 << 19);
				faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p4};
				v1 = {spec + (shade << 22) + XTEX, p0};
				v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p6};
				v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p2};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z, true), face_dir::PLUSX)) {
				spec = baseSpec + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p1};
				v1 = {spec + (shade << 22) + XTEX, p5};
				v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p3};
				v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p7};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z, true), face_dir::MINUSY)) {
				spec = baseSpec + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec + (shade << 22), p0};
				v1 = {spec + (shade << 22) + XTEX, p1};
				v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p2};
				v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p3};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z, true), face_dir::PLUSY)) {
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
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
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
			if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z, true), face_dir::MINUSX)) {
				spec = baseSpec + (3 << 19);
				faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p4};
				v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p0};
				v2 = {spec + (shade << 22), p6};
				v3 = {spec + (shade << 22) + XTEX, p2};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z, true), face_dir::PLUSX)) {
				spec = baseSpec + (4 << 19);
				faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
				spec += (faceLight << 24);
				v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p1};
				v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p5};
				v2 = {spec + (shade << 22), p3};
				v3 = {spec + (shade << 22) + XTEX, p7};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z, true), face_dir::MINUSY)) {
				spec = baseSpec + (1 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p0};
				v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p1};
				v2 = {spec + (shade << 22), p2};
				v3 = {spec + (shade << 22) + XTEX, p3};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z, true), face_dir::PLUSY)) {
				spec = baseSpec + (2 << 19);
				faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
				spec += (faceLight << 24);
				v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p5};
				v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p4};
				v2 = {spec + (shade << 22), p7};
				v3 = {spec + (shade << 22) + XTEX, p6};
				face_vertices(vertices, v0, v1, v2, v3);
			}
			if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
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
		switch ((value >> 9) & 0x7) {
			case face_dir::MINUSX:
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
				if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z, true), face_dir::PLUSX)) {
					spec = baseSpec + (4 << 19);
					faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p1};
					v1 = {spec + (shade << 22) + XTEX, p5};
					v2 = {spec + (shade << 22) + YTEX, p3};
					v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z, true), face_dir::MINUSY)) {
					spec = baseSpec + (1 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
					spec += (faceLight << 24);
					v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p2};
					v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p0};
					v2 = {spec + (shade << 22) + XTEX, p3};
					v3 = {spec + (shade << 22), p1};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z, true), face_dir::PLUSY)) {
					spec = baseSpec + (2 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p7};
					v1 = {spec + (shade << 22) + XTEX, p5};
					v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p6};
					v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p4};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
					spec = baseSpec + (0 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
					spec += (faceLight << 24);
					v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p0};
					v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p4};
					v2 = {spec + (shade << 22) + XTEX, p1};
					v3 = {spec + (shade << 22), p5};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
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
			case face_dir::PLUSX:
				p1 -= glm::vec3(0.8125f, 0, 0);
				p3 -= glm::vec3(0.8125f, 0, 0);
				p5 -= glm::vec3(0.8125f, 0, 0);
				p7 -= glm::vec3(0.8125f, 0, 0);
				if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z, true), face_dir::MINUSX)) {
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
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z, true), face_dir::MINUSY)) {
					spec = baseSpec + (1 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y - 1, pos.z);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p2};
					v1 = {spec + (shade << 22) + XTEX, p0};
					v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p3};
					v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p1};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z, true), face_dir::PLUSY)) {
					spec = baseSpec + (2 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
					spec += (faceLight << 24);
					v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p7};
					v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p5};
					v2 = {spec + (shade << 22) + XTEX, p6};
					v3 = {spec + (shade << 22), p4};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
					spec = baseSpec + (0 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p0};
					v1 = {spec + (shade << 22) + XTEX, p4};
					v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p1};
					v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p5};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
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
			case face_dir::MINUSY:
				p0 += glm::vec3(0, 0.8125f, 0);
				p1 += glm::vec3(0, 0.8125f, 0);
				p2 += glm::vec3(0, 0.8125f, 0);
				p3 += glm::vec3(0, 0.8125f, 0);
				if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z, true), face_dir::MINUSX)) {
					spec = baseSpec + (3 << 19);
					faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p6};
					v1 = {spec + (shade << 22) + XTEX, p4};
					v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p2};
					v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p0};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z, true), face_dir::PLUSX)) {
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
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y + 1, pos.z, true), face_dir::PLUSY)) {
					spec = baseSpec + (2 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y + 1, pos.z);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p5};
					v1 = {spec + (shade << 22) + XTEX, p4};
					v2 = {spec + (shade << 22) + YTEX, p7};
					v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
					spec = baseSpec + (0 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p4};
					v1 = {spec + (shade << 22) + XTEX, p5};
					v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p0};
					v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p1};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
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
			case face_dir::PLUSY:
				p4 -= glm::vec3(0, 0.8125f, 0);
				p5 -= glm::vec3(0, 0.8125f, 0);
				p6 -= glm::vec3(0, 0.8125f, 0);
				p7 -= glm::vec3(0, 0.8125f, 0);
				if (visible_face(value, chunk->getBlockAt(pos.x - 1, pos.y, pos.z, true), face_dir::MINUSX)) {
					spec = baseSpec + (3 << 19);
					faceLight = chunk->computeLight(pos.x - 1, pos.y, pos.z);
					spec += (faceLight << 24);
					v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p6};
					v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p4};
					v2 = {spec + (shade << 22) + XTEX, p2};
					v3 = {spec + (shade << 22), p0};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x + 1, pos.y, pos.z, true), face_dir::PLUSX)) {
					spec = baseSpec + (4 << 19);
					faceLight = chunk->computeLight(pos.x + 1, pos.y, pos.z);
					spec += (faceLight << 24);
					v0 = {spec + (shade << 22), p3};
					v1 = {spec + (shade << 22) + XTEX, p1};
					v2 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p7};
					v3 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p5};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y - 1, pos.z, true), face_dir::MINUSY)) {
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
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
					spec = baseSpec + (0 << 19);
					faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
					spec += (faceLight << 24);
					v0 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + XTEX + YTEX, p4};
					v1 = {spec - (1 << 12) + (3 << 8) + (shade << 22) + YTEX, p5};
					v2 = {spec + (shade << 22) + XTEX, p0};
					v3 = {spec + (shade << 22), p1};
					face_vertices(vertices, v0, v1, v2, v3);
				}
				if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
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

void Fence::addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::ivec3 pos, int value ) const
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
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
		spec = baseSpec + (0 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		spec += (faceLight << 24);
		v0 = {spec + 6 + (6 << 8) + (shade << 22), pp4};
		v1 = {spec - 6 + (6 << 8) + (shade << 22) + XTEX, pp5};
		v2 = {spec + 6 - (6 << 8) + (shade << 22) + YTEX, pp0};
		v3 = {spec - 6 - (6 << 8) + (shade << 22) + XTEX + YTEX, pp1};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
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
	if ((value >> 12) & FENCE::MX) {
		pp0 = p0 + glm::vec3( 0,                7.0f * ONE16TH, -ONE16TH);
		pp1 = p1 + glm::vec3(-10.0f * ONE16TH,  7.0f * ONE16TH, -ONE16TH);
		pp2 = p2 + glm::vec3( 0,                7.0f * ONE16TH, 12.0f * ONE16TH);
		pp3 = p3 + glm::vec3(-10.0f * ONE16TH,  7.0f * ONE16TH, 12.0f * ONE16TH);
		pp4 = p4 + glm::vec3( 0,               -7.0f * ONE16TH, -ONE16TH);
		pp5 = p5 + glm::vec3(-10.0f * ONE16TH, -7.0f * ONE16TH, -ONE16TH);
		pp6 = p6 + glm::vec3( 0,               -7.0f * ONE16TH, 12.0f * ONE16TH);
		pp7 = p7 + glm::vec3(-10.0f * ONE16TH, -7.0f * ONE16TH, 12.0f * ONE16TH);
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
			pp0.z -= 6.0f * ONE16TH;
			pp1.z -= 6.0f * ONE16TH;
			pp2.z -= 6.0f * ONE16TH;
			pp3.z -= 6.0f * ONE16TH;
			pp4.z -= 6.0f * ONE16TH;
			pp5.z -= 6.0f * ONE16TH;
			pp6.z -= 6.0f * ONE16TH;
			pp7.z -= 6.0f * ONE16TH;
		}
	}
	if ((value >> 12) & FENCE::PX) {
		pp0 = p0 + glm::vec3( 10.0f * ONE16TH,  7.0f * ONE16TH, -ONE16TH);
		pp1 = p1 + glm::vec3( 0,                7.0f * ONE16TH, -ONE16TH);
		pp2 = p2 + glm::vec3( 10.0f * ONE16TH,  7.0f * ONE16TH, 12.0f * ONE16TH);
		pp3 = p3 + glm::vec3( 0,                7.0f * ONE16TH, 12.0f * ONE16TH);
		pp4 = p4 + glm::vec3( 10.0f * ONE16TH, -7.0f * ONE16TH, -ONE16TH);
		pp5 = p5 + glm::vec3( 0,               -7.0f * ONE16TH, -ONE16TH);
		pp6 = p6 + glm::vec3( 10.0f * ONE16TH, -7.0f * ONE16TH, 12.0f * ONE16TH);
		pp7 = p7 + glm::vec3( 0,               -7.0f * ONE16TH, 12.0f * ONE16TH);
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
			pp0.z -= 6.0f * ONE16TH;
			pp1.z -= 6.0f * ONE16TH;
			pp2.z -= 6.0f * ONE16TH;
			pp3.z -= 6.0f * ONE16TH;
			pp4.z -= 6.0f * ONE16TH;
			pp5.z -= 6.0f * ONE16TH;
			pp6.z -= 6.0f * ONE16TH;
			pp7.z -= 6.0f * ONE16TH;
		}
	}
	if ((value >> 12) & FENCE::MY) {
		pp0 = p0 + glm::vec3( 7.0f * ONE16TH,  0,               -ONE16TH);
		pp1 = p1 + glm::vec3(-7.0f * ONE16TH,  0,               -ONE16TH);
		pp2 = p2 + glm::vec3( 7.0f * ONE16TH,  0,               12.0f * ONE16TH);
		pp3 = p3 + glm::vec3(-7.0f * ONE16TH,  0,               12.0f * ONE16TH);
		pp4 = p4 + glm::vec3( 7.0f * ONE16TH, -10.0f * ONE16TH, -ONE16TH);
		pp5 = p5 + glm::vec3(-7.0f * ONE16TH, -10.0f * ONE16TH, -ONE16TH);
		pp6 = p6 + glm::vec3( 7.0f * ONE16TH, -10.0f * ONE16TH, 12.0f * ONE16TH);
		pp7 = p7 + glm::vec3(-7.0f * ONE16TH, -10.0f * ONE16TH, 12.0f * ONE16TH);
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
			pp0.z -= 6.0f * ONE16TH;
			pp1.z -= 6.0f * ONE16TH;
			pp2.z -= 6.0f * ONE16TH;
			pp3.z -= 6.0f * ONE16TH;
			pp4.z -= 6.0f * ONE16TH;
			pp5.z -= 6.0f * ONE16TH;
			pp6.z -= 6.0f * ONE16TH;
			pp7.z -= 6.0f * ONE16TH;
		}
	}
	if ((value >> 12) & FENCE::PY) {
		pp0 = p0 + glm::vec3( 7.0f * ONE16TH, 10.0f * ONE16TH, -ONE16TH);
		pp1 = p1 + glm::vec3(-7.0f * ONE16TH, 10.0f * ONE16TH, -ONE16TH);
		pp2 = p2 + glm::vec3( 7.0f * ONE16TH, 10.0f * ONE16TH, 12.0f * ONE16TH);
		pp3 = p3 + glm::vec3(-7.0f * ONE16TH, 10.0f * ONE16TH, 12.0f * ONE16TH);
		pp4 = p4 + glm::vec3( 7.0f * ONE16TH, 0,               -ONE16TH);
		pp5 = p5 + glm::vec3(-7.0f * ONE16TH, 0,               -ONE16TH);
		pp6 = p6 + glm::vec3( 7.0f * ONE16TH, 0,               12.0f * ONE16TH);
		pp7 = p7 + glm::vec3(-7.0f * ONE16TH, 0,               12.0f * ONE16TH);
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
			pp0.z -= 6.0f * ONE16TH;
			pp1.z -= 6.0f * ONE16TH;
			pp2.z -= 6.0f * ONE16TH;
			pp3.z -= 6.0f * ONE16TH;
			pp4.z -= 6.0f * ONE16TH;
			pp5.z -= 6.0f * ONE16TH;
			pp6.z -= 6.0f * ONE16TH;
			pp7.z -= 6.0f * ONE16TH;
		}
	}
}

void GlassPane::addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::ivec3 pos, int value ) const
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
	int baseSpec = (this->texX(face_dir::PLUSZ, FENCE::ARM_END) << 4) + (this->texY() << 12);
	int faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	int spec;
	t_shaderInput v0, v1, v2, v3;
	if ((value >> 12) & FENCE::MX) {
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
		spec = baseSpec + (3 << 19) + (!!((value >> 12) & (FENCE::MY | FENCE::PY)) << 12);
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p4 + glm::vec3(0.4375f, 0, 0)};
		v1 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0.4375f, 0, 0)};
		v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0.4375f, 0, 0)};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0.4375f, 0, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if ((value >> 12) & FENCE::PX) {
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
		spec = baseSpec + (4 << 19) + (!!((value >> 12) & (FENCE::MY | FENCE::PY)) << 12);
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p1 - glm::vec3(0.4375f, 0, 0)};
		v1 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0.4375f, 0, 0)};
		v2 = {spec + (shade << 22) + YTEX, p3 - glm::vec3(0.4375f, 0, 0)};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0.4375f, 0, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if ((value >> 12) & FENCE::MY) {
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
		spec = baseSpec + (1 << 19) + (!!((value >> 12) & (FENCE::MX | FENCE::PX)) << 12);
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.4375f, 0)};
		v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.4375f, 0)};
		v2 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0, 0.4375f, 0)};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0.4375f, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if ((value >> 12) & FENCE::PY) {
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
		spec = baseSpec + (2 << 19) + (!!((value >> 12) & (FENCE::MX | FENCE::PX)) << 12);
		spec += (faceLight << 24);
		v0 = {spec + (shade << 22), p5 - glm::vec3(0, 0.4375f, 0)};
		v1 = {spec + (shade << 22) + XTEX, p4 - glm::vec3(0, 0.4375f, 0)};
		v2 = {spec + (shade << 22) + YTEX, p7 - glm::vec3(0, 0.4375f, 0)};
		v3 = {spec + (shade << 22) + XTEX + YTEX, p6 - glm::vec3(0, 0.4375f, 0)};
		face_vertices(vertices, v0, v1, v2, v3);
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z + 1, true), face_dir::PLUSZ)) {
		spec = baseSpec + (0 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z + 1);
		spec += (faceLight << 24);
		if ((value >> 12) & FENCE::PY) {
			v0 = {spec + (shade << 22), p4};
			v1 = {spec + (shade << 22) + XTEX, p5};
		} else {
			v0 = {spec + (7 << 8) + (shade << 22), p4 - glm::vec3(0, 0.4375f, 0)};
			v1 = {spec + (7 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.4375f, 0)};
		}
		if ((value >> 12) & FENCE::MY) {
			v2 = {spec + (shade << 22) + YTEX, p0};
			v3 = {spec + (shade << 22) + XTEX + YTEX, p1};
		} else {
			v2 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.4375f, 0)};
			v3 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.4375f, 0)};
		}
		face_vertices(vertices, v0, v1, v2, v3);
		if ((value >> 12) & FENCE::MX) {
			v0 = {spec + (shade << 22), p0};
			v1 = {spec + (shade << 22) + XTEX, p4};
			v2 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0.4375f, 0, 0)};
			v3 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0.4375f, 0, 0)};
			face_vertices(vertices, v0, v1, v2, v3);
		}
		if ((value >> 12) & FENCE::PX) {
			v0 = {spec + (shade << 22), p1};
			v1 = {spec + (shade << 22) + XTEX, p5};
			v2 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + YTEX, p1 - glm::vec3(0.4375f, 0, 0)};
			v3 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + XTEX + YTEX, p5 - glm::vec3(0.4375f, 0, 0)};
			face_vertices(vertices, v0, v1, v2, v3);
		}
	}
	if (visible_face(value, chunk->getBlockAt(pos.x, pos.y, pos.z - 1, true), face_dir::MINUSZ)) {
		spec = baseSpec + (5 << 19);
		faceLight = chunk->computeLight(pos.x, pos.y, pos.z - 1);
		spec += (faceLight << 24);
		if ((value >> 12) & FENCE::MY) {
			v0 = {spec + (shade << 22), p2};
			v1 = {spec + (shade << 22) + XTEX, p3};
		} else {
			v0 = {spec + (7 << 8) + (shade << 22), p2 + glm::vec3(0, 0.4375f, 0)};
			v1 = {spec + (7 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(0, 0.4375f, 0)};
		}
		if ((value >> 12) & FENCE::PY) {
			v2 = {spec + (shade << 22) + YTEX, p6};
			v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
		} else {
			v2 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + YTEX, p6 - glm::vec3(0, 0.4375f, 0)};
			v3 = {spec - (1 << 12) + (9 << 8) + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0, 0.4375f, 0)};
		}
		face_vertices(vertices, v0, v1, v2, v3);
		if ((value >> 12) & FENCE::MX) {
			v0 = {spec + (shade << 22), p6};
			v1 = {spec + (shade << 22) + XTEX, p2};
			v2 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0.4375f, 0, 0)};
			v3 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0.4375f, 0, 0)};
			face_vertices(vertices, v0, v1, v2, v3);
		}
		if ((value >> 12) & FENCE::PX) {
			v0 = {spec + (shade << 22), p7};
			v1 = {spec + (shade << 22) + XTEX, p3};
			v2 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + YTEX, p7 - glm::vec3(0.4375f, 0, 0)};
			v3 = {spec - (1 << 12) + (7 << 8) + (shade << 22) + XTEX + YTEX, p3 - glm::vec3(0.4375f, 0, 0)};
			face_vertices(vertices, v0, v1, v2, v3);
		}
	}
}

void Crop::addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::ivec3 pos, int value ) const
{
	(void)value;
	glm::vec3 p0 = {start.x + pos.x + 0, start.y + pos.y + 0, pos.z + 1 - ONE16TH};
	glm::vec3 p1 = {start.x + pos.x + 1, start.y + pos.y + 0, pos.z + 1 - ONE16TH};
	glm::vec3 p2 = {start.x + pos.x + 0, start.y + pos.y + 0, pos.z - ONE16TH};
	glm::vec3 p3 = {start.x + pos.x + 1, start.y + pos.y + 0, pos.z - ONE16TH};

	glm::vec3 p4 = {start.x + pos.x + 0, start.y + pos.y + 1, pos.z + 1 - ONE16TH};
	glm::vec3 p5 = {start.x + pos.x + 1, start.y + pos.y + 1, pos.z + 1 - ONE16TH};
	glm::vec3 p6 = {start.x + pos.x + 0, start.y + pos.y + 1, pos.z - ONE16TH};
	glm::vec3 p7 = {start.x + pos.x + 1, start.y + pos.y + 1, pos.z - ONE16TH};

	int spec = (textureX << 4) + (textureY << 12) + (0 << 19) + (chunk->computeLight(pos.x, pos.y, pos.z) << 24);
	t_shaderInput v0 = {spec, {p4.x + 3.0f * ONE16TH, p4.y, p4.z}};
	t_shaderInput v1 = {spec + XTEX, {p0.x + 3.0f * ONE16TH, p0.y, p0.z}};
	t_shaderInput v2 = {spec + YTEX, {p6.x + 3.0f * ONE16TH, p6.y, p6.z}};
	t_shaderInput v3 = {spec + XTEX + YTEX, {p2.x + 3.0f * ONE16TH, p2.y, p2.z}};
	face_vertices(vertices, v0, v1, v2, v3); // -x
	v0 = {spec + XTEX, {p0.x + 13.0f * ONE16TH, p0.y, p0.z}};
	v1 = {spec, {p4.x + 13.0f * ONE16TH, p4.y, p4.z}};
	v2 = {spec + XTEX + YTEX, {p2.x + 13.0f * ONE16TH, p2.y, p2.z}};
	v3 = {spec + YTEX, {p6.x + 13.0f * ONE16TH, p6.y, p6.z}};
	face_vertices(vertices, v0, v1, v2, v3); // +x
	v0 = {spec, {p0.x, p0.y + 3.0f * ONE16TH, p0.z}};
	v1 = {spec + XTEX, {p1.x, p1.y + 3.0f * ONE16TH, p1.z}};
	v2 = {spec + YTEX, {p2.x, p2.y + 3.0f * ONE16TH, p2.z}};
	v3 = {spec + XTEX + YTEX, {p3.x, p3.y + 3.0f * ONE16TH, p3.z}};
	face_vertices(vertices, v0, v1, v2, v3); // -y
	v0 = {spec + XTEX, {p1.x, p1.y + 13.0f * ONE16TH, p1.z}};
	v1 = {spec, {p0.x, p0.y + 13.0f * ONE16TH, p0.z}};
	v2 = {spec + XTEX + YTEX, {p3.x, p3.y + 13.0f * ONE16TH, p3.z}};
	v3 = {spec + YTEX, {p2.x, p2.y + 13.0f * ONE16TH, p2.z}};
	face_vertices(vertices, v0, v1, v2, v3); // +y
}

void Lever::addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::ivec3 pos, int value ) const
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
	int placement = (value >> 12) & 0x3;
	int orientation = (value >> 9) & 0x7;
	switch (placement) {
		case PLACEMENT::WALL:
			switch (orientation) {
				case face_dir::MINUSX:
					p0 += glm::vec3( 13.0f * ONE16TH,  5.0f * ONE16TH, -4.0f * ONE16TH);
					p1 += glm::vec3( 0,                5.0f * ONE16TH, -4.0f * ONE16TH);
					p2 += glm::vec3( 13.0f * ONE16TH,  5.0f * ONE16TH,  4.0f * ONE16TH);
					p3 += glm::vec3( 0,                5.0f * ONE16TH,  4.0f * ONE16TH);

					p4 += glm::vec3( 13.0f * ONE16TH, -5.0f * ONE16TH, -4.0f * ONE16TH);
					p5 += glm::vec3( 0,               -5.0f * ONE16TH, -4.0f * ONE16TH);
					p6 += glm::vec3( 13.0f * ONE16TH, -5.0f * ONE16TH,  4.0f * ONE16TH);
					p7 += glm::vec3( 0,               -5.0f * ONE16TH,  4.0f * ONE16TH);

					x_texMX = 5; x_texPX = 5;  x_texMY = 4; x_texPY = 4;
					y_texMX = 0; y_texPX = 13; y_texMY = 4; y_texPY = 4;
					z_texMX = 13; z_texPX = 0;  z_texMY = 5;  z_texPY = 5;
					break ;
				case face_dir::PLUSX:
					p0 += glm::vec3( 0,               5.0f * ONE16TH, -4.0f * ONE16TH);
					p1 += glm::vec3(-13.0f * ONE16TH, 5.0f * ONE16TH, -4.0f * ONE16TH);
					p2 += glm::vec3( 0,               5.0f * ONE16TH,  4.0f * ONE16TH);
					p3 += glm::vec3(-13.0f * ONE16TH, 5.0f * ONE16TH,  4.0f * ONE16TH);

					p4 += glm::vec3( 0,              -5.0f * ONE16TH, -4.0f * ONE16TH);
					p5 += glm::vec3(-13.0f * ONE16TH,-5.0f * ONE16TH, -4.0f * ONE16TH);
					p6 += glm::vec3( 0,              -5.0f * ONE16TH,  4.0f * ONE16TH);
					p7 += glm::vec3(-13.0f * ONE16TH,-5.0f * ONE16TH,  4.0f * ONE16TH);

					x_texMX = 5; x_texPX = 5;  x_texMY = 4; x_texPY = 4;
					y_texMX = 0; y_texPX = 13; y_texMY = 4; y_texPY = 4;
					z_texMX = 0; z_texPX = 13;  z_texMY = 5;  z_texPY = 5;
					break ;
				case face_dir::MINUSY:
					p0 += glm::vec3( 5.0f * ONE16TH, 13.0f * ONE16TH, -4.0f * ONE16TH);
					p1 += glm::vec3(-5.0f * ONE16TH, 13.0f * ONE16TH, -4.0f * ONE16TH);
					p2 += glm::vec3( 5.0f * ONE16TH, 13.0f * ONE16TH,  4.0f * ONE16TH);
					p3 += glm::vec3(-5.0f * ONE16TH, 13.0f * ONE16TH,  4.0f * ONE16TH);

					p4 += glm::vec3( 5.0f * ONE16TH, 0,               -4.0f * ONE16TH);
					p5 += glm::vec3(-5.0f * ONE16TH, 0,               -4.0f * ONE16TH);
					p6 += glm::vec3( 5.0f * ONE16TH, 0,                4.0f * ONE16TH);
					p7 += glm::vec3(-5.0f * ONE16TH, 0,                4.0f * ONE16TH);

					x_texMX = 0; x_texPX = 13; x_texMY = 4; x_texPY = 4;
					y_texMX = 5; y_texPX = 5;  y_texMY = 4; y_texPY = 4;
					z_texMX = 5; z_texPX = 5;  z_texMY = 13;  z_texPY = 0;
					break ;
				case face_dir::PLUSY:
					p0 += glm::vec3( 5.0f * ONE16TH, 0,               -4.0f * ONE16TH);
					p1 += glm::vec3(-5.0f * ONE16TH, 0,               -4.0f * ONE16TH);
					p2 += glm::vec3( 5.0f * ONE16TH, 0,                4.0f * ONE16TH);
					p3 += glm::vec3(-5.0f * ONE16TH, 0,                4.0f * ONE16TH);

					p4 += glm::vec3( 5.0f * ONE16TH, -13.0f * ONE16TH, -4.0f * ONE16TH);
					p5 += glm::vec3(-5.0f * ONE16TH, -13.0f * ONE16TH, -4.0f * ONE16TH);
					p6 += glm::vec3( 5.0f * ONE16TH, -13.0f * ONE16TH,  4.0f * ONE16TH);
					p7 += glm::vec3(-5.0f * ONE16TH, -13.0f * ONE16TH,  4.0f * ONE16TH);

					x_texMX = 0; x_texPX = 13; x_texMY = 4; x_texPY = 4;
					y_texMX = 5; y_texPX = 5;  y_texMY = 4; y_texPY = 4;
					z_texMX = 5; z_texPX = 5;  z_texMY = 0;  z_texPY = 13;
					break ;
			}
			break ;
		case PLACEMENT::FLOOR:
		case PLACEMENT::CEILING:
			ztop = (placement == PLACEMENT::FLOOR) ? 13 : 0;
			zbot = (placement == PLACEMENT::FLOOR) ? 0 : 13;
			switch (orientation) {
				case face_dir::MINUSX:
				case face_dir::PLUSX:
					p0 += glm::vec3( 4.0f * ONE16TH, 5.0f * ONE16TH, -ztop * ONE16TH);
					p1 += glm::vec3(-4.0f * ONE16TH, 5.0f * ONE16TH, -ztop * ONE16TH);
					p2 += glm::vec3( 4.0f * ONE16TH, 5.0f * ONE16TH,  zbot * ONE16TH);
					p3 += glm::vec3(-4.0f * ONE16TH, 5.0f * ONE16TH,  zbot * ONE16TH);

					p4 += glm::vec3( 4.0f * ONE16TH,-5.0f * ONE16TH, -ztop * ONE16TH);
					p5 += glm::vec3(-4.0f * ONE16TH,-5.0f * ONE16TH, -ztop * ONE16TH);
					p6 += glm::vec3( 4.0f * ONE16TH,-5.0f * ONE16TH,  zbot * ONE16TH);
					p7 += glm::vec3(-4.0f * ONE16TH,-5.0f * ONE16TH,  zbot * ONE16TH);

					x_texMX = 5; x_texPX = 5; x_texMY = ztop; x_texPY = zbot;
					y_texMX = 4; y_texPX = 4; y_texMY = ztop; y_texPY = zbot;
					z_texMX = 4; z_texPX = 4; z_texMY = 5;  z_texPY = 5;
					break ;
				case face_dir::MINUSY:
				case face_dir::PLUSY:
					p0 += glm::vec3( 5.0f * ONE16TH, 4.0f * ONE16TH, -ztop * ONE16TH);
					p1 += glm::vec3(-5.0f * ONE16TH, 4.0f * ONE16TH, -ztop * ONE16TH);
					p2 += glm::vec3( 5.0f * ONE16TH, 4.0f * ONE16TH,  zbot * ONE16TH);
					p3 += glm::vec3(-5.0f * ONE16TH, 4.0f * ONE16TH,  zbot * ONE16TH);

					p4 += glm::vec3( 5.0f * ONE16TH,-4.0f * ONE16TH, -ztop * ONE16TH);
					p5 += glm::vec3(-5.0f * ONE16TH,-4.0f * ONE16TH, -ztop * ONE16TH);
					p6 += glm::vec3( 5.0f * ONE16TH,-4.0f * ONE16TH,  zbot * ONE16TH);
					p7 += glm::vec3(-5.0f * ONE16TH,-4.0f * ONE16TH,  zbot * ONE16TH);

					x_texMX = 4; x_texPX = 4; x_texMY = ztop; x_texPY = zbot;
					y_texMX = 5; y_texPX = 5; y_texMY = ztop; y_texPY = zbot;
					z_texMX = 4; z_texPX = 4; z_texMY = 5;  z_texPY = 5;
					break ;
			}
			break ;
	}
	// drawing lever's base
	spec = (s_blocks[blocks::COBBLESTONE]->texX() << 4) + (s_blocks[blocks::COBBLESTONE]->texY() << 12) + (0 << 19);
	int faceLight = chunk->computeLight(pos.x, pos.y, pos.z);
	spec += (faceLight << 24);
	if (placement != PLACEMENT::WALL || orientation != face_dir::PLUSX) {
		v0 = {spec + x_texMX + (x_texMY << 8), p4};
		v1 = {spec - x_texPX + XTEX + (x_texMY << 8), p0};
		v2 = {spec + x_texMX - (x_texPY << 8) + YTEX, p6};
		v3 = {spec - x_texPX + XTEX - (x_texPY << 8) + YTEX, p2};
		face_vertices(vertices, v0, v1, v2, v3); // -x
	}
	if (placement != PLACEMENT::WALL || orientation != face_dir::MINUSX) {
		v0 = {spec + x_texMX + (x_texMY << 8), p1};
		v1 = {spec - x_texPX + XTEX + (x_texMY << 8), p5};
		v2 = {spec + x_texMX - (x_texPY << 8) + YTEX, p3};
		v3 = {spec - x_texPX + XTEX - (x_texPY << 8) + YTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3); // +x
	}
	if (placement != PLACEMENT::WALL || orientation != face_dir::PLUSY) {
		v0 = {spec + y_texMX + (y_texMY << 8), p0};
		v1 = {spec - y_texPX + XTEX + (y_texMY << 8), p1};
		v2 = {spec + y_texMX - (y_texPY << 8) + YTEX, p2};
		v3 = {spec - y_texPX + XTEX - (y_texPY << 8) + YTEX, p3};
		face_vertices(vertices, v0, v1, v2, v3); // -y
	}
	if (placement != PLACEMENT::WALL || orientation != face_dir::MINUSY) {
		v0 = {spec + y_texMX + (y_texMY << 8), p5};
		v1 = {spec - y_texPX + XTEX + (y_texMY << 8), p4};
		v2 = {spec + y_texMX - (y_texPY << 8) + YTEX, p7};
		v3 = {spec - y_texPX + XTEX - (y_texPY << 8) + YTEX, p6};
		face_vertices(vertices, v0, v1, v2, v3); // +y
	}
	if (placement != PLACEMENT::CEILING) {
		v0 = {spec + z_texMX + (z_texMY << 8), p4};
		v1 = {spec - z_texPX + (z_texMY << 8) + XTEX, p5};
		v2 = {spec + z_texMX - (z_texPY << 8) + YTEX, p0};
		v3 = {spec - z_texPX - (z_texPY << 8) + YTEX + XTEX, p1};
		face_vertices(vertices, v0, v1, v2, v3); // +z
	}
	if (placement != PLACEMENT::FLOOR) {
		v0 = {spec + z_texMX + (z_texMY << 8), p2};
		v1 = {spec - z_texPX + (z_texMY << 8) + XTEX, p3};
		v2 = {spec + z_texMX - (z_texPY << 8) + YTEX, p6};
		v3 = {spec - z_texPX - (z_texPY << 8) + YTEX + XTEX, p7};
		face_vertices(vertices, v0, v1, v2, v3); // -z
	}
	// it's lever time
	glm::vec3 top, right, front;
	bool powered = (value >> REDSTONE::POWERED_OFFSET) & 0x1;
	switch (placement) {
	case PLACEMENT::WALL:
		switch (orientation) {
			case face_dir::MINUSX:
				top = glm::vec3(-1, 0, (powered) ? -1 : 1);
				break ;
			case face_dir::PLUSX:
				top = glm::vec3( 1, 0, (powered) ? -1 : 1);
				break ;
			case face_dir::MINUSY:
				top = glm::vec3(0, -1, (powered) ? -1 : 1);
				break ;
			case face_dir::PLUSY:
				top = glm::vec3(0,  1, (powered) ? -1 : 1);
				break ;
		}
		break ;
	case PLACEMENT::FLOOR:
	case PLACEMENT::CEILING:
		switch (orientation) {
			case face_dir::MINUSX:
				top = glm::vec3((powered) ? -1 : 1, 0, (placement == PLACEMENT::FLOOR) ? 1 : -1);
				break ;
			case face_dir::PLUSX:
				top = glm::vec3((powered) ? 1 : -1, 0, (placement == PLACEMENT::FLOOR) ? 1 : -1);
				break ;
			case face_dir::MINUSY:
				top = glm::vec3(0, (powered) ? -1 : 1, (placement == PLACEMENT::FLOOR) ? 1 : -1);
				break ;
			case face_dir::PLUSY:
				top = glm::vec3(0, (powered) ? 1 : -1, (placement == PLACEMENT::FLOOR) ? 1 : -1);
				break ;
		}
		break ;
	}
	top   = glm::normalize(top);
	right = glm::normalize(glm::cross(top, glm::vec3(0, 0, 1)));
	front = glm::normalize(glm::cross(top, right));
	glm::vec3 baseCenter = (p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7) / 8.0f;

	p0 = baseCenter - right * ONE16TH - front * ONE16TH + top * 10.0f * ONE16TH;
	p1 = baseCenter + right * ONE16TH - front * ONE16TH + top * 10.0f * ONE16TH;
	p2 = baseCenter - right * ONE16TH - front * ONE16TH;
	p3 = baseCenter + right * ONE16TH - front * ONE16TH;

	p4 = baseCenter - right * ONE16TH + front * ONE16TH + top * 10.0f * ONE16TH;
	p5 = baseCenter + right * ONE16TH + front * ONE16TH + top * 10.0f * ONE16TH;
	p6 = baseCenter - right * ONE16TH + front * ONE16TH;
	p7 = baseCenter + right * ONE16TH + front * ONE16TH;

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
