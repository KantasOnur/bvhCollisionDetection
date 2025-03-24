struct Box
{
    vec4 min;
    vec4 max;
};

struct LeafNode
{

    uint code;		// 0 aligned

    uint i0;		// 4 aligned
    uint i1;		// 8 aligned
    uint i2;		// 12 aligned

    Box box;		// 16 aligned

    int parent;	// 48 aligned

    uint pad0;
    uint pad1;
    uint pad2;
};

struct InternalNode {
    uint start;               // offset 0
    uint end;                 // offset 4
    uint left;                // offset 8
    uint right;               // offset 12
    int parent;				  // offset 16
    uint is_left_internal;    // offset 20 (4 bytes)
    uint is_right_internal;   // offset 24 (4 bytes)
    uint pad0;                // padding: offset 28-32 (4 bytes)
    Box box;                  // offset 32, size 32 bytes (vec4 min & vec4 max)
    uint count_arrival;       // offset 64, size 4 bytes
    uint pad1;                // padding: offset 68-80 (12 bytes total, could be three uints)
    uint pad2;
    uint pad3;
};