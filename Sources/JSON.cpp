#include "OpenGL_Manager.hpp"
#include "Settings.hpp"
#include <fstream>
extern siv::PerlinNoise::seed_type perlin_seed;

// ************************************************************************** //
//                                 Upload                                     //
// ************************************************************************** //

void OpenGL_Manager::saveWorld( void )
{
	// first delete chunks, this updates _backups
	stopThread();
	_visible_chunks.clear();
	mtx_perimeter.lock();
	_perimeter_chunks.clear();
	mtx_perimeter.unlock();
	mtx.lock();
	for (auto& c: _chunks) {
		c->setBackup(_backups);
		delete c;
	}
	_chunks.clear();
	mtx.unlock();
	_camera->setCurrentChunkPtr(NULL);
	_block_hit = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 0, 0, 0};

	// then store everything
	std::string json = "{\n\t\"version\": \"" + settings::consts::json::current_version
		+ "\",\n\t\"seed\": " + std::to_string(perlin_seed)
		+ ",\n\t\"game_mode\": " + std::to_string(_game_mode)
		+ ",\n\t\"debug_mode\": " + ((_debug_mode) ? "true" : "false")
		+ ",\n\t\"f5_mode\": " + ((_ui->_hideUI) ? "true" : "false")
		+ ",\n\t\"outline\": " + ((_outline) ? "true" : "false")
		+ ",\n\t\"flat_world\": " + std::to_string(Settings::Get()->getInt(settings::ints::flat_world_block))
		+ DayCycle::Get()->saveString()
		+ _camera->saveString()
		+ _inventory->saveString()
		+ saveBackupString()
		+ "\n}";
	try {
		std::ofstream ofs("Worlds/" + _world_name, std::ofstream::out | std::ofstream::trunc);
		ofs << json;
		ofs.close();
	}
	catch (std::exception & e) {
		std::cerr << e.what() << std::endl << "world save on Worlds/" << _world_name << " failure .. hope you did some snapshot" << std::endl;
	}
	glClearColor(0, 0, 0, 1);
	startThread();
}

std::string DayCycle::saveString( void )
{
	std::string res = ",\n\t\"dayCycle\": {\"day\": "
		+ std::to_string(_day) + ", \"ticks\": " + std::to_string(_ticks) + "},\n\t";
	return (res);
}

std::string Camera::saveString( void )
{
	_mtx.lock();
	std::string res = "\"camera\": {\n\t\t\"pos\": {\"x\": "
		+ std::to_string(_position.x) + ", \"y\": " + std::to_string(_position.y) + ", \"z\": " + std::to_string(_position.z)
		+ "},\n\t\t\"spawnpoint\": {\"x\": "
		+ std::to_string(_spawnpoint.x) + ", \"y\": " + std::to_string(_spawnpoint.y) + ", \"z\": " + std::to_string(_spawnpoint.z)
		+ "},\n\t\t\"yaw\": " + std::to_string(_yaw)
		+ ",\n\t\t\"pitch\": " + std::to_string(_pitch)
		+ ",\n\t\t\"health_points\": " + std::to_string(_health_points)
		+ ",\n\t\t\"foodLevel\": " + std::to_string(_foodLevel)
		+ ",\n\t\t\"foodSaturation\": " + std::to_string(_foodSaturationLevel)
		+ ",\n\t\t\"touch_ground\": " + ((_touchGround) ? "true" : "false")
		+ "\n\t},\n\t";
	_mtx.unlock();
	return (res);
}

