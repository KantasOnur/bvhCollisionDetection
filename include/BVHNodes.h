#include <glm/glm.hpp>

struct Box
{
	glm::vec4 min;
	glm::vec4 max;
};

struct LeafNode
{
	unsigned int code;

	unsigned int i0;
	unsigned int i1;
	unsigned int i2;

	Box box;

	int parent;

	unsigned int pad0;
	unsigned int pad1;
	unsigned int pad2;

};

struct InternalNode {
	unsigned int start;             // offset 0
	unsigned int end;               // offset 4
	unsigned int left;              // offset 8
	unsigned int right;             // offset 12
	int parent = -1;			// offset 16
	unsigned is_left_internal = 1;			// offset 20 (4 bytes)
	unsigned is_right_internal = 1;		// offset 24 (4 bytes)
	unsigned int pad0;              // padding: offset 28-32 (4 bytes)
	Box box;						// offset 32, size 32 bytes (vec4 min & vec4 max)
	unsigned int count_arrival = 0;     // offset 64, size 4 bytes
	unsigned int pad1;              // padding: offset 68-80 (12 bytes total, could be three uints)
	unsigned int pad2;
	unsigned int pad3;
};