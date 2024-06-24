#include "Blocks.hpp"
#include "logs.hpp"

void Block::getHitbox( glm::vec3* hitbox, int value ) const
{
	(void)value;
	hitbox[0] = hitboxCenter;
	hitbox[1] = hitboxHalfSize;
}

void Block::getSecondaryHitbox( glm::vec3* hitbox, int value ) const
{
	(void)hitbox;
	(void)value;
	LOGERROR("ERROR: getSecondaryHitbox of " << name << " called ");
}

void Slab::getHitbox( glm::vec3* hitbox, int value ) const
{
	hitbox[0] = {.5f, .5f, (value & mask::slab::top) ? .75f : .25f};
	hitbox[1] = {.5f, .5f, .25f};
}

void Stairs::getHitbox( glm::vec3* hitbox, int value ) const
{
	hitbox[0] = {.5f, .5f, (value & mask::stairs::top) ? .75f : .25f};
	hitbox[1] = {.5f, .5f, .25f};
}

void Stairs::getSecondaryHitbox( glm::vec3* hitbox, int value ) const
{
	const int orientation = (value >> offset::blocks::orientation) & 0x7;
	const int corners = (value >> offset::blocks::bitfield) & 0xF;
	const float z = (value & mask::stairs::top) ? .25f : .75f;
	switch (corners) {
		case corners::mm | corners::mp:
			hitbox[0] = {0.25f, 0.5f, z}; // hitboxCenter
			hitbox[1] = {0.25f, 0.5f, 0.25f}; // hitboxHalfSize
			break ;
		case corners::pm | corners::pp:
			hitbox[0] = {0.75f, 0.5f, z};
			hitbox[1] = {0.25f, 0.5f, 0.25f};
			break ;
		case corners::mm | corners::pm:
			hitbox[0] = {0.5f, 0.25f, z};
			hitbox[1] = {0.5f, 0.25f, 0.25f};
			break ;
		case corners::mp | corners::pp:
			hitbox[0] = {0.5f, 0.75f, z};
			hitbox[1] = {0.5f, 0.25f, 0.25f};
			break ;
		case corners::mm:
			hitbox[0] = {0.25f, 0.25f, z};
			hitbox[1] = {0.25f, 0.25f, 0.25f};
			break ;
		case corners::mp:
			hitbox[0] = {0.25f, 0.75f, z};
			hitbox[1] = {0.25f, 0.25f, 0.25f};
			break ;
		case corners::pm:
			hitbox[0] = {0.75f, 0.25f, z};
			hitbox[1] = {0.25f, 0.25f, 0.25f};
			break ;
		case corners::pp:
			hitbox[0] = {0.75f, 0.75f, z};
			hitbox[1] = {0.25f, 0.25f, 0.25f};
			break ;
		case corners::mm | corners::mp | corners::pm:
			if (orientation == face_dir::plus_x) {
				hitbox[0] = {0.25f, 0.5f, z};
				hitbox[1] = {0.25f, 0.5f, 0.25f};
			} else {
				hitbox[0] = {0.5f, 0.25f, z};
				hitbox[1] = {0.5f, 0.25f, 0.25f};
			}
			break ;
		case corners::mm | corners::mp | corners::pp:
			if (orientation == face_dir::plus_x) {
				hitbox[0] = {0.25f, 0.5f, z};
				hitbox[1] = {0.25f, 0.5f, 0.25f};
			} else {
				hitbox[0] = {0.5f, 0.75f, z};
				hitbox[1] = {0.5f, 0.25f, 0.25f};
			}
			break ;
		case corners::pm | corners::pp | corners::mm:
			if (orientation == face_dir::minus_x) {
				hitbox[0] = {0.75f, 0.5f, z};
				hitbox[1] = {0.25f, 0.5f, 0.25f};
			} else {
				hitbox[0] = {0.5f, 0.25f, z};
				hitbox[1] = {0.5f, 0.25f, 0.25f};
			}
			break ;
		case corners::pm | corners::pp | corners::mp:
			if (orientation == face_dir::minus_x) {
				hitbox[0] = {0.75f, 0.5f, z};
				hitbox[1] = {0.25f, 0.5f, 0.25f};
			} else {
				hitbox[0] = {0.5f, 0.75f, z};
				hitbox[1] = {0.5f, 0.25f, 0.25f};
			}
			break ;
	}
}