std::string Inventory::saveString( void )
{
	std::string res = "\"inventory\": {\n\t\t\"slot\": " + std::to_string(_slot)
		+ ",\n\t\t\"content\": [";
	for (int index = 0; index < 9; index++) {
		res += '[' + std::to_string(_content[index].type) + ", " + std::to_string(_content[index].amount) + ", " + std::to_string(_content[index].dura.x) + ']';
		if (index < 8) {
			res += ", ";
		}
	}
	res += "],\n\t\t\"backpack\": [";
	for (int index = 0; index < 27; index++) {
		res += '[' + std::to_string(_backpack[index].type) + ", " + std::to_string(_backpack[index].amount) + ", " + std::to_string(_backpack[index].dura.x) + ']';
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
					res += '[' + std::to_string(ch.second->getItem(index)->type) + ", " + std::to_string(ch.second->getItem(index)->amount) + ", " + std::to_string(ch.second->getItem(index)->dura.x) + ']';
					if (index < 26) {
						res += ", ";
					}
				}
				res += "]}";
				delete ch.second;
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
				res += "{\"pos\": " + std::to_string(fur.first)
					+ ", \"composant\": [" + std::to_string(fur.second->getComposant().type) + ", " + std::to_string(fur.second->getComposant().amount) + ", " + std::to_string(fur.second->getComposant().dura.x)
					+ "], \"fuel\": [" + std::to_string(fur.second->getFuel().type) + ", " + std::to_string(fur.second->getFuel().amount) + ", " + std::to_string(fur.second->getFuel().dura.x)
					+ "], \"production\": [" + std::to_string(fur.second->getProduction().type) + ", " + std::to_string(fur.second->getProduction().amount) + ", " + std::to_string(fur.second->getProduction().dura.x)
					+ "]}"; // TODO add current times for better backup
				delete fur.second;
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
				delete sign.second;
			}
		}
		res += "]}";
	}
	_backups.clear();
	mtx_backup.unlock();
	res += "\n\t]";
	return (res);
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

		ofs << "Opening file " << file << std::endl;
		std::ifstream indata(file.c_str());
		if (!indata.is_open()) {
			throw InvalidFileException();
		}
		Settings::Get()->setString(settings::json_version, settings::consts::json::past_versions[0]); // reset version
		std::string line;
		while (!indata.eof()) {
			std::getline(indata, line);
			line = trim_spaces(line);
			if (line.empty() || line[0] == '#') {
				continue ;
			} else if (line == "{" || line == "}") {
			} else if (!line.compare(0, 11, "\"version\": ")) {
				std::string version = line.substr(12, line.size() - 14);
				Settings::Get()->setString(settings::json_version, version);
				ofs << "version set to " << version << std::endl;
			} else if (!line.compare(0, 8, "\"seed\": ")) {
				perlin_seed = std::atoi(&line[8]);
				ofs << "seed set to " << perlin_seed << std::endl;
			} else if (!line.compare(0, 13, "\"game_mode\": ")) {
				_game_mode = std::atoi(&line[13]);
				ofs << "game mode set to " << _game_mode << std::endl;
			} else if (!line.compare(0, 14, "\"debug_mode\": ")) {
				_debug_mode = line.substr(14, 4) == "true";
				ofs << "debug mode set to " << _debug_mode << std::endl;
			} else if (!line.compare(0, 11, "\"f5_mode\": ")) {
				_ui->_hideUI = line.substr(11, 4) == "true";
				ofs << "f5 mode set to " << _ui->_hideUI << std::endl;
			} else if (!line.compare(0, 11, "\"outline\": ")) {
				_outline = line.substr(11, 4) == "true";
			} else if (!line.compare(0, 14, "\"flat_world\": ")) {
				int value = std::atoi(&line[14]);
				value = (value >= 0 && value < S_BLOCKS_SIZE) ? value : blocks::air;
				Settings::Get()->setInt(settings::ints::flat_world_block, value);
				Settings::Get()->setBool(settings::bools::flat_world, value != blocks::air);
				ofs << "flat_world_block set to " << s_blocks[value]->name << std::endl;
			} else if (!line.compare(0, 12, "\"dayCycle\": ")) {
				DayCycle::Get()->loadWorld(ofs, line);
			} else if (!line.compare(0, 10, "\"camera\": ")) {
				_camera->loadWorld(ofs, indata);
			} else if (!line.compare(0, 13, "\"inventory\": ")) {
				_inventory->loadWorld(ofs, indata);
			} else if (!line.compare(0, 11, "\"backups\": ")) {
				loadBackups(ofs, indata);
			} else {
				ofs << "FOREIGN LINE IN LOADWORLD: " << line << std::endl;
			}
		}
		indata.close();
		ofs.close();
	}
	catch (std::exception & e) {
		std::cerr << e.what() << std::endl;
		exit (1); // TODO might want to return to main menu instead
	}
}

