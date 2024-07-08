#include <fstream>
#include <sstream>
#include "Chunk.hpp"
#include "logs.hpp"

namespace utils::string {
	std::string get_file_content( std::string file_name )
	{
		std::ifstream indata (file_name.c_str());
		if (!indata.is_open()) {
			LOGERROR("Error\nCould not open " << file_name);
			exit(1);
		}
		std::string data;
		std::string line;
		while (!indata.eof()) {
			std::getline( indata, line );
			data += line;
			if (!indata.eof())
				data += '\n';
		}
		indata.close();
		return (data);
	}

	std::string	trim_spaces( std::string str )
	{
		int index = 0;
		bool inQuotes = false;
		std::string new_string;
		while (str[index] == ' ' || str[index] == '\t')
			++index;
		while (str[index])
		{
			if (str[index] == '\t')
				new_string += ' ';
			else
				new_string += str[index];
			if (str[index] == '\"' && (!index || str[index - 1] != '\\')) {
				inQuotes = !inQuotes;
			}
			if (!inQuotes && (str[index] == ' ' || str[index] == '\t'))
			{
				while (str[index] == ' ' || str[index] == '\t')
					++index;
				--index;
			}
			++index;
		}
		if (new_string.empty())
			return ("");
		if (new_string[new_string.size() - 1] == '\r')
			new_string = new_string.substr(0, new_string.size() - 1);
		if (new_string[new_string.size() - 1] == ' ')
			new_string = new_string.substr(0, new_string.size() - 1);
		return (new_string);
	}

	std::vector<std::string> split( std::string &str, char sep )
	{
		std::string tmp = "";
		std::vector<std::string> res;
		
		for (size_t i = 0; i < str.size(); ++i) {
			if (str[i] != sep) {
				tmp += std::tolower(str[i]);
			} else if (tmp[0]) {
				res.push_back(tmp);
				tmp = "";
			}
		}
		if (tmp[0]) {
			res.push_back(tmp);
		}
		return (res);
	}

	bool case_insensitive_compare( std::string a, std::string b )
	{
		size_t sizeA = a.size(), sizeB = b.size();

		if (sizeA != sizeB) {
			return (true);
		}
		for (size_t index = 0; index < sizeA; ++index) {
			if (std::tolower(static_cast<unsigned char>(a[index])) != std::tolower(static_cast<unsigned char>(b[index]))) {
				return (true);
			}
		}
		return (false);
	}
	
	std::string doubleDigits( int nb )
	{
		if (nb < 10) {
			return (std::string("0") + static_cast<char>('0' + nb));
		}
		return (std::to_string(nb));
	}
	
	static std::string to_string_with_precision( const double value, const int precision, const bool zero_allowed )
	{
		std::ostringstream out;
		out.precision(precision);
		out << std::fixed << value;
		if (zero_allowed) {
			return (std::move(out).str());
		}
		std::string res = std::move(out).str();
		size_t index = res.find('.'), i = 1;
		if (index == std::string::npos) {
			return (res);
		}
		while (res[index + i] == '0') ++i;
		if (res[index + i]) {
			return (res);
		}
		return (res.substr(0, index));
	}

	std::string toBytes( size_t bytes )
	{
		const std::array<std::string, 5> suffixes = {"B", "KB", "MB", "GB", "TB"};
		int index = 0;
		double dBytes = bytes;

		while (dBytes >= 1024.0 && index < 5) {
			++index;
			dBytes /= 1024.0;
		}
		return (to_string_with_precision(dBytes, 2, false) + suffixes[index]);
	}
};

namespace utils::shader {
	static void compile_shader( GLuint ptrShader, std::string name )
	{
		glCompileShader(ptrShader);

		GLint status;
		glGetShaderiv(ptrShader, GL_COMPILE_STATUS, &status);
		if (status) {
			MAINLOG(LOG(name << " shader compiled successfully"));
		} else {
			char buffer[512];
			glGetShaderInfoLog(ptrShader, 512, NULL, buffer);

			MAINLOG(LOGERROR(name << " shader did not compile, error log:\n" << buffer));
			exit(1);
		}
	}

	GLuint createShaderProgram( std::string vertex, std::string geometry, std::string fragment )
	{
		std::string vertex_shader_data = ::utils::string::get_file_content(vertex);
		char *vertexSource = &vertex_shader_data[0];

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		compile_shader(vertexShader, vertex);

		GLuint geometryShader;
		if (geometry[0]) {
			std::string geometry_shader_data = ::utils::string::get_file_content(geometry);
			char *geometrySource = &geometry_shader_data[0];

			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometryShader, 1, &geometrySource, NULL);
			compile_shader(geometryShader, geometry);
		}

