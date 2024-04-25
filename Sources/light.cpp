#include "Chunk.hpp"
#include "utils.h"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// if skySpread, _lights[offset] is bitshifted by 4 to work on sky light and not block light
// posX, posY are in [0; CHUNK_SIZE[
void Chunk::light_spread( int posX, int posY, int posZ, bool skySpread, int recurse )
{
	if (!recurse) {
		return ;
	}
	// if (skySpread && _startX == 256 && _startY == 320) {
	// 	static int cnt;
	// 	std::cout << _startX << ", " << _startY << " at " << posX << ", " << posY << ", " << posZ << " light_spread " << ((skySpread) ? "SKY" : "BLOCK") << " recurse level " << ++cnt << std::endl;
	// }
	int shift = 8 * skySpread;
	int offset = (((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ;
	short saveLight = _lights[offset];
	short level = ((saveLight >> shift) & 0xFF);
	// std::cout << "light_spread, level is " << (int)level << std::endl;
	// TODO once diff light sources exist with diff light_level, rework this condition
	if (!(level & 0xF0)) { // not a source block, we check if level should change
		short maxLevel = 0;
		for (int index = 0; index < 6; index++) {
			const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
			if (posZ + delta[2] < 0 || posZ + delta[2] > 255) {
			} else {
				short adj = getLightLevel(posX + delta[0], posY + delta[1], posZ + delta[2]) >> shift;
				maxLevel = maxs(maxLevel, adj & 0xF);
				if (skySpread && index == face_dir::PLUSZ && (adj & 0xF0)) {
					maxLevel = (adj & 0xFF) + 1; // if sky light above is source, own sky light becomes source too
				}
			}
		}
		if ((maxLevel && maxLevel - 1 != level) || (!maxLevel && level)) {
			level = maxs(0, maxLevel - 1);
			// if (level < ((saveLight >> shift) & 0xFF)) { // if light gets dimmer, we accelerate process and put it to 0, it will spread to a nearby light source which will spread again
			// 	level = 0;
			// }
			_lights[offset] = (saveLight & (0xFF << (8 - shift))) + (level << shift);
			_light_update = true;
		} else {
			return ; // stop spread when not source and level is unchanged
		}
	} else if (skySpread && posZ + 1 < WORLD_HEIGHT) { // skySpread + source block
		short above = _lights[offset + 1] >> 8;
		if (!(above & 0xF0)) { // if block above is not a source anymore
			_lights[offset] &= 0xFF;
			return (light_spread(posX, posY, posZ, skySpread, LIGHT_RECURSE));
		}
	}
	// std::cout << "level is " << (int)level << std::endl;

	// spread in all 6 directions
	for (int index = 0; index < 6; index++) {
		const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
		if (skySpread && index == face_dir::MINUSZ && level == 0xFF) {
			light_try_spread(posX + delta[0], posY + delta[1], posZ + delta[2], 0xF0, skySpread, recurse - 1);
		} else {
			light_try_spread(posX + delta[0], posY + delta[1], posZ + delta[2], level, skySpread, recurse - 1);
		}
	}
}

void Chunk::generate_lights( void )
{
	// fill sky_light with 0 for underground and 15 (- leaves and water damper) for rest
	// voxel is considered underground if there is at least 1 solid block above it
	// this also fills block_light with 0
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			char light_level = 15;
			for (int level = WORLD_HEIGHT - 1; level >= 0; level--) {
				if (light_level) {
					int type = _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] & 0xFF;
					if (!s_blocks[type]->transparent) { // block hit
						light_level = 0;
					} else if (type == blocks::OAK_LEAVES || type >= blocks::WATER) {
						--light_level;
					}
					if (light_level == 15) {
						_lights[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = (light_level + (light_level << 4)) << 8; // we consider blocks directly under sky as light source
					} else {
						_lights[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = light_level << 8;
					}
				} else {
					_lights[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = 0;
				}
			}
		}
	}
}

// uses sky light and block light to output a shade value 
// -> obsolete, now compute on shader
// now returns 0xF0 skylight + 0x0F blocklight
// row and col are in [-1:CHUNK_SIZE]
int Chunk::computeLight( int row, int col, int level )
{
	// (void)row;(void)col;(void)level;return (0);
	short light = getLightLevel(row, col, level);
	// int blockLightAmplitude = 5; // amount by which light decreases on each block
	int blockLight = light & 0xF;
	int skyLight = (light >> 8) & 0xF;
	return (blockLight + (skyLight << 4));
}

// same as computeLight, but we handle a corner this time
// result is max of 4 light levels surrounding corner
// used to have a smooth lighting from one block to another
int Chunk::computeSmoothLight( int faceLight, int row, int col, int level, std::array<int, 9> offsets )
{
	#if 0
	(void)row;(void)col;(void)level;(void)offsets;return(faceLight);
	#endif
	short light = getLightLevel(row + offsets[0], col + offsets[1], level + offsets[2]);
	faceLight = maxs(faceLight & 0xF, light & 0xF) + (maxs((faceLight >> 4) & 0xF, (light >> 8) & 0xF) << 4);
	light = getLightLevel(row + offsets[3], col + offsets[4], level + offsets[5]);
	faceLight = maxs(faceLight & 0xF, light & 0xF) + (maxs((faceLight >> 4) & 0xF, (light >> 8) & 0xF) << 4);
	light = getLightLevel(row + offsets[6], col + offsets[7], level + offsets[8]);
	faceLight = maxs(faceLight & 0xF, light & 0xF) + (maxs((faceLight >> 4) & 0xF, (light >> 8) & 0xF) << 4);
	return (faceLight);
}

// https://minecraft.fandom.com/wiki/Light#AO_texture_pattern fct returns (side1 + side2 + corner)
int Chunk::computeShade( int row, int col, int level, std::array<int, 9> offsets )
{
	// (void)row;(void)col;(void)level;(void)offsets;return (0);
	return (!!air_flower(getBlockAt(row + offsets[0], col + offsets[1], level + offsets[2], true), true, true, false)
			+ !!air_flower(getBlockAt(row + offsets[3], col + offsets[4], level + offsets[5], true), true, true, false)
			+ !!air_flower(getBlockAt(row + offsets[6], col + offsets[7], level + offsets[8], true), true, true, false));
}

void Chunk::fill_vertex_array( void )
{
	// std::cout << "filling " << _startX << ", " << _startY << "; expecting " << _displayed_faces << std::endl;
	_mtx.lock();
	_vertices.clear();
	int spec, faceLight, cornerLight, shade;
	glm::vec3 p0, p1, p2, p3, p4, p5, p6, p7;
	t_shaderInput v0, v1, v2, v3;
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				GLint block_value = _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level], type = block_value & 0xFF;
				if (block_value & blocks::NOTVISIBLE || type == blocks::CHEST) // chests are drawn as entities
					continue;
				if (type >= blocks::WHEAT_CROP && type <= blocks::WHEAT_CROP7) {
					p0 = {_startX + row + 0, _startY + col + 0, level + FIFTEEN_SIXTEENTH};
					p1 = {_startX + row + 1, _startY + col + 0, level + FIFTEEN_SIXTEENTH};
					p2 = {_startX + row + 0, _startY + col + 0, level - ONE_SIXTEENTH};
					p3 = {_startX + row + 1, _startY + col + 0, level - ONE_SIXTEENTH};

					p4 = {_startX + row + 0, _startY + col + 1, level + FIFTEEN_SIXTEENTH};
					p5 = {_startX + row + 1, _startY + col + 1, level + FIFTEEN_SIXTEENTH};
					p6 = {_startX + row + 0, _startY + col + 1, level - ONE_SIXTEENTH};
					p7 = {_startX + row + 1, _startY + col + 1, level - ONE_SIXTEENTH};

					spec = s_blocks[blocks::WHEAT_CROP]->texX() + (s_blocks[blocks::WHEAT_CROP]->texY(face_dir::MINUSX, block_value - blocks::WHEAT_CROP) << 4) + (0 << 19) + (computeLight(row, col, level) << 24);
					v0 = {spec, {p4.x + THREE_SIXTEENTH, p4.y, p4.z}};
					v1 = {spec + XTEX, {p0.x + THREE_SIXTEENTH, p0.y, p0.z}};
					v2 = {spec + YTEX, {p6.x + THREE_SIXTEENTH, p6.y, p6.z}};
					v3 = {spec + XTEX + YTEX, {p2.x + THREE_SIXTEENTH, p2.y, p2.z}};
					face_vertices(_vertices, v0, v1, v2, v3); // -x
					v0 = {spec + XTEX, {p0.x + THIRTEEN_SIXTEENTH, p0.y, p0.z}};
					v1 = {spec, {p4.x + THIRTEEN_SIXTEENTH, p4.y, p4.z}};
					v2 = {spec + XTEX + YTEX, {p2.x + THIRTEEN_SIXTEENTH, p2.y, p2.z}};
					v3 = {spec + YTEX, {p6.x + THIRTEEN_SIXTEENTH, p6.y, p6.z}};
					face_vertices(_vertices, v0, v1, v2, v3); // +x
					v0 = {spec, {p0.x, p0.y + THREE_SIXTEENTH, p0.z}};
					v1 = {spec + XTEX, {p1.x, p1.y + THREE_SIXTEENTH, p1.z}};
					v2 = {spec + YTEX, {p2.x, p2.y + THREE_SIXTEENTH, p2.z}};
					v3 = {spec + XTEX + YTEX, {p3.x, p3.y + THREE_SIXTEENTH, p3.z}};
					face_vertices(_vertices, v0, v1, v2, v3); // -y
					v0 = {spec + XTEX, {p1.x, p1.y + THIRTEEN_SIXTEENTH, p1.z}};
					v1 = {spec, {p0.x, p0.y + THIRTEEN_SIXTEENTH, p0.z}};
					v2 = {spec + XTEX + YTEX, {p3.x, p3.y + THIRTEEN_SIXTEENTH, p3.z}};
					v3 = {spec + YTEX, {p2.x, p2.y + THIRTEEN_SIXTEENTH, p2.z}};
					face_vertices(_vertices, v0, v1, v2, v3); // +y



				} else if (type != blocks::AIR && type < blocks::WATER && type != blocks::GLASS) {
					float zSize = ((type == blocks::OAK_SLAB_BOTTOM) ? 0.5f : ((type == blocks::FARMLAND) ? FIFTEEN_SIXTEENTH: 1.0f));
					float bSize = ((type == blocks::OAK_SLAB_TOP) ? 0.5f : 0.0f);
					p0 = {_startX + row + 0, _startY + col + 0, level + zSize};
					p1 = {_startX + row + 1, _startY + col + 0, level + zSize};
					p2 = {_startX + row + 0, _startY + col + 0, level + bSize};
					p3 = {_startX + row + 1, _startY + col + 0, level + bSize};

					p4 = {_startX + row + 0, _startY + col + 1, level + zSize};
					p5 = {_startX + row + 1, _startY + col + 1, level + zSize};
					p6 = {_startX + row + 0, _startY + col + 1, level + bSize};
					p7 = {_startX + row + 1, _startY + col + 1, level + bSize};



					if (type == blocks::OAK_SLAB_BOTTOM || type == blocks::OAK_SLAB_TOP) {
						int baseSpec = s_blocks[type]->texX() + (s_blocks[type]->texY() << 4);
						if (visible_face(type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
							spec = baseSpec + (3 << 19);
							faceLight = computeLight(row - 1, col, level);
							shade = 0;//computeShade(row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
							spec += (faceLight << 24);
							v0 = {spec + (8 << 8) + (shade << 22), p4};
							v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p0};
							v2 = {spec + (shade << 22) + YTEX, p6};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
							spec = baseSpec + (4 << 19);
							faceLight = computeLight(row + 1, col, level);
							shade = 0;//computeShade(row + 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
							spec += (faceLight << 24);
							v0 = {spec + (8 << 8) + (shade << 22), p1};
							v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5};
							v2 = {spec + (shade << 22) + YTEX, p3};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
							spec = baseSpec + (1 << 19);
							faceLight = computeLight(row, col - 1, level);
							shade = 0;//computeShade(row, col - 1, level, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
							spec += (faceLight << 24);
							v0 = {spec + (8 << 8) + (shade << 22), p0};
							v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p1};
							v2 = {spec + (shade << 22) + YTEX, p2};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
							spec = baseSpec + (2 << 19);
							faceLight = computeLight(row, col + 1, level);
							shade = 0;//computeShade(row, col + 1, level, {1, 0, 0, 1, 0, 1, 0, 0, 1});
							spec += (faceLight << 24);
							v0 = {spec + (8 << 8) + (shade << 22), p5};
							v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p4};
							v2 = {spec + (shade << 22) + YTEX, p7};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
							spec = baseSpec + (0 << 19);
							faceLight = computeLight(row, col, level + 1);
							shade = 0;//computeShade(row, col, level + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
							spec += (faceLight << 24);
							// if (shade & 0xFF)std::cout << "shade is " << shade << std::endl;
							// if (shade & 0xFFFFFF00)std::cout << "problem" << std::endl;
							v0 = {spec + (shade << 22), p4};
							v1 = {spec + (shade << 22) + XTEX, p5};
							v2 = {spec + (shade << 22) + YTEX, p0};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p1};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
							spec = baseSpec + (5 << 19);
							faceLight = computeLight(row, col, level - 1);
							shade = 0;//computeShade(row, col, level - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22), p2};
							v1 = {spec + (shade << 22) + XTEX, p3};
							v2 = {spec + (shade << 22) + YTEX, p6};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
							face_vertices(_vertices, v0, v1, v2, v3);
						}



					} else if (type == blocks::OAK_STAIRS_BOTTOM) {
						int corners = (block_value >> 12) & 0xF;
						if (visible_face(type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
							spec = s_blocks[type]->texX(face_dir::MINUSX, face_dir::PLUSX) + (s_blocks[type]->texY(face_dir::MINUSX, face_dir::PLUSX) << 4) + (3 << 19);
							faceLight = computeLight(row - 1, col, level);
							shade = 0;
							spec += (faceLight << 24);
							switch (corners & (CORNERS::MM | CORNERS::MP)) {
								case CORNERS::MM:
									spec += 1;
									v0 = {spec + (shade << 22) + XTEX, p4};
									v1 = {spec + (shade << 22), p0};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p6};
									v3 = {spec + (shade << 22) + YTEX, p2};
									break ;
								case CORNERS::MP:
									spec += 1;
								case CORNERS::MM | CORNERS::MP:
									v0 = {spec + (shade << 22), p4};
									v1 = {spec + (shade << 22) + XTEX, p0};
									v2 = {spec + (shade << 22) + YTEX, p6};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
									break ;
								default:
									v0 = {spec + (8 << 8) + (shade << 22), p4 - glm::vec3(0, 0, 0.5f)};
									v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p0 - glm::vec3(0, 0, 0.5f)};
									v2 = {spec + (shade << 22) + YTEX, p6};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
									break ;
							}
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
							spec = s_blocks[type]->texX(face_dir::PLUSX, face_dir::MINUSX) + (s_blocks[type]->texY(face_dir::PLUSX, face_dir::MINUSX) << 4) + (4 << 19);
							faceLight = computeLight(row + 1, col, level);
							shade = 0;
							spec += (faceLight << 24);
							switch (corners & (CORNERS::PM | CORNERS::PP)) {
								case CORNERS::PP:
									spec += 1;
									v0 = {spec + (shade << 22) + XTEX, p1};
									v1 = {spec + (shade << 22), p5};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p3};
									v3 = {spec + (shade << 22) + YTEX, p7};
									break ;
								case CORNERS::PM:
									spec += 1;
								case CORNERS::PM | CORNERS::PP:
									v0 = {spec + (shade << 22), p1};
									v1 = {spec + (shade << 22) + XTEX, p5};
									v2 = {spec + (shade << 22) + YTEX, p3};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
									break ;
								default:
									v0 = {spec + (8 << 8) + (shade << 22), p1 - glm::vec3(0, 0, 0.5f)};
									v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0, 0.5f)};
									v2 = {spec + (shade << 22) + YTEX, p3};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
									break ;
							}
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
							spec = s_blocks[type]->texX(face_dir::MINUSY, face_dir::PLUSY) + (s_blocks[type]->texY(face_dir::MINUSY, face_dir::PLUSY) << 4) + (1 << 19);
							faceLight = computeLight(row, col - 1, level);
							shade = 0;
							spec += (faceLight << 24);
							switch (corners & (CORNERS::MM | CORNERS::PM)) {
								case CORNERS::PM:
									spec += 1;
									v0 = {spec + (shade << 22) + XTEX, p0};
									v1 = {spec + (shade << 22), p1};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p2};
									v3 = {spec + (shade << 22) + YTEX, p3};
									break ;
								case CORNERS::MM:
									spec += 1;
								case CORNERS::MM | CORNERS::PM:
									v0 = {spec + (shade << 22), p0};
									v1 = {spec + (shade << 22) + XTEX, p1};
									v2 = {spec + (shade << 22) + YTEX, p2};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
									break ;
								default:
									v0 = {spec + (8 << 8) + (shade << 22), p0 - glm::vec3(0, 0, 0.5f)};
									v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p1 - glm::vec3(0, 0, 0.5f)};
									v2 = {spec + (shade << 22) + YTEX, p2};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
									break ;
							}
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
							spec = s_blocks[type]->texX(face_dir::PLUSY, face_dir::MINUSY) + (s_blocks[type]->texY(face_dir::PLUSY, face_dir::MINUSY) << 4) + (2 << 19);
							faceLight = computeLight(row, col + 1, level);
							shade = 0;
							spec += (faceLight << 24);
							switch (corners & (CORNERS::MP | CORNERS::PP)) {
								case CORNERS::MP:
									spec += 1;
									v0 = {spec + (shade << 22) + XTEX, p5};
									v1 = {spec + (shade << 22), p4};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p7};
									v3 = {spec + (shade << 22) + YTEX, p6};
									break ;
								case CORNERS::PP:
									spec += 1;
								case CORNERS::MP | CORNERS::PP:
									v0 = {spec + (shade << 22), p5};
									v1 = {spec + (shade << 22) + XTEX, p4};
									v2 = {spec + (shade << 22) + YTEX, p7};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
									break ;
								default:
									v0 = {spec + (8 << 8) + (shade << 22), p5 - glm::vec3(0, 0, 0.5f)};
									v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 - glm::vec3(0, 0, 0.5f)};
									v2 = {spec + (shade << 22) + YTEX, p7};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
									break ;
							}
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
							spec = s_blocks[type]->texX(face_dir::PLUSZ, face_dir::PLUSZ) + (s_blocks[type]->texY(face_dir::PLUSZ, face_dir::PLUSZ) << 4) + (0 << 19);
							faceLight = computeLight(row, col, level + 1);
							shade = 0;
							spec += (faceLight << 24);
							switch (corners) { // 4 MP 5 PP 0 MM 1 PM
								case CORNERS::MP:
									--spec;
								case CORNERS::MP | CORNERS::PP:
									v0 = {spec + (8 << 8) + (shade << 22), p4};
									v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5};
									v2 = {spec + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, 0)};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, 0)};
									break ;
								case CORNERS::MM:
									--spec;
								case CORNERS::MM | CORNERS::PM:
									v0 = {spec + (8 << 8) + (shade << 22), p4 - glm::vec3(0, 0.5f, 0)};
									v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.5f, 0)};
									v2 = {spec + (shade << 22) + YTEX, p0};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p1};
									break ;
								case CORNERS::PP | CORNERS::MM | CORNERS::PM:
									spec += 3;
								case CORNERS::PM | CORNERS::PP:
									--spec;
								case CORNERS::MP | CORNERS::PP | CORNERS::PM:
									--spec;
									v0 = {spec + (shade << 22) + XTEX, p4};
									v1 = {spec + (shade << 22), p5};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p0};
									v3 = {spec + (shade << 22) + YTEX, p1};
									break ;
								case CORNERS::MP | CORNERS::MM | CORNERS::PM:
									spec += 3;
								case CORNERS::MM | CORNERS::MP:
									--spec;
								case CORNERS::MP | CORNERS::PP | CORNERS::MM:
									--spec;
									v0 = {spec + (shade << 22), p4};
									v1 = {spec + (shade << 22) + XTEX, p5};
									v2 = {spec + (shade << 22) + YTEX, p0};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p1};
									break ;
								case CORNERS::PP:
									spec -= 1;
									v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p4};
									v1 = {spec + (8 << 8) + (shade << 22), p5};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0, 0.5f, 0)};
									v3 = {spec + (shade << 22) + YTEX, p1 + glm::vec3(0, 0.5f, 0)};
									break ;
								case CORNERS::PM:
									spec -= 1;
									v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 - glm::vec3(0, 0.5f, 0)};
									v1 = {spec + (8 << 8) + (shade << 22), p5 - glm::vec3(0, 0.5f, 0)};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p0};
									v3 = {spec + (shade << 22) + YTEX, p1};
									break ;
							}
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						// up of first step
						spec = s_blocks[type]->texX(face_dir::PLUSZ, face_dir::PLUSZ) + (s_blocks[type]->texY(face_dir::PLUSZ, face_dir::PLUSZ) << 4) + (0 << 19);
						faceLight = computeLight(row, col, level + 1); // TODO rework light on stairs and use row col level here
						shade = 0;
						spec += (faceLight << 24);
						switch (corners) { // 4 MP 5 PP 0 MM 1 PM
							case CORNERS::PP | CORNERS::MM | CORNERS::PM:
								--spec;
							case CORNERS::MM | CORNERS::PM:
								v0 = {spec + (8 << 8) + (shade << 22), p4 - glm::vec3(0, 0, 0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0, 0.5f)};
								v2 = {spec + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
								break ;
							case CORNERS::MP | CORNERS::PP | CORNERS::PM:
								--spec;
							case CORNERS::MP | CORNERS::PP:
								v0 = {spec + (8 << 8) + (shade << 22), p4 - glm::vec3(0, 0.5f, 0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.5f, 0.5f)};
								v2 = {spec + (shade << 22) + YTEX, p0 - glm::vec3(0, 0, 0.5f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p1 - glm::vec3(0, 0, 0.5f)};
								break ;
							case CORNERS::MP:
								spec += 3;
							case CORNERS::MM | CORNERS::MP:
								--spec;
							case CORNERS::MM:
								--spec;
								v0 = {spec + (shade << 22) + XTEX, p4 - glm::vec3(0, 0, 0.5f)};
								v1 = {spec + (shade << 22), p5 - glm::vec3(0, 0, 0.5f)};
								v2 = {spec + (shade << 22) + XTEX + YTEX, p0 - glm::vec3(0, 0, 0.5f)};
								v3 = {spec + (shade << 22) + YTEX, p1 - glm::vec3(0, 0, 0.5f)};
								break ;
							case CORNERS::PP:
								spec += 3;
							case CORNERS::PM | CORNERS::PP:
								--spec;
							case CORNERS::PM:
								--spec;
								v0 = {spec + (shade << 22), p4 - glm::vec3(0, 0, 0.5f)};
								v1 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0, 0, 0.5f)};
								v2 = {spec + (shade << 22) + YTEX, p0 - glm::vec3(0, 0, 0.5f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p1 - glm::vec3(0, 0, 0.5f)};
								break ;
							case CORNERS::MP | CORNERS::MM | CORNERS::PM:
								spec -= 1;
								v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 - glm::vec3(0, 0, 0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22), p5 - glm::vec3(0, 0, 0.5f)};
								v2 = {spec + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
								v3 = {spec + (shade << 22) + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
								break ;
							case CORNERS::MP | CORNERS::PP | CORNERS::MM:
								spec -= 1;
								v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 - glm::vec3(0, 0.5f, 0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22), p5 - glm::vec3(0, 0.5f, 0.5f)};
								v2 = {spec + (shade << 22) + XTEX + YTEX, p0 - glm::vec3(0, 0, 0.5f)};
								v3 = {spec + (shade << 22) + YTEX, p1 - glm::vec3(0, 0, 0.5f)};
								break ;
						}
						face_vertices(_vertices, v0, v1, v2, v3);
						spec = s_blocks[type]->texX(face_dir::PLUSZ, face_dir::PLUSZ) + (s_blocks[type]->texY(face_dir::PLUSZ, face_dir::PLUSZ) << 4);
						faceLight = computeLight(row, col, level + 1);
						spec += (faceLight << 24);
						// front MINUSX of second step
						spec += (3 << 19);
						switch (corners) {
							case CORNERS::PM | CORNERS::PP: // full width
								v0 = {spec + (8 << 8) + (shade << 22), p1 - glm::vec3(0.5f, 0, 0)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0.5f, 0, 0)};
								v2 = {spec + (shade << 22) + YTEX, p1 - glm::vec3(0.5f, 0, 0.5f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p5 - glm::vec3(0.5f, 0, 0.5f)};
								break ;
							case CORNERS::PM: // only 'right'
							case CORNERS::PM | CORNERS::PP | CORNERS::MP:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22), p1 - glm::vec3(0.5f, 0, 0)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0.5f, 0, 0)};
								v2 = {spec + (shade << 22) + YTEX, p1 - glm::vec3(0.5f, 0, 0.5f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p5 - glm::vec3(0.5f, 0, 0.5f)};
								++spec;
								break ;
							case CORNERS::PP: // only 'left'
							case CORNERS::PM | CORNERS::PP | CORNERS::MM:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p1 - glm::vec3(0.5f, 0, 0)};
								v1 = {spec + (8 << 8) + (shade << 22), p5 - glm::vec3(0.5f, 0, 0)};
								v2 = {spec + (shade << 22) + XTEX + YTEX, p1 - glm::vec3(0.5f, 0, 0.5f)};
								v3 = {spec + (shade << 22) + YTEX, p5 - glm::vec3(0.5f, 0, 0.5f)};
								++spec;
								break ;
						}
						face_vertices(_vertices, v0, v1, v2, v3);
						// front PLUSX of second step
						spec += (1 << 19);
						switch (corners) {
							case CORNERS::MM | CORNERS::MP: // full width
								v0 = {spec + (8 << 8) + (shade << 22), p4 + glm::vec3(0.5f, 0, 0)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0, 0)};
								v2 = {spec + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, 0, -0.5f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
								break ;
							case CORNERS::MP: // only 'right'
							case CORNERS::MM | CORNERS::MP | CORNERS::PM:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22), p4 + glm::vec3(0.5f, 0, 0)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0, 0)};
								v2 = {spec + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, 0, -0.5f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
								++spec;
								break ;
							case CORNERS::MM: // only 'left'
							case CORNERS::MM | CORNERS::MP | CORNERS::PP:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 + glm::vec3(0.5f, 0, 0)};
								v1 = {spec + (8 << 8) + (shade << 22), p0 + glm::vec3(0.5f, 0, 0)};
								v2 = {spec + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0.5f, 0, -0.5f)};
								v3 = {spec + (shade << 22) + YTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
								++spec;
								break ;
						}
						face_vertices(_vertices, v0, v1, v2, v3);
						// front MINUSY of second step
						spec -= (3 << 19);
						switch (corners) {
							case CORNERS::MP | CORNERS::PP: // full width
								v0 = {spec + (8 << 8) + (shade << 22), p5 - glm::vec3(0, 0.5f, 0)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 - glm::vec3(0, 0.5f, 0)};
								v2 = {spec + (shade << 22) + YTEX, p5 - glm::vec3(0, 0.5f, 0.5f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p4 - glm::vec3(0, 0.5f, 0.5f)};
								break ;
							case CORNERS::PP: // only 'right'
							case CORNERS::MP | CORNERS::PP | CORNERS::MM:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22), p5 - glm::vec3(0, 0.5f, 0)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 - glm::vec3(0, 0.5f, 0)};
								v2 = {spec + (shade << 22) + YTEX, p5 - glm::vec3(0, 0.5f, 0.5f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p4 - glm::vec3(0, 0.5f, 0.5f)};
								++spec;
								break ;
							case CORNERS::MP: // only 'left'
							case CORNERS::MP | CORNERS::PP | CORNERS::PM:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.5f, 0)};
								v1 = {spec + (8 << 8) + (shade << 22), p4 - glm::vec3(0, 0.5f, 0)};
								v2 = {spec + (shade << 22) + XTEX + YTEX, p5 - glm::vec3(0, 0.5f, 0.5f)};
								v3 = {spec + (shade << 22) + YTEX, p4 - glm::vec3(0, 0.5f, 0.5f)};
								++spec;
								break ;
						}
						face_vertices(_vertices, v0, v1, v2, v3);
						// front PLUSY of second step
						spec += (1 << 19);
						switch (corners) {
							case CORNERS::MM | CORNERS::PM: // full width
								v0 = {spec + (8 << 8) + (shade << 22), p0 + glm::vec3(0, 0.5f, 0)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.5f, 0)};
								v2 = {spec + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
								break ;
							case CORNERS::MM: // only 'left'
							case CORNERS::MM | CORNERS::PM | CORNERS::PP:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22), p0 + glm::vec3(0, 0.5f, 0)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.5f, 0)};
								v2 = {spec + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
								++spec;
								break ;
							case CORNERS::PM: // only 'right'
							case CORNERS::MM | CORNERS::PM | CORNERS::MP:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p0 + glm::vec3(0, 0.5f, 0)};
								v1 = {spec + (8 << 8) + (shade << 22), p1 + glm::vec3(0, 0.5f, 0)};
								v2 = {spec + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
								v3 = {spec + (shade << 22) + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
								++spec;
								break ;
						}
						face_vertices(_vertices, v0, v1, v2, v3);
						if (visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
							spec = s_blocks[type]->texX(face_dir::MINUSZ, face_dir::MINUSZ) + (s_blocks[type]->texY(face_dir::MINUSZ, face_dir::MINUSZ) << 4) + (5 << 19);
							faceLight = computeLight(row, col, level - 1);
							shade = 0;
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22), p2};
							v1 = {spec + (shade << 22) + XTEX, p3};
							v2 = {spec + (shade << 22) + YTEX, p6};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
							face_vertices(_vertices, v0, v1, v2, v3);
						}



					} else if (type == blocks::OAK_STAIRS_TOP) {
						int corners = (block_value >> 12) & 0xF;
						if (visible_face(type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
							spec = s_blocks[type]->texX(face_dir::MINUSX, face_dir::PLUSX) + (s_blocks[type]->texY(face_dir::MINUSX, face_dir::PLUSX) << 4) + (3 << 19);
							faceLight = computeLight(row - 1, col, level);
							shade = 0;
							spec += (faceLight << 24);
							switch (corners & (CORNERS::MM | CORNERS::MP)) {
								case CORNERS::MM:
									spec -= 1;
									v0 = {spec + (shade << 22) + XTEX, p4};
									v1 = {spec + (shade << 22), p0};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p6};
									v3 = {spec + (shade << 22) + YTEX, p2};
									break ;
								case CORNERS::MP:
									spec -= 1;
								case CORNERS::MM | CORNERS::MP:
									v0 = {spec + (shade << 22), p4};
									v1 = {spec + (shade << 22) + XTEX, p0};
									v2 = {spec + (shade << 22) + YTEX, p6};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
									break ;
								default:
									v0 = {spec + (8 << 8) + (shade << 22), p4};
									v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p0};
									v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0, 0, 0.5f)};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0, 0, 0.5f)};
									break ;
							}
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
							spec = s_blocks[type]->texX(face_dir::PLUSX, face_dir::MINUSX) + (s_blocks[type]->texY(face_dir::PLUSX, face_dir::MINUSX) << 4) + (4 << 19);
							faceLight = computeLight(row + 1, col, level);
							shade = 0;
							spec += (faceLight << 24);
							switch (corners & (CORNERS::PM | CORNERS::PP)) {
								case CORNERS::PP:
									spec -= 1;
									v0 = {spec + (shade << 22) + XTEX, p1};
									v1 = {spec + (shade << 22), p5};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p3};
									v3 = {spec + (shade << 22) + YTEX, p7};
									break ;
								case CORNERS::PM:
									spec -= 1;
								case CORNERS::PM | CORNERS::PP:
									v0 = {spec + (shade << 22), p1};
									v1 = {spec + (shade << 22) + XTEX, p5};
									v2 = {spec + (shade << 22) + YTEX, p3};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
									break ;
								default:
									v0 = {spec + (8 << 8) + (shade << 22), p1};
									v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5};
									v2 = {spec + (shade << 22) + YTEX, p3 + glm::vec3(0, 0, 0.5f)};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, 0, 0.5f)};
									break ;
							}
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
							spec = s_blocks[type]->texX(face_dir::MINUSY, face_dir::PLUSY) + (s_blocks[type]->texY(face_dir::MINUSY, face_dir::PLUSY) << 4) + (1 << 19);
							faceLight = computeLight(row, col - 1, level);
							shade = 0;
							spec += (faceLight << 24);
							switch (corners & (CORNERS::MM | CORNERS::PM)) {
								case CORNERS::PM:
									spec -= 1;
									v0 = {spec + (shade << 22) + XTEX, p0};
									v1 = {spec + (shade << 22), p1};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p2};
									v3 = {spec + (shade << 22) + YTEX, p3};
									break ;
								case CORNERS::MM:
									spec -= 1;
								case CORNERS::MM | CORNERS::PM:
									v0 = {spec + (shade << 22), p0};
									v1 = {spec + (shade << 22) + XTEX, p1};
									v2 = {spec + (shade << 22) + YTEX, p2};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
									break ;
								default:
									v0 = {spec + (8 << 8) + (shade << 22), p0};
									v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p1};
									v2 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0, 0, 0.5f)};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0, 0.5f)};
									break ;
							}
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
							spec = s_blocks[type]->texX(face_dir::PLUSY, face_dir::MINUSY) + (s_blocks[type]->texY(face_dir::PLUSY, face_dir::MINUSY) << 4) + (2 << 19);
							faceLight = computeLight(row, col + 1, level);
							shade = 0;
							spec += (faceLight << 24);
							switch (corners & (CORNERS::MP | CORNERS::PP)) {
								case CORNERS::MP:
									spec -= 1;
									v0 = {spec + (shade << 22) + XTEX, p5};
									v1 = {spec + (shade << 22), p4};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p7};
									v3 = {spec + (shade << 22) + YTEX, p6};
									break ;
								case CORNERS::PP:
									spec -= 1;
								case CORNERS::MP | CORNERS::PP:
									v0 = {spec + (shade << 22), p5};
									v1 = {spec + (shade << 22) + XTEX, p4};
									v2 = {spec + (shade << 22) + YTEX, p7};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
									break ;
								default:
									v0 = {spec + (8 << 8) + (shade << 22), p5};
									v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p4};
									v2 = {spec + (shade << 22) + YTEX, p7 + glm::vec3(0, 0, 0.5f)};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p6 + glm::vec3(0, 0, 0.5f)};
									break ;
							}
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
							spec = s_blocks[type]->texX(face_dir::MINUSZ, face_dir::MINUSZ) + (s_blocks[type]->texY(face_dir::MINUSZ, face_dir::PLUSZ) << 4) + (5 << 19);
							faceLight = computeLight(row, col, level - 1);
							shade = 0;
							spec += (faceLight << 24);
							switch (corners) { // 4 MP 5 PP 0 MM 1 PM
								case CORNERS::MP:
									--spec;
								case CORNERS::MP | CORNERS::PP:
									v0 = {spec + (8 << 8) + (shade << 22), p6};
									v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p7};
									v2 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0, 0.5f, 0)};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0.5f, 0)};
									break ;
								case CORNERS::MM:
									--spec;
								case CORNERS::MM | CORNERS::PM:
									v0 = {spec + (8 << 8) + (shade << 22), p6 - glm::vec3(0, 0.5f, 0)};
									v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p7 - glm::vec3(0, 0.5f, 0)};
									v2 = {spec + (shade << 22) + YTEX, p2};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
									break ;
								case CORNERS::PP | CORNERS::MM | CORNERS::PM:
									spec += 3;
								case CORNERS::PM | CORNERS::PP:
									--spec;
								case CORNERS::MP | CORNERS::PP | CORNERS::PM:
									--spec;
									v0 = {spec + (shade << 22) + XTEX, p6};
									v1 = {spec + (shade << 22), p7};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p2};
									v3 = {spec + (shade << 22) + YTEX, p3};
									break ;
								case CORNERS::MP | CORNERS::MM | CORNERS::PM:
									spec += 3;
								case CORNERS::MM | CORNERS::MP:
									--spec;
								case CORNERS::MP | CORNERS::PP | CORNERS::MM:
									--spec;
									v0 = {spec + (shade << 22), p6};
									v1 = {spec + (shade << 22) + XTEX, p7};
									v2 = {spec + (shade << 22) + YTEX, p2};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
									break ;
								case CORNERS::PP:
									spec -= 1;
									v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p6};
									v1 = {spec + (8 << 8) + (shade << 22), p7};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0, 0.5f, 0)};
									v3 = {spec + (shade << 22) + YTEX, p3 + glm::vec3(0, 0.5f, 0)};
									break ;
								case CORNERS::PM:
									spec -= 1;
									v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p6 - glm::vec3(0, 0.5f, 0)};
									v1 = {spec + (8 << 8) + (shade << 22), p7 - glm::vec3(0, 0.5f, 0)};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p2};
									v3 = {spec + (shade << 22) + YTEX, p3};
									break ;
							}
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						// 'up' of first step
						spec = s_blocks[type]->texX(face_dir::MINUSZ, face_dir::MINUSZ) + (s_blocks[type]->texY(face_dir::MINUSZ, face_dir::MINUSZ) << 4) + (5 << 19);
						faceLight = computeLight(row, col, level - 1); // TODO rework light on stairs and use row col level here
						shade = 0;
						spec += (faceLight << 24);
						switch (corners) { // 4 MP 5 PP 0 MM 1 PM
							case CORNERS::PP | CORNERS::MM | CORNERS::PM:
								--spec;
							case CORNERS::MM | CORNERS::PM:
								v0 = {spec + (8 << 8) + (shade << 22), p4 - glm::vec3(0, 0, 0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0, 0.5f)};
								v2 = {spec + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
								break ;
							case CORNERS::MP | CORNERS::PP | CORNERS::PM:
								--spec;
							case CORNERS::MP | CORNERS::PP:
								v0 = {spec + (8 << 8) + (shade << 22), p4 - glm::vec3(0, 0.5f, 0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.5f, 0.5f)};
								v2 = {spec + (shade << 22) + YTEX, p0 - glm::vec3(0, 0, 0.5f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p1 - glm::vec3(0, 0, 0.5f)};
								break ;
							case CORNERS::MP:
								spec += 3;
							case CORNERS::MM | CORNERS::MP:
								--spec;
							case CORNERS::MM:
								--spec;
								v0 = {spec + (shade << 22) + XTEX, p4 - glm::vec3(0, 0, 0.5f)};
								v1 = {spec + (shade << 22), p5 - glm::vec3(0, 0, 0.5f)};
								v2 = {spec + (shade << 22) + XTEX + YTEX, p0 - glm::vec3(0, 0, 0.5f)};
								v3 = {spec + (shade << 22) + YTEX, p1 - glm::vec3(0, 0, 0.5f)};
								break ;
							case CORNERS::PP:
								spec += 3;
							case CORNERS::PM | CORNERS::PP:
								--spec;
							case CORNERS::PM:
								--spec;
								v0 = {spec + (shade << 22), p4 - glm::vec3(0, 0, 0.5f)};
								v1 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0, 0, 0.5f)};
								v2 = {spec + (shade << 22) + YTEX, p0 - glm::vec3(0, 0, 0.5f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p1 - glm::vec3(0, 0, 0.5f)};
								break ;
							case CORNERS::MP | CORNERS::MM | CORNERS::PM:
								spec -= 1;
								v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 - glm::vec3(0, 0, 0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22), p5 - glm::vec3(0, 0, 0.5f)};
								v2 = {spec + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
								v3 = {spec + (shade << 22) + YTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
								break ;
							case CORNERS::MP | CORNERS::PP | CORNERS::MM:
								spec -= 1;
								v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 - glm::vec3(0, 0.5f, 0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22), p5 - glm::vec3(0, 0.5f, 0.5f)};
								v2 = {spec + (shade << 22) + XTEX + YTEX, p0 - glm::vec3(0, 0, 0.5f)};
								v3 = {spec + (shade << 22) + YTEX, p1 - glm::vec3(0, 0, 0.5f)};
								break ;
						}
						face_vertices(_vertices, v0, v1, v2, v3);
						if (visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
							spec = s_blocks[type]->texX(face_dir::PLUSZ, face_dir::PLUSZ) + (s_blocks[type]->texY(face_dir::PLUSZ, face_dir::PLUSZ) << 4) + (0 << 19);
							faceLight = computeLight(row, col, level + 1);
							shade = 0;
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22), p4};
							v1 = {spec + (shade << 22) + XTEX, p5};
							v2 = {spec + (shade << 22) + YTEX, p0};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p1};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						spec = s_blocks[type]->texX(face_dir::PLUSZ, face_dir::PLUSZ) + (s_blocks[type]->texY(face_dir::PLUSZ, face_dir::PLUSZ) << 4);
						faceLight = computeLight(row, col, level - 1);
						spec += (faceLight << 24);
						// front MINUSX of second step
						spec += (3 << 19);
						switch (corners) {
							case CORNERS::PM | CORNERS::PP: // full width
								v0 = {spec + (8 << 8) + (shade << 22), p1 - glm::vec3(0.5f, 0, 0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0.5f, 0, 0.5f)};
								v2 = {spec + (shade << 22) + YTEX, p1 - glm::vec3(0.5f, 0, 1)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p5 - glm::vec3(0.5f, 0, 1)};
								break ;
							case CORNERS::PM: // only 'right'
							case CORNERS::PM | CORNERS::PP | CORNERS::MP:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22), p1 - glm::vec3(0.5f, 0, 0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0.5f, 0, 0.5f)};
								v2 = {spec + (shade << 22) + YTEX, p1 - glm::vec3(0.5f, 0, 1)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p5 - glm::vec3(0.5f, 0, 1)};
								++spec;
								break ;
							case CORNERS::PP: // only 'left'
							case CORNERS::PM | CORNERS::PP | CORNERS::MM:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p1 - glm::vec3(0.5f, 0, 0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22), p5 - glm::vec3(0.5f, 0, 0.5f)};
								v2 = {spec + (shade << 22) + XTEX + YTEX, p1 - glm::vec3(0.5f, 0, 1)};
								v3 = {spec + (shade << 22) + YTEX, p5 - glm::vec3(0.5f, 0, 1)};
								++spec;
								break ;
						}
						face_vertices(_vertices, v0, v1, v2, v3);
						// front PLUSX of second step
						spec += (1 << 19);
						switch (corners) {
							case CORNERS::MM | CORNERS::MP: // full width
								v0 = {spec + (8 << 8) + (shade << 22), p4 + glm::vec3(0.5f, 0, -0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
								v2 = {spec + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, 0, -1)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0, -1)};
								break ;
							case CORNERS::MP: // only 'right'
							case CORNERS::MM | CORNERS::MP | CORNERS::PM:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22), p4 + glm::vec3(0.5f, 0, -0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p0 + glm::vec3(0.5f, 0, -0.5f)};
								v2 = {spec + (shade << 22) + YTEX, p4 + glm::vec3(0.5f, 0, -1)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0.5f, 0, -1)};
								++spec;
								break ;
							case CORNERS::MM: // only 'left'
							case CORNERS::MM | CORNERS::MP | CORNERS::PP:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 + glm::vec3(0.5f, 0, -0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22), p0 + glm::vec3(0.5f, 0, -0.5f)};
								v2 = {spec + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0.5f, 0, -1)};
								v3 = {spec + (shade << 22) + YTEX, p0 + glm::vec3(0.5f, 0, -1)};
								++spec;
								break ;
						}
						face_vertices(_vertices, v0, v1, v2, v3);
						// front MINUSY of second step
						spec -= (3 << 19);
						switch (corners) {
							case CORNERS::MP | CORNERS::PP: // full width
								v0 = {spec + (8 << 8) + (shade << 22), p5 - glm::vec3(0, 0.5f, 0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 - glm::vec3(0, 0.5f, 0.5f)};
								v2 = {spec + (shade << 22) + YTEX, p5 - glm::vec3(0, 0.5f, 1)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p4 - glm::vec3(0, 0.5f, 1)};
								break ;
							case CORNERS::PP: // only 'right'
							case CORNERS::MP | CORNERS::PP | CORNERS::MM:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22), p5 - glm::vec3(0, 0.5f, 0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p4 - glm::vec3(0, 0.5f, 0.5f)};
								v2 = {spec + (shade << 22) + YTEX, p5 - glm::vec3(0, 0.5f, 1)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p4 - glm::vec3(0, 0.5f, 1)};
								++spec;
								break ;
							case CORNERS::MP: // only 'left'
							case CORNERS::MP | CORNERS::PP | CORNERS::PM:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.5f, 0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22), p4 - glm::vec3(0, 0.5f, 0.5f)};
								v2 = {spec + (shade << 22) + XTEX + YTEX, p5 - glm::vec3(0, 0.5f, 1)};
								v3 = {spec + (shade << 22) + YTEX, p4 - glm::vec3(0, 0.5f, 1)};
								++spec;
								break ;
						}
						face_vertices(_vertices, v0, v1, v2, v3);
						// front PLUSY of second step
						spec += (1 << 19);
						switch (corners) {
							case CORNERS::MM | CORNERS::PM: // full width
								v0 = {spec + (8 << 8) + (shade << 22), p0 + glm::vec3(0, 0.5f, -0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
								v2 = {spec + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -1)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -1)};
								break ;
							case CORNERS::MM: // only 'left'
							case CORNERS::MM | CORNERS::PM | CORNERS::PP:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22), p0 + glm::vec3(0, 0.5f, -0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.5f, -0.5f)};
								v2 = {spec + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.5f, -1)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.5f, -1)};
								++spec;
								break ;
							case CORNERS::PM: // only 'right'
							case CORNERS::MM | CORNERS::PM | CORNERS::MP:
								--spec;
								v0 = {spec + (8 << 8) + (shade << 22) + XTEX, p0 + glm::vec3(0, 0.5f, -0.5f)};
								v1 = {spec + (8 << 8) + (shade << 22), p1 + glm::vec3(0, 0.5f, -0.5f)};
								v2 = {spec + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0, 0.5f, -1)};
								v3 = {spec + (shade << 22) + YTEX, p1 + glm::vec3(0, 0.5f, -1)};
								++spec;
								break ;
						}
						face_vertices(_vertices, v0, v1, v2, v3);



					} else if (type == blocks::OAK_DOOR) {
						int orientation = 0, xtex_l, xtex_r;
						int bitfield = block_value >> 12;
						switch ((block_value >> 9) & 0x7) {
							case face_dir::MINUSX:
								if (!(bitfield & DOOR::OPEN)) {
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
								if (!(bitfield & DOOR::OPEN)) {
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
								if (!(bitfield & DOOR::OPEN)) {
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
								if (!(bitfield & DOOR::OPEN)) {
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
								spec = s_blocks[type]->texX(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) << 4) + (3 << 19);
								faceLight = computeLight(row, col, level);
								shade = 0;
								spec += (faceLight << 24);
								v0 = {spec + (shade << 22) + xtex_r, p4 + glm::vec3(0.8125f, 0, 0)};
								v1 = {spec + (shade << 22) + xtex_l, p0 + glm::vec3(0.8125f, 0, 0)};
								v2 = {spec + (shade << 22) + xtex_r + YTEX, p6 + glm::vec3(0.8125f, 0, 0)};
								v3 = {spec + (shade << 22) + xtex_l + YTEX, p2 + glm::vec3(0.8125f, 0, 0)};
								face_vertices(_vertices, v0, v1, v2, v3);
								if (visible_face(type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
									spec = s_blocks[type]->texX(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) << 4) + (4 << 19);
									faceLight = computeLight(row + 1, col, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22) + xtex_l, p1};
									v1 = {spec + (shade << 22) + xtex_r, p5};
									v2 = {spec + (shade << 22) + xtex_l + YTEX, p3};
									v3 = {spec + (shade << 22) + xtex_r + YTEX, p7};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (visible_face(type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
									spec = s_blocks[type]->texX(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) << 4) + (1 << 19);
									faceLight = computeLight(row, col - 1, level);
									spec += (faceLight << 24) + 1;
									v0 = {spec + (shade << 22) + XTEX, p0};
									v1 = {spec + (shade << 22), p1};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p2};
									v3 = {spec + (shade << 22) + YTEX, p3};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (visible_face(type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
									spec = s_blocks[type]->texX(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) << 4) + (2 << 19);
									faceLight = computeLight(row, col + 1, level);
									spec += (faceLight << 24) + 1;
									v0 = {spec + (shade << 22), p5};
									v1 = {spec + (shade << 22) + XTEX, p4};
									v2 = {spec + (shade << 22) + YTEX, p7};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if ((bitfield & DOOR::UPPER_HALF) && visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
									spec = s_blocks[type]->texX(face_dir::PLUSZ) + (s_blocks[type]->texY(face_dir::PLUSZ) << 4) + (0 << 19);
									faceLight = computeLight(row, col, level + 1);
									shade = 0;
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p0 + glm::vec3(0.8125f, 0, 0)};
									v1 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0.8125f, 0, 0)};
									v2 = {spec + (shade << 22) - (1 << 4) + (3 << 8) + YTEX, p1};
									v3 = {spec + (shade << 22) - (1 << 4) + (3 << 8) + XTEX + YTEX, p5};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (!(bitfield & DOOR::UPPER_HALF) && visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
									spec = s_blocks[type]->texX(face_dir::MINUSZ, face_dir::MINUSZ) + (s_blocks[type]->texY(face_dir::MINUSZ, face_dir::MINUSZ) << 4) + (5 << 19);
									faceLight = computeLight(row, col, level - 1);
									shade = 0;
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22) + (13 << 8), p6 + glm::vec3(0.8125f, 0, 0)};
									v1 = {spec + (shade << 22) + (13 << 8) + XTEX, p2 + glm::vec3(0.8125f, 0, 0)};
									v2 = {spec + (shade << 22) + YTEX, p7};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								break ;
							case face_dir::PLUSX:
								if (visible_face(type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
									spec = s_blocks[type]->texX(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) << 4) + (3 << 19);
									faceLight = computeLight(row - 1, col, level);
									shade = 0;
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22) + xtex_r, p4};
									v1 = {spec + (shade << 22) + xtex_l, p0};
									v2 = {spec + (shade << 22) + xtex_r + YTEX, p6};
									v3 = {spec + (shade << 22) + xtex_l + YTEX, p2};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								spec = s_blocks[type]->texX(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) << 4) + (4 << 19);
								faceLight = computeLight(row, col, level);
								spec += (faceLight << 24);
								v0 = {spec + (shade << 22) + xtex_l, p1 - glm::vec3(0.8125f, 0, 0)};
								v1 = {spec + (shade << 22) + xtex_r, p5 - glm::vec3(0.8125f, 0, 0)};
								v2 = {spec + (shade << 22) + xtex_l + YTEX, p3 - glm::vec3(0.8125f, 0, 0)};
								v3 = {spec + (shade << 22) + xtex_r + YTEX, p7 - glm::vec3(0.8125f, 0, 0)};
								face_vertices(_vertices, v0, v1, v2, v3);
								if (visible_face(type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
									spec = s_blocks[type]->texX(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) << 4) + (1 << 19);
									faceLight = computeLight(row, col - 1, level);
									spec += (faceLight << 24) + 1;
									v0 = {spec + (shade << 22), p0};
									v1 = {spec + (shade << 22) + XTEX, p1};
									v2 = {spec + (shade << 22) + YTEX, p2};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (visible_face(type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
									spec = s_blocks[type]->texX(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) << 4) + (2 << 19);
									faceLight = computeLight(row, col + 1, level);
									spec += (faceLight << 24) + 1;
									v0 = {spec + (shade << 22) + XTEX, p5};
									v1 = {spec + (shade << 22), p4};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p7};
									v3 = {spec + (shade << 22) + YTEX, p6};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if ((bitfield & DOOR::UPPER_HALF) && visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
									spec = s_blocks[type]->texX(face_dir::PLUSZ) + (s_blocks[type]->texY(face_dir::PLUSZ) << 4) + (0 << 19);
									faceLight = computeLight(row, col, level + 1);
									shade = 0;
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p0};
									v1 = {spec + (shade << 22) + XTEX, p4};
									v2 = {spec + (shade << 22) - (1 << 4) + (3 << 8) + YTEX, p1 - glm::vec3(0.8125f, 0, 0)};
									v3 = {spec + (shade << 22) - (1 << 4) + (3 << 8) + XTEX + YTEX, p5 - glm::vec3(0.8125f, 0, 0)};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (!(bitfield & DOOR::UPPER_HALF) && visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
									spec = s_blocks[type]->texX(face_dir::MINUSZ, face_dir::MINUSZ) + (s_blocks[type]->texY(face_dir::MINUSZ, face_dir::MINUSZ) << 4) + (5 << 19);
									faceLight = computeLight(row, col, level - 1);
									shade = 0;
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22) + (13 << 8), p6};
									v1 = {spec + (shade << 22) + (13 << 8) + XTEX, p2};
									v2 = {spec + (shade << 22) + YTEX, p7 - glm::vec3(0.8125f, 0, 0)};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p3 - glm::vec3(0.8125f, 0, 0)};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								break ;
							case face_dir::MINUSY:
								if (visible_face(type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
									spec = s_blocks[type]->texX(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) << 4) + (3 << 19);
									faceLight = computeLight(row - 1, col, level);
									shade = 0;
									spec += (faceLight << 24) + 1;
									v0 = {spec + (shade << 22), p4};
									v1 = {spec + (shade << 22) + XTEX, p0};
									v2 = {spec + (shade << 22) + YTEX, p6};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (visible_face(type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
									spec = s_blocks[type]->texX(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) << 4) + (4 << 19);
									faceLight = computeLight(row + 1, col, level);
									spec += (faceLight << 24) + 1;
									v0 = {spec + (shade << 22) + XTEX, p1};
									v1 = {spec + (shade << 22), p5};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p3};
									v3 = {spec + (shade << 22) + YTEX, p7};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								spec = s_blocks[type]->texX(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) << 4) + (1 << 19);
								faceLight = computeLight(row, col, level);
								spec += (faceLight << 24);
								v0 = {spec + (shade << 22) + xtex_r, p0 + glm::vec3(0, 0.8125f, 0)};
								v1 = {spec + (shade << 22) + xtex_l, p1 + glm::vec3(0, 0.8125f, 0)};
								v2 = {spec + (shade << 22) + xtex_r + YTEX, p2 + glm::vec3(0, 0.8125f, 0)};
								v3 = {spec + (shade << 22) + xtex_l + YTEX, p3 + glm::vec3(0, 0.8125f, 0)};
								face_vertices(_vertices, v0, v1, v2, v3);
								if (visible_face(type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
									spec = s_blocks[type]->texX(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) << 4) + (2 << 19);
									faceLight = computeLight(row, col + 1, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22) + xtex_l, p5};
									v1 = {spec + (shade << 22) + xtex_r, p4};
									v2 = {spec + (shade << 22) + xtex_l + YTEX, p7};
									v3 = {spec + (shade << 22) + xtex_r + YTEX, p6};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if ((bitfield & DOOR::UPPER_HALF) && visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
									spec = s_blocks[type]->texX(face_dir::PLUSZ) + (s_blocks[type]->texY(face_dir::PLUSZ) << 4) + (0 << 19);
									faceLight = computeLight(row, col, level + 1);
									shade = 0;
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p1 + glm::vec3(0, 0.8125f, 0)};
									v1 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0, 0.8125f, 0)};
									v2 = {spec + (shade << 22) - (1 << 4) + (3 << 8) + YTEX, p5};
									v3 = {spec + (shade << 22) - (1 << 4) + (3 << 8) + XTEX + YTEX, p4};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (!(bitfield & DOOR::UPPER_HALF) && visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
									spec = s_blocks[type]->texX(face_dir::MINUSZ, face_dir::MINUSZ) + (s_blocks[type]->texY(face_dir::MINUSZ, face_dir::MINUSZ) << 4) + (5 << 19);
									faceLight = computeLight(row, col, level - 1);
									shade = 0;
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22) + (13 << 8), p7};
									v1 = {spec + (shade << 22) + (13 << 8) + XTEX, p6};
									v2 = {spec + (shade << 22) + YTEX, p3 + glm::vec3(0, 0.8125f, 0)};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0, 0.8125f, 0)};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								break ;
							case face_dir::PLUSY:
								if (visible_face(type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
									spec = s_blocks[type]->texX(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::MINUSX, bitfield & DOOR::UPPER_HALF) << 4) + (3 << 19);
									faceLight = computeLight(row - 1, col, level);
									shade = 0;
									spec += (faceLight << 24) + 1;
									v0 = {spec + (shade << 22) + XTEX, p4};
									v1 = {spec + (shade << 22), p0};
									v2 = {spec + (shade << 22) + XTEX + YTEX, p6};
									v3 = {spec + (shade << 22) + YTEX, p2};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (visible_face(type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
									spec = s_blocks[type]->texX(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::PLUSX, bitfield & DOOR::UPPER_HALF) << 4) + (4 << 19);
									faceLight = computeLight(row + 1, col, level);
									spec += (faceLight << 24) + 1;
									v0 = {spec + (shade << 22), p1};
									v1 = {spec + (shade << 22) + XTEX, p5};
									v2 = {spec + (shade << 22) + YTEX, p3};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (visible_face(type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
									spec = s_blocks[type]->texX(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::MINUSY, bitfield & DOOR::UPPER_HALF) << 4) + (1 << 19);
									faceLight = computeLight(row, col - 1, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22) + xtex_r, p0};
									v1 = {spec + (shade << 22) + xtex_l, p1};
									v2 = {spec + (shade << 22) + xtex_r + YTEX, p2};
									v3 = {spec + (shade << 22) + xtex_l + YTEX, p3};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								spec = s_blocks[type]->texX(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) + (s_blocks[type]->texY(face_dir::PLUSY, bitfield & DOOR::UPPER_HALF) << 4) + (2 << 19);
								faceLight = computeLight(row, col, level);
								spec += (faceLight << 24);
								v0 = {spec + (shade << 22) + xtex_l, p5 - glm::vec3(0, 0.8125f, 0)};
								v1 = {spec + (shade << 22) + xtex_r, p4 - glm::vec3(0, 0.8125f, 0)};
								v2 = {spec + (shade << 22) + xtex_l + YTEX, p7 - glm::vec3(0, 0.8125f, 0)};
								v3 = {spec + (shade << 22) + xtex_r + YTEX, p6 - glm::vec3(0, 0.8125f, 0)};
								face_vertices(_vertices, v0, v1, v2, v3);
								if ((bitfield & DOOR::UPPER_HALF) && visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
									spec = s_blocks[type]->texX(face_dir::PLUSZ) + (s_blocks[type]->texY(face_dir::PLUSZ) << 4) + (0 << 19);
									faceLight = computeLight(row, col, level + 1);
									shade = 0;
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p1};
									v1 = {spec + (shade << 22) + XTEX, p0};
									v2 = {spec + (shade << 22) - (1 << 4) + (3 << 8) + YTEX, p5 - glm::vec3(0, 0.8125f, 0)};
									v3 = {spec + (shade << 22) - (1 << 4) + (3 << 8) + XTEX + YTEX, p4 - glm::vec3(0, 0.8125f, 0)};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (!(bitfield & DOOR::UPPER_HALF) && visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
									spec = s_blocks[type]->texX(face_dir::MINUSZ, face_dir::MINUSZ) + (s_blocks[type]->texY(face_dir::MINUSZ, face_dir::MINUSZ) << 4) + (5 << 19);
									faceLight = computeLight(row, col, level - 1);
									shade = 0;
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22) + (13 << 8), p7 - glm::vec3(0, 0.8125f, 0)};
									v1 = {spec + (shade << 22) + (13 << 8) + XTEX, p6 - glm::vec3(0, 0.8125f, 0)};
									v2 = {spec + (shade << 22) + YTEX, p3};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								break ;
						}



					} else if (type == blocks::OAK_TRAPDOOR) {
						shade = 0;
						int bitfield = block_value >> 12;
						int baseSpec = s_blocks[type]->texX() + (s_blocks[type]->texY() << 4);
						if (!(bitfield & DOOR::OPEN)) {
							if (!(bitfield & DOOR::UPPER_HALF)) {
								if (visible_face(type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
									spec = baseSpec + (3 << 19);
									faceLight = computeLight(row - 1, col, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p4 - glm::vec3(0, 0, 0.8125f)};
									v1 = {spec + (shade << 22) + XTEX, p0 - glm::vec3(0, 0, 0.8125f)};
									v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p6};
									v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p2};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (visible_face(type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
									spec = baseSpec + (4 << 19);
									faceLight = computeLight(row + 1, col, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p1 - glm::vec3(0, 0, 0.8125f)};
									v1 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0, 0, 0.8125f)};
									v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p3};
									v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p7};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (visible_face(type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
									spec = baseSpec + (1 << 19);
									faceLight = computeLight(row, col - 1, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p0 - glm::vec3(0, 0, 0.8125f)};
									v1 = {spec + (shade << 22) + XTEX, p1 - glm::vec3(0, 0, 0.8125f)};
									v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p2};
									v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p3};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (visible_face(type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
									spec = baseSpec + (2 << 19);
									faceLight = computeLight(row, col + 1, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p5 - glm::vec3(0, 0, 0.8125f)};
									v1 = {spec + (shade << 22) + XTEX, p4 - glm::vec3(0, 0, 0.8125f)};
									v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p7};
									v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p6};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								spec = baseSpec + (0 << 19);
								faceLight = computeLight(row, col, level);
								spec += (faceLight << 24);
								v0 = {spec + (shade << 22), p4 - glm::vec3(0, 0, 0.8125f)};
								v1 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0, 0, 0.8125f)};
								v2 = {spec + (shade << 22) + YTEX, p0 - glm::vec3(0, 0, 0.8125f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p1 - glm::vec3(0, 0, 0.8125f)};
								face_vertices(_vertices, v0, v1, v2, v3);
								if (visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
									spec = baseSpec + (5 << 19);
									faceLight = computeLight(row, col, level - 1);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p2};
									v1 = {spec + (shade << 22) + XTEX, p3};
									v2 = {spec + (shade << 22) + YTEX, p6};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
							} else {
								if (visible_face(type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
									spec = baseSpec + (3 << 19);
									faceLight = computeLight(row - 1, col, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p4};
									v1 = {spec + (shade << 22) + XTEX, p0};
									v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0, 0, 0.8125f)};
									v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0, 0, 0.8125f)};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (visible_face(type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
									spec = baseSpec + (4 << 19);
									faceLight = computeLight(row + 1, col, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p1};
									v1 = {spec + (shade << 22) + XTEX, p5};
									v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p3 + glm::vec3(0, 0, 0.8125f)};
									v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, 0, 0.8125f)};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (visible_face(type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
									spec = baseSpec + (1 << 19);
									faceLight = computeLight(row, col - 1, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p0};
									v1 = {spec + (shade << 22) + XTEX, p1};
									v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p2 + glm::vec3(0, 0, 0.8125f)};
									v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0, 0.8125f)};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (visible_face(type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
									spec = baseSpec + (2 << 19);
									faceLight = computeLight(row, col + 1, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p5};
									v1 = {spec + (shade << 22) + XTEX, p4};
									v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p7 + glm::vec3(0, 0, 0.8125f)};
									v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p6 + glm::vec3(0, 0, 0.8125f)};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								if (visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
									spec = baseSpec + (0 << 19);
									faceLight = computeLight(row, col, level + 1);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p4};
									v1 = {spec + (shade << 22) + XTEX, p5};
									v2 = {spec + (shade << 22) + YTEX, p0};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p1};
									face_vertices(_vertices, v0, v1, v2, v3);
								}
								spec = baseSpec + (5 << 19);
								faceLight = computeLight(row, col, level);
								spec += (faceLight << 24);
								v0 = {spec + (shade << 22), p2 + glm::vec3(0, 0, 0.8125f)};
								v1 = {spec + (shade << 22) + XTEX, p3 + glm::vec3(0, 0, 0.8125f)};
								v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0, 0, 0.8125f)};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p7 + glm::vec3(0, 0, 0.8125f)};
								face_vertices(_vertices, v0, v1, v2, v3);
							}
						} else { // open trapdoor
							switch ((block_value >> 9) & 0x7) {
								case face_dir::MINUSX:
									p0 += glm::vec3(0.8125f, 0, 0);
									p2 += glm::vec3(0.8125f, 0, 0);
									p4 += glm::vec3(0.8125f, 0, 0);
									p6 += glm::vec3(0.8125f, 0, 0);
									spec = baseSpec + (3 << 19);
									faceLight = computeLight(row, col, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p4};
									v1 = {spec + (shade << 22) + XTEX, p0};
									v2 = {spec + (shade << 22) + YTEX, p6};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
									face_vertices(_vertices, v0, v1, v2, v3);
									if (visible_face(type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
										spec = baseSpec + (4 << 19);
										faceLight = computeLight(row + 1, col, level);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p1};
										v1 = {spec + (shade << 22) + XTEX, p5};
										v2 = {spec + (shade << 22) + YTEX, p3};
										v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									if (visible_face(type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
										spec = baseSpec + (1 << 19);
										faceLight = computeLight(row, col - 1, level);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p2};
										v1 = {spec + (shade << 22) + XTEX, p0};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p3};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p1};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									if (visible_face(type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
										spec = baseSpec + (2 << 19);
										faceLight = computeLight(row, col + 1, level);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p7};
										v1 = {spec + (shade << 22) + XTEX, p5};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p6};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p4};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									if (visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
										spec = baseSpec + (0 << 19);
										faceLight = computeLight(row, col, level + 1);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p0};
										v1 = {spec + (shade << 22) + XTEX, p4};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p1};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p5};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									if (visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
										spec = baseSpec + (5 << 19);
										faceLight = computeLight(row, col, level - 1);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p6};
										v1 = {spec + (shade << 22) + XTEX, p2};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p7};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p3};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									break ;
								case face_dir::PLUSX:
									p1 -= glm::vec3(0.8125f, 0, 0);
									p3 -= glm::vec3(0.8125f, 0, 0);
									p5 -= glm::vec3(0.8125f, 0, 0);
									p7 -= glm::vec3(0.8125f, 0, 0);
									if (visible_face(type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
										spec = baseSpec + (3 << 19);
										faceLight = computeLight(row - 1, col, level);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p4};
										v1 = {spec + (shade << 22) + XTEX, p0};
										v2 = {spec + (shade << 22) + YTEX, p6};
										v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									spec = baseSpec + (4 << 19);
									faceLight = computeLight(row, col, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p1};
									v1 = {spec + (shade << 22) + XTEX, p5};
									v2 = {spec + (shade << 22) + YTEX, p3};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
									face_vertices(_vertices, v0, v1, v2, v3);
									if (visible_face(type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
										spec = baseSpec + (1 << 19);
										faceLight = computeLight(row, col - 1, level);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p2};
										v1 = {spec + (shade << 22) + XTEX, p0};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p3};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p1};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									if (visible_face(type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
										spec = baseSpec + (2 << 19);
										faceLight = computeLight(row, col + 1, level);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p7};
										v1 = {spec + (shade << 22) + XTEX, p5};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p6};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p4};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									if (visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
										spec = baseSpec + (0 << 19);
										faceLight = computeLight(row, col, level + 1);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p0};
										v1 = {spec + (shade << 22) + XTEX, p4};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p1};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p5};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									if (visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
										spec = baseSpec + (5 << 19);
										faceLight = computeLight(row, col, level - 1);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p6};
										v1 = {spec + (shade << 22) + XTEX, p2};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p7};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p3};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									break ;
								case face_dir::MINUSY:
									p0 += glm::vec3(0, 0.8125f, 0);
									p1 += glm::vec3(0, 0.8125f, 0);
									p2 += glm::vec3(0, 0.8125f, 0);
									p3 += glm::vec3(0, 0.8125f, 0);
									if (visible_face(type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
										spec = baseSpec + (3 << 19);
										faceLight = computeLight(row - 1, col, level);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p6};
										v1 = {spec + (shade << 22) + XTEX, p4};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p2};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p0};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									if (visible_face(type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
										spec = baseSpec + (4 << 19);
										faceLight = computeLight(row + 1, col, level);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p3};
										v1 = {spec + (shade << 22) + XTEX, p1};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p7};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p5};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									spec = baseSpec + (1 << 19);
									faceLight = computeLight(row, col, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p0};
									v1 = {spec + (shade << 22) + XTEX, p1};
									v2 = {spec + (shade << 22) + YTEX, p2};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
									face_vertices(_vertices, v0, v1, v2, v3);
									if (visible_face(type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
										spec = baseSpec + (2 << 19);
										faceLight = computeLight(row, col + 1, level);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p5};
										v1 = {spec + (shade << 22) + XTEX, p4};
										v2 = {spec + (shade << 22) + YTEX, p7};
										v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									if (visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
										spec = baseSpec + (0 << 19);
										faceLight = computeLight(row, col, level + 1);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p4};
										v1 = {spec + (shade << 22) + XTEX, p5};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p0};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p1};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									if (visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
										spec = baseSpec + (5 << 19);
										faceLight = computeLight(row, col, level - 1);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p2};
										v1 = {spec + (shade << 22) + XTEX, p3};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p6};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p7};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									break ;
								case face_dir::PLUSY:
									p4 -= glm::vec3(0, 0.8125f, 0);
									p5 -= glm::vec3(0, 0.8125f, 0);
									p6 -= glm::vec3(0, 0.8125f, 0);
									p7 -= glm::vec3(0, 0.8125f, 0);
									if (visible_face(type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
										spec = baseSpec + (3 << 19);
										faceLight = computeLight(row - 1, col, level);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p6};
										v1 = {spec + (shade << 22) + XTEX, p4};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p2};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p0};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									if (visible_face(type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
										spec = baseSpec + (4 << 19);
										faceLight = computeLight(row + 1, col, level);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p3};
										v1 = {spec + (shade << 22) + XTEX, p1};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p7};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p5};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									if (visible_face(type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
										spec = baseSpec + (1 << 19);
										faceLight = computeLight(row, col - 1, level);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p0};
										v1 = {spec + (shade << 22) + XTEX, p1};
										v2 = {spec + (shade << 22) + YTEX, p2};
										v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									spec = baseSpec + (2 << 19);
									faceLight = computeLight(row, col, level);
									spec += (faceLight << 24);
									v0 = {spec + (shade << 22), p5};
									v1 = {spec + (shade << 22) + XTEX, p4};
									v2 = {spec + (shade << 22) + YTEX, p7};
									v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
									face_vertices(_vertices, v0, v1, v2, v3);
									if (visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
										spec = baseSpec + (0 << 19);
										faceLight = computeLight(row, col, level + 1);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p4};
										v1 = {spec + (shade << 22) + XTEX, p5};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p0};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p1};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									if (visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
										spec = baseSpec + (5 << 19);
										faceLight = computeLight(row, col, level - 1);
										spec += (faceLight << 24);
										v0 = {spec + (shade << 22), p2};
										v1 = {spec + (shade << 22) + XTEX, p3};
										v2 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + YTEX, p6};
										v3 = {spec - (1 << 4) + (3 << 8) + (shade << 22) + XTEX + YTEX, p7};
										face_vertices(_vertices, v0, v1, v2, v3);
									}
									break ;
							}
						}



					} else if (type == blocks::OAK_FENCE) {
						shade = 0;
						// central pole
						int baseSpec = s_blocks[type]->texX(face_dir::PLUSZ, FENCE::BASE) + (s_blocks[type]->texY() << 4);
						faceLight = computeLight(row, col, level);
						spec = baseSpec + (3 << 19);
						spec += (faceLight << 24);
						v0 = {spec + (shade << 22), p4 + glm::vec3(0.375f, 0, 0)};
						v1 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0.375f, 0, 0)};
						v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0.375f, 0, 0)};
						v3 = {spec + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0.375f, 0, 0)};
						face_vertices(_vertices, v0, v1, v2, v3);
						spec = baseSpec + (4 << 19);
						spec += (faceLight << 24);
						v0 = {spec + (shade << 22), p1 - glm::vec3(0.375f, 0, 0)};
						v1 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0.375f, 0, 0)};
						v2 = {spec + (shade << 22) + YTEX, p3 - glm::vec3(0.375f, 0, 0)};
						v3 = {spec + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0.375f, 0, 0)};
						face_vertices(_vertices, v0, v1, v2, v3);
						spec = baseSpec + (1 << 19);
						spec += (faceLight << 24);
						v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.375f, 0)};
						v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.375f, 0)};
						v2 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0, 0.375f, 0)};
						v3 = {spec + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0.375f, 0)};
						face_vertices(_vertices, v0, v1, v2, v3);
						spec = baseSpec + (2 << 19);
						spec += (faceLight << 24);
						v0 = {spec + (shade << 22), p5 - glm::vec3(0, 0.375f, 0)};
						v1 = {spec + (shade << 22) + XTEX, p4 - glm::vec3(0, 0.375f, 0)};
						v2 = {spec + (shade << 22) + YTEX, p7 - glm::vec3(0, 0.375f, 0)};
						v3 = {spec + (shade << 22) + XTEX + YTEX, p6 - glm::vec3(0, 0.375f, 0)};
						face_vertices(_vertices, v0, v1, v2, v3);
						if (visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
							spec = baseSpec + (0 << 19);
							faceLight = computeLight(row, col, level + 1);
							spec += (faceLight << 24);
							v0 = {spec + (6 << 8) + (shade << 22), p4 - glm::vec3(0, 0.375f, 0)};
							v1 = {spec + (6 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.375f, 0)};
							v2 = {spec - (1 << 4) + (10 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.375f, 0)};
							v3 = {spec - (1 << 4) + (10 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.375f, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
							spec = baseSpec + (5 << 19);
							faceLight = computeLight(row, col, level - 1);
							spec += (faceLight << 24);
							v0 = {spec + (6 << 8) + (shade << 22), p2 + glm::vec3(0, 0.375f, 0)};
							v1 = {spec + (6 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(0, 0.375f, 0)};
							v2 = {spec - (1 << 4) + (10 << 8) + (shade << 22) + YTEX, p6 - glm::vec3(0, 0.375f, 0)};
							v3 = {spec - (1 << 4) + (10 << 8) + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0, 0.375f, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						// arms
						if ((block_value >> 12) & FENCE::MX) {
							if (visible_face(type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
								spec = s_blocks[type]->texX(face_dir::MINUSX, FENCE::ARM_END) + (s_blocks[type]->texY() << 4) + (3 << 19);
								faceLight = computeLight(row - 1, col, level);
								spec += (faceLight << 24);
								v0 = {spec + (shade << 22), p4};
								v1 = {spec + (shade << 22) + XTEX, p0};
								v2 = {spec + (shade << 22) + YTEX, p6};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p2};
								face_vertices(_vertices, v0, v1, v2, v3);
							}
							baseSpec = s_blocks[type]->texX(face_dir::PLUSZ, FENCE::ARM) + (s_blocks[type]->texY() << 4);
							faceLight = computeLight(row, col, level);
							spec = baseSpec + (1 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.4375f, 0)};
							v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.4375f, 0)};
							v2 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0, 0.4375f, 0)};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0.4375f, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
							spec = baseSpec + (2 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.4375f, 0)};
							v1 = {spec + (shade << 22), p4 - glm::vec3(0, 0.4375f, 0)};
							v2 = {spec + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0, 0.4375f, 0)};
							v3 = {spec + (shade << 22) + YTEX, p6 - glm::vec3(0, 0.4375f, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// top of top arm
							spec = baseSpec + (0 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (7 << 8) + (shade << 22), p4 - glm::vec3(0, 0.4375f, 0.0625f)};
							v1 = {spec + (7 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.4375f, 0.0625f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.4375f, -0.0625f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.4375f, -0.0625f)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// top of bottom arm
							v0 = {spec + (7 << 8) + (shade << 22), p4 - glm::vec3(0, 0.4375f, 0.4375f)};
							v1 = {spec + (7 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.4375f, 0.4375f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.4375f, -0.4375f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.4375f, -0.4375f)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// bottom of top arm
							spec = baseSpec + (5 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (7 << 8) + (shade << 22), p2 + glm::vec3(0, 0.4375f, 0.75f)};
							v1 = {spec + (7 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(0, 0.4375f, 0.75f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p6 - glm::vec3(0, 0.4375f, -0.75f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0, 0.4375f, -0.75f)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// bottom of bottom arm
							v0 = {spec + (7 << 8) + (shade << 22), p2 + glm::vec3(0, 0.4375f, 0.375f)};
							v1 = {spec + (7 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(0, 0.4375f, 0.375f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p6 - glm::vec3(0, 0.4375f, -0.375f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0, 0.4375f, -0.375f)};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if ((block_value >> 12) & FENCE::PX) {
							if (visible_face(type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
								spec = s_blocks[type]->texX(face_dir::PLUSX, FENCE::ARM_END) + (s_blocks[type]->texY() << 4) + (3 << 19);
								faceLight = computeLight(row + 1, col, level);
								spec += (faceLight << 24);
								v0 = {spec + (shade << 22), p1};
								v1 = {spec + (shade << 22) + XTEX, p5};
								v2 = {spec + (shade << 22) + YTEX, p3};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
								face_vertices(_vertices, v0, v1, v2, v3);
							}
							baseSpec = s_blocks[type]->texX(face_dir::PLUSZ, FENCE::ARM) + (s_blocks[type]->texY() << 4);
							faceLight = computeLight(row, col, level);
							spec = baseSpec + (1 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0, 0.4375f, 0)};
							v1 = {spec + (shade << 22), p1 + glm::vec3(0, 0.4375f, 0)};
							v2 = {spec + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0, 0.4375f, 0)};
							v3 = {spec + (shade << 22) + YTEX, p3 + glm::vec3(0, 0.4375f, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
							spec = baseSpec + (2 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22), p5 - glm::vec3(0, 0.4375f, 0)};
							v1 = {spec + (shade << 22) + XTEX, p4 - glm::vec3(0, 0.4375f, 0)};
							v2 = {spec + (shade << 22) + YTEX, p7 - glm::vec3(0, 0.4375f, 0)};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p6 - glm::vec3(0, 0.4375f, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// top of top arm
							spec = baseSpec + (0 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (7 << 8) + (shade << 22) + XTEX, p4 - glm::vec3(0, 0.4375f, 0.0625f)};
							v1 = {spec + (7 << 8) + (shade << 22), p5 - glm::vec3(0, 0.4375f, 0.0625f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0, 0.4375f, -0.0625f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p1 + glm::vec3(0, 0.4375f, -0.0625f)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// top of bottom arm
							v0 = {spec + (7 << 8) + (shade << 22) + XTEX, p4 - glm::vec3(0, 0.4375f, 0.4375f)};
							v1 = {spec + (7 << 8) + (shade << 22), p5 - glm::vec3(0, 0.4375f, 0.4375f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p0 + glm::vec3(0, 0.4375f, -0.4375f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p1 + glm::vec3(0, 0.4375f, -0.4375f)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// bottom of top arm
							spec = baseSpec + (5 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (7 << 8) + (shade << 22) + XTEX, p2 + glm::vec3(0, 0.4375f, 0.75f)};
							v1 = {spec + (7 << 8) + (shade << 22), p3 + glm::vec3(0, 0.4375f, 0.75f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p6 - glm::vec3(0, 0.4375f, -0.75f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p7 - glm::vec3(0, 0.4375f, -0.75f)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// bottom of bottom arm
							v0 = {spec + (7 << 8) + (shade << 22) + XTEX, p2 + glm::vec3(0, 0.4375f, 0.375f)};
							v1 = {spec + (7 << 8) + (shade << 22), p3 + glm::vec3(0, 0.4375f, 0.375f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p6 - glm::vec3(0, 0.4375f, -0.375f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p7 - glm::vec3(0, 0.4375f, -0.375f)};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if ((block_value >> 12) & FENCE::MY) {
							if (visible_face(type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
								spec = s_blocks[type]->texX(face_dir::MINUSY, FENCE::ARM_END) + (s_blocks[type]->texY() << 4) + (1 << 19);
								faceLight = computeLight(row, col - 1, level);
								spec += (faceLight << 24);
								v0 = {spec + (shade << 22), p0};
								v1 = {spec + (shade << 22) + XTEX, p1};
								v2 = {spec + (shade << 22) + YTEX, p2};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p3};
								face_vertices(_vertices, v0, v1, v2, v3);
							}
							baseSpec = s_blocks[type]->texX(face_dir::PLUSZ, FENCE::ARM) + (s_blocks[type]->texY() << 4);
							faceLight = computeLight(row, col, level);
							spec = baseSpec + (3 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0.4375f, 0, 0)};
							v1 = {spec + (shade << 22), p0 + glm::vec3(0.4375f, 0, 0)};
							v2 = {spec + (shade << 22) + XTEX + YTEX, p6 + glm::vec3(0.4375f, 0, 0)};
							v3 = {spec + (shade << 22) + YTEX, p2+ glm::vec3(0.4375f, 0, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
							spec = baseSpec + (4 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22), p1 - glm::vec3(0.4375f, 0, 0)};
							v1 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0.4375f, 0, 0)};
							v2 = {spec + (shade << 22) + YTEX, p3 - glm::vec3(0.4375f, 0, 0)};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0.4375f, 0, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// top of top arm
							spec = baseSpec + (0 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (7 << 8) + (shade << 22), p0 + glm::vec3(0.4375f, 0, -0.0625f)};
							v1 = {spec + (7 << 8) + (shade << 22) + XTEX, p4 + glm::vec3(0.4375f, 0, -0.0625f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p1 - glm::vec3(0.4375f, 0, 0.0625f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p5 - glm::vec3(0.4375f, 0, 0.0625f)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// top of bottom arm
							v0 = {spec + (7 << 8) + (shade << 22), p0 + glm::vec3(0.4375f, 0, -0.4375f)};
							v1 = {spec + (7 << 8) + (shade << 22) + XTEX, p4 + glm::vec3(0.4375f, 0, -0.4375f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p1 - glm::vec3(0.4375f, 0, 0.4375f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p5 - glm::vec3(0.4375f, 0, 0.4375f)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// bottom of top arm
							spec = baseSpec + (5 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (7 << 8) + (shade << 22) + XTEX, p6 + glm::vec3(0.4375f, 0, 0.75f)};
							v1 = {spec + (7 << 8) + (shade << 22), p2 + glm::vec3(0.4375f, 0, 0.75f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0.4375f, 0, -0.75f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p3 - glm::vec3(0.4375f, 0, -0.75f)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// bottom of bottom arm
							v0 = {spec + (7 << 8) + (shade << 22) + XTEX, p6 + glm::vec3(0.4375f, 0, 0.375f)};
							v1 = {spec + (7 << 8) + (shade << 22), p2 + glm::vec3(0.4375f, 0, 0.375f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0.4375f, 0, -0.375f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p3 - glm::vec3(0.4375f, 0, -0.375f)};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if ((block_value >> 12) & FENCE::PY) {
							if (visible_face(type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
								spec = s_blocks[type]->texX(face_dir::PLUSY, FENCE::ARM_END) + (s_blocks[type]->texY() << 4) + (2 << 19);
								faceLight = computeLight(row, col + 1, level);
								spec += (faceLight << 24);
								v0 = {spec + (shade << 22), p5};
								v1 = {spec + (shade << 22) + XTEX, p4};
								v2 = {spec + (shade << 22) + YTEX, p7};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p6};
								face_vertices(_vertices, v0, v1, v2, v3);
							}
							baseSpec = s_blocks[type]->texX(face_dir::PLUSZ, FENCE::ARM) + (s_blocks[type]->texY() << 4);
							faceLight = computeLight(row, col, level);
							spec = baseSpec + (3 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22), p4 + glm::vec3(0.4375f, 0, 0)};
							v1 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0.4375f, 0, 0)};
							v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0.4375f, 0, 0)};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p2+ glm::vec3(0.4375f, 0, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
							spec = baseSpec + (4 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22) + XTEX, p1 - glm::vec3(0.4375f, 0, 0)};
							v1 = {spec + (shade << 22), p5 - glm::vec3(0.4375f, 0, 0)};
							v2 = {spec + (shade << 22) + XTEX + YTEX, p3 - glm::vec3(0.4375f, 0, 0)};
							v3 = {spec + (shade << 22) + YTEX, p7 - glm::vec3(0.4375f, 0, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// top of top arm
							spec = baseSpec + (0 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (7 << 8) + (shade << 22) + XTEX, p0 + glm::vec3(0.4375f, 0, -0.0625f)};
							v1 = {spec + (7 << 8) + (shade << 22), p4 + glm::vec3(0.4375f, 0, -0.0625f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p1 - glm::vec3(0.4375f, 0, 0.0625f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p5 - glm::vec3(0.4375f, 0, 0.0625f)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// top of bottom arm
							v0 = {spec + (7 << 8) + (shade << 22) + XTEX, p0 + glm::vec3(0.4375f, 0, -0.4375f)};
							v1 = {spec + (7 << 8) + (shade << 22), p4 + glm::vec3(0.4375f, 0, -0.4375f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p1 - glm::vec3(0.4375f, 0, 0.4375f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p5 - glm::vec3(0.4375f, 0, 0.4375f)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// bottom of top arm
							spec = baseSpec + (5 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (7 << 8) + (shade << 22), p6 + glm::vec3(0.4375f, 0, 0.75f)};
							v1 = {spec + (7 << 8) + (shade << 22) + XTEX, p2 + glm::vec3(0.4375f, 0, 0.75f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p7 - glm::vec3(0.4375f, 0, -0.75f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p3 - glm::vec3(0.4375f, 0, -0.75f)};
							face_vertices(_vertices, v0, v1, v2, v3);
							// bottom of bottom arm
							v0 = {spec + (7 << 8) + (shade << 22), p6 + glm::vec3(0.4375f, 0, 0.375f)};
							v1 = {spec + (7 << 8) + (shade << 22) + XTEX, p2 + glm::vec3(0.4375f, 0, 0.375f)};
							v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p7 - glm::vec3(0.4375f, 0, -0.375f)};
							v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p3 - glm::vec3(0.4375f, 0, -0.375f)};
							face_vertices(_vertices, v0, v1, v2, v3);
						}



					} else if (type == blocks::GLASS_PANE) { // TODO move this to sky_vertex
						shade = 0;
						int baseSpec = s_blocks[type]->texX(face_dir::PLUSZ, FENCE::ARM_END) + (s_blocks[type]->texY() << 4);
						faceLight = computeLight(row, col, level);
						if ((block_value >> 12) & FENCE::MX) {
							spec = baseSpec + 2 + (1 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.4375f, 0)};
							v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.4375f, 0)};
							v2 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0, 0.4375f, 0)};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0.4375f, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
							spec += (1 << 19);
							v0 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.4375f, 0)};
							v1 = {spec + (shade << 22), p4 - glm::vec3(0, 0.4375f, 0)};
							v2 = {spec + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0, 0.4375f, 0)};
							v3 = {spec + (shade << 22) + YTEX, p6 - glm::vec3(0, 0.4375f, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
						} else {
							spec = baseSpec + (3 << 19) + (!!((block_value >> 12) & (FENCE::MY | FENCE::PY)) << 4);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22), p4 + glm::vec3(0.4375f, 0, 0)};
							v1 = {spec + (shade << 22) + XTEX, p0 + glm::vec3(0.4375f, 0, 0)};
							v2 = {spec + (shade << 22) + YTEX, p6 + glm::vec3(0.4375f, 0, 0)};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0.4375f, 0, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if ((block_value >> 12) & FENCE::PX) {
							spec = baseSpec + 3 + (1 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.4375f, 0)};
							v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.4375f, 0)};
							v2 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0, 0.4375f, 0)};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0.4375f, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
							spec += (1 << 19);
							v0 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.4375f, 0)};
							v1 = {spec + (shade << 22), p4 - glm::vec3(0, 0.4375f, 0)};
							v2 = {spec + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0, 0.4375f, 0)};
							v3 = {spec + (shade << 22) + YTEX, p6 - glm::vec3(0, 0.4375f, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
						} else {
							spec = baseSpec + (4 << 19) + (!!((block_value >> 12) & (FENCE::MY | FENCE::PY)) << 4);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22), p1 - glm::vec3(0.4375f, 0, 0)};
							v1 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0.4375f, 0, 0)};
							v2 = {spec + (shade << 22) + YTEX, p3 - glm::vec3(0.4375f, 0, 0)};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0.4375f, 0, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if ((block_value >> 12) & FENCE::MY) {
							spec = baseSpec + 2 + (3 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0.4375f, 0, 0)};
							v1 = {spec + (shade << 22), p0 + glm::vec3(0.4375f, 0, 0)};
							v2 = {spec + (shade << 22) + XTEX + YTEX, p6 + glm::vec3(0.4375f, 0, 0)};
							v3 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0.4375f, 0, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
							spec += (1 << 19);
							v0 = {spec + (shade << 22), p1 - glm::vec3(0.4375f, 0, 0)};
							v1 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0.4375f, 0, 0)};
							v2 = {spec + (shade << 22) + YTEX, p3 - glm::vec3(0.4375f, 0, 0)};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0.4375f, 0, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
						} else {
							spec = baseSpec + (1 << 19) + (!!((block_value >> 12) & (FENCE::MX | FENCE::PX)) << 4);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22), p0 + glm::vec3(0, 0.4375f, 0)};
							v1 = {spec + (shade << 22) + XTEX, p1 + glm::vec3(0, 0.4375f, 0)};
							v2 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0, 0.4375f, 0)};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p3 + glm::vec3(0, 0.4375f, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if ((block_value >> 12) & FENCE::PY) {
							spec = baseSpec + 3 + (3 << 19);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22) + XTEX, p4 + glm::vec3(0.4375f, 0, 0)};
							v1 = {spec + (shade << 22), p0 + glm::vec3(0.4375f, 0, 0)};
							v2 = {spec + (shade << 22) + XTEX + YTEX, p6 + glm::vec3(0.4375f, 0, 0)};
							v3 = {spec + (shade << 22) + YTEX, p2 + glm::vec3(0.4375f, 0, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
							spec += (1 << 19);
							v0 = {spec + (shade << 22), p1 - glm::vec3(0.4375f, 0, 0)};
							v1 = {spec + (shade << 22) + XTEX, p5 - glm::vec3(0.4375f, 0, 0)};
							v2 = {spec + (shade << 22) + YTEX, p3 - glm::vec3(0.4375f, 0, 0)};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0.4375f, 0, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
						} else {
							spec = baseSpec + (2 << 19) + (!!((block_value >> 12) & (FENCE::MX | FENCE::PX)) << 4);
							spec += (faceLight << 24);
							v0 = {spec + (shade << 22), p5 - glm::vec3(0, 0.4375f, 0)};
							v1 = {spec + (shade << 22) + XTEX, p4 - glm::vec3(0, 0.4375f, 0)};
							v2 = {spec + (shade << 22) + YTEX, p7 - glm::vec3(0, 0.4375f, 0)};
							v3 = {spec + (shade << 22) + XTEX + YTEX, p6 - glm::vec3(0, 0.4375f, 0)};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
							spec = baseSpec + (0 << 19);
							faceLight = computeLight(row, col, level + 1);
							spec += (faceLight << 24);
							if ((block_value >> 12) & FENCE::PY) {
								v0 = {spec + (shade << 22), p4};
								v1 = {spec + (shade << 22) + XTEX, p5};
							} else {
								v0 = {spec + (7 << 8) + (shade << 22), p4 - glm::vec3(0, 0.4375f, 0)};
								v1 = {spec + (7 << 8) + (shade << 22) + XTEX, p5 - glm::vec3(0, 0.4375f, 0)};
							}
							if ((block_value >> 12) & FENCE::MY) {
								v2 = {spec + (shade << 22) + YTEX, p0};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p1};
							} else {
								v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0, 0.4375f, 0)};
								v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p1 + glm::vec3(0, 0.4375f, 0)};
							}
							face_vertices(_vertices, v0, v1, v2, v3);
							if ((block_value >> 12) & FENCE::MX) {
								v0 = {spec + (shade << 22), p0};
								v1 = {spec + (shade << 22) + XTEX, p4};
								v2 = {spec - (1 << 4) + (7 << 8) + (shade << 22) + YTEX, p0 + glm::vec3(0.4375f, 0, 0)};
								v3 = {spec - (1 << 4) + (7 << 8) + (shade << 22) + XTEX + YTEX, p4 + glm::vec3(0.4375f, 0, 0)};
								face_vertices(_vertices, v0, v1, v2, v3);
							}
							if ((block_value >> 12) & FENCE::PX) {
								v0 = {spec + (shade << 22), p1};
								v1 = {spec + (shade << 22) + XTEX, p5};
								v2 = {spec - (1 << 4) + (7 << 8) + (shade << 22) + YTEX, p1 - glm::vec3(0.4375f, 0, 0)};
								v3 = {spec - (1 << 4) + (7 << 8) + (shade << 22) + XTEX + YTEX, p5 - glm::vec3(0.4375f, 0, 0)};
								face_vertices(_vertices, v0, v1, v2, v3);
							}
						}
						if (visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
							spec = baseSpec + (5 << 19);
							faceLight = computeLight(row, col, level - 1);
							spec += (faceLight << 24);
							if ((block_value >> 12) & FENCE::MY) {
								v0 = {spec + (shade << 22), p2};
								v1 = {spec + (shade << 22) + XTEX, p3};
							} else {
								v0 = {spec + (7 << 8) + (shade << 22), p2 + glm::vec3(0, 0.4375f, 0)};
								v1 = {spec + (7 << 8) + (shade << 22) + XTEX, p3 + glm::vec3(0, 0.4375f, 0)};
							}
							if ((block_value >> 12) & FENCE::PY) {
								v2 = {spec + (shade << 22) + YTEX, p6};
								v3 = {spec + (shade << 22) + XTEX + YTEX, p7};
							} else {
								v2 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + YTEX, p6 - glm::vec3(0, 0.4375f, 0)};
								v3 = {spec - (1 << 4) + (9 << 8) + (shade << 22) + XTEX + YTEX, p7 - glm::vec3(0, 0.4375f, 0)};
							}
							face_vertices(_vertices, v0, v1, v2, v3);
							if ((block_value >> 12) & FENCE::MX) {
								v0 = {spec + (shade << 22), p6};
								v1 = {spec + (shade << 22) + XTEX, p2};
								v2 = {spec - (1 << 4) + (7 << 8) + (shade << 22) + YTEX, p6 + glm::vec3(0.4375f, 0, 0)};
								v3 = {spec - (1 << 4) + (7 << 8) + (shade << 22) + XTEX + YTEX, p2 + glm::vec3(0.4375f, 0, 0)};
								face_vertices(_vertices, v0, v1, v2, v3);
							}
							if ((block_value >> 12) & FENCE::PX) {
								v0 = {spec + (shade << 22), p7};
								v1 = {spec + (shade << 22) + XTEX, p3};
								v2 = {spec - (1 << 4) + (7 << 8) + (shade << 22) + YTEX, p7 - glm::vec3(0.4375f, 0, 0)};
								v3 = {spec - (1 << 4) + (7 << 8) + (shade << 22) + XTEX + YTEX, p3 - glm::vec3(0.4375f, 0, 0)};
								face_vertices(_vertices, v0, v1, v2, v3);
							}
						}



					} else if (type < blocks::POPPY) {
						int offset = 0;
						int orientation = -1, litFurnace = 0;
						if (type >= blocks::CRAFTING_TABLE && type < blocks::BEDROCK) {
							orientation = (block_value >> 9) & 0x7;
							litFurnace = (block_value >> 12) & 0x1;
							offset = orientation + (litFurnace << 4);
						} else if (type == blocks::FARMLAND) {
							offset = (block_value & blocks::WET_FARMLAND);
						} else if (type == blocks::OAK_LOG) {
							offset = (block_value >> 9) & 0x3;
						}
						if (visible_face(type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
							spec = s_blocks[type]->texX(face_dir::MINUSX, offset) + (s_blocks[type]->texY(face_dir::MINUSX, offset) << 4) + (3 << 19);
							faceLight = computeLight(row - 1, col, level);
							cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
							shade = computeShade(row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
							// spec += (faceLight << 24);
							v0 = {spec + (cornerLight << 24) + (shade << 22), p4};
							cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
							shade = computeShade(row - 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
							v1 = {spec + (cornerLight << 24) + (shade << 22) + XTEX, p0};
							cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
							shade = computeShade(row - 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
							v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p6};
							cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
							shade = computeShade(row - 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
							v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p2};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
							spec = s_blocks[type]->texX(face_dir::PLUSX, offset) + (s_blocks[type]->texY(face_dir::PLUSX, offset) << 4) + (4 << 19);
							faceLight = computeLight(row + 1, col, level);
							cornerLight = computeSmoothLight(faceLight, row + 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
							shade = computeShade(row + 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
							// spec += (faceLight << 24);
							v0 = {spec + (cornerLight << 24) + (shade << 22), p1};
							cornerLight = computeSmoothLight(faceLight, row + 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
							shade = computeShade(row + 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
							v1 = {spec + (cornerLight << 24) + (shade << 22) + XTEX, p5};
							cornerLight = computeSmoothLight(faceLight, row + 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
							shade = computeShade(row + 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
							v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p3};
							cornerLight = computeSmoothLight(faceLight, row + 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
							shade = computeShade(row + 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
							v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p7};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
							spec = s_blocks[type]->texX(face_dir::MINUSY, offset) + (s_blocks[type]->texY(face_dir::MINUSY, offset) << 4) + (1 << 19);
							faceLight = computeLight(row, col - 1, level);
							cornerLight = computeSmoothLight(faceLight, row, col - 1, level, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
							shade = computeShade(row, col - 1, level, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
							// spec += (faceLight << 24);
							v0 = {spec + (cornerLight << 24) + (shade << 22), p0};
							cornerLight = computeSmoothLight(faceLight, row, col - 1, level, {1, 0, 0, 1, 0, 1, 0, 0, 1});
							shade = computeShade(row, col - 1, level, {1, 0, 0, 1, 0, 1, 0, 0, 1});
							v1 = {spec + (cornerLight << 24) + (shade << 22) + XTEX, p1};
							cornerLight = computeSmoothLight(faceLight, row, col - 1, level, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
							shade = computeShade(row, col - 1, level, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
							v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p2};
							cornerLight = computeSmoothLight(faceLight, row, col - 1, level, {1, 0, 0, 1, 0, -1, 0, 0, -1});
							shade = computeShade(row, col - 1, level, {1, 0, 0, 1, 0, -1, 0, 0, -1});
							v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p3};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
							spec = s_blocks[type]->texX(face_dir::PLUSY, offset) + (s_blocks[type]->texY(face_dir::PLUSY, offset) << 4) + (2 << 19);
							faceLight = computeLight(row, col + 1, level);
							cornerLight = computeSmoothLight(faceLight, row, col + 1, level, {1, 0, 0, 1, 0, 1, 0, 0, 1});
							shade = computeShade(row, col + 1, level, {1, 0, 0, 1, 0, 1, 0, 0, 1});
							// spec += (faceLight << 24);
							v0 = {spec + (cornerLight << 24) + (shade << 22), p5};
							cornerLight = computeSmoothLight(faceLight, row, col + 1, level, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
							shade = computeShade(row, col + 1, level, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
							v1 = {spec + (cornerLight << 24) + (shade << 22) + XTEX, p4};
							cornerLight = computeSmoothLight(faceLight, row, col + 1, level, {1, 0, 0, 1, 0, -1, 0, 0, -1});
							shade = computeShade(row, col + 1, level, {1, 0, 0, 1, 0, -1, 0, 0, -1});
							v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p7};
							cornerLight = computeSmoothLight(faceLight, row, col + 1, level, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
							shade = computeShade(row, col + 1, level, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
							v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p6};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
							spec = s_blocks[type]->texX(face_dir::PLUSZ, offset) + (s_blocks[type]->texY(face_dir::PLUSZ, offset) << 4);
							faceLight = computeLight(row, col, level + 1);
							cornerLight = computeSmoothLight(faceLight, row, col, level + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
							shade = computeShade(row, col, level + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
							// spec += (faceLight << 24);
							if (type == blocks::DIRT_PATH) {
								p4.z -= ONE_SIXTEENTH;
								p5.z -= ONE_SIXTEENTH;
								p0.z -= ONE_SIXTEENTH;
								p1.z -= ONE_SIXTEENTH;
							}
							v0 = {spec + (cornerLight << 24) + (shade << 22), p4};
							cornerLight = computeSmoothLight(faceLight, row, col, level + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
							shade = computeShade(row, col, level + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
							v1 = {spec + (cornerLight << 24) + (shade << 22) + XTEX, p5};
							cornerLight = computeSmoothLight(faceLight, row, col, level + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
							shade = computeShade(row, col, level + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
							v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p0};
							cornerLight = computeSmoothLight(faceLight, row, col, level + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
							shade = computeShade(row, col, level + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
							v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p1};
							face_vertices(_vertices, v0, v1, v2, v3);
						}
						if (visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
							spec = s_blocks[type]->texX(face_dir::MINUSZ, offset) + (s_blocks[type]->texY(face_dir::MINUSZ, offset) << 4) + (5 << 19);
							faceLight = computeLight(row, col, level - 1);
							cornerLight = computeSmoothLight(faceLight, row, col, level - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
							shade = computeShade(row, col, level - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
							// spec += (faceLight << 24);
							v0 = {spec + (cornerLight << 24) + (shade << 22), p2};
							cornerLight = computeSmoothLight(faceLight, row, col, level - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
							shade = computeShade(row, col, level - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
							v1 = {spec + (cornerLight << 24) + (shade << 22) + XTEX, p3};
							cornerLight = computeSmoothLight(faceLight, row, col, level - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
							shade = computeShade(row, col, level - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
							v2 = {spec + (cornerLight << 24) + (shade << 22) + YTEX, p6};
							cornerLight = computeSmoothLight(faceLight, row, col, level - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
							shade = computeShade(row, col, level - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
							v3 = {spec + (cornerLight << 24) + (shade << 22) + XTEX + YTEX, p7};
							face_vertices(_vertices, v0, v1, v2, v3);
						}



					} else if (type == blocks::TORCH) {
						// TODO for now torches only in default middle-of-block configuration
						spec = s_blocks[type]->texX() + (s_blocks[type]->texY() << 4) + (0 << 19) + (15 << 24);
						switch ((block_value >> 9) & 0x7) { // orientation
							case face_dir::MINUSZ: // default
								v0 = {spec, {p4.x + 7.0 / 16, p4.y, p4.z}};
								v1 = {spec + XTEX, {p0.x + 7.0 / 16, p0.y, p0.z}};
								v2 = {spec + YTEX, {p6.x + 7.0 / 16, p6.y, p6.z}};
								v3 = {spec + XTEX + YTEX, {p2.x + 7.0 / 16, p2.y, p2.z}};
								face_vertices(_vertices, v0, v1, v2, v3); // -x
								v0 = {spec, {p0.x + 9.0 / 16, p0.y, p0.z}};
								v1 = {spec + XTEX, {p4.x + 9.0 / 16, p4.y, p4.z}};
								v2 = {spec + YTEX, {p2.x + 9.0 / 16, p2.y, p2.z}};
								v3 = {spec + XTEX + YTEX, {p6.x + 9.0 / 16, p6.y, p6.z}};
								face_vertices(_vertices, v0, v1, v2, v3); // +x
								v0 = {spec, {p0.x, p0.y + 7.0 / 16, p0.z}};
								v1 = {spec + XTEX, {p1.x, p1.y + 7.0 / 16, p1.z}};
								v2 = {spec + YTEX, {p2.x, p2.y + 7.0 / 16, p2.z}};
								v3 = {spec + XTEX + YTEX, {p3.x, p3.y + 7.0 / 16, p3.z}};
								face_vertices(_vertices, v0, v1, v2, v3); // -y
								v0 = {spec, {p1.x, p1.y + 9.0 / 16, p1.z}};
								v1 = {spec + XTEX, {p0.x, p0.y + 9.0 / 16, p0.z}};
								v2 = {spec + YTEX, {p3.x, p3.y + 9.0 / 16, p3.z}};
								v3 = {spec + XTEX + YTEX, {p2.x, p2.y + 9.0 / 16, p2.z}};
								face_vertices(_vertices, v0, v1, v2, v3); // +y
								break ;
							case face_dir::PLUSX:
								v0 = {spec, {p4.x + 8.6 / 16, p4.y, p4.z + 3.0/16}};
								v1 = {spec + XTEX, {p0.x + 8.6 / 16, p0.y, p0.z + 3.0/16}};
								v2 = {spec + YTEX, {p6.x + 15.0 / 16, p6.y, p6.z + 3.0/16}};
								v3 = {spec + XTEX + YTEX, {p2.x + 15.0 / 16, p2.y, p2.z + 3.0/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // -x
								v0 = {spec, {p0.x + 10.6 / 16, p0.y, p0.z + 4.0/16}};
								v1 = {spec + XTEX, {p4.x + 10.6 / 16, p4.y, p4.z + 4.0/16}};
								v2 = {spec + YTEX, {p2.x + 17.0 / 16, p2.y, p2.z + 4.0/16}};
								v3 = {spec + XTEX + YTEX, {p6.x + 17.0 / 16, p6.y, p6.z + 4.0/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // +x
								v0 = {spec, {p0.x + 1.6/16, p0.y + 7.0 / 16, p0.z - 0.5/16}};
								v1 = {spec + XTEX, {p1.x + 1.6/16, p1.y + 7.0 / 16, p1.z + 7.5/16}};
								v2 = {spec + YTEX, {p2.x + 8.0/16, p2.y + 7.0 / 16, p2.z - 0.5/16}};
								v3 = {spec + XTEX + YTEX, {p3.x + 8.0/16, p3.y + 7.0 / 16, p3.z + 7.5/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // -y
								v0 = {spec, {p1.x + 1.6/16, p1.y + 9.0 / 16, p1.z + 7.5/16}};
								v1 = {spec + XTEX, {p0.x + 1.6/16, p0.y + 9.0 / 16, p0.z - 0.5/16}};
								v2 = {spec + YTEX, {p3.x + 8.0/16, p3.y + 9.0 / 16, p3.z + 7.5/16}};
								v3 = {spec + XTEX + YTEX, {p2.x + 8.0/16, p2.y + 9.0 / 16, p2.z - 0.5/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // +y
								break ;
							case face_dir::MINUSX:
								v0 = {spec, {p4.x + 1 - 8.6/16, p4.y, p4.z + 3.0/16}};
								v1 = {spec + XTEX, {p0.x + 1 - 8.6/16, p0.y, p0.z + 3.0/16}};
								v2 = {spec + YTEX, {p6.x + 1 - 15.0/16, p6.y, p6.z + 3.0/16}};
								v3 = {spec + XTEX + YTEX, {p2.x + 1 - 15.0/16, p2.y, p2.z + 3.0/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // -x
								v0 = {spec, {p0.x + 1 - 10.6/16, p0.y, p0.z + 4.0/16}};
								v1 = {spec + XTEX, {p4.x + 1 - 10.6/16, p4.y, p4.z + 4.0/16}};
								v2 = {spec + YTEX, {p2.x + 1 - 17.0/16, p2.y, p2.z + 4.0/16}};
								v3 = {spec + XTEX + YTEX, {p6.x + 1 - 17.0/16, p6.y, p6.z + 4.0/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // +x
								v0 = {spec, {p0.x - 1.6/16, p0.y + 7.0 / 16, p0.z + 7.5/16}};
								v1 = {spec + XTEX, {p1.x - 1.6/16, p1.y + 7.0 / 16, p1.z - 0.5/16}};
								v2 = {spec + YTEX, {p2.x - 8.0/16, p2.y + 7.0 / 16, p2.z + 7.5/16}};
								v3 = {spec + XTEX + YTEX, {p3.x - 8.0/16, p3.y + 7.0 / 16, p3.z - 0.5/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // -y
								v0 = {spec, {p1.x - 1.6/16, p1.y + 9.0 / 16, p1.z - 0.5/16}};
								v1 = {spec + XTEX, {p0.x - 1.6/16, p0.y + 9.0 / 16, p0.z + 7.5/16}};
								v2 = {spec + YTEX, {p3.x - 8.0/16, p3.y + 9.0 / 16, p3.z - 0.5/16}};
								v3 = {spec + XTEX + YTEX, {p2.x - 8.0/16, p2.y + 9.0 / 16, p2.z + 7.5/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // +y
								break ;
							case face_dir::PLUSY:
								v0 = {spec, {p4.x + 7.0/16, p4.y + 1.6 / 16, p4.z + 7.5/16}};
								v1 = {spec + XTEX, {p0.x + 7.0/16, p0.y + 1.6 / 16, p0.z - 0.5/16}};
								v2 = {spec + YTEX, {p6.x + 7.0/16, p6.y + 8.0 / 16, p6.z + 7.5/16}};
								v3 = {spec + XTEX + YTEX, {p2.x + 7.0/16, p2.y + 8.0 / 16, p2.z - 0.5/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // -x
								v0 = {spec, {p0.x + 9.0 / 16, p0.y + 1.6/16, p0.z - 0.5/16}};
								v1 = {spec + XTEX, {p4.x + 9.0 / 16, p4.y + 1.6/16, p4.z + 7.5/16}};
								v2 = {spec + YTEX, {p2.x + 9.0 / 16, p2.y + 8.0/16, p2.z - 0.5/16}};
								v3 = {spec + XTEX + YTEX, {p6.x + 9.0 / 16, p6.y + 8.0/16, p6.z + 7.5/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // +x
								v0 = {spec, {p0.x, p0.y + 8.6 / 16, p0.z + 3.0/16}};
								v1 = {spec + XTEX, {p1.x, p1.y + 8.6 / 16, p1.z + 3.0/16}};
								v2 = {spec + YTEX, {p2.x, p2.y + 15.0 / 16, p2.z + 3.0/16}};
								v3 = {spec + XTEX + YTEX, {p3.x, p3.y + 15.0 / 16, p3.z + 3.0/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // -y
								v0 = {spec, {p1.x, p1.y + 10.6 / 16, p1.z + 4.0/16}};
								v1 = {spec + XTEX, {p0.x, p0.y + 10.6 / 16, p0.z + 4.0/16}};
								v2 = {spec + YTEX, {p3.x, p3.y + 17.0 / 16, p3.z + 4.0/16}};
								v3 = {spec + XTEX + YTEX, {p2.x, p2.y + 17.0 / 16, p2.z + 4.0/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // +y
								break ;
							case face_dir::MINUSY:
								v0 = {spec, {p4.x + 7.0/16, p4.y - 1.6 / 16, p4.z - 0.5/16}};
								v1 = {spec + XTEX, {p0.x + 7.0/16, p0.y - 1.6 / 16, p0.z + 7.5/16}};
								v2 = {spec + YTEX, {p6.x + 7.0/16, p6.y - 8.0 / 16, p6.z - 0.5/16}};
								v3 = {spec + XTEX + YTEX, {p2.x + 7.0/16, p2.y - 8.0 / 16, p2.z + 7.5/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // -x
								v0 = {spec, {p0.x + 9.0 / 16, p0.y - 1.6/16, p0.z + 7.5/16}};
								v1 = {spec + XTEX, {p4.x + 9.0 / 16, p4.y - 1.6/16, p4.z - 0.5/16}};
								v2 = {spec + YTEX, {p2.x + 9.0 / 16, p2.y - 8.0/16, p2.z + 7.5/16}};
								v3 = {spec + XTEX + YTEX, {p6.x + 9.0 / 16, p6.y - 8.0/16, p6.z - 0.5/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // +x
								v0 = {spec, {p0.x, p0.y + 1 - 8.6 / 16, p0.z + 3.0/16}};
								v1 = {spec + XTEX, {p1.x, p1.y + 1 - 8.6 / 16, p1.z + 3.0/16}};
								v2 = {spec + YTEX, {p2.x, p2.y + 1 - 15.0 / 16, p2.z + 3.0/16}};
								v3 = {spec + XTEX + YTEX, {p3.x, p3.y + 1 - 15.0 / 16, p3.z + 3.0/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // -y
								v0 = {spec, {p1.x, p1.y + 1 - 10.6 / 16, p1.z + 4.0/16}};
								v1 = {spec + XTEX, {p0.x, p0.y + 1 - 10.6 / 16, p0.z + 4.0/16}};
								v2 = {spec + YTEX, {p3.x, p3.y + 1 - 17.0 / 16, p3.z + 4.0/16}};
								v3 = {spec + XTEX + YTEX, {p2.x, p2.y + 1 - 17.0 / 16, p2.z + 4.0/16}};
								face_vertices(_vertices, v0, v1, v2, v3); // +y
								break ;
						}



					} else { // flowers
						spec = s_blocks[type]->texX() + (s_blocks[type]->texY() << 4) + (0 << 19);
						faceLight = computeLight(row, col, level);
						spec += (faceLight << 24);
						v0 = {spec, p0};
						v1 = {spec + XTEX, p5};
						v2 = {spec + YTEX, p2};
						v3 = {spec + XTEX + YTEX, p7};
						face_vertices(_vertices, v0, v1, v2, v3);
						v0 = {spec, p1};
						v1 = {spec + XTEX, p4};
						v2 = {spec + YTEX, p3};
						v3 = {spec + XTEX + YTEX, p6};
						face_vertices(_vertices, v0, v1, v2, v3);
					}
				}
			}
		}
	}
	_mtx.unlock();
	_light_update = false;
	_vertex_update = false;
	_vaoReset = false;
	_vaoVIP = true;
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

GLint Chunk::getCamLightLevel( glm::ivec3 location )
{
	return (getLightLevel(location.x - _startX, location.y - _startY, location.z));
}

int Chunk::computePosLight( glm::vec3 pos )
{
	return (computeLight(glm::floor(pos.x - _startX), glm::floor(pos.y - _startY), glm::floor(pos.z)));
}

short Chunk::getLightLevel( int posX, int posY, int posZ )
{
	if (!_lights || posZ < 0) {
		return (0);
	} else if (posZ >= WORLD_HEIGHT) {
		return (0xFF00);
	}
	if (posX < 0) {
		if (_neighbours[face_dir::MINUSX]) {
			return (_neighbours[face_dir::MINUSX]->getLightLevel(posX + CHUNK_SIZE, posY, posZ));
		}
	} else if (posX >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSX]) {
			return (_neighbours[face_dir::PLUSX]->getLightLevel(posX - CHUNK_SIZE, posY, posZ));
		}
	} else if (posY < 0) {
		if (_neighbours[face_dir::MINUSY]) {
			return (_neighbours[face_dir::MINUSY]->getLightLevel(posX, posY + CHUNK_SIZE, posZ));
		}
	} else if (posY >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSY]) {
			return (_neighbours[face_dir::PLUSY]->getLightLevel(posX, posY - CHUNK_SIZE, posZ));
		}
	} else {
		return (_lights[(((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ]);
	}
	return (0xF00);
}

void Chunk::light_try_spread( int posX, int posY, int posZ, short level, bool skySpread, int recurse )
{
	if (posZ < 0 || posZ > 255) {
		return ;
	}
	if (posX < 0 || posX >= CHUNK_SIZE || posY < 0 || posY >= CHUNK_SIZE) {
		// spread neighbour
		if (posX == -1) {
			if (_neighbours[face_dir::MINUSX]) {
				_neighbours[face_dir::MINUSX]->light_try_spread(CHUNK_SIZE - 1, posY, posZ, level, skySpread, recurse);
			}
		} else if (posY == -1) {
			if (_neighbours[face_dir::MINUSY]) {
				_neighbours[face_dir::MINUSY]->light_try_spread(posX, CHUNK_SIZE - 1, posZ, level, skySpread, recurse);
			}
		} else if (posX == CHUNK_SIZE) {
			if (_neighbours[face_dir::PLUSX]) {
				_neighbours[face_dir::PLUSX]->light_try_spread(0, posY, posZ, level, skySpread, recurse);
			}
		} else if (posY == CHUNK_SIZE) {
			if (_neighbours[face_dir::PLUSY]) {
				_neighbours[face_dir::PLUSY]->light_try_spread(posX, 0, posZ, level, skySpread, recurse);
			}
		}
	} else {
		int type = _blocks[(((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ] & 0xFF;
		// if (air_flower(type, false, true, true) && type != blocks::OAK_SLAB_BOTTOM && type != blocks::FARMLAND && type != blocks::DIRT_PATH) {
		// 	return ;
		// }
		if (!s_blocks[type]->transparent) {
			return ;
		}
		short neighbour = (_lights[(((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ] >> (8 * skySpread));
		if ((!(neighbour & 0xF0) || (skySpread && !(level & 0xF0))) && ((neighbour & 0xF) != maxs(0, ((level & 0xF) - 1) & 0xF))) { // only spread to non source blocks whose value is not expected value. Also spread to source block if from non source block and skySpread
			light_spread(posX, posY, posZ, skySpread, recurse);
		}
	}
}
