#include "vox.h"

// ************************************************************************** //
//                                 Upload                                     //
// ************************************************************************** //

void OpenGL_Manager::saveWorld( void )
{
	// first delete chunks, this updates _backups
	if (_thread.joinable()) {
		_thread.join();
	}
	_visible_chunks.clear();
	mtx_perimeter.lock();
	_perimeter_chunks.clear();
	mtx_perimeter.unlock();
	mtx.lock();
	for (auto& c: _chunks) {
		c->setBackup(&_backups);
		delete c;
	}
	_chunks.clear();
	mtx.unlock();
	_camera->setCurrentChunkPtr(NULL);

	// then store everything
	std::string json = "{\n\t\"seed\": " + std::to_string(perlin_seed)
		+ ",\n\t\"game_mode\": " + std::to_string(_game_mode)
		+ ",\n\t\"debug_mode\": " + ((_debug_mode) ? "true" : "false")
		+ ",\n\t\"f5_mode\": " + ((_f5_mode) ? "true" : "false")
		+ ",\n\t\"outline\": " + ((_outline) ? "true" : "false")
		+ ",\n\t\"render_distance\": " + std::to_string(_render_distance) + ",\n\t"
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
}

std::string DayCycle::saveString( void )
{
	std::string res = "\"dayCycle\": {\"day\": "
		+ std::to_string(_day) + ", \"hour\": " + std::to_string(_hour) + ", \"minute\": " + std::to_string(_minute)
		+ ", \"light\": " + std::to_string(_internal_light) + ", \"state\": " + std::to_string(_state)
		+ "},\n\t";
	return (res);
}