void Fence::getHitbox( glm::vec3* hitbox, int value ) const
{
	hitbox[0] = {0.5f, 0.5f, 0.5f};
	hitbox[1] = {0.125f, 0.125f, 0.5f};
	int bitfield = (value >> offset::blocks::bitfield);
	switch (bitfield & (fence::mx | fence::px)) {
		case fence::mx:
			hitbox[0].x = 0.375f;
			hitbox[1].x = 0.375f;
			break ;
		case fence::px:
			hitbox[0].x = 1 - 0.375f;
			hitbox[1].x = 0.375f;
			break ;
		case fence::mx | fence::px:
			hitbox[1].x = 0.5f;
			break ;
	}
	switch (bitfield & (fence::my | fence::py)) {
		case fence::my:
			hitbox[0].y = 0.375f;
			hitbox[1].y = 0.375f;
			break ;
		case fence::py:
			hitbox[0].y = 1 - 0.375f;
			hitbox[1].y = 0.375f;
			break ;
		case fence::my | fence::py:
			hitbox[1].y = 0.5f;
			break ;
	}
}

void Door::getHitbox( glm::vec3* hitbox, int value ) const
{
	int orientation = ((value >> offset::blocks::orientation) & 0x7);
	int bitfield = (value >> offset::blocks::bitfield);
	bool open = !!(bitfield & door::open) ^ ((value >> offset::redstone::powered) & 0x1);
	switch (orientation) {
		case face_dir::minus_x:
			if (!open) {
				hitbox[0] = {0.90625f, 0.5f, 0.5f};
				hitbox[1] = {0.09375f, 0.5f, 0.5f};
			} else {
				hitbox[0] = {0.5f, (bitfield & door::right_hinge) ? 0.09375f : 0.90625f, 0.5f};
				hitbox[1] = {0.5f, 0.09375f, 0.5f};
			}
			break ;
		case face_dir::plus_x:
			if (!open) {
				hitbox[0] = {0.09375f, 0.5f, 0.5f};
				hitbox[1] = {0.09375f, 0.5f, 0.5f};
			} else {
				hitbox[0] = {0.5f, (bitfield & door::right_hinge) ? 0.90625f : 0.09375f, 0.5f};
				hitbox[1] = {0.5f, 0.09375f, 0.5f};
			}
			break ;
		case face_dir::minus_y:
			if (!open) {
				hitbox[0] = {0.5f, 0.90625f, 0.5f};
				hitbox[1] = {0.5f, 0.09375f, 0.5f};
			} else {
				hitbox[0] = {(bitfield & door::right_hinge) ? 0.90625f : 0.09375f, 0.5f, 0.5f};
				hitbox[1] = {0.09375f, 0.5f, 0.5f};
			}
			break ;
		case face_dir::plus_y:
			if (!open) {
				hitbox[0] = {0.5f, 0.09375f, 0.5f};
				hitbox[1] = {0.5f, 0.09375f, 0.5f};
			} else {
				hitbox[0] = {(bitfield & door::right_hinge) ? 0.09375f : 0.90625f, 0.5f, 0.5f};
				hitbox[1] = {0.09375f, 0.5f, 0.5f};
			}
			break ;
	}
}

void Trapdoor::getHitbox( glm::vec3* hitbox, int value ) const
{
	int orientation = ((value >> offset::blocks::orientation) & 0x7);
	int bitfield = (value >> offset::blocks::bitfield);
	bool open = !!(bitfield & door::open) ^ !!((bitfield << 12) & mask::redstone::powered);
	if (!open) {
		hitbox[0] = {0.5f, 0.5f, (bitfield & door::upper_half) ? 0.90625f : 0.09375f};
		hitbox[1] = {0.5f, 0.5f, 0.09375f};
	} else {
		switch (orientation) {
			case face_dir::minus_x:
				hitbox[0] = {0.90625f, 0.5f, 0.5f};
				hitbox[1] = {0.09375f, 0.5f, 0.5f};
				break ;
			case face_dir::plus_x:
				hitbox[0] = {0.09375f, 0.5f, 0.5f};
				hitbox[1] = {0.09375f, 0.5f, 0.5f};
				break ;
			case face_dir::minus_y:
				hitbox[0] = {0.5f, 0.90625f, 0.5f};
				hitbox[1] = {0.5f, 0.09375f, 0.5f};
				break ;
			case face_dir::plus_y:
				hitbox[0] = {0.5f, 0.09375f, 0.5f};
				hitbox[1] = {0.5f, 0.09375f, 0.5f};
				break ;
		}
	}
}

