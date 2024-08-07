#include "OpenGL_Manager.hpp"
#include "Settings.hpp"
#include "logs.hpp"
#include "callbacks.hpp"
#include <fstream>
extern siv::PerlinNoise::seed_type perlin_seed;

// ************************************************************************** //
//                                 Upload                                     //
// ************************************************************************** //

void OpenGL_Manager::saveWorld( void )
{
	// first delete chunks, this updates _backups
	stopThread();
	clearChunks();
	_block_hit = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 0, 0, 0};

	// then store everything
	std::string json = "{\n\t\"version\": \"" + settings::consts::json::versions[settings::consts::json::nbr_versions - 1]
		+ "\",\n\t\"seed\": " + std::to_string(perlin_seed)
		+ ",\n\t\"game_mode\": " + std::to_string(_game_mode)
		+ ",\n\t\"debug_mode\": " + ((_debug_mode) ? "true" : "false")
		+ ",\n\t\"f5_mode\": " + ((_ui->_hideUI) ? "true" : "false")
		+ ",\n\t\"outline\": " + ((_outline) ? "true" : "false")
		+ ",\n\t\"flat_world\": " + std::to_string(Settings::Get()->getInt(settings::ints::flat_world_block))
		+ DayCycle::Get()->saveString()
		+ _player->saveString()
		+ _inventory->saveString()
		+ saveBackupString()
		+ "\n}";
	try {
		std::ofstream ofs("Worlds/" + _world_name, std::ofstream::out | std::ofstream::trunc);
		ofs << json;
		ofs.close();
	}
	catch (std::exception & e) {
		LOGERROR(e.what() << "\nworld save on Worlds/" << _world_name << " failure .. hope you did some snapshot");
	}
	glClearColor(0, 0, 0, 1);
	startThread();
}

std::string DayCycle::saveString( void )
{
	std::string res = ",\n\t\"dayCycle\": {\"day\": "
		+ std::to_string(_day) + ", \"ticks\": " + std::to_string(_ticks)
		+ "},\n\t\"time_multiplier\": " + std::to_string(_timeMultiplier) + ",\n\t";
	return (res);
}

std::string Player::saveString( void )
{
	std::string res = "\"camera\": {\n\t\t\"pos\": {\"x\": "
		+ std::to_string(_position.x) + ", \"y\": " + std::to_string(_position.y) + ", \"z\": " + std::to_string(_position.z)
		+ "},\n\t\t\"spawnpoint\": {\"x\": "
		+ std::to_string(_spawnpoint.x) + ", \"y\": " + std::to_string(_spawnpoint.y) + ", \"z\": " + std::to_string(_spawnpoint.z)
		+ "},\n\t\t\"yaw\": " + std::to_string(_yaw)
		+ ",\n\t\t\"pitch\": " + std::to_string(_pitch)
		+ ",\n\t\t\"health_points\": " + std::to_string(_health)
		+ ",\n\t\t\"foodLevel\": " + std::to_string(_foodLevel)
		+ ",\n\t\t\"foodSaturation\": " + std::to_string(_foodSaturationLevel)
		+ ",\n\t\t\"touch_ground\": " + ((_touchGround) ? "true" : "false")
		+ "\n\t},\n\t";
	return (res);
}

static void saveItem( std::string& res, t_item item )
{
	std::string title;
	res += '[' + std::to_string(item.type) + ", " + std::to_string(item.amount);
	if (item.tag) {
		switch (item.tag->getType()) {
			case tags::name_tag:
				res += ", {\"nameTag\": [";
				title = item.tag->getName();
				res += std::to_string(title.size()) + ", \"";
				for (char c : title) {
					if (c == '"') res += "\\\"";
					else if (c == '\n') res += "\\n";
					else res += c;
				}
				res += "\"]}";
				break ;
			case tags::tool_tag:
				res += ", {\"toolTag\": [";
				title = item.tag->getName();
				res += std::to_string(title.size()) + ", \"";
				for (char c : title) {
					if (c == '"') res += "\\\"";
					else if (c == '\n') res += "\\n";
					else res += c;
				}
				res += "\", " + std::to_string(static_cast<ToolTag*>(item.tag.get())->getDura()) + "]}";
				break ;
			case tags::book_tag:
				res += ", {\"bookTag\": [";
				title = item.tag->getName();
				res += std::to_string(title.size()) + ", \"";
				for (char c : title) {
					if (c == '"') res += "\\\"";
					else if (c == '\n') res += "\\n";
					else res += c;
				}
				res += "\"";
				for (auto page : static_cast<BookTag*>(item.tag.get())->getContent()) {
					res += ", " + std::to_string(page.size()) + ", \"";
					for (char c : page) {
						if (c == '"') res += "\\\"";
						else if (c == '\n') res += "\\n";
						else res += c;
					}
					res += "\"";
				}
				res += "]}";
				break ;
			default:
				LOGERROR("saveItem tag with unrecognised type.");
		}
	}
	res += ']';
}

std::string Inventory::saveString( void )
{
	std::string res = "\"inventory\": {\n\t\t\"slot\": " + std::to_string(_slot)
		+ ",\n\t\t\"content\": [";
	for (int index = 0; index < 9; index++) {
		saveItem(res, _content[index]);
		if (index < 8) {
			res += ", ";
		}
	}
	res += "],\n\t\t\"backpack\": [";
	for (int index = 0; index < 27; index++) {
		saveItem(res, _backpack[index]);
		if (index < 26) {
			res += ", ";
		}
	}
	res += "]\n\t},\n\t";
	spillInventory(NULL);
	setSlot(0);
	return (res);
}

