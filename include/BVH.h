#pragma once
#include "Mesh.h"
#include "GLBuffer.h"
#include "ComputeShader.h"

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

namespace BVH
{

	constexpr int maxPrimitives = 10;

	struct Node
	{
		unsigned int startIndex;
		unsigned int endIndex;
		Node* left;
		Node* right;

	};



	class BVH : public Mesh
	{
	private:
		Node* m_hierarchy = nullptr;
		
		// Dont understand why I cant initialize GLBuffer later.
		// Hacky but it works.
		std::unique_ptr<GLBuffer<LeafNode>> m_leafNodes = nullptr;
		std::unique_ptr<GLBuffer<InternalNode>> m_internalNodes = nullptr;


		unsigned int m_entityID;
		unsigned int m_numLeaves;
		unsigned int m_numInternals;

	private:
		ComputeShader _computeMortonCodes = ComputeShader("computeMortonCodes");
		ComputeShader _computeBounds = ComputeShader("computeBounds");
		ComputeShader _computeInternalsNodes = ComputeShader("computeInternalNodes");

		Shader shader = Shader("AABB");

		void _constructLeafNodes();
		void _constructInternalNodes();
		void _constructBounds();


		void _destroyRecursive(Node* node);
		std::vector<LeafNode> _getMortonCodes();

		void drawRecursive(const Camera& camera, const unsigned int depth, 
			const std::vector<InternalNode>& nodes, const unsigned int currentNode);
	public:
		BVH(const unsigned int& id);
		~BVH();
		void draw(const Camera& camera) override;
	};
}