std::string Camera::saveString( void )
{
	_mtx.lock();
	std::string res = "\"camera\": {\n\t\t\"pos\": {\"x\": "
		+ std::to_string(_position.x) + ", \"y\": " + std::to_string(_position.y) + ", \"z\": " + std::to_string(_position.z)
		+ "},\n\t\t\"yaw\": " + std::to_string(_yaw)
		+ ",\n\t\t\"pitch\": " + std::to_string(_pitch)
		+ ",\n\t\t\"fov\": " + std::to_string(_fov)
		+ ",\n\t\t\"health_points\": " + std::to_string(_health_points)
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
		res += '[' + std::to_string(_content[index].x) + ", " + std::to_string(_content[index].y) + ']';
		if (index < 8) {
			res += ", ";
		}
	}
	res += "],\n\t\t\"backpack\": [";
	for (int index = 0; index < 27; index++) {
		res += '[' + std::to_string(_backpack[index].x) + ", " + std::to_string(_backpack[index].y) + ']';
		if (index < 26) {
			res += ", ";
		}
	}
	res += "],\n\t\t\"durabilities\": [";
	bool start = true;
	for (auto& dura: _durabilities) {
		if (!start) {
			res += ',';
		}
		start = false;
		res += "\n\t\t\t{\"location\": " + std::to_string(dura.x) + ", \"dura\": " + std::to_string(dura.y) + ", \"max_dura\": " + std::to_string(dura.z) + "}";
	}
	_durabilities.clear();
	res += "\n\t\t]\n\t},\n\t";
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
		res += "\n\t\t{\"pos\": [" + std::to_string(bup.first.first) + ", " + std::to_string(bup.first.second) + "],"
			+ "\n\t\t\"orientations\": [";
		bool ostart = true;
		for (auto& orient: bup.second.orientations) {
			if (!ostart) {
				res += ", ";
			}
			ostart = false;
			res += "[" + std::to_string(orient.first) + ", " + std::to_string(orient.second) + ']';
		}
		res += "],\n\t\t\"added\": [";
		bool astart = true;
		for (auto& add: bup.second.added) {
			if (!astart) {
				res += ", ";
			}
			astart = false;
			res += "[" + std::to_string(add.first) + ", " + std::to_string(add.second) + ']';
		}
		res += "],\n\t\t\"removed\": [";
		bool rmstart = true;
		for (auto& rm: bup.second.removed) {
			if (!rmstart) {
				res += ", ";
			}
			rmstart = false;
			res += "[" + std::to_string(rm) + ']';
		}
		res += "],\n\t\t\"furnaces\": [";
		bool fstart = true;
		for (auto& fur: bup.second.furnaces) {
			if (!fstart) {
				res += ", ";
			}
			fstart = false;
			res += "{\"pos\": " + std::to_string(fur.first)
				+ ", \"composant\": [" + std::to_string(fur.second.getComposant().x) + ", " + std::to_string(fur.second.getComposant().y)
				+ "], \"fuel\": [" + std::to_string(fur.second.getFuel().x) + ", " + std::to_string(fur.second.getFuel().y)
				+ "], \"production\": [" + std::to_string(fur.second.getProduction().x) + ", " + std::to_string(fur.second.getProduction().y)
				+ "]}"; // TODO add current times for better backup
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
		std::string line;
		while (!indata.eof()) {
			std::getline(indata, line);
			line = trim_spaces(line);
			if (line.empty() || line[0] == '#') {
				continue ;
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
				_f5_mode = line.substr(11, 4) == "true";
				ofs << "f5 mode set to " << _f5_mode << std::endl;
			} else if (!line.compare(0, 11, "\"outline\": ")) {
				_outline = line.substr(11, 4) == "true";
				ofs << "outline set to " << _outline << std::endl;
			} else if (!line.compare(0, 19, "\"render_distance\": ")) {
				_render_distance = std::atoi(&line[19]);
				glUseProgram(_shaderProgram);
				glUniform1f(_uniFog, (1 + _render_distance) << CHUNK_SHIFT);
				glUseProgram(_skyShaderProgram);
				glUniform1f(_skyUniFog, (1 + _render_distance) << CHUNK_SHIFT);
				glUseProgram(_shaderProgram);
				ofs << "render dist set to " << _render_distance << std::endl;
			} else if (!line.compare(0, 12, "\"dayCycle\": ")) {
				DayCycle::Get()->loadWorld(ofs, line);
			} else if (!line.compare(0, 10, "\"camera\": ")) {
				_camera->loadWorld(ofs, indata);
			} else if (!line.compare(0, 13, "\"inventory\": ")) {
				_inventory->loadWorld(ofs, indata);
			} else if (!line.compare(0, 11, "\"backups\": ")) {
				loadBackups(ofs, indata);
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
	_hour = std::atoi(&line[index + 1]);
	for (index += 2; line[index] && line[index] != ':'; index++);
	_minute = std::atoi(&line[index + 1]);
	for (index += 2; line[index] && line[index] != ':'; index++);
	_internal_light = std::atoi(&line[index + 1]);
	glUniform1i(_uniInternalLight, _internal_light);
	glClearColor(gradient(_internal_light, 4, 15, 0, 120 / 255.0), gradient(_internal_light, 4, 15, 0, 169 / 255.0), gradient(_internal_light, 4, 15, 0, 1), 1.0f);
	for (index += 2; line[index] && line[index] != ':'; index++);
	int value = std::atoi(&line[index + 1]);
	switch (value) {
		case 0:
			_state = dayCycle_state::DAYTIME;
			break ;
		case 1:
			_state = dayCycle_state::SUNSET;
			break ;
		case 2:
			_state = dayCycle_state::NIGHTTIME;
			break ;
		case 3:
			_state = dayCycle_state::SUNRISE;
			break ;
	}
	const std::string dayCycle_states[4] = {"DAYTIME", "SUNSET", "NIGHTTIME", "SUNRISE"};
	ofs << "dayCycle " << dayCycle_states[_state] << " set to day " << _day << " " << _hour << ":" << _minute << " (light " << _internal_light << ")" << std::endl;
}

void Camera::loadWorld( std::ofstream & ofs, std::ifstream & indata )
{
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
			ofs << "camera pos set to " << _position.x << ", " << _position.y << ", " << _position.z << std::endl;
			_mtx.unlock();
		} else if (!line.compare(0, 7, "\"yaw\": ")) {
			_yaw = std::stof(&line[7]);
			ofs << "camera yaw set to " << _yaw << std::endl;
		} else if (!line.compare(0, 9, "\"pitch\": ")) {
			_pitch = std::stof(&line[9]);
			ofs << "camera pitch set to " << _pitch << std::endl;
		} else if (!line.compare(0, 7, "\"fov\": ")) {
			_fov = std::stof(&line[7]);
			ofs << "camera fov set to " << _fov << std::endl;
		} else if (!line.compare(0, 17, "\"health_points\": ")) {
			_health_points = std::atoi(&line[17]);
			ofs << "camera health points set to " << _health_points << std::endl;
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
				_content[cindex].x = std::atoi(&line[index + 1]);
				for (; line[index] && line[index] != ','; index++);
				_content[cindex].y = std::atoi(&line[index + 2]);
				for (index = index + 2; line[index] && line[index] != '['; index++);
				ofs << "inventory slot " << cindex << " set to " << _content[cindex].x << ", " << _content[cindex].y << std::endl;
			}
		} else if (!line.compare(0, 12, "\"backpack\": ")) {
			index = 13;
			for (int bindex = 0; bindex < 27 && line[index]; bindex++) {
				_backpack[bindex].x = std::atoi(&line[index + 1]);
				for (; line[index] && line[index] != ','; index++);
				_backpack[bindex].y = std::atoi(&line[index + 2]);
				for (index = index + 2; line[index] && line[index] != '['; index++);
				ofs << "inventory backpack slot " << bindex << " set to " << _backpack[bindex].x << ", " << _backpack[bindex].y << std::endl;
			}
		} else if (!line.compare(0, 16, "\"durabilities\": ")) {
			while (!indata.eof()) {
				std::getline(indata, line);
				line = trim_spaces(line);
				if (line.empty() || line[0] == '#') {
					continue ;
				} else if (!line.compare(0, 13, "{\"location\": ")) {
					glm::ivec3 newDura;
					newDura.x = std::atoi(&line[13]);
					for (index = 13; line[index] && line[index] != ':'; index++);
					newDura.y = std::atoi(&line[index + 2]);
					for (index = index + 2; line[index] && line[index] != ':'; index++);
					newDura.z = std::atoi(&line[index + 2]);
					_durabilities.push_back(newDura);
					ofs << "inventory new dura set to " << newDura.x << ", " << newDura.y << ", " << newDura.z << std::endl;
				} else if (line == "]") {
					break ;
				} else {
					std::cerr << "foreigh line in durabilities: " << line << std::endl;
				}
			}
		} else if (line == "},") {
			return ;
		} else {
			std::cerr << "foreigh line in inventory: " << line << std::endl;
		}
	}
	throw UnclosedBracketException();
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
				} else if (!line.compare(0, 16, "\"orientations\": ")) {
					index = 16;
					while (line[index + 1] == '[') {
						int okey = std::atoi(&line[index + 2]);
						for (; line[index] && line[index] != ','; index++);
						backups_value.orientations[okey] = std::atoi(&line[index + 2]);
						ofs << "backups new orientation " << okey << ", " << backups_value.orientations[okey] << std::endl;
						for (; line[index + 1] && line[index + 1] != '['; index++);
					}
				} else if (!line.compare(0, 9, "\"added\": ")) {
					index = 9;
					while (line[index + 1] == '[') {
						int addkey = std::atoi(&line[index + 2]);
						for (; line[index] && line[index] != ','; index++);
						backups_value.added[addkey] = std::atoi(&line[index + 2]);
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
				} else if (!line.compare(0, 12, "\"furnaces\": ")) {
					index = 12;
					while (line[index + 1] == '{') {
						int fkey = std::atoi(&line[index + 9]);
						FurnaceInstance fur;
						glm::ivec2 value;
						for (index = index + 9; line[index] && line[index] != ':'; index++);
						value.x = std::atoi(&line[index + 3]);
						for (; line[index] && line[index] != ','; index++);
						value.y = std::atoi(&line[index + 2]);
						fur.setComposant(value);
						for (; line[index] && line[index] != ':'; index++);
						value.x = std::atoi(&line[index + 3]);
						for (; line[index] && line[index] != ','; index++);
						value.y = std::atoi(&line[index + 2]);
						fur.setFuel(value);
						for (; line[index] && line[index] != ':'; index++);
						value.x = std::atoi(&line[index + 3]);
						for (; line[index] && line[index] != ','; index++);
						value.y = std::atoi(&line[index + 2]);
						fur.setProduction(value);
						backups_value.furnaces[fkey] = fur;
						ofs << "one more furnace at " << fkey << std::endl;
						for (; line[index] && line[index] != '{'; index++);
					}
					break ;
				} else {
					std::cerr << "foreigh line in backup: " << line << std::endl;
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