std::string OpenGL_Manager::saveBackupString( void )
{
	std::string res = "\"backups\": [";
	bool start = true;
	mtx_backup.lock();
	for (auto& bup: _backups) {
		if (!start) {
			res += ',';
		}
		start = false;
		res += "\n\t\t{\"pos\": [" + std::to_string(bup.first.first) + ", " + std::to_string(bup.first.second);
		if (bup.second.added.size()) {
			res += "], \n\t\t\"added\": [";
			bool astart = true;
			for (auto& add: bup.second.added) {
				if (!astart) {
					res += ", ";
				}
				astart = false;
				res += "[" + std::to_string(add.first) + ", " + std::to_string(add.second) + ']';
			}
		}
		if (bup.second.removed.size()) {
			res += "],\n\t\t\"removed\": [";
			bool rmstart = true;
			for (auto& rm: bup.second.removed) {
				if (!rmstart) {
					res += ", ";
				}
				rmstart = false;
				res += "[" + std::to_string(rm) + ']';
			}
		}
		if (bup.second.chests.size()) {
			res += "],\n\t\t\"chests\": [";
			bool chstart = true;
			for (auto& ch: bup.second.chests) {
				if (!chstart) {
					res += ", ";
				}
				chstart = false;
				res += "{\"pos\": " + std::to_string(ch.first)
					+ ", \"orientation\": " + std::to_string(ch.second->getOrientation())
					+ ", \"content\": [";
				for (int index = 0; index < 27; index++) {
					saveItem(res, *(ch.second->getItem(index)));
					if (index < 26) {
						res += ", ";
					}
				}
				res += "]}";
			}
		}
		if (bup.second.furnaces.size()) {
			res += "],\n\t\t\"furnaces\": [";
			bool fstart = true;
			for (auto& fur: bup.second.furnaces) {
				if (!fstart) {
					res += ", ";
				}
				fstart = false;
				res += "{\"pos\": " + std::to_string(fur.first) + ", \"composant\": ";
				saveItem(res, fur.second->getComposant());
				res += ", \"fuel\": ";
				saveItem(res, fur.second->getFuel());
				res += ", \"production\": ";
				saveItem(res, fur.second->getProduction());
				res += "}"; // TODO add current times for better backup
			}
		}
		if (bup.second.signs.size()) {
			res += "],\n\t\t\"signs\": [";
			bool fstart = true;
			for (auto& sign: bup.second.signs) {
				if (!fstart) {
					res += ", ";
				}
				fstart = false;
				res += "{\"pos\": " + std::to_string(sign.first) + ", \"value\": " + std::to_string(sign.second->getValue()) + ", \"content\": [";
				bool lstart = true;
				for (int i = 0; i < 4; ++i) {
					std::string line = sign.second->getContent(i);
					if (!lstart) {
						res += ", ";
					}
					lstart = false;
					res += std::to_string(line.size()) + ", \"";
					for (char c : line) {
						if (c == '"') res += "\\\"";
						else res += c;
					}
					res += "\"";
				}
				res += "]}";
			}
		}
		if (bup.second.entities.size()) {
			res += "],\n\t\t\"entities\": [";
			bool fstart = true;
			for (auto& frame: bup.second.entities) {
				if (!fstart) {
					res += ", ";
				}
				fstart = false;
				switch (frame->getType()) {
					case entities::item_frame:
						static_cast<ItemFrameEntity*>(frame.get())->saveString(res);
						break ;
					case entities::lectern:
						static_cast<LecternEntity*>(frame.get())->saveString(res);
						break ;
				}
			}
		}
		res += "]}";
	}
	_backups.clear();
	mtx_backup.unlock();
	res += "\n\t]";
	return (res);
}

void ItemFrameEntity::saveString( std::string& str )
{
	int value = ((_front.z > .5f) ? placement::ceiling : (_front.z < -.5f) ? placement::floor : placement::wall) << offset::blocks::bitfield;
	value |= (((_front.x < -.5f) ? face_dir::plus_x : (_front.x > .5f) ? face_dir::minus_x
				: (_front.y < -.5f) ? face_dir::plus_y : (_front.y > .5f) ? face_dir::minus_y : 0) << offset::blocks::orientation);
	str += "\"item_frame\", {\"pos\": [" + std::to_string(static_cast<int>(glm::floor(_pos.x))) + ", " + std::to_string(static_cast<int>(glm::floor(_pos.y))) + ", " + std::to_string(static_cast<int>(glm::floor(_pos.z)))
		+ "], \"value\": " + std::to_string(value) + ", \"item\": ";
	saveItem(str, _item);
	str += ", \"rotation\": " + std::to_string(_rotation) + "}";
}

void LecternEntity::saveString( std::string& str )
{
	int value = (((_front.x < -.8f) ? face_dir::plus_x : (_front.x > .8f) ? face_dir::minus_x
				: (_front.y < -.8f) ? face_dir::plus_y : (_front.y > .8f) ? face_dir::minus_y : 0) << offset::blocks::orientation);
	str += "\"lectern\", {\"pos\": [" + std::to_string(static_cast<int>(glm::floor(_pos.x))) + ", " + std::to_string(static_cast<int>(glm::floor(_pos.y))) + ", " + std::to_string(static_cast<int>(glm::floor(_pos.z)))
		+ "], \"value\": " + std::to_string(value) + ", \"item\": ";
	saveItem(str, _item);
	str += ", \"page\": " + std::to_string(_page) + "}";
}

// ************************************************************************** //
//                                Download                                    //
// ************************************************************************** //

class InvalidFileException : public std::exception
{
	public:
		const char *what() const throw()
		{
			return ("[InvalidFileException] Input file could not be opened.");
		}
};

class UnclosedBracketException : public std::exception
{
	public:
		const char *what() const throw()
		{
			return ("[UnclosedBracketException] Reached end of file with an unclosed bracket.");
		}
};