		std::string fragment_shader_data = ::utils::string::get_file_content(fragment);
		char *fragmentSource = &fragment_shader_data[0];

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
		compile_shader(fragmentShader, fragment);

		// Combining shaders into a program
		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		if (geometry[0]) {
			glAttachShader(shaderProgram, geometryShader);
		}
		glAttachShader(shaderProgram, fragmentShader);

		glDeleteShader(fragmentShader);
		if (geometry[0]) {
			glDeleteShader(geometryShader);
		}
		glDeleteShader(vertexShader);

		return (shaderProgram);
	}

	void check_glstate( std::string str, bool displayDebug )
	{
		GLenum error_check = glGetError();	
		if (error_check) {
			LOGERROR("glGetError set to " << error_check << " when trying to " << str << ", quitting now");
			exit(1);
		}
		if (displayDebug) {
			MAINLOG(LOG(str));
		}
	}

	#include "SOIL/SOIL.h"
	typedef struct {
		unsigned char *content;
		int width;
		int height;
	}				t_tex;

	void loadTextureShader( int index, GLuint texture, std::string texture_file )
	{
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, texture);

		// load image
		t_tex img;
		img.content = SOIL_load_image(texture_file.c_str(), &img.width, &img.height, 0, SOIL_LOAD_RGBA);
		if (!img.content) {
			LOGERROR("failed to load image " << texture_file << " because:\n" << SOIL_last_result());
			exit(1);
		}

		// load image as texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height,
			0, GL_RGBA, GL_UNSIGNED_BYTE, img.content);
				
		// set settings for texture wraping and size modif
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because pixel art, otherwise GL_LINEAR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		SOIL_free_image_data(img.content);

		check_glstate("Successfully loaded img[" + std::to_string(index) + "] " + texture_file, true);
	}

	void loadSubTextureArray( int width, int height, int layer, std::string texture_file )
	{
		// load image
		t_tex img;
		img.content = SOIL_load_image(texture_file.c_str(), &img.width, &img.height, 0, SOIL_LOAD_RGBA);
		if (!img.content) {
			LOGERROR("failed to load image " << texture_file << " because:\n" << SOIL_last_result());
			exit(1);
		}

		if (img.width != width || img.height != height) {
			LOGERROR(texture_file << ": image size not " << width << "x" << height << " but " << img.width << "x" << img.height);
			exit(1);
		}
		// Upload pixel data.
		// The first 0 refers to the mipmap level (level 0, since there's only 1)
		// The following 2 zeroes refers to the x and y offsets in case you only want to specify a subrectangle.
		// 256x256 size of rect, 1 = depth of layer
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, img.width, img.height, 1, GL_RGBA, GL_UNSIGNED_BYTE, img.content);
				
		// set settings for texture wraping and size modif
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		SOIL_free_image_data(img.content);

		check_glstate("Successfully loaded subTextureArray[" + std::to_string(layer) + "] " + texture_file, false);
	}
	
	/**
	 * @brief push back rectangle made out of 4 given pts to arr, (1 << 20) and (1 << 21) are added
	 * @param arr vector containing vertices
	 * @param pts 4 corners of rectangle, in clockwise order
	 * @param texture texture specifications of top left corner
	 * @param light lighting specifications of top left corner
	 * @param dx offset in x coord of texture for right pts
	 * @param dy offset in y coord of texture for bottom pts
	 * @param xflip [optional] if true, texture is flipped along the x axis
	 * @param yflip [optional] if true, texture is flipped along the y axis
	 */
	void addQuads( std::vector<t_shaderInput>& arr, std::array<glm::vec3, 4> pts, int texture, int light, int dx, int dy, bool xflip, bool yflip )
	{
		dx = ((dx - 1) << 12) + (1 << 20);
		dy = ((dy - 1) << 16) + (1 << 21);
		int adx = 0, ady = 0;
		if (xflip) {
			adx = dx;
			dx = 0;
		}
		if (yflip) {
			ady = dy;
			dy = 0;
		}
		t_shaderInput v0 = {texture + adx + ady, light, pts[0]};
		t_shaderInput v1 = {texture + dx  + ady, light, pts[1]};
		t_shaderInput v2 = {texture + adx + dy, light, pts[2]};
		t_shaderInput v3 = {texture + dx  + dy, light, pts[3]};

		arr.push_back(v0);
		arr.push_back(v1);
		arr.push_back(v2);

		arr.push_back(v1);
		arr.push_back(v3);
		arr.push_back(v2);
	}

	/**
	 * @brief same as other addQuads, but 4 lights are used for light corners
	 */
	void addQuads( std::vector<t_shaderInput>& arr, std::array<glm::vec3, 4> pts, int texture, std::array<int, 4> lights, int dx, int dy, bool xflip, bool yflip )
	{
		dx = ((dx - 1) << 12) + (1 << 20);
		dy = ((dy - 1) << 16) + (1 << 21);
		int adx = 0, ady = 0;
		if (xflip) {
			adx = dx;
			dx = 0;
		}
		if (yflip) {
			ady = dy;
			dy = 0;
		}
		t_shaderInput v0 = {texture + adx + ady, lights[0], pts[0]};
		t_shaderInput v1 = {texture + dx  + ady, lights[1], pts[1]};
		t_shaderInput v2 = {texture + adx + dy, lights[2], pts[2]};
		t_shaderInput v3 = {texture + dx  + dy, lights[3], pts[3]};

		arr.push_back(v0);
		arr.push_back(v1);
		arr.push_back(v2);

		arr.push_back(v1);
		arr.push_back(v3);
		arr.push_back(v2);
	}

	/**
	 * @brief push back rectangle made out of 4 given pts to model shader, (1 << 17) and (1 << 18) are added
	 * @param arr vector containing vertices
	 * @param pts 4 corners of rectangle, in clockwise order
	 * @param spec specifications of top left corner
	 * @param dx offset in x coord of texture for right pts
	 * @param dy offset in y coord of texture for bottom pts
	 * @param xflip [optional] if true, texture is flipped along the x axis
	 * @param yflip [optional] if true, texture is flipped along the y axis
	 */
	void addQuads( std::vector<t_shaderInputModel>& arr, std::array<glm::vec3, 4> pts, int spec, int dx, int dy, bool xflip, bool yflip )
	{
		dx = (dx - 1) + (1 << 17);
		dy = ((dy - 1) << 6) + (1 << 18);
		int adx = 0, ady = 0;
		if (xflip) {
			adx = dx;
			dx = 0;
		}
		if (yflip) {
			ady = dy;
			dy = 0;
		}
		t_shaderInputModel v0 = {spec + adx + ady, pts[0]};
		t_shaderInputModel v1 = {spec + dx  + ady, pts[1]};
		t_shaderInputModel v2 = {spec + adx + dy, pts[2]};
		t_shaderInputModel v3 = {spec + dx  + dy, pts[3]};

		arr.push_back(v0);
		arr.push_back(v1);
		arr.push_back(v2);

		arr.push_back(v1);
		arr.push_back(v3);
		arr.push_back(v2);
	}
			
	/**
	 * @brief push back rectangle made out of 4 given pts to model shader, (1 << 17) and (1 << 18) are added
	 * @param arr vector containing vertices
	 * @param pts 4 corners of rectangle, in clockwise order
	 * @param spec specifications of top left corner
	 * @param dx offset in x coord of texture for right pts
	 * @param dy offset in y coord of texture for bottom pts
	 * @param xflip [optional] if true, texture is flipped along the x axis
	 * @param yflip [optional] if true, texture is flipped along the y axis
	 */
	void addQuads( std::vector<t_shaderInputPart>& arr, std::array<glm::vec3, 4> pts, int spec, int dx, int dy, bool xflip, bool yflip )
	{
		dx = (dx - 1) + (1 << 17);
		dy = ((dy - 1) << 8) + (1 << 18);
		int adx = 0, ady = 0;
		if (xflip) {
			adx = dx;
			dx = 0;
		}
		if (yflip) {
			ady = dy;
			dy = 0;
		}
		t_shaderInputPart v0 = {spec + adx + ady, pts[0]};
		t_shaderInputPart v1 = {spec + dx  + ady, pts[1]};
		t_shaderInputPart v2 = {spec + adx + dy, pts[2]};
		t_shaderInputPart v3 = {spec + dx  + dy, pts[3]};

		arr.push_back(v0);
		arr.push_back(v1);
		arr.push_back(v2);

		arr.push_back(v1);
		arr.push_back(v3);
		arr.push_back(v2);
	}
};

