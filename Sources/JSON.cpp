#include "vox.h"

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
	std::list<Chunk *>::iterator it = _chunks.begin();
	for (; it != _chunks.end(); it++) {
		(*it)->setBackup(&_backups);
		delete (*it);
	}
	_chunks.clear();
	mtx.unlock();

	// then store everything
	std::string json = "{\n\t\"seed\": " + std::to_string(perlin_seed)
		+ ",\n\t\"game_mode\": " + std::to_string(_game_mode)
		+ ",\n\t\"debug_mode\": " + ((_debug_mode) ? "true" : "false")
		+ ",\n\t\"f5_mode\": " + ((_f5_mode) ? "true" : "false")
		+ ",\n\t\"outline\": " + ((_outline) ? "true" : "false")
		+ ",\n\t\"render_distance\": " + std::to_string(_render_distance) + ",\n\t"
		+ _camera->saveString()
		+ _inventory->saveString()
		+ saveBackupString()
		+ "\n}";
	try {
		std::ofstream ofs("Worlds/default.json", std::ofstream::out | std::ofstream::trunc);
		ofs << json;
		ofs.close();
	}
	catch (std::exception & e) {
		std::cerr << e.what() << std::endl << "world save failure .. hope you did some snapshot" << std::endl;
	}
}

std::string Camera::saveString( void )
{
	std::string res = "\"camera\": {\n\t\t\"pos\": {\"x\": "
		+ std::to_string(_position.x) + ", \"y\": " + std::to_string(_position.y) + ", \"z\": " + std::to_string(_position.z)
		+ "},\n\t\t\"dir\": {\"x\": " + std::to_string(_front.x) + ", \"y\": " + std::to_string(_front.y) + ", \"z\": " + std::to_string(_front.z)
		+ "},\n\t\t\"fov\": " + std::to_string(_fov)
		+ ",\n\t\t\"health_points\": " + std::to_string(_health_points)
		+ ",\n\t\t\"touch_ground\": " + ((_touchGround) ? "true" : "false")
		+ "\n\t},\n\t";
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
	res += "\n\t\t]\n\t},\n\t";
	return (res);
}

std::string OpenGL_Manager::saveBackupString( void )
{
	std::string res = "\"backups\": [";
	bool start = true;
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
				res += ',';
			}
			ostart = false;
			res += "[" + std::to_string(orient.first) + ", " + std::to_string(orient.second) + ']';
		}
		res += "],\n\t\t\"added\": [";
		bool astart = true;
		for (auto& add: bup.second.added) {
			if (!astart) {
				res += ',';
			}
			astart = false;
			res += "[" + std::to_string(add.first) + ", " + std::to_string(add.second) + ']';
		}
		res += "],\n\t\t\"removed\": [";
		bool rmstart = true;
		for (auto& rm: bup.second.removed) {
			if (!rmstart) {
				res += ',';
			}
			rmstart = false;
			res += "[" + std::to_string(rm.first) + ", " + std::to_string(rm.second) + ']';
		}
		res += "],\n\t\t\"furnaces\": [";
		bool fstart = true;
		for (auto& fur: bup.second.furnaces) {
			if (!fstart) {
				res += ',';
			}
			fstart = false;
			res += "{\"pos\": " + std::to_string(fur.first)
				+ ", \"composant\": [" + std::to_string(fur.second.getComposant().x) + ", " + std::to_string(fur.second.getComposant().y)
				+ "], \"fuel\": [" + std::to_string(fur.second.getFuel().x) + ", " + std::to_string(fur.second.getFuel().y)
				+ "], \"production\": [" + std::to_string(fur.second.getProduction().x) + ", " + std::to_string(fur.second.getProduction().y)
				+ "]}";
		}
		res += "]}";
	}
	res += ']';
	return (res);
}