void OpenGL_Manager::loadWorld( std::string file )
{
	try {
		std::ofstream ofs("Worlds/loading.log", std::ofstream::out | std::ofstream::trunc);

		LOADINGLOG(ofs << "Opening file " << file << std::endl);
		std::ifstream indata(file.c_str());
		if (!indata.is_open()) {
			throw InvalidFileException();
		}
		Settings::Get()->setInt(settings::ints::json_version, 0); // reset version
		DayCycle::Get()->setTimeMultiplier(1); // reset timeMultiplier
		std::string line;
		while (!indata.eof()) {
			std::getline(indata, line);
			line = utils::string::trim_spaces(line);
			if (line.empty() || line[0] == '#') {
				continue ;
			} else if (line == "{" || line == "}") {
			} else if (!line.compare(0, 11, "\"version\": ")) {
				std::string version = line.substr(12, line.size() - 14);
				int versionInt = 0;
				for (int i = 0; i < settings::consts::json::nbr_versions; ++i) {
					if (version == settings::consts::json::versions[i]) {
						versionInt = i;
						break ;
					}
				}
				Settings::Get()->setInt(settings::ints::json_version, versionInt);
				LOADINGLOG(ofs << "version set to " << version << ", which is index " << versionInt << std::endl);
			} else if (!line.compare(0, 8, "\"seed\": ")) {
				perlin_seed = std::atoi(&line[8]);
				LOADINGLOG(ofs << "seed set to " << perlin_seed << std::endl);
			} else if (!line.compare(0, 13, "\"game_mode\": ")) {
				_game_mode = std::atoi(&line[13]);
				Settings::Get()->setInt(settings::ints::game_mode, _game_mode);
				LOADINGLOG(ofs << "game mode set to " << _game_mode << std::endl);
			} else if (!line.compare(0, 14, "\"debug_mode\": ")) {
				_debug_mode = line.substr(14, 4) == "true";
				LOADINGLOG(ofs << "debug mode set to " << _debug_mode << std::endl);
			} else if (!line.compare(0, 11, "\"f5_mode\": ")) {
				_ui->_hideUI = line.substr(11, 4) == "true";
				Settings::Get()->setBool(settings::bools::hide_ui, _ui->_hideUI);
				LOADINGLOG(ofs << "f5 mode set to " << _ui->_hideUI << std::endl);
			} else if (!line.compare(0, 11, "\"outline\": ")) {
				_outline = line.substr(11, 4) == "true";
			} else if (!line.compare(0, 14, "\"flat_world\": ")) {
				int value = std::atoi(&line[14]);
				value = (value >= 0 && value < S_BLOCKS_SIZE) ? value : blocks::air;
				Settings::Get()->setInt(settings::ints::flat_world_block, value);
				Settings::Get()->setBool(settings::bools::flat_world, value != blocks::air);
				LOADINGLOG(ofs << "flat_world_block set to " << s_blocks[value]->name << std::endl);
			} else if (!line.compare(0, 12, "\"dayCycle\": ")) {
				DayCycle::Get()->loadWorld(ofs, line);
			} else if (!line.compare(0, 19, "\"time_multiplier\": ")) {
				DayCycle::Get()->setTimeMultiplier(std::atoi(&line[19]));
			} else if (!line.compare(0, 10, "\"camera\": ")) {
				_player->loadWorld(ofs, indata);
			} else if (!line.compare(0, 13, "\"inventory\": ")) {
				_inventory->loadWorld(ofs, indata);
			} else if (!line.compare(0, 11, "\"backups\": ")) {
				loadBackups(ofs, indata);
			} else {
				LOADINGLOG(ofs << "FOREIGN LINE IN LOADWORLD: " << line << std::endl);
			}
		}
		indata.close();
		LOADINGLOG(ofs.close());
	}
	catch (std::exception & e) {
		LOGERROR(e.what());
		exit (1); // TODO might want to return to main menu instead
	}
}

void DayCycle::loadWorld( std::ofstream& ofs, std::string line )
{
	(void)ofs;
	int index = 12;
	for (index = 12; line[index] && line[index] != ':'; index++);
	_day = std::atoi(&line[index + 1]);
	for (index += 2; line[index] && line[index] != ':'; index++);
	_ticks = std::atoi(&line[index + 1]);
	_forceReset = true;
	setInternals();
	LOADINGLOG(ofs << "dayCycle " << _ticks << " set to day " << _day << " " << _hour << ":" << _minute << " (light " << _internalLight << ")" << std::endl);
}

void Player::loadWorld( std::ofstream& ofs, std::ifstream& indata )
{
	(void)ofs;
	_lastTp = {0, 0, 0};
	int index;
	std::string line;
	while (!indata.eof()) {
		std::getline(indata, line);
		line = utils::string::trim_spaces(line);
		if (line.empty() || line[0] == '#') {
			continue ;
		} else if (!line.compare(0, 7, "\"pos\": ")) {
			_position.x = std::stof(&line[13]);
			for (index = 13; line[index] && line[index] != ':'; index++);
			_position.y = std::stof(&line[index + 2]);
			for (index = index + 2; line[index] && line[index] != ':'; index++);
			_position.z = std::stof(&line[index + 2]);
			_z0 = _position.z;
			LOADINGLOG(ofs << "camera pos set to " << _position.x << ", " << _position.y << ", " << _position.z << std::endl);
		} else if (!line.compare(0, 14, "\"spawnpoint\": ")) {
			_spawnpoint.x = std::stof(&line[20]);
			for (index = 20; line[index] && line[index] != ':'; index++);
			_spawnpoint.y = std::stof(&line[index + 2]);
			for (index = index + 2; line[index] && line[index] != ':'; index++);
			_spawnpoint.z = std::stof(&line[index + 2]);
			LOADINGLOG(ofs << "camera spawnpoint set to " << _spawnpoint.x << ", " << _spawnpoint.y << ", " << _spawnpoint.z << std::endl);
		} else if (!line.compare(0, 7, "\"yaw\": ")) {
			_yaw = std::stof(&line[7]);
			LOADINGLOG(ofs << "camera yaw set to " << _yaw << std::endl);
		} else if (!line.compare(0, 9, "\"pitch\": ")) {
			_pitch = std::stof(&line[9]);
			LOADINGLOG(ofs << "camera pitch set to " << _pitch << std::endl);
		} else if (!line.compare(0, 17, "\"health_points\": ")) {
			_health = std::atoi(&line[17]);
			LOADINGLOG(ofs << "camera health points set to " << _health << std::endl);
		} else if (!line.compare(0, 13, "\"foodLevel\": ")) {
			_foodLevel = std::atoi(&line[13]);
			LOADINGLOG(ofs << "camera food level set to " << _foodLevel << std::endl);
		} else if (!line.compare(0, 18, "\"foodSaturation\": ")) {
			_foodSaturationLevel = std::stof(&line[18]);
			LOADINGLOG(ofs << "camera food saturation level set to " << _foodSaturationLevel << std::endl);
		} else if (!line.compare(0, 16, "\"touch_ground\": ")) {
			_touchGround = line.substr(16, 4) == "true";
			LOADINGLOG(ofs << "camera touch ground set to " << _touchGround << std::endl);
		} else if (line == "},") {
			updateVectors();
			return ;
		} else {
			LOGERROR("foreigh line in camera: " << line);
		}
	}
	throw UnclosedBracketException();
}