namespace utils::block {
	/* * we simulate that flowers are air block in order to display adjacent blocks properly
	* also use this for all 'see-through' blocks like leaves, water..
	* if air_leaves, leaves are considered as air
	* if air_glass, glass is considered as air
	* if air_water, water is NOT considered as air (I know this is dumb)*/
	int air_flower( int value, bool air_leaves, bool air_glass, bool air_water )
	{
		value &= mask::blocks::type;
		if (air_water && value == blocks::water) {
			return (value);
		}
		(void)air_leaves;(void)air_glass;
		// if (value >= blocks::poppy || value == blocks::cactus
		// 	|| (air_leaves && value == blocks::oak_leaves)
		// 	|| (air_glass && value == blocks::glass)) {
		if (s_blocks[value]->transparent) {
			return (0);
		}
		return (value);
	}

	int opposite_dir( int dir )
	{
		switch (dir) {
			case face_dir::minus_x:
				return (face_dir::plus_x);
			case face_dir::plus_x:
				return (face_dir::minus_x);
			case face_dir::minus_y:
				return (face_dir::plus_y);
			case face_dir::plus_y:
				return (face_dir::minus_y);
			case face_dir::minus_z:
				return (face_dir::plus_z);
			case face_dir::plus_z:
				return (face_dir::minus_z);
		}
		return (face_dir::minus_x);
	}