void Button::getHitbox( glm::vec3* hitbox, int value ) const
{
	int orientation = ((value >> offset::blocks::orientation) & 0x7);
	int bitfield = (value >> offset::blocks::bitfield);
	hitbox[0] = {0.5f, 0.5f, 1.5f * one16th};
	hitbox[1] = {0,    0,    1.5f * one16th};
	switch (bitfield & 0x3) {
		case placement::wall:
			switch (orientation) {
				case face_dir::minus_x:
					hitbox[0] = {14.5f * one16th, 0.5f,           0.5f};
					hitbox[1] = { 1.5f * one16th, 3.0f * one16th, 4.0f * one16th};
					break ;
				case face_dir::plus_x:
					hitbox[0] = { 1.5f * one16th, 0.5f,           0.5f};
					hitbox[1] = { 1.5f * one16th, 3.0f * one16th, 4.0f * one16th};
					break ;
				case face_dir::minus_y:
					hitbox[0] = {0.5f,           14.5f * one16th, 0.5f};
					hitbox[1] = {3.0f * one16th,  1.5f * one16th, 4.0f * one16th};
					break ;
				case face_dir::plus_y:
					hitbox[0] = {0.5f,           1.5f * one16th, 0.5f};
					hitbox[1] = {3.0f * one16th, 1.5f * one16th, 4.0f * one16th};
					break ;
			}
			break ;
		case placement::floor:
		case placement::ceiling:
			hitbox[0].z = ((bitfield & 0x3) == placement::floor) ? 1.5f * one16th : 14.5f * one16th;
			switch (orientation) {
				case face_dir::minus_x:
				case face_dir::plus_x:
					hitbox[1].x = 4.0f * one16th;
					hitbox[1].y = 3.0F * one16th;
					break ;
				case face_dir::minus_y:
				case face_dir::plus_y:
					hitbox[1].x = 3.0f * one16th;
					hitbox[1].y = 4.0f * one16th;
					break ;
			}
			break ;
	}
}

void Lever::getHitbox( glm::vec3* hitbox, int value ) const
{
	int orientation = ((value >> offset::blocks::orientation) & 0x7);
	int bitfield = (value >> offset::blocks::bitfield);
	hitbox[0] = {0.5f, 0.5f, 1.5f * one16th};
	hitbox[1] = {0,    0,    1.5f * one16th};
	switch (bitfield & 0x3) {
		case placement::wall:
			switch (orientation) {
				case face_dir::minus_x:
					hitbox[0] = {14.5f * one16th, 0.5f,           0.5f};
					hitbox[1] = { 1.5f * one16th, 3.0f * one16th, 4.0f * one16th};
					break ;
				case face_dir::plus_x:
					hitbox[0] = { 1.5f * one16th, 0.5f,           0.5f};
					hitbox[1] = { 1.5f * one16th, 3.0f * one16th, 4.0f * one16th};
					break ;
				case face_dir::minus_y:
					hitbox[0] = {0.5f,           14.5f * one16th, 0.5f};
					hitbox[1] = {3.0f * one16th,  1.5f * one16th, 4.0f * one16th};
					break ;
				case face_dir::plus_y:
					hitbox[0] = {0.5f,           1.5f * one16th, 0.5f};
					hitbox[1] = {3.0f * one16th, 1.5f * one16th, 4.0f * one16th};
					break ;
			}
			break ;
		case placement::floor:
		case placement::ceiling:
			hitbox[0].z = ((bitfield & 0x3) == placement::floor) ? 1.5f * one16th : 14.5f * one16th;
			switch (orientation) {
				case face_dir::minus_x:
				case face_dir::plus_x:
					hitbox[1].x = 4.0f * one16th;
					hitbox[1].y = 3.0F * one16th;
					break ;
				case face_dir::minus_y:
				case face_dir::plus_y:
					hitbox[1].x = 3.0f * one16th;
					hitbox[1].y = 4.0f * one16th;
					break ;
			}
			break ;
	}
}

void OakSign::getHitbox( glm::vec3* hitbox, int value ) const
{
	int orientation = ((value >> offset::blocks::orientation) & 0x7);
	switch (orientation) {
		case face_dir::minus_x:
			hitbox[0] = {1.75f * one16th, 0.5f, 0.5f};
			hitbox[1] = {1.75f * one16th, 0.5f, 0.25f};
			break ;
		case face_dir::plus_x:
			hitbox[0] = {14.25f * one16th, 0.5f, 0.5f};
			hitbox[1] = {1.75f * one16th, 0.5f, 0.25f};
			break ;
		case face_dir::minus_y:
			hitbox[0] = {0.5f, 1.75f * one16th, 0.5f};
			hitbox[1] = {0.5f, 1.75f * one16th, 0.25f};
			break ;
		case face_dir::plus_y:
			hitbox[0] = {0.5f, 14.25f * one16th, 0.5f};
			hitbox[1] = {0.5f, 1.75f * one16th, 0.25f};
			break ;
	}
}