static void convertTag( std::string line, int& index, t_item& item, char sep )
{
	int dura, pageSize;
	bool first;
	switch (Settings::Get()->getInt(settings::ints::json_version)) {
		case 0: // no change until 1.4, just read dura as 3rd arg of item
		case 1:
		case 2:
		case 3:
		case 4:
			for (; line[index] && line[index] != ','; index++);
			dura = std::atoi(&line[index + 2]);
			if (dura) {
				item.tag = std::make_shared<ToolTag>(dura, s_blocks[TYPE(item.type)]->durability);
			}
			for (index = index + 2; line[index] && line[index] != sep; index++);
			return ;
		case 5: // v1.4 added tags to items
			for (; line[index] && line[index] != ',' && line[index] != ']'; index++);
			if (line[index] == ',') {
				if (!line.compare(index + 4, 7, "toolTag")) {
					dura = std::atoi(&line[index + 14]);
					item.tag = std::make_shared<ToolTag>(dura, s_blocks[TYPE(item.type)]->durability);
					index += 13;
				} else if (!line.compare(index + 4, 7, "bookTag")) {
					index += 13;
					item.tag = std::make_shared<BookTag>();
					first = true;
					while (line[index] && line[index] != ']') {
						pageSize = std::atoi(&line[index + 2]);
						for (index = index + 2; line[index] && line[index] != ','; index++);
						index += 3;
						std::string str;
						for (int strIndex = 0; strIndex < pageSize; ++strIndex, ++index) {
							if (line[index] == '\\' && line[index + 1] == '\"') {
								str += '\"';
								++pageSize; ++strIndex; ++index;
							} else if (line[index] == '\\' && line[index + 1] == 'n') {
								str += '\n';
								++pageSize; ++strIndex; ++index;
							} else {
								str += line[index];
							}
						}
						if (first) {
							item.tag->setName(str);
							first = false;
						} else {
							static_cast<BookTag*>(item.tag.get())->pushPage(str);
						}
						for (; line[index] && line[index] != ',' && line[index] != ']'; index++);
					}
				}
			}
			for (index = index + 2; line[index] && line[index] != sep; index++);
			return ;
		case 6:
			for (; line[index] && line[index] != ',' && line[index] != ']'; index++);
			if (line[index] == ',') {
				if (!line.compare(index + 4, 7, "nameTag")) {
					index += 13;
					pageSize = std::atoi(&line[index + 2]);
					for (index = index + 2; line[index] && line[index] != ','; index++);
					index += 3;
					std::string str;
					for (int strIndex = 0; strIndex < pageSize; ++strIndex, ++index) {
						if (line[index] == '\\' && line[index + 1] == '\"') {
							str += '\"';
							++pageSize; ++strIndex; ++index;
						} else if (line[index] == '\\' && line[index + 1] == 'n') {
							str += '\n';
							++pageSize; ++strIndex; ++index;
						} else {
							str += line[index];
						}
					}
					item.tag = std::make_shared<NameTag>();
					item.tag->setName(str);
				} else if (!line.compare(index + 4, 7, "toolTag")) {
					index += 13;
					pageSize = std::atoi(&line[index + 2]);
					for (index = index + 2; line[index] && line[index] != ','; index++);
					index += 3;
					std::string str;
					for (int strIndex = 0; strIndex < pageSize; ++strIndex, ++index) {
						if (line[index] == '\\' && line[index + 1] == '\"') {
							str += '\"';
							++pageSize; ++strIndex; ++index;
						} else if (line[index] == '\\' && line[index + 1] == 'n') {
							str += '\n';
							++pageSize; ++strIndex; ++index;
						} else {
							str += line[index];
						}
					}
					for (; line[index] && line[index] != ',' && line[index] != ']'; index++);
					dura = std::atoi(&line[index + 2]);
					item.tag = std::make_shared<ToolTag>(dura, s_blocks[TYPE(item.type)]->durability);
					item.tag->setName(str);
				} else if (!line.compare(index + 4, 7, "bookTag")) {
					index += 13;
					item.tag = std::make_shared<BookTag>();
					first = true;
					while (line[index] && line[index] != ']') {
						pageSize = std::atoi(&line[index + 2]);
						for (index = index + 2; line[index] && line[index] != ','; index++);
						index += 3;
						std::string str;
						for (int strIndex = 0; strIndex < pageSize; ++strIndex, ++index) {
							if (line[index] == '\\' && line[index + 1] == '\"') {
								str += '\"';
								++pageSize; ++strIndex; ++index;
							} else if (line[index] == '\\' && line[index + 1] == 'n') {
								str += '\n';
								++pageSize; ++strIndex; ++index;
							} else {
								str += line[index];
							}
						}
						if (first) {
							item.tag->setName(str);
							first = false;
						} else {
							static_cast<BookTag*>(item.tag.get())->pushPage(str);
						}
						for (; line[index] && line[index] != ',' && line[index] != ']'; index++);
					}
				}
			}
			for (index = index + 2; line[index] && line[index] != sep; index++);
			return ;
	}
	LOGERROR("json version is " << Settings::Get()->getInt(settings::ints::json_version));
	assert((0 == 1) && "unrecognised json version.");
}