	/**
	 * @brief checks if a specific face should be drawn by using neighbour
	 * @param value  value of block being drawn
	 * @param next   value of neighbouring block
	 * @param dir    dir from value to next
	 * @return bool face_should_be_drawn
	 */
	bool visible_face( int value, int next, int dir )
	{
		int type = TYPE(value);
		int nextType = TYPE(next);
		geometry valueShape = s_blocks[type]->geometry;
		geometry nextShape = s_blocks[nextType]->geometry;

		switch (valueShape) {
			case geometry::none:
				return (type == blocks::oak_sign);
			case geometry::farmland:
				if (dir == face_dir::plus_z) {
					return (true);
				}
				break ;
			default:
				break ;
		}
		if (nextType == blocks::oak_leaves && (type != blocks::oak_leaves
			|| (type == blocks::oak_leaves
			&& (dir == face_dir::plus_x || dir == face_dir::plus_y || dir == face_dir::plus_z)))) {
			return (true);
		}
		switch (nextShape) {
			case geometry::none:
			case geometry::cross:
			case geometry::torch:
			case geometry::fence:
			case geometry::door:
			case geometry::trapdoor:
			case geometry::crop:
			case geometry::lever:
			case geometry::dust:
			case geometry::piston: // might want to change this..
			case geometry::button:
				return (true);
			case geometry::repeater:
				return (dir != face_dir::plus_z && valueShape != geometry::repeater);
			case geometry::glass:
			case geometry::glass_pane:
				return (valueShape != nextShape);
			case geometry::slab:
				if ((dir == face_dir::plus_z && !(next & mask::slab::top))
					|| (dir == face_dir::minus_z && (next & mask::slab::top))) {
					return (false);
				}
				return (true);
			case geometry::stairs:
				if ((dir == face_dir::plus_z && !(next & mask::stairs::top))
					|| (dir == face_dir::minus_z && (next & mask::stairs::top))) {
					return (false);
				}
				return (true);
			case geometry::farmland:
				if (dir == face_dir::minus_z) {
					return (true);
				} else if (dir == face_dir::plus_z) {
					return (false);
				}
				return (valueShape != geometry::farmland);
			default:
				break ;
		}
		return (false);
	}
};

namespace utils::fluid {
	/* takes offsets of face's corners and compute texture coordinates to represent direction of liquid's flow
	* offsets [0:7], 0 being highest
	* 1 << 8 => waterStill
	* 1 << 9 => waterFlow
	* 1 << 10 => texture point +1 x
	* 1 << 11 => texture point +1 y
	* res[0] = top left
	* res[1] = top right
	* res[2] = bottom left
	* res[3] = bottom right
	* res[4] = texture used
	*/
	std::array<int, 5> compute_texcoord_offsets( int o0, int o1, int o2, int o3 )
	{
		if (o0 == o1 && o0 == o2 && o0 == o3) {
			return {0, 1 << 10, 2 << 10, 3 << 10, 1 << 8}; // still water
		}
		// from now on, flowing water
		int max = std::min({o0, o1, o2, o3}); // min is highest point
		if (max == o0) {
			if (o0 == o2) {
				return {1 << 10, 3 << 10, 0, 2 << 10, 1 << 9}; // +x 0y
			} else if (o0 == o1) {
				return {0, 1 << 10, 2 << 10, 3 << 10, 1 << 9}; // 0x -y
			}
			return {4 << 10, 9 << 10, 8 << 10, 6 << 10, 1 << 9}; // +x -y
		} else if (max == o1) {
			if (o1 == o3) {
				return {2 << 10, 0, 3 << 10, 1 << 10, 1 << 9}; // -x 0y
			}
			return {8 << 10, 4 << 10, 6 << 10, 9 << 10, 1 << 9}; // -x -y
		} else if (max == o2) {
			if (o2 == o3) {
				return {3 << 10, 2 << 10, 1 << 10, 0, 1 << 9}; // 0x +y
			}
			return {9 << 10, 6 << 10, 4 << 10, 8 << 10, 1 << 9}; // +x +y

		}
		return {6 << 10, 8 << 10, 9 << 10, 4 << 10, 1 << 9}; // -x +y
	}

	void face_water_vertices( std::vector<glm::ivec4> &vertices, glm::ivec4 &v0, glm::ivec4 &v1, glm::ivec4 &v2, glm::ivec4 &v3 )
	{
		vertices.push_back(v0);
		vertices.push_back(v1);
		vertices.push_back(v2);

		vertices.push_back(v1);
		vertices.push_back(v3);
		vertices.push_back(v2);
	}
};

namespace utils::math {
	// takes value in range [start; end] and outputs gradient in range [value_start; value_end]
	// if value out of range, outputs value_start or value_end
	double gradient( double value, double start, double end, double value_start, double value_end )
	{
		if (value <= start) {
			return (value_start);
		} else if (value >= end) {
			return (value_end);
		}

		double progress = (value - start) / (end - start);
		return (value_start + progress * (value_end - value_start));
	}

	float dist2( glm::vec3 pa, glm::vec3 pb )
	{
		float a = pb.x - pa.x;
		float b = pb.y - pa.y;
		float c = pb.z - pa.z;
		return (a * a + b * b + c * c);
	}

	short maxs( short a, short b )
	{
		return ((a > b) ? a : b);
	}

	int chunk_pos( int pos )
	{
		if (settings::consts::chunk_size == 16) {
			(pos >= 0 || !(pos & 0xF))
					? pos -= pos & 0xF
					: pos -= settings::consts::chunk_size + pos & 0xF;
			return (pos);
		}
		(pos >= 0 || !(pos % settings::consts::chunk_size))
				? pos -= pos % settings::consts::chunk_size
				: pos -= settings::consts::chunk_size + pos % settings::consts::chunk_size;
		return (pos);
	}

	void sort_chunks( glm::vec3 pos, std::vector<std::shared_ptr<Chunk>>& chunks )
	{
		// Bench b;
		int posX = chunk_pos(pos.x);
		int posY = chunk_pos(pos.y);

		#if 1
		// size_t size = chunks.size();
		std::vector<std::pair<int, std::shared_ptr<Chunk>>> dists;
		dists.reserve(chunks.capacity());
		for (auto& c: chunks) {
			dists.push_back(std::pair<int, std::shared_ptr<Chunk>>(c->manhattanDist(posX, posY), c));
		}
		// std::cout << "in sort chunks, dists size = " << dists.size() << std::endl;
		// b.stamp("SORT - manhattan");
		std::sort(dists.begin(), dists.end(), []( std::pair<int, std::shared_ptr<Chunk>> a, std::pair<int, std::shared_ptr<Chunk>> b )
								{
									return (a.first > b.first);
								});
		// b.stamp("SORT - chunks");

		chunks.clear();
		chunks.reserve(dists.capacity());
		// int cnt = 0;
		// sky and water sorted only if new chunk or chunk in cross centered on player
		for (auto d: dists) {
			std::shared_ptr<Chunk> c = d.second;
			int diffX = posX - c->getStartX();
			int diffY = posY - c->getStartY();
			if (!c->getSortedOnce() || (diffX <= settings::consts::chunk_size && diffX >= -settings::consts::chunk_size) || (diffY <= settings::consts::chunk_size && diffY >= -settings::consts::chunk_size)) {
				c->sort_sky(pos, false);
				c->sort_water(pos, false);
				// ++cnt;
			}
			chunks.push_back(c);
		}
		// std::cout << "\tsorted " << cnt << std::endl;
		// b.stamp("SORT - sky water");
		dists.clear();
		// b.stop("TOTAL");
		#else
		std::multimap<int, std::shared_ptr<Chunk>> dists;
		for (auto c: chunks) {
			dists.insert({c->manhattanDist(posX, posY), c});
		}
		// std::cout << "in sort chunks, dists size = " << dists.size() << std::endl;
		b.stamp("SORT - manhattan");
		std::sort(dists.begin(), dists.end());
		b.stamp("SORT - chunks");

		chunks.clear();
		chunks.reserve(dists.size());
		// int cnt = 0;
		// sky and water sorted only if new chunk or chunk in cross centered on player
		for (auto it = dists.rbegin(); it != dists.rend(); it++) {
			std::shared_ptr<Chunk> c = it->second;
			int diffX = posX - c->getStartX();
			int diffY = posY - c->getStartY();
			if (!c->getSortedOnce() || (diffX <= settings::consts::chunk_size && diffX >= -settings::consts::chunk_size) || (diffY <= settings::consts::chunk_size && diffY >= -settings::consts::chunk_size)) {
				c->sort_sky(pos, false);
				c->sort_water(pos, false);
				// ++cnt;
			}
			chunks.push_back(c);
		}
		// std::cout << "\tsorted " << cnt << std::endl;
		b.stamp("SORT - sky water");
		dists.clear();

		#endif
		// std::cout << "out sort chunks, chunks size " << chunks.size() << std::endl;
	}