void DayCycle::loadWorld( std::ofstream & ofs, std::string line )
{
	int index = 12;
	for (index = 12; line[index] && line[index] != ':'; index++);
	_day = std::atoi(&line[index + 1]);
	for (index += 2; line[index] && line[index] != ':'; index++);
	_ticks = std::atoi(&line[index + 1]);
	_forceReset = true;
	setInternals();
	ofs << "dayCycle " << _ticks << " set to day " << _day << " " << _hour << ":" << _minute << " (light " << _internal_light << ")" << std::endl;
}

void Camera::loadWorld( std::ofstream & ofs, std::ifstream & indata )
{
	_lastTp = {0, 0, 0};
	int index;
	std::string line;
	while (!indata.eof()) {
		std::getline(indata, line);
		line = trim_spaces(line);
		if (line.empty() || line[0] == '#') {
			continue ;
		} else if (!line.compare(0, 7, "\"pos\": ")) {
			_mtx.lock();
			_position.x = std::stof(&line[13]);
			for (index = 13; line[index] && line[index] != ':'; index++);
			_position.y = std::stof(&line[index + 2]);
			for (index = index + 2; line[index] && line[index] != ':'; index++);
			_position.z = std::stof(&line[index + 2]);
			_z0 = _position.z;
			ofs << "camera pos set to " << _position.x << ", " << _position.y << ", " << _position.z << std::endl;
			_mtx.unlock();
		} else if (!line.compare(0, 14, "\"spawnpoint\": ")) {
			_spawnpoint.x = std::stof(&line[20]);
			for (index = 20; line[index] && line[index] != ':'; index++);
			_spawnpoint.y = std::stof(&line[index + 2]);
			for (index = index + 2; line[index] && line[index] != ':'; index++);
			_spawnpoint.z = std::stof(&line[index + 2]);
			ofs << "camera spawnpoint set to " << _spawnpoint.x << ", " << _spawnpoint.y << ", " << _spawnpoint.z << std::endl;
		} else if (!line.compare(0, 7, "\"yaw\": ")) {
			_yaw = std::stof(&line[7]);
			ofs << "camera yaw set to " << _yaw << std::endl;
		} else if (!line.compare(0, 9, "\"pitch\": ")) {
			_pitch = std::stof(&line[9]);
			ofs << "camera pitch set to " << _pitch << std::endl;
		} else if (!line.compare(0, 17, "\"health_points\": ")) {
			_health_points = std::atoi(&line[17]);
			ofs << "camera health points set to " << _health_points << std::endl;
		} else if (!line.compare(0, 13, "\"foodLevel\": ")) {
			_foodLevel = std::atoi(&line[13]);
			ofs << "camera food level set to " << _foodLevel << std::endl;
		} else if (!line.compare(0, 18, "\"foodSaturation\": ")) {
			_foodSaturationLevel = std::stof(&line[18]);
			ofs << "camera food saturation level set to " << _foodSaturationLevel << std::endl;
		} else if (!line.compare(0, 16, "\"touch_ground\": ")) {
			_touchGround = line.substr(16, 4) == "true";
			ofs << "camera touch ground set to " << _touchGround << std::endl;
		} else if (line == "},") {
			updateCameraVectors();
			return ;
		} else {
			std::cerr << "foreigh line in camera: " << line << std::endl;
		}
	}
	throw UnclosedBracketException();
}

