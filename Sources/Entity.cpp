#include "vox.h"

Entity::Entity( glm::vec3 position, glm::vec2 dir, int value, int amount )
    : _value(value), _amount(amount), _dura(0), _tool_dura(0), _rotation(0), _startTime(0), _position(position), _dir(dir, 1.0f)
{
	(void)_amount;
	(void)_dura;
	(void)_tool_dura;
	(void)_rotation;
    std::cout << "new Entity at " << position.x << ", " << position.y << ", " << position.z << ": " << s_blocks[value].name << std::endl;
}

Entity::~Entity( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

// update entity's position and push_back arr with new pos
// returns true if entity dispawns this frame
bool Entity::update( Chunk *chunk, std::vector<std::pair<int, glm::vec3>> &arr, double currentTime )
{
    double lifeTime = currentTime - _startTime;
    if (!_startTime) {
        _startTime = currentTime;
    } else if (lifeTime > 300) {
        return (true);
    }

    if (!chunk->collisionBox({_position.x, _position.y, _position.z + _dir.z}, 0, 0)) {
        _position.z += _dir.z;
        _dir = {_dir.x * 0.001f, _dir.y * 0.001f, _dir.z - 0.1f};
    }
    /*
    // if (_dir.x && !chunk->collisionBox({_position.x + _dir.x, _position.y, _position.z}, 0, 0)) {
    //     _position.x += _dir.x;
    //     _dir.x *= 0.9f;
    //     if (_dir.x < 0.1f) {
    //         _dir.x = 0;
    //     }
    // } else {
    //     _dir.x = 0;
    // }
    // if (_dir.y && !chunk->collisionBox({_position.x, _position.y + _dir.y, _position.z}, 0, 0)) {
    //     _position.y += _dir.y;
    //     _dir.y *= 0.9f;
    //     if (_dir.y < 0.1f) {
    //         _dir.y = 0;
    //     }
    // } else {
    //     _dir.y = 0;
    // }
    if (!chunk->collisionBox({_position.x, _position.y, _position.z + _dir.z}, 0, 0)) {
        _position.z += _dir.z;
        _dir.x -= 0.1f;
    } else {
        _dir.z = 0;
    }
    */
    //TODO check dist to player.
    float cosRot = glm::cos(lifeTime);
    float sinRot = glm::sin(lifeTime);

    if (_value > blocks::AIR && _value < blocks::WATER) {
        glm::vec3 p0 = {_position.x - 0.17f * cosRot, _position.y - 0.17f * sinRot, _position.z + 0.125f + cosRot / 4};
        glm::vec3 p1 = {_position.x + 0.17f * sinRot, _position.y - 0.17f * cosRot, _position.z + 0.125f + cosRot / 4};
        glm::vec3 p2 = {_position.x - 0.17f * cosRot, _position.y - 0.17f * sinRot, _position.z - 0.125f + cosRot / 4};
        glm::vec3 p3 = {_position.x + 0.17f * sinRot, _position.y - 0.17f * cosRot, _position.z - 0.125f + cosRot / 4};

        glm::vec3 p4 = {_position.x - 0.17f * sinRot, _position.y + 0.17f * cosRot, _position.z + 0.125f + cosRot / 4};
        glm::vec3 p5 = {_position.x + 0.17f * cosRot, _position.y + 0.17f * sinRot, _position.z + 0.125f + cosRot / 4};
        glm::vec3 p6 = {_position.x - 0.17f * sinRot, _position.y + 0.17f * cosRot, _position.z - 0.125f + cosRot / 4};
        glm::vec3 p7 = {_position.x + 0.17f * cosRot, _position.y + 0.17f * sinRot, _position.z - 0.125f + cosRot / 4};

        if (_value < blocks::POPPY) {
            bool orientedItem = 2 * (_value >= blocks::CRAFTING_TABLE && _value < blocks::BEDROCK);
            int itemLight = chunk->computePosLight(_position);

            int spec = blockGridX(_value, 0) + (blockGridY(_value) << 4) + (3 << 19) + (itemLight << 24);
            std::pair<int, glm::vec3> v0 = {spec, p4};
            std::pair<int, glm::vec3> v1 = {spec + 1 + (1 << 9) + (1 << 8), p0};
            std::pair<int, glm::vec3> v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p6};
            std::pair<int, glm::vec3> v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p2};
            arr.push_back(v0);
            arr.push_back(v1);
            arr.push_back(v2);
            arr.push_back(v1);
            arr.push_back(v3);
            arr.push_back(v2);
            spec = blockGridX(_value, 0) + (blockGridY(_value) << 4) + (4 << 19) + (itemLight << 24);
            v0 = {spec, p1};
            v1 = {spec + 1 + (1 << 9) + (1 << 8), p5};
            v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p3};
            v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
            arr.push_back(v0);
            arr.push_back(v1);
            arr.push_back(v2);
            arr.push_back(v1);
            arr.push_back(v3);
            arr.push_back(v2);
            spec = blockGridX(_value, orientedItem) + (blockGridY(_value) << 4) + (1 << 19) + (itemLight << 24);
            v0 = {spec, p0};
            v1 = {spec + 1 + (1 << 9) + (1 << 8), p1};
            v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
            v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p3};
            arr.push_back(v0);
            arr.push_back(v1);
            arr.push_back(v2);
            arr.push_back(v1);
            arr.push_back(v3);
            arr.push_back(v2);
            spec = blockGridX(_value, 0) + (blockGridY(_value) << 4) + (2 << 19) + (itemLight << 24);
            v0 = {spec, p5};
            v1 = {spec + 1 + (1 << 9) + (1 << 8), p4};
            v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p7};
            v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p6};
            arr.push_back(v0);
            arr.push_back(v1);
            arr.push_back(v2);
            arr.push_back(v1);
            arr.push_back(v3);
            arr.push_back(v2);
            spec = blockGridX(_value, 1) + (blockGridY(_value) << 4) + (0 << 19) + (itemLight << 24);
            v0 = {spec, p4};
            v1 = {spec + 1 + (1 << 9) + (1 << 8), p5};
            v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p0};
            v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p1};
            arr.push_back(v0);
            arr.push_back(v1);
            arr.push_back(v2);
            arr.push_back(v1);
            arr.push_back(v3);
            arr.push_back(v2);
            spec = blockGridX(_value, 1 + (_value == blocks::GRASS_BLOCK)) + (blockGridY(_value) << 4) + (5 << 19) + (itemLight << 24);
            v0 = {spec, p2};
            v1 = {spec + 1 + (1 << 9) + (1 << 8), p3};
            v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p6};
            v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
            arr.push_back(v0);
            arr.push_back(v1);
            arr.push_back(v2);
            arr.push_back(v1);
            arr.push_back(v3);
            arr.push_back(v2);
        /*} else if (_value == blocks::TORCH) {
            // TODO for now torches only in default middle-of-block configuration
            int spec = blockGridX(_value, 0) + (blockGridY(_value) << 4) + (0 << 19) + (15 << 24);
            p4 = {_position.x + 7.0f / 16 - 0.17f * sinRot, _position.y + 0.17f * cosRot, _position.z + 0.125f + cosRot / 4};
            std::pair<int, glm::vec3> v0 = {spec, p4};
            p0 = {_position.x + 7.0f / 16 - 0.17f * cosRot, _position.y - 0.17f * sinRot, _position.z + 0.125f + cosRot / 4};
            std::pair<int, glm::vec3> v1 = {spec + 1 + (1 << 8), p0};
            p6 = {_position.x + 7.0f / 16 - 0.17f * sinRot, _position.y + 0.17f * cosRot, _position.z - 0.125f + cosRot / 4};
            std::pair<int, glm::vec3> v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p6};
            p2 = {_position.x + 7.0f / 16 - 0.17f * cosRot, _position.y - 0.17f * sinRot, _position.z - 0.125f + cosRot / 4};
            std::pair<int, glm::vec3> v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p2};
            arr.push_back(v0);
            arr.push_back(v1);
            arr.push_back(v2);
            arr.push_back(v1);
            arr.push_back(v3);
            arr.push_back(v2);
            p0 = {_position.x + 9.0f / 16 - 0.17f * cosRot, _position.y - 0.17f * sinRot, _position.z + 0.125f + cosRot / 4};
            v0 = {spec, p0};
            p4 = {_position.x + 9.0f / 16 - 0.17f * sinRot, _position.y + 0.17f * cosRot, _position.z + 0.125f + cosRot / 4};
            v1 = {spec + 1 + (1 << 8), {p4.x + 9.0 / 16, p4.y, p4.z}};
            p2 = {_position.x + 9.0f / 16 - 0.17f * cosRot, _position.y - 0.17f * sinRot, _position.z - 0.125f + cosRot / 4};
            v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
            p6 = {_position.x + 9.0f / 16 - 0.17f * sinRot, _position.y + 0.17f * cosRot, _position.z - 0.125f + cosRot / 4};
            v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), {p6.x + 9.0 / 16, p6.y, p6.z}};
            arr.push_back(v0);
            arr.push_back(v1);
            arr.push_back(v2);
            arr.push_back(v1);
            arr.push_back(v3);
            arr.push_back(v2);
            p0 = {_position.x - 0.17f * cosRot, _position.y + 7.0f / 16 - 0.17f * sinRot, _position.z + 0.125f + cosRot / 4};
            v0 = {spec, p0};
            p1 = {_position.x + 0.17f * sinRot, _position.y + 7.0f / 16 - 0.17f * cosRot, _position.z + 0.125f + cosRot / 4};
            v1 = {spec + 1 + (1 << 8), p1};
            p2 = {_position.x - 0.17f * cosRot, _position.y + 7.0f / 16 - 0.17f * sinRot, _position.z - 0.125f + cosRot / 4};
            v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
            p3 = {_position.x + 0.17f * sinRot, _position.y + 7.0f / 16 - 0.17f * cosRot, _position.z - 0.125f + cosRot / 4};
            v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p3};
            arr.push_back(v0);
            arr.push_back(v1);
            arr.push_back(v2);
            arr.push_back(v1);
            arr.push_back(v3);
            arr.push_back(v2);
            p1 = {_position.x + 0.17f * sinRot, _position.y + 9.0f / 16 - 0.17f * cosRot, _position.z + 0.125f + cosRot / 4};
            v0 = {spec, p1};
            p0 = {_position.x - 0.17f * cosRot, _position.y + 9.0f / 16 - 0.17f * sinRot, _position.z + 0.125f + cosRot / 4};
            v1 = {spec + 1 + (1 << 8), p0};
            p3 = {_position.x + 0.17f * sinRot, _position.y + 9.0f / 16 - 0.17f * cosRot, _position.z - 0.125f + cosRot / 4};
            v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p3};
            p2 = {_position.x - 0.17f * cosRot, _position.y + 9.0f / 16 - 0.17f * sinRot, _position.z - 0.125f + cosRot / 4};
            v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p2};
            arr.push_back(v0);
            arr.push_back(v1);
            arr.push_back(v2);
            arr.push_back(v1);
            arr.push_back(v3);
            arr.push_back(v2);*/
        } else { // flowers
            int itemLight = chunk->computePosLight(_position);
            int spec = blockGridX(_value, 0) + (blockGridY(_value) << 4) + (0 << 19) + (itemLight << 24);
            std::pair<int, glm::vec3> v0 = {spec, p0};
            std::pair<int, glm::vec3> v1 = {spec + 1 + (1 << 9) + (1 << 8), p5};
            std::pair<int, glm::vec3> v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
            std::pair<int, glm::vec3> v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
            arr.push_back(v0);
            arr.push_back(v1);
            arr.push_back(v2);
            arr.push_back(v1);
            arr.push_back(v3);
            arr.push_back(v2);
            v0 = {spec, p1};
            v1 = {spec + 1 + (1 << 9) + (1 << 8), p4};
            v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p3};
            v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p6};
            arr.push_back(v0);
            arr.push_back(v1);
            arr.push_back(v2);
            arr.push_back(v1);
            arr.push_back(v3);
            arr.push_back(v2);
        }
    }
    return (false);
}