	/**
	 * @brief returns all the voxels that are traversed by a ray going from start to end
	 * @param start : world position where the ray starts
	 * @param end   : world position where the ray end
	 * @return vector of voxel ids hit by the ray in temporal order
	 *
	 * J. Amanatides, A. Woo. A Fast Voxel Traversal Algorithm for Ray Tracing. Eurographics '87
	 */
	std::vector<glm::ivec3> voxel_traversal( const glm::vec3 ray_start, const glm::vec3 ray_end )
	{
		std::vector<glm::ivec3> visited_voxels;

		// This id of the first/current voxel hit by the ray.
		// Using floor (round down) is actually very important,
		// the implicit int-casting will round up for negative numbers.
		glm::ivec3 current_voxel(std::floor(ray_start.x),
									std::floor(ray_start.y),
									std::floor(ray_start.z));

		// The id of the last voxel hit by the ray.
		const glm::ivec3 last_voxel(std::floor(ray_end.x),
									std::floor(ray_end.y),
									std::floor(ray_end.z));

		// Compute ray direction.
		const glm::vec3 ray = ray_end - ray_start;

		// In which direction the voxel ids are incremented.
		const int stepX = (ray.x >= 0) ? 1 : -1;
		const int stepY = (ray.y >= 0) ? 1 : -1;
		const int stepZ = (ray.z >= 0) ? 1 : -1;

		// Distance along the ray to the next voxel border from the current position.
		float next_voxel_boundary_x = current_voxel.x + stepX;
		float next_voxel_boundary_y = current_voxel.y + stepY;
		float next_voxel_boundary_z = current_voxel.z + stepZ;

		// we do this because if we are in x = 1.7 and go to x--, dist to next block is 0.7 and not 1.7
		if (current_voxel.x != last_voxel.x && ray.x < 0) { ++next_voxel_boundary_x; }
		if (current_voxel.y != last_voxel.y && ray.y < 0) { ++next_voxel_boundary_y; }
		if (current_voxel.z != last_voxel.z && ray.z < 0) { ++next_voxel_boundary_z; }

		// tMaxX, tMaxY, tMaxZ -- time until next intersection with voxel-border
		// the value of t at which the ray crosses the first vertical voxel boundary
		float tMaxX = (ray.x != 0) ? (next_voxel_boundary_x - ray_start.x) / ray.x : FLT_MAX; //
		float tMaxY = (ray.y != 0) ? (next_voxel_boundary_y - ray_start.y) / ray.y : FLT_MAX; //
		float tMaxZ = (ray.z != 0) ? (next_voxel_boundary_z - ray_start.z) / ray.z : FLT_MAX; //

		// tDeltaX, tDeltaY, tDeltaZ --
		// time it takes to travel a distance of 1! voxel in a given direction
		// can only be FLT_MAX if we never go in that direction
		const float tDeltaX = (ray.x != 0) ? 1.0f / ray.x * stepX : FLT_MAX;
		const float tDeltaY = (ray.y != 0) ? 1.0f / ray.y * stepY : FLT_MAX;
		const float tDeltaZ = (ray.z != 0) ? 1.0f / ray.z * stepZ : FLT_MAX;

		visited_voxels.push_back(current_voxel);
		while (last_voxel != current_voxel) {
			if (tMaxX < tMaxY && tMaxX < tMaxZ) { // x will reach border before others
				current_voxel.x += stepX;
				tMaxX += tDeltaX;
			} else if (tMaxY < tMaxZ) { // y will reach border before others
				current_voxel.y += stepY;
				tMaxY += tDeltaY;
			} else { // z will reach border before others
				current_voxel.z += stepZ;
				tMaxZ += tDeltaZ;
			}
			visited_voxels.push_back(current_voxel);
		}
		return (visited_voxels);
	}