static int convert( int value ) // used when I change s_blocks big time
{
	switch (Settings::Get()->getInt(settings::ints::json_version)) {
		case 0: // v0.0
			// transition from v0.0 to v1.0: 0xF00 becomes 0xF000 and 0xF000 becomes 0xF000000
			// got rid of wheat_crop1-7
			if (value >= blocks::wheat_crop && value <= blocks::wheat_crop + 7) {
				value = blocks::wheat_crop + ((value - blocks::wheat_crop) << offset::blocks::bitfield);
			}
			value = ((value & 0XFFFF00FF) | (((value >> 8) & 0xF) << 12) | (((value >> 12) & 0xF) << 24));
		case 1: // v1.0
			// transition from v1.0 to f1.1: got rid of water1-7
			if (value >= blocks::water && value <= blocks::water + 7) {
				value = blocks::water + ((value - blocks::water) << offset::blocks::bitfield);
			}
		case 2: // v1.1
			// transition from v1.1 to f1.2: merged slab_bottom and slab_top into slab + data_value
			switch (TYPE(value)) {
				case blocks::oak_slab + 1:
				case blocks::stone_slab + 1:
				case blocks::smooth_stone_slab + 1:
				case blocks::cobblestone_slab + 1:
				case blocks::stone_bricks_slab + 1:
					value = (TYPE(value) - 1) | (value & (mask::all_bits - mask::blocks::type - mask::slab::top)) | mask::slab::top;
					break ;
			}
		case 3: // v1.2
			// transition from v1.2 to f1.3: merged stairs_bottom and stairs_top into stairs + data_value
			switch (TYPE(value)) {
				case blocks::oak_stairs + 1:
				case blocks::stone_stairs + 1:
				case blocks::smooth_stone_stairs + 1:
				case blocks::cobblestone_stairs + 1:
				case blocks::stone_bricks_stairs + 1:
					value = (TYPE(value) - 1) | (value & (mask::all_bits - mask::blocks::type - mask::stairs::top)) | mask::stairs::top;
					break ;
			}
		case 4: // v1.3
		case 5: // v1.4 // no change from 1.3 to 1.4 on values
		case 6: // v1.4 // no change from 1.4 to 1.5 on values // current version
			return (value);
	}
	LOGERROR("json version is " << Settings::Get()->getInt(settings::ints::json_version));
	assert((0 == 1) && "unrecognised json version.");
}

void Inventory::loadWorld( std::ofstream & ofs, std::ifstream & indata )
{
	(void)ofs;
	_modif = true;
	int index;
	std::string line;
	while (!indata.eof()) {
		std::getline(indata, line);
		line = utils::string::trim_spaces(line);
		if (line.empty() || line[0] == '#') {
			continue ;
		} else if (!line.compare(0, 8, "\"slot\": ")) {
			_slot = std::atoi(&line[8]);
			LOADINGLOG(ofs << "inventory slot set to " << _slot << std::endl);
		} else if (!line.compare(0, 11, "\"content\": ")) {
			index = 12;
			for (int cindex = 0; cindex < 9 && line[index]; cindex++) {
				_content[cindex].type = convert(std::atoi(&line[index + 1]));
				for (; line[index] && line[index] != ','; index++);
				_content[cindex].amount = std::atoi(&line[index + 2]);
				++index;
				convertTag(line, index, _content[cindex], '[');
				LOADINGLOG(ofs << "inventory slot " << cindex << " set to " << _content[cindex].type << ", " << _content[cindex].amount);
				if (_content[cindex].tag && _content[cindex].tag->getType() == tags::tool_tag) {
					LOADINGLOG(ofs << ", " << static_cast<ToolTag*>(_content[cindex].tag.get())->getDura() << ", " << s_blocks[_content[cindex].type]->durability);
				}
				LOADINGLOG(ofs << std::endl);
			}
		} else if (!line.compare(0, 12, "\"backpack\": ")) {
			index = 13;
			for (int bindex = 0; bindex < 27 && line[index]; bindex++) {
				_backpack[bindex].type = convert(std::atoi(&line[index + 1]));
				for (; line[index] && line[index] != ','; index++);
				_backpack[bindex].amount = std::atoi(&line[index + 2]);
				++index;
				convertTag(line, index, _backpack[bindex], '[');
				LOADINGLOG(ofs << "inventory backpack slot " << bindex << " set to " << _backpack[bindex].type << ", " << _backpack[bindex].amount);
				if (_backpack[bindex].tag && _backpack[bindex].tag->getType() == tags::tool_tag) {
					LOADINGLOG(ofs << ", " << static_cast<ToolTag*>(_backpack[bindex].tag.get())->getDura() << ", " << s_blocks[_backpack[bindex].type]->durability);
				}
				LOADINGLOG(ofs << std::endl);
			}
		} else if (line == "},") {
			return ;
		} else {
			LOGERROR("foreigh line in inventory: " << line);
		}
	}
	throw UnclosedBracketException();
}

void ChestInstance::loadContent( std::ofstream & ofs, std::string &line, int &index )
{
	(void)ofs;
	for (; line[index] && line[index] != '['; ++index);
	index += !!line[index];
	for (int cindex = 0; cindex < 27 && line[index]; cindex++) {
		_content[cindex].type = convert(std::atoi(&line[index + 1]));
		for (; line[index] && line[index] != ','; index++);
		_content[cindex].amount = std::atoi(&line[index + 2]);
		++index;
		convertTag(line, index, _content[cindex], (cindex == 26) ? '}' : '[');
		LOADINGLOG(ofs << "chest content slot " << cindex << " set to " << _content[cindex].type << ", " << _content[cindex].amount);
		if (_content[cindex].tag && _content[cindex].tag->getType() == tags::tool_tag) {
			LOADINGLOG(ofs << ", " << static_cast<ToolTag*>(_content[cindex].tag.get())->getDura() << ", " << s_blocks[_content[cindex].type]->durability);
		}
		LOADINGLOG(ofs << std::endl);
	}
}

void OpenGL_Manager::loadBackups( std::ofstream & ofs, std::ifstream & indata )
{
	int index;
	std::string line;
	while (!indata.eof()) {
		std::getline(indata, line);
		line = utils::string::trim_spaces(line);
		if (line.empty() || line[0] == '#') {
			continue ;
		} else if (!line.compare(0, 8, "{\"pos\": ")) {
			std::pair<int, int> key;
			key.first = std::atoi(&line[9]);
			for (index = 9; line[index] && line[index] != ','; index++);
			key.second = std::atoi(&line[index + 2]);
			s_backup backups_value;
			while (!indata.eof()) {
				std::getline(indata, line);
				line = utils::string::trim_spaces(line);
				if (line.empty() || line[0] == '#') {
					continue ;
				} else if (!line.compare(0, 9, "\"added\": ")) {
					index = 9;
					while (line[index + 1] == '[') {
						int addkey = std::atoi(&line[index + 2]);
						for (; line[index] && line[index] != ','; index++);
						backups_value.added[addkey] = convert(std::atoi(&line[index + 2]));
						LOADINGLOG(ofs << "backups new added " << addkey << ", " << backups_value.added[addkey] << std::endl);
						for (; line[index + 1] && line[index + 1] != '['; index++);
					}
				} else if (!line.compare(0, 11, "\"removed\": ")) {
					index = 11;
					while (line[index + 1] == '[') {
						int rmvalue = std::atoi(&line[index + 2]);
						backups_value.removed.insert(rmvalue);
						LOADINGLOG(ofs << "backups new removed " << rmvalue << std::endl);
						for (index++; line[index + 1] && line[index + 1] != '['; index++);
					}
				} else if (!line.compare(0, 10, "\"chests\": ")) {
					index = 10;
					while (line[index + 1] == '{') {
						int chkey = std::atoi(&line[index + 9]);
						for (index = index + 9; line[index] && line[index] != ':'; index++);
						int orientation = std::atoi(&line[index + 2]);
						backups_value.chests.emplace(chkey, std::make_shared<ChestInstance>(nullptr, glm::ivec3(0), orientation));
						LOADINGLOG(ofs << "one more chest at " << chkey << " with orientation " << orientation <<  std::endl);
						backups_value.chests.at(chkey)->loadContent(ofs, line, index);
						for (; line[index] && line[index] != '{'; index++);
						--index;
					}
				} else if (!line.compare(0, 12, "\"furnaces\": ")) {
					index = 12;
					while (line[index + 1] == '{') {
						int fkey = std::atoi(&line[index + 9]);
						auto fur = std::make_shared<FurnaceInstance>();
						t_item item;
						for (index = index + 9; line[index] && line[index] != ':'; index++);
						item.type = convert(std::atoi(&line[index + 3]));
						for (; line[index] && line[index] != ','; index++);
						item.amount = std::atoi(&line[index + 2]);
						++index;
						convertTag(line, index, item, ':');
						fur->setComposant(item);
						item.type = convert(std::atoi(&line[index + 3]));
						for (; line[index] && line[index] != ','; index++);
						item.amount = std::atoi(&line[index + 2]);
						++index;
						convertTag(line, index, item, ':');
						fur->setFuel(item);
						item.type = convert(std::atoi(&line[index + 3]));
						for (; line[index] && line[index] != ','; index++);
						item.amount = std::atoi(&line[index + 2]);
						++index;
						convertTag(line, index, item, '{');
						fur->setProduction(item);
						backups_value.furnaces[fkey] = fur;
						LOADINGLOG(ofs << "one more furnace at " << fkey << std::endl);
						--index;
					}
				} else if (!line.compare(0, 9, "\"signs\": ")) {
					index = 9;
					while (line[index + 1] == '{') {
						int skey = std::atoi(&line[index + 9]);
						for (index = index + 9; line[index] && line[index] != ':'; index++);
						int value = convert(std::atoi(&line[index + 2]));
						for (index = index + 2; line[index] && line[index] != '['; index++);
						std::vector<std::string> content;
						for (int lineIndex = 0; lineIndex < 4; ++lineIndex) {
							int strSize = std::atoi(&line[index + 1]);
							for (index = index + 2; line[index] && line[index] != ','; index++);
							index += 3;
							std::string str;
							for (int strIndex = 0; strIndex < strSize; ++strIndex, ++index) {
								if (line[index] == '\\' && line[index + 1] == '\"') {
									str += '\"';
									++strSize;
									++strIndex;
									++index;
								} else {
									str += line[index];
								}
							}
							content.push_back(str);
							for (; line[index] && line[index] != ',' && line[index] != ']'; index++);
						}
						glm::ivec3 pos;
						pos.z = skey & (settings::consts::world_height - 1);
						pos.y = ((skey >> settings::consts::world_shift) & (settings::consts::chunk_size - 1));
						pos.x = ((skey >> settings::consts::world_shift) >> settings::consts::chunk_shift);
						auto sign = std::make_shared<SignInstance>(nullptr, value, pos);
						sign->setContent(content);
						backups_value.signs[skey] = sign;
						LOADINGLOG(ofs << "one more sign at " << skey << " with content \"" << content[0] << "\", \"" << content[1] << "\", \"" << content[2] << "\", \"" << content[3] << '\"' << std::endl);
						for (;line[index + 1] && line[index + 1] != '{'; ++index);
					}
				} else if (!line.compare(0, 12, "\"entities\": ")) {
					index = 12;
					while (line[index + 1] == '\"') {
						short type = (line[index + 2] == 'l') ? entities::lectern : entities::item_frame;
						for (index = index + 3; line[index] && line[index] != ','; ++index);
						glm::ivec3 pos;
						pos.x = std::atoi(&line[index + 11]);
						for (index = index + 11; line[index] && line[index] != ','; index++);
						pos.y = std::atoi(&line[index + 2]);
						for (index = index + 2; line[index] && line[index] != ','; index++);
						pos.z = std::atoi(&line[index + 2]);
						for (index = index + 2; line[index] && line[index] != ':'; index++);
						int value = std::atoi(&line[index + 2]);
						for (index = index + 2; line[index] && line[index] != ':'; index++);
						t_item item;
						item.type = convert(std::atoi(&line[index + 3]));
						for (; line[index] && line[index] != ','; index++);
						item.amount = std::atoi(&line[index + 2]);
						++index;
						convertTag(line, index, item, ':');
						int rotation = std::atoi(&line[index + 2]);
						if (type == entities::item_frame) {
							auto frame = std::make_shared<ItemFrameEntity>(nullptr, pos, value);
							frame->setContent({1, 1}); // random content other than air to make sure rotations go through
							for (int i = 0; i < rotation; ++i) {
								frame->rotate({blocks::air});
							}
							frame->setContent(item);
							backups_value.entities.push_back(frame);
							LOADINGLOG(ofs << "one more item_frame entity at " << POS(pos) << " with value " << value << ", type " << item.type << ", and rotation " << rotation << std::endl);
						} else if (type == entities::lectern) {
							auto lectern = std::make_shared<LecternEntity>(nullptr, pos, value);
							lectern->setPage(rotation);
							lectern->setContent(item);
							backups_value.entities.push_back(lectern);
							LOADINGLOG(ofs << "one more lectern entity at " << POS(pos) << " with value " << value << ", type " << item.type << ", and page " << rotation << std::endl);
						}
						for (;line[index + 1] && line[index + 1] != '\"'; ++index);
					}
				} else {
					LOGERROR("foreigh line in backup: " << line);
				}
				if (line.size() > 1 && (!line.compare(line.size() - 2, 2, "},") || line[line.size() - 1] == '}')) {
					// std::cerr << "breaking on line " << line << std::endl;
					break ;
				}
			}
			mtx_backup.lock();
			_backups[key] = backups_value;
			mtx_backup.unlock();
		} else if (line == "]") {
			return ;
		} else {
			LOGERROR("foreigh line in backups: " << line);
		}
	}
	throw UnclosedBracketException();
}

// ************************************************************************** //
//                                Settings                                    //
// ************************************************************************** //

void Menu::loadSettings( void )
{
	try {
		std::ofstream ofs("Resources/loadingSettings.log", std::ofstream::out | std::ofstream::trunc);

		LOADINGLOG(ofs << "Opening file Resources/settings.json" << std::endl);
		std::ifstream indata("Resources/settings.json");
		if (!indata.is_open()) {
			throw InvalidFileException();
		}
		std::string line;
		while (!indata.eof()) {
			std::getline(indata, line);
			line = utils::string::trim_spaces(line);
			if (line.empty() || line[0] == '#') {
				continue ;
			} else if (!line.compare(0, 7, "\"fov\": ")) {
				_fov_gradient = std::stof(&line[7]);
				Settings::Get()->setFloat(settings::floats::fov, _fov_gradient);
				LOADINGLOG(ofs << "fov set to " << _fov_gradient << std::endl);
			} else if (!line.compare(0, 19, "\"render_distance\": ")) {
				int render = std::atoi(&line[19]);
				_render_gradient = render;
				Settings::Get()->setInt(settings::ints::render_dist, render);
				LOADINGLOG(ofs << "render dist set to " << render << std::endl);
			} else if (!line.compare(0, 14, "\"brightness\": ")) {
				_brightness_gradient = std::stof(&line[14]);
				Settings::Get()->setFloat(settings::floats::brightness, _brightness_gradient);
				LOADINGLOG(ofs << "brightness set to " << _brightness_gradient << std::endl);
			} else if (!line.compare(0, 10, "\"clouds\": ")) {
				int clouds = std::atoi(&line[10]);
				Settings::Get()->setInt(settings::ints::clouds, clouds);
				const std::array<std::string, 3> cloudstr = {"Clouds - Fancy", "Clouds - Fast", "Clouds - OFF"};
				LOADINGLOG(ofs << "clouds set to " << cloudstr[clouds] << std::endl);
			} else if (!line.compare(0, 10, "\"skybox\": ")) {
				int skybox = std::atoi(&line[10]);
				Settings::Get()->setBool(settings::bools::skybox, skybox);
				LOADINGLOG(ofs << "skybox set to " << ((skybox) ? "true" : "false") << std::endl);
			} else if (!line.compare(0, 13, "\"particles\": ")) {
				int particles = std::atoi(&line[13]);
				Settings::Get()->setBool(settings::bools::particles, particles);
				LOADINGLOG(ofs << "particles set to " << ((particles) ? "true" : "false") << std::endl);
			} else if (!line.compare(0, 16, "\"face_culling\": ")) {
				int face_culling = std::atoi(&line[16]);
				Settings::Get()->setBool(settings::bools::face_culling, face_culling);
				LOADINGLOG(ofs << "face_culling set to " << face_culling << std::endl);
			} else if (!line.compare(0, 19, "\"smooth_lighting\": ")) {
				int smooth_lighting = std::atoi(&line[19]);
				Settings::Get()->setBool(settings::bools::smooth_lighting, smooth_lighting);
				LOADINGLOG(ofs << "smooth_lighting set to " << smooth_lighting << std::endl);
			} else if (!line.compare(0, 13, "\"gui_scale\": ")) {
				_gui_size = std::atoi(&line[13]) - 1;
				changeGuiSize();
				LOADINGLOG(ofs << "gui_size set to " << _gui_size << std::endl);
			} else if (!line.compare(0, 19, "\"resource_packs\": [")) {
				size_t index = 19;
				size_t endindex = line.rfind(']');
				LOADINGLOG(ofs << "resource packs" << std::endl);
				if (endindex == std::string::npos) throw UnclosedBracketException();
				while (index < endindex) {
					size_t start = line.find('\"', index);
					if (start == std::string::npos) throw UnclosedBracketException();
					size_t end = line.find('\"', start + 1);
					if (end == std::string::npos) throw UnclosedBracketException();
					std::string pack = line.substr(start + 1, end - 1 - start);
					LOADINGLOG(ofs << "push resource pack " << pack << std::endl);
					Settings::Get()->pushResourcePack(pack);
					index = end + 1;
				}
			} else if (!line.compare(0, 13, "\"bindings\": {")) {
				inputs::loadBindings(ofs, line);
			}
		}
	}
	catch (std::exception & e) {
		LOGERROR(e.what());
		exit (1);
	}
}