static int convert( int value ) // used when I change s_blocks big time
{
	if (Settings::Get()->getString(settings::json_version) == settings::consts::json::current_version) {
		return (value);
	}
	for (int i = 0; i < settings::consts::json::nbr_past_versions; ++i) {
		if (Settings::Get()->getString(settings::json_version) == settings::consts::json::past_versions[i]) {
			switch (i) {
				case 0: //v0.0
					// transition from v0.0 to v1.0: 0xF00 becomes 0xF000 and 0xF000 becomes 0xF000000
					// got rid of wheat_crop1-7
					if (value >= blocks::wheat_crop && value <= blocks::wheat_crop + 7) {
						value = blocks::wheat_crop + ((value - blocks::wheat_crop) << offset::blocks::bitfield);
					}
					value = ((value & 0XFFFF00FF) | (((value >> 8) & 0xF) << 12) | (((value >> 12) & 0xF) << 24));
				case 1: //v1.0
					// transition from v1.0 to f1.1: got rid of water1-7
					if (value >= blocks::water && value <= blocks::water + 7) {
						value = blocks::water + ((value - blocks::water) << offset::blocks::bitfield);
					}
					return (value);
			}
		}
	}
	std::cerr << "json version is " << Settings::Get()->getString(settings::json_version) << std::endl;
	assert((0 == 1) && "unrecognised json version.");
}

void Inventory::loadWorld( std::ofstream & ofs, std::ifstream & indata )
{
	_modif = true;
	int index;
	std::string line;
	while (!indata.eof()) {
		std::getline(indata, line);
		line = trim_spaces(line);
		if (line.empty() || line[0] == '#') {
			continue ;
		} else if (!line.compare(0, 8, "\"slot\": ")) {
			_slot = std::atoi(&line[8]);
			ofs << "inventory slot set to " << _slot << std::endl;
		} else if (!line.compare(0, 11, "\"content\": ")) {
			index = 12;
			for (int cindex = 0; cindex < 9 && line[index]; cindex++) {
				_content[cindex].type = convert(std::atoi(&line[index + 1]));
				for (; line[index] && line[index] != ','; index++);
				_content[cindex].amount = std::atoi(&line[index + 2]);
				++index;
				for (; line[index] && line[index] != ','; index++);
				_content[cindex].dura.x = std::atoi(&line[index + 2]);
				_content[cindex].dura.y = s_blocks[_content[cindex].type & mask::blocks::type]->durability;
				for (index = index + 2; line[index] && line[index] != '['; index++);
				ofs << "inventory slot " << cindex << " set to " << _content[cindex].type << ", " << _content[cindex].amount << ", " << _content[cindex].dura.x << ", " << _content[cindex].dura.y << std::endl;
			}
		} else if (!line.compare(0, 12, "\"backpack\": ")) {
			index = 13;
			for (int bindex = 0; bindex < 27 && line[index]; bindex++) {
				_backpack[bindex].type = convert(std::atoi(&line[index + 1]));
				for (; line[index] && line[index] != ','; index++);
				_backpack[bindex].amount = std::atoi(&line[index + 2]);
				++index;
				for (; line[index] && line[index] != ','; index++);
				_backpack[bindex].dura.x = std::atoi(&line[index + 2]);
				_backpack[bindex].dura.y = s_blocks[_backpack[bindex].type & mask::blocks::type]->durability;
				for (index = index + 2; line[index] && line[index] != '['; index++);
				ofs << "inventory backpack slot " << bindex << " set to " << _backpack[bindex].type << ", " << _backpack[bindex].amount << ", " << _backpack[bindex].dura.x << ", " << _backpack[bindex].dura.y << std::endl;
			}
		} else if (line == "},") {
			return ;
		} else {
			std::cerr << "foreigh line in inventory: " << line << std::endl;
		}
	}
	throw UnclosedBracketException();
}