	/**
	 * @brief returns intersection point between line and plane
	 * @param camPos : world position where the ray starts (la)
	 * @param camDir : vector director of ray (lab)
	 * @param p0 : point in plane
	 * @param cross : vectorial | cross product (p01 x p02)
	 * @return Intersection point. Or vec3 with z == -1 if no intersection.
	 *
	 * https://en.wikipedia.org/wiki/Line-plane_intersection
	 */
	glm::vec3 line_plane_intersection( const glm::vec3 camPos, const glm::vec3 camDir, const glm::vec3 p0, const glm::vec3 cross ) {
		float determinant = -glm::dot(camDir, cross);
		if (!determinant) { // div zero -> line parallel to plane -> no intersection
			return {FLT_MIN, FLT_MIN, FLT_MIN};
		}
		float t = glm::dot(cross, camPos - p0) / determinant;
		return {camPos.x + camDir.x * t, camPos.y + camDir.y * t, camPos.z + camDir.z * t};
	}

	static bool line_rectangle_intersection( const glm::vec3 camPos, const glm::vec3 camDir, const glm::vec3 p0, const glm::vec3 size )
	{
		glm::vec3 res;
		if (size.z) {
			res = line_plane_intersection(camPos, camDir, p0, glm::cross(glm::vec3(size.x, size.y, 0), glm::vec3(0, 0, size.z)));
		} else {
			res = line_plane_intersection(camPos, camDir, p0, glm::cross(glm::vec3(size.x, 0, 0), glm::vec3(0, size.y, 0)));
		}
		return (res.x >= p0.x && res.x <= p0.x + size.x
				&& res.y >= p0.y && res.y <= p0.y + size.y
				&& res.z >= p0.z && res.z <= p0.z + size.z);
	}

	// 'cube' but actually parallelepiped rectangle
	bool line_cube_intersection( const glm::vec3 camPos, const glm::vec3 camDir, const glm::vec3 cubeCenter, const glm::vec3 cubeHalfSize )
	{
		// std::cout << "line_cube_intersection: " << camPos.x << ", " << camPos.y << ", " << camPos.z << " -> " << camDir.x << ", " << camDir.y << ", " << camDir.z << " and " << cubeCenter.x << ", " << cubeCenter.y << ", " << cubeCenter.z << " -> " << cubeHalfSize.x << ", " << cubeHalfSize.y << ", " << cubeHalfSize.z << std::endl;
		return (line_rectangle_intersection(camPos, camDir, cubeCenter - cubeHalfSize, {2 * cubeHalfSize.x, 0, 2 * cubeHalfSize.z}) // face_dir::minus_y
				|| line_rectangle_intersection(camPos, camDir, cubeCenter - cubeHalfSize, {0, 2 * cubeHalfSize.y, 2 * cubeHalfSize.z}) // face_dir::minus_x
				|| line_rectangle_intersection(camPos, camDir, {cubeCenter.x + cubeHalfSize.x, cubeCenter.y - cubeHalfSize.y, cubeCenter.z - cubeHalfSize.z}, {0, 2 * cubeHalfSize.y, 2 * cubeHalfSize.z}) // face_dir::plus_x
				|| line_rectangle_intersection(camPos, camDir, {cubeCenter.x - cubeHalfSize.x, cubeCenter.y + cubeHalfSize.y, cubeCenter.z - cubeHalfSize.z}, {2 * cubeHalfSize.x, 0, 2 * cubeHalfSize.z}) // face_dir::plus_y
				|| line_rectangle_intersection(camPos, camDir, {cubeCenter.x - cubeHalfSize.x, cubeCenter.y - cubeHalfSize.y, cubeCenter.z + cubeHalfSize.z}, {2 * cubeHalfSize.x, 2 * cubeHalfSize.y, 0}) // face_dir::plus_z
				// no need for this check|| line_rectangle_intersection(camPos, camDir, {cubeCenter.x - cubeHalfSize.x, cubeCenter.y - cubeHalfSize.y, cubeCenter.z - cubeHalfSize.z}, {2 * cubeHalfSize.x, 2 * cubeHalfSize.y, 0}) // face_dir::minus_z
				);
	}

	/**
	 * @brief return whether point is between segStart and segEnd, we already know point is part of line seg is part of
	 */
	static bool point_within_segment( const glm::vec3 segStart, const glm::vec3 segEnd, const glm::vec3 point )
	{
		return ((segStart.x != segEnd.x) ? (segStart.x <= point.x && point.x <= segEnd.x) || (segEnd.x <= point.x && point.x <= segStart.x)
										: (segStart.y <= point.y && point.y <= segEnd.y) || (segEnd.y <= point.y && point.y <= segStart.y));
	}