void Menu::saveSettings( void )
{
	std::string json = "{\n\t\"fov\": " + std::to_string(Settings::Get()->getFloat(settings::floats::fov))
					+ ",\n\t\"render_distance\": " + std::to_string(Settings::Get()->getInt(settings::ints::render_dist))
					+ ",\n\t\"clouds\": " + std::to_string(Settings::Get()->getInt(settings::ints::clouds))
					+ ",\n\t\"gui_scale\": " + std::to_string(_gui_size)
					+ ",\n\t\"skybox\": " + std::to_string(Settings::Get()->getBool(settings::bools::skybox))
					+ ",\n\t\"brightness\": " + std::to_string(Settings::Get()->getFloat(settings::floats::brightness))
					+ ",\n\t\"particles\": " + std::to_string(Settings::Get()->getBool(settings::bools::particles))
					+ ",\n\t\"face_culling\": " + std::to_string(Settings::Get()->getBool(settings::bools::face_culling))
					+ ",\n\t\"smooth_lighting\": " + std::to_string(Settings::Get()->getBool(settings::bools::smooth_lighting))
					+ ",\n\t\"resource_packs\": [";
	std::vector<std::string>& packs = Settings::Get()->getResourcePacks();
	bool start = true;
	for (auto& pack: packs) {
		if (!start) {
			json += ", ";
		}
		json += '\"' + pack + '\"';
		start = false;
	}
	json += "],\n\t" + inputs::saveBindings() + "\n}";
	try {
		std::ofstream ofs("Resources/settings.json", std::ofstream::out | std::ofstream::trunc);
		ofs << json;
		ofs.close();
	}
	catch (std::exception& e) {
		LOGERROR(e.what() << "\nSettings save on Resources/settings.json failure.");
	}
}

// ************************************************************************** //
//                            Resource packs                                  //
// ************************************************************************** //

bool Settings::loadResourcePacks( void )
{
	const std::array<std::string, settings::strings::size> keys = {
		"\"BLOCK_ATLAS\": ", "\"ASCII_ATLAS\": ", "\"UI_ATLAS\": ", "\"TEX_INVENTORY\": ", "\"TEX_CRAFTING_TABLE\": ",
		"\"TEX_FURNACE\": ", "\"TEX_CHEST\": ", "\"TEX_BOOK\": ", "\"TEX_ANVIL\": ", "\"TEX_ZOMBIE\": ",
		"\"TEX_SKELETON\": ", "\"TEX_PLAYER\": ", "\"PARTICLE_ATLAS\": ", "\"WATER_STILL\": ", "\"WATER_FLOW\": ",

		"\"MAIN_FRAGMENT_SHADER\": ", "\"MAIN_VERTEX_SHADER\": ", "\"ITEM_FRAGMENT_SHADER\": ", "\"ITEM_VERTEX_SHADER\": ",
		"\"MODEL_FRAGMENT_SHADER\": ", "\"MODEL_VERTEX_SHADER\": ", "\"PARTICLE_FRAGMENT_SHADER\": ", "\"PARTICLE_VERTEX_SHADER\": ",
		"\"SKY_FRAGMENT_SHADER\": ", "\"SKY_VERTEX_SHADER\": ", "\"SKYBOX_FRAGMENT_SHADER\": ", "\"SKYBOX_VERTEX_SHADER\": ",
		"\"TEXT_FRAGMENT_SHADER\": ", "\"TEXT_VERTEX_SHADER\": ", "\"UI_FRAGMENT_SHADER\": ", "\"UI_VERTEX_SHADER\": ",
	};

	std::ofstream ofs("Resources/loadingSettings.log", std::ofstream::out | std::ofstream::app);
	std::array<bool, settings::strings::size_textures> check_set = {false};
	for (auto &pack: _packs) {
		try {
			LOADINGLOG(ofs << "Opening file Resources/Resource_Packs/" << pack << std::endl);
			std::ifstream indata("Resources/Resource_Packs/" + pack);
			if (!indata.is_open()) {
				throw InvalidFileException();
			}
			std::string line;
			while (!indata.eof()) {
				std::getline(indata, line);
				line = utils::string::trim_spaces(line);
				if (line.empty() || line[0] == '#') {
					continue ;
				}
				
				for (int index = 0; index < settings::strings::size; ++index) {
					size_t strlen = keys[index].size();
					if (!line.compare(0, strlen, keys[index])) {
						size_t end = line.find('\"', strlen + 1);
						if (end == std::string::npos) throw UnclosedBracketException();
						_strings[index] = line.substr(strlen + 1, end - (strlen + 1));
						if (index < settings::strings::size_textures) {
							check_set[index] = true;
						}
						LOADINGLOG(ofs << "Resource " << keys[index] << " at index " << index << " set to |" << _strings[index] << "|" << std::endl);
						break ;
					}
				}
				
			}
		}
		catch (std::exception & e) {
			LOGERROR(e.what() << "\nSettings::loadResourcePacks");
		}
	}
	for (int i = 0; i < settings::strings::size_textures; ++i) {
		if (!check_set[i]) {
			LOGERROR("missing field " << i << " in check_set");
			return (1);
		}
	}
	return (0);
}