void ChestInstance::loadContent( std::ofstream & ofs, std::string &line, int &index )
{
	for (; line[index] && line[index] != '['; ++index);
	index += !!line[index];
	for (int cindex = 0; cindex < 27 && line[index]; cindex++) {
		_content[cindex].type = convert(std::atoi(&line[index + 1]));
		for (; line[index] && line[index] != ','; index++);
		_content[cindex].amount = std::atoi(&line[index + 2]);
		++index;
		for (; line[index] && line[index] != ','; index++);
		_content[cindex].dura.x = std::atoi(&line[index + 2]);
		_content[cindex].dura.y = s_blocks[_content[cindex].type & mask::blocks::type]->durability;
		if (cindex < 26) {
			for (index = index + 2; line[index] && line[index] != '['; index++);
		}
		ofs << "chest content slot " << cindex << " set to " << _content[cindex].type << ", " << _content[cindex].amount << ", " << _content[cindex].dura.x << ", " << _content[cindex].dura.y << std::endl;
	}
	// ofs << "at end of loadContent, index is " << index << ", char is " << line[index] << " into " << line.substr(index, 5) << std::endl;
}

void OpenGL_Manager::loadBackups( std::ofstream & ofs, std::ifstream & indata )
{
	int index;
	std::string line;
	while (!indata.eof()) {
		std::getline(indata, line);
		line = trim_spaces(line);
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
				line = trim_spaces(line);
				if (line.empty() || line[0] == '#') {
					continue ;
				} else if (!line.compare(0, 9, "\"added\": ")) {
					index = 9;
					while (line[index + 1] == '[') {
						int addkey = std::atoi(&line[index + 2]);
						for (; line[index] && line[index] != ','; index++);
						backups_value.added[addkey] = convert(std::atoi(&line[index + 2]));
						ofs << "backups new added " << addkey << ", " << backups_value.added[addkey] << std::endl;
						for (; line[index + 1] && line[index + 1] != '['; index++);
					}
				} else if (!line.compare(0, 11, "\"removed\": ")) {
					index = 11;
					while (line[index + 1] == '[') {
						int rmvalue = std::atoi(&line[index + 2]);
						backups_value.removed.insert(rmvalue);
						ofs << "backups new removed " << rmvalue << std::endl;
						for (index++; line[index + 1] && line[index + 1] != '['; index++);
					}
				} else if (!line.compare(0, 10, "\"chests\": ")) {
					index = 10;
					while (line[index + 1] == '{') {
						int chkey = std::atoi(&line[index + 9]);
						for (index = index + 9; line[index] && line[index] != ':'; index++);
						int orientation = std::atoi(&line[index + 2]);
						backups_value.chests.emplace(chkey, new ChestInstance(NULL, {0, 0, 0}, orientation));
						ofs << "one more chest at " << chkey << " with orientation " << orientation <<  std::endl;
						backups_value.chests.at(chkey)->loadContent(ofs, line, index);
						for (; line[index] && line[index] != '{'; index++);
						--index;
					}
				} else if (!line.compare(0, 12, "\"furnaces\": ")) {
					index = 12;
					while (line[index + 1] == '{') {
						int fkey = std::atoi(&line[index + 9]);
						FurnaceInstance *fur = new FurnaceInstance();
						t_item item;
						for (index = index + 9; line[index] && line[index] != ':'; index++);
						item.type = convert(std::atoi(&line[index + 3]));
						for (; line[index] && line[index] != ','; index++);
						item.amount = std::atoi(&line[index + 2]);
						for (; line[index] && line[index] != ','; index++);
						item.dura.x = std::atoi(&line[index + 2]);
						item.dura.y = s_blocks[item.type & mask::blocks::type]->durability;
						fur->setComposant(item);
						for (; line[index] && line[index] != ':'; index++);
						item.type = convert(std::atoi(&line[index + 3]));
						for (; line[index] && line[index] != ','; index++);
						item.amount = std::atoi(&line[index + 2]);
						for (; line[index] && line[index] != ','; index++);
						item.dura.x = std::atoi(&line[index + 2]);
						item.dura.y = s_blocks[item.type & mask::blocks::type]->durability;
						fur->setFuel(item);
						for (; line[index] && line[index] != ':'; index++);
						item.type = convert(std::atoi(&line[index + 3]));
						for (; line[index] && line[index] != ','; index++);
						item.amount = std::atoi(&line[index + 2]);
						for (; line[index] && line[index] != ','; index++);
						item.dura.x = std::atoi(&line[index + 2]);
						item.dura.y = s_blocks[item.type & mask::blocks::type]->durability;
						fur->setProduction(item);
						backups_value.furnaces[fkey] = fur;
						ofs << "one more furnace at " << fkey << std::endl;
						for (; line[index] && line[index] != '{'; index++);
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
								if (line[index + strIndex] == '\\' && line[index + strIndex + 1] == '\"') {
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
						SignInstance *sign = new SignInstance(NULL, value, pos);
						sign->setContent(content);
						backups_value.signs[skey] = sign;
						ofs << "one more sign at " << skey << " with content \"" << content[0] << "\", \"" << content[1] << "\", \"" << content[2] << "\", \"" << content[3] << '\"' << std::endl;
						for (;line[index + 1] && line[index + 1] != '{'; ++index);
					}
				} else {
					std::cerr << "foreigh line in backup: " << line << std::endl;
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
			std::cerr << "foreigh line in backups: " << line << std::endl;
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

		ofs << "Opening file Resources/settings.json" << std::endl;
		std::ifstream indata("Resources/settings.json");
		if (!indata.is_open()) {
			throw InvalidFileException();
		}
		std::string line;
		while (!indata.eof()) {
			std::getline(indata, line);
			line = trim_spaces(line);
			if (line.empty() || line[0] == '#') {
				continue ;
			} else if (!line.compare(0, 7, "\"fov\": ")) {
				_fov_gradient = std::stof(&line[7]);
				Settings::Get()->setFloat(settings::floats::fov, _fov_gradient);
				ofs << "fov set to " << _fov_gradient << std::endl;
			} else if (!line.compare(0, 19, "\"render_distance\": ")) {
				int render = std::atoi(&line[19]);
				_render_gradient = render;
				Settings::Get()->setInt(settings::ints::render_dist, render);
				ofs << "render dist set to " << render << std::endl;
			} else if (!line.compare(0, 14, "\"brightness\": ")) {
				_brightness_gradient = std::stof(&line[14]);
				Settings::Get()->setFloat(settings::floats::brightness, _brightness_gradient);
				ofs << "brightness set to " << _brightness_gradient << std::endl;
			} else if (!line.compare(0, 10, "\"clouds\": ")) {
				int clouds = std::atoi(&line[10]);
				Settings::Get()->setInt(settings::ints::clouds, clouds);
				const std::array<std::string, 3> cloudstr = {"Clouds - Fancy", "Clouds - Fast", "Clouds - OFF"};
				ofs << "clouds set to " << cloudstr[clouds] << std::endl;
			} else if (!line.compare(0, 10, "\"skybox\": ")) {
				int skybox = std::atoi(&line[10]);
				Settings::Get()->setBool(settings::bools::skybox, skybox);
				ofs << "skybox set to " << ((skybox) ? "true" : "false") << std::endl;
			} else if (!line.compare(0, 13, "\"particles\": ")) {
				int particles = std::atoi(&line[13]);
				Settings::Get()->setBool(settings::bools::particles, particles);
				ofs << "particles set to " << ((particles) ? "true" : "false") << std::endl;
			} else if (!line.compare(0, 16, "\"face_culling\": ")) {
				int face_culling = std::atoi(&line[16]);
				Settings::Get()->setBool(settings::bools::face_culling, face_culling);
				ofs << "face_culling set to " << face_culling << std::endl;
			} else if (!line.compare(0, 19, "\"smooth_lighting\": ")) {
				int smooth_lighting = std::atoi(&line[19]);
				Settings::Get()->setBool(settings::bools::smooth_lighting, smooth_lighting);
				ofs << "smooth_lighting set to " << smooth_lighting << std::endl;
			} else if (!line.compare(0, 13, "\"gui_scale\": ")) {
				_gui_size = std::atoi(&line[13]) - 1;
				changeGuiSize();
				ofs << "gui_size set to " << _gui_size << std::endl;
			} else if (!line.compare(0, 19, "\"resource_packs\": [")) {
				size_t index = 19;
				size_t endindex = line.rfind(']');
				ofs << "resource packs" << std::endl;
				if (endindex == std::string::npos) throw UnclosedBracketException();
				while (index < endindex) {
					size_t start = line.find('\"', index);
					if (start == std::string::npos) throw UnclosedBracketException();
					size_t end = line.find('\"', start + 1);
					if (end == std::string::npos) throw UnclosedBracketException();
					std::string pack = line.substr(start + 1, end - 1 - start);
					ofs << "push resource pack " << pack << std::endl;
					Settings::Get()->pushResourcePack(pack);
					index = end + 1;
				}
			}
		}
	}
	catch (std::exception & e) {
		std::cerr << e.what() << std::endl;
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
	std::vector<std::string> &packs = Settings::Get()->getResourcePacks();
	bool start = true;
	for (auto &pack: packs) {
		if (!start) {
			json += ", ";
		}
		json += '\"' + pack + '\"';
		start = false;
	}
	json += "]\n}";
	try {
		std::ofstream ofs("Resources/settings.json", std::ofstream::out | std::ofstream::trunc);
		ofs << json;
		ofs.close();
	}
	catch (std::exception & e) {
		std::cerr << e.what() << std::endl << "Settings save on Resources/settings.json failure." << std::endl;
	}
}

bool Settings::loadResourcePacks( void )
{
	std::ofstream ofs("Resources/loadingSettings.log", std::ofstream::out | std::ofstream::app);
	std::array<bool, settings::strings::size_textures> check_set = {false};
	for (auto &pack: _packs) {
		try {
			ofs << "Opening file Resources/Resource_Packs/" << pack << std::endl;
			std::ifstream indata("Resources/Resource_Packs/" + pack);
			if (!indata.is_open()) {
				throw InvalidFileException();
			}
			std::string line;
			while (!indata.eof()) {
				std::getline(indata, line);
				line = trim_spaces(line);
				if (line.empty() || line[0] == '#') {
					continue ;
				} else if (!line.compare(0, 15, "\"BLOCK_ATLAS\": ")) {
					size_t end = line.find('\"', 16);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::block_atlas] = line.substr(16, end - 16);
					check_set[settings::strings::block_atlas] = true;
				} else if (!line.compare(0, 15, "\"ASCII_ATLAS\": ")) {
					size_t end = line.find('\"', 16);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::ascii_atlas] = line.substr(16, end - 16);
					check_set[settings::strings::ascii_atlas] = true;
				} else if (!line.compare(0, 12, "\"UI_ATLAS\": ")) {
					size_t end = line.find('\"', 13);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::ui_atlas] = line.substr(13, end - 13);
					check_set[settings::strings::ui_atlas] = true;
				} else if (!line.compare(0, 17, "\"TEX_INVENTORY\": ")) {
					size_t end = line.find('\"', 18);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::tex_inventory] = line.substr(18, end - 18);
					check_set[settings::strings::tex_inventory] = true;
				} else if (!line.compare(0, 22, "\"TEX_CRAFTING_TABLE\": ")) {
					size_t end = line.find('\"', 23);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::tex_crafting_table] = line.substr(23, end - 23);
					check_set[settings::strings::tex_crafting_table] = true;
				} else if (!line.compare(0, 15, "\"TEX_FURNACE\": ")) {
					size_t end = line.find('\"', 16);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::tex_furnace] = line.substr(16, end - 16);
					check_set[settings::strings::tex_furnace] = true;
				} else if (!line.compare(0, 13, "\"TEX_CHEST\": ")) {
					size_t end = line.find('\"', 14);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::tex_chest] = line.substr(14, end - 14);
					check_set[settings::strings::tex_chest] = true;
				} else if (!line.compare(0, 18, "\"PARTICLE_ATLAS\": ")) {
					size_t end = line.find('\"', 19);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::particle_atlas] = line.substr(19, end - 19);
					check_set[settings::strings::particle_atlas] = true;
				} else if (!line.compare(0, 15, "\"MODEL_ATLAS\": ")) {
					size_t end = line.find('\"', 16);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::model_atlas] = line.substr(16, end - 16);
					check_set[settings::strings::model_atlas] = true;
				} else if (!line.compare(0, 15, "\"WATER_STILL\": ")) {
					size_t end = line.find('\"', 16);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::water_still] = line.substr(16, end - 16);
					check_set[settings::strings::water_still] = true;
				} else if (!line.compare(0, 14, "\"WATER_FLOW\": ")) {
					size_t end = line.find('\"', 15);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::water_flow] = line.substr(15, end - 15);
					check_set[settings::strings::water_flow] = true;
				} else if (!line.compare(0, 24, "\"MAIN_FRAGMENT_SHADER\": ")) {
					size_t end = line.find('\"', 25);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::main_fragment_shader] = line.substr(25, end - 25);
				} else if (!line.compare(0, 22, "\"MAIN_VERTEX_SHADER\": ")) {
					size_t end = line.find('\"', 23);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::main_vertex_shader] = line.substr(23, end - 23);
				} else if (!line.compare(0, 24, "\"ITEM_FRAGMENT_SHADER\": ")) {
					size_t end = line.find('\"', 25);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[::settings::strings::item_fragment_shader] = line.substr(25, end - 25);
				} else if (!line.compare(0, 22, "\"ITEM_VERTEX_SHADER\": ")) {
					size_t end = line.find('\"', 23);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[::settings::strings::item_vertex_shader] = line.substr(23, end - 23);
				} else if (!line.compare(0, 28, "\"PARTICLE_FRAGMENT_SHADER\": ")) {
					size_t end = line.find('\"', 29);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::particle_fragment_shader] = line.substr(29, end - 29);
				} else if (!line.compare(0, 26, "\"PARTICLE_VERTEX_SHADER\": ")) {
					size_t end = line.find('\"', 27);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::particle_vertex_shader] = line.substr(27, end - 27);
				} else if (!line.compare(0, 23, "\"SKY_FRAGMENT_SHADER\": ")) {
					size_t end = line.find('\"', 24);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::sky_fragment_shader] = line.substr(24, end - 24);
				} else if (!line.compare(0, 21, "\"SKY_VERTEX_SHADER\": ")) {
					size_t end = line.find('\"', 22);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::sky_vertex_shader] = line.substr(22, end - 22);
				} else if (!line.compare(0, 26, "\"SKYBOX_FRAGMENT_SHADER\": ")) {
					size_t end = line.find('\"', 27);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::skybox_fragment_shader] = line.substr(27, end - 27);
				} else if (!line.compare(0, 24, "\"SKYBOX_VERTEX_SHADER\": ")) {
					size_t end = line.find('\"', 25);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::skybox_vertex_shader] = line.substr(25, end - 25);
				} else if (!line.compare(0, 24, "\"TEXT_FRAGMENT_SHADER\": ")) {
					size_t end = line.find('\"', 25);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::text_fragment_shader] = line.substr(25, end - 25);
				} else if (!line.compare(0, 22, "\"TEXT_VERTEX_SHADER\": ")) {
					size_t end = line.find('\"', 23);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::text_vertex_shader] = line.substr(23, end - 23);
				} else if (!line.compare(0, 22, "\"UI_FRAGMENT_SHADER\": ")) {
					size_t end = line.find('\"', 23);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::ui_fragment_shader] = line.substr(23, end - 23);
				} else if (!line.compare(0, 20, "\"UI_VERTEX_SHADER\": ")) {
					size_t end = line.find('\"', 21);
					if (end == std::string::npos) throw UnclosedBracketException();
					_strings[settings::strings::ui_vertex_shader] = line.substr(21, end - 21);
				}
			}
		}
		catch (std::exception & e) {
			std::cerr << e.what() << std::endl << "Settings::loadResourcePacks" << std::endl;
		}
	}
	for (int i = 0; i < settings::strings::size_textures; ++i) {
		if (!check_set[i]) {
			std::cerr << "missing field " << i << " in check_set" << std::endl;
			return (1);
		}
	}
	return (0);
}