	/**
	 * @brief same as line_regtangle_intersection, but also check if intersection is between segment start and end
	 */
	static bool segment_rectangle_intersection( const glm::vec3 segStart, const glm::vec3 segEnd, const glm::vec3 p0, const glm::vec3 size )
	{
		glm::vec3 res = (size.z) ? line_plane_intersection(segStart, glm::normalize(segEnd - segStart), p0, glm::cross(glm::vec3(size.x, size.y, 0), glm::vec3(0, 0, size.z)))
								: line_plane_intersection(segStart, glm::normalize(segEnd - segStart), p0, glm::cross(glm::vec3(size.x, 0, 0), glm::vec3(0, size.y, 0)));

		return (res.x >= p0.x && res.x <= p0.x + size.x
				&& res.y >= p0.y && res.y <= p0.y + size.y
				&& res.z >= p0.z && res.z <= p0.z + size.z
				&& point_within_segment(segStart, segEnd, res));
	}

	/**
	 * @brief same as line_cube_intersection, but also check if intersection is between segment start and end
	 */
	bool segment_cube_intersection( const glm::vec3 segStart, const glm::vec3 segEnd, const glm::vec3 cubeCenter, const glm::vec3 cubeHalfSize )
	{
		return (segment_rectangle_intersection(segStart, segEnd, cubeCenter - cubeHalfSize, {2 * cubeHalfSize.x, 0, 2 * cubeHalfSize.z}) // face_dir::minus_y
				|| segment_rectangle_intersection(segStart, segEnd, cubeCenter - cubeHalfSize, {0, 2 * cubeHalfSize.y, 2 * cubeHalfSize.z}) // face_dir::minus_x
				|| segment_rectangle_intersection(segStart, segEnd, {cubeCenter.x + cubeHalfSize.x, cubeCenter.y - cubeHalfSize.y, cubeCenter.z - cubeHalfSize.z}, {0, 2 * cubeHalfSize.y, 2 * cubeHalfSize.z}) // face_dir::plus_x
				|| segment_rectangle_intersection(segStart, segEnd, {cubeCenter.x - cubeHalfSize.x, cubeCenter.y + cubeHalfSize.y, cubeCenter.z - cubeHalfSize.z}, {2 * cubeHalfSize.x, 0, 2 * cubeHalfSize.z}) // face_dir::plus_y
				|| segment_rectangle_intersection(segStart, segEnd, {cubeCenter.x - cubeHalfSize.x, cubeCenter.y - cubeHalfSize.y, cubeCenter.z + cubeHalfSize.z}, {2 * cubeHalfSize.x, 2 * cubeHalfSize.y, 0}) // face_dir::plus_z
				);
	}

	// returns if cubes a and b intersect
	// posA, posB are center points of cubes
	// WATCHOUT posA.z is bottom of cube a
	// sizeA, sizeB are halfSizes of cubes
	// WATCHOUT sizeA.z is fullHeight of cube a
	bool cube_cube_intersection( const glm::vec3 posA, const glm::vec3 sizeA, const glm::vec3 posB, const glm::vec3 sizeB )
	{
		return (posA.x - sizeA.x < posB.x + sizeB.x && posA.x + sizeA.x > posB.x - sizeB.x
				&& posA.y - sizeA.y < posB.y + sizeB.y && posA.y + sizeA.y > posB.y - sizeB.y
				&& posA.z < posB.z + sizeB.z && posA.z + sizeA.z > posB.z - sizeB.z);
	}
};

namespace utils::memory {
	// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

	#if __linux__

		#include <sys/sysinfo.h>

		void getMemoryUsage( size_t& residentSize, size_t& virtualSize )
		{
			struct sysinfo info;

			if (sysinfo(&info) == -1) {
				residentSize = 0;
				virtualSize = 0;
			} else {
				residentSize = info.totalram - info.freeram;
				virtualSize = info.totalram;
			}
		}

	#else

		#include <mach/mach.h>
		// #include <unistd.h>
		void getMemoryUsage( size_t& residentSize, size_t& virtualSize )
		{
			struct task_basic_info t_info;
			mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

			if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count) != KERN_SUCCESS) {
				residentSize = 0;
				virtualSize = 0;
			} else {
				// size_t bytesPerPage = sysconf(_SC_PAGESIZE);
				// residentSize = t_info.resident_size * bytesPerPage;
				// virtualSize = t_info.virtual_size * bytesPerPage;
				residentSize = t_info.resident_size;
				virtualSize = t_info.virtual_size;
			}
		}

	#endif
};