void GlassPane::getHitbox( glm::vec3* hitbox, int value ) const
{
	int bitfield = (value >> offset::blocks::bitfield);
	hitbox[0] = {0.5f, 0.5f, 0.5f};
	hitbox[1] = {0.0625, 0.0625, 0.5f};
	switch (bitfield & (fence::mx | fence::px)) {
		case fence::mx:
			hitbox[0].x = 0.28125f;
			hitbox[1].x = 0.28125f;
			break ;
		case fence::px:
			hitbox[0].x = 1 - 0.28125f;
			hitbox[1].x = 0.28125f;
			break ;
		case fence::mx | fence::px:
			hitbox[1].x = 0.5f;
			break ;
	}
	switch (bitfield & (fence::my | fence::py)) {
		case fence::my:
			hitbox[0].y = 0.28125f;
			hitbox[1].y = 0.28125f;
			break ;
		case fence::py:
			hitbox[0].y = 1 - 0.28125f;
			hitbox[1].y = 0.28125f;
			break ;
		case fence::my | fence::py:
			hitbox[1].y = 0.5f;
			break ;
	}
}

void Torch::getHitbox( glm::vec3* hitbox, int value ) const
{
	int orientation = ((value >> offset::blocks::orientation) & 0x7);
	switch (orientation) {
		case face_dir::minus_x:
			hitbox[0] = {2 * one16th, 0.5f, 8 * one16th};
			hitbox[1] = {2 * one16th, one16th, 5 * one16th};
			break ;
		case face_dir::plus_x:
			hitbox[0] = {14 * one16th, 0.5f, 8 * one16th};
			hitbox[1] = {2 * one16th, one16th, 5 * one16th};
			break ;
		case face_dir::minus_y:
			hitbox[0] = {0.5f, 2 * one16th, 8 * one16th};
			hitbox[1] = {one16th, 2 * one16th, 5 * one16th};
			break ;
		case face_dir::plus_y:
			hitbox[0] = {0.5f, 14 * one16th, 8 * one16th};
			hitbox[1] = {one16th, 2 * one16th, 5 * one16th};
			break ;
		default:
			hitbox[0] = {0.5f, 0.5f, 5 * one16th};
			hitbox[1] = {one16th, one16th, 5 * one16th};
			break ;
	}
}

void WheatCrop::getHitbox( glm::vec3* hitbox, int value ) const
{
	int bitfield = (value >> offset::blocks::bitfield);
	hitbox[0] = {0.5f, 0.5f, (.5f + bitfield) * one16th};
	hitbox[1] = {0.4f, 0.4f, (.5f + bitfield) * one16th};
}

void ItemFrame::getHitbox( glm::vec3* hitbox, int value ) const
{
	int orientation = ((value >> offset::blocks::orientation) & 0x7);
	int bitfield = (value >> offset::blocks::bitfield);
	switch (bitfield & 0x3) {
		case placement::wall:
			switch (orientation) {
				case face_dir::minus_x:
					hitbox[0] = {15.5f * one16th, .4f,           .4f};
					hitbox[1] = {  .5f * one16th, .4f, 4.0f * one16th};
					break ;
				case face_dir::plus_x:
					hitbox[0] = { .5f * one16th, .4f,           .4f};
					hitbox[1] = { .5f * one16th, .4f, 4.0f * one16th};
					break ;
				case face_dir::minus_y:
					hitbox[0] = {.4f, 15.5f * one16th, .4f};
					hitbox[1] = {.4f,   .5f * one16th, 4.0f * one16th};
					break ;
				case face_dir::plus_y:
					hitbox[0] = {.4f, .5f * one16th, .4f};
					hitbox[1] = {.4f, .5f * one16th, 4.0f * one16th};
					break ;
			}
			break ;
		case placement::floor:
		case placement::ceiling:
			hitbox[0] = {.4f, .4f, ((bitfield & 0x3) == placement::floor) ? .5f * one16th : 15.5f * one16th};
			hitbox[1] = {.4f, .4f, one16th};
			break ;
	}
}
