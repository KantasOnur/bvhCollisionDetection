#pragma once
#include "Mesh.h"
#include "GLBuffer.h"
#include "ComputeShader.h"
#include "BVHNodes.h"
#include "RadixSort.h"

class BVH : public Mesh
{
private:
	std::unique_ptr<GLBuffer<LeafNode>> m_leafNodes = nullptr;
	std::unique_ptr<GLBuffer<InternalNode>> m_internalNodes = nullptr;
	std::unique_ptr<RadixSort> m_sorter = nullptr;

	unsigned int m_entityID;
	unsigned int m_numLeaves;
	unsigned int m_numInternals;
	int m_depth = 0;
private:
	ComputeShader _computeMortonCodes = ComputeShader("computeMortonCodes");
	ComputeShader _computeBounds = ComputeShader("computeBounds");
	ComputeShader _computeInternalsNodes = ComputeShader("computeInternalNodes");



	Shader shader = Shader("AABB");

	void _constructLeafNodes();
	void _constructInternalNodes();
	void _constructBounds();

	void _getMortonCodes();

	void drawRecursive(const Camera& camera, const unsigned int depth, 
	const std::vector<InternalNode>& nodes, const unsigned int currentNode);
public:
	BVH(const unsigned int& id);
	~BVH();
	void draw(const Camera& camera) override;
	void constructBVH();
	void bindToLocation(const unsigned int& i0, const unsigned& i1);
	unsigned int getNumLeaves() { return m_numLeaves; };
	unsigned int getNumInternals() { return m_numInternals; };
};