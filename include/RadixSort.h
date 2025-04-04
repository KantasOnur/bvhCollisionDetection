#include "ComputeShader.h"
#include "GLBuffer.h"
#include <memory>
#include "BVHNodes.h"

class RadixSort
{
private:
	ComputeShader m_prefixSum			= ComputeShader("RadixSort/PrefixSum/prefixSum");
	ComputeShader m_prefixSumBlock		= ComputeShader("RadixSort/PrefixSum/prefixSumBlock");
	ComputeShader m_uniformIncrement	= ComputeShader("RadixSort/PrefixSum/uniformIncrement");

	ComputeShader m_computeHistogram	= ComputeShader("RadixSort/computeHistogram");
	ComputeShader m_buildBuffers		= ComputeShader("RadixSort/buildBuffers");
	ComputeShader m_applyOffsets		= ComputeShader("RadixSort/applyOffsets");
	ComputeShader m_copyElements		= ComputeShader("RadixSort/copyElements");

	ComputeShader m_prefixSumBlock2d	= ComputeShader("RadixSort/PrefixSum2D/prefixSumBlock");
	ComputeShader m_prefixSum2d			= ComputeShader("RadixSort/PrefixSum2D/prefixSum");
	ComputeShader m_uniformIncrement2d	= ComputeShader("RadixSort/PrefixSum2D/uniformIncrement");

	ComputeShader m_prefixSumBlock4d = ComputeShader("RadixSort/PrefixSum4D/prefixSumBlockBCF");
	ComputeShader m_prefixSum4d = ComputeShader("RadixSort/PrefixSum4D/prefixSumBCF");
	ComputeShader m_uniformIncrement4d = ComputeShader("RadixSort/PrefixSum4D/uniformIncrement");

	
private:

	const unsigned int m_n;

	std::unique_ptr<GLBuffer<LeafNode>> m_temp;

	std::unique_ptr<GLBuffer<unsigned int>> m_relativeOffsets;
	std::unique_ptr<GLBuffer<unsigned int>> m_isBelonging;

	GLBuffer<unsigned int> m_histogram = GLBuffer<unsigned int>
		(GL_SHADER_STORAGE_BUFFER, nullptr, 4, GL_STATIC_DRAW);
	GLBuffer<unsigned int> m_offsets = GLBuffer<unsigned int>
		(GL_SHADER_STORAGE_BUFFER, nullptr, 4, GL_STATIC_DRAW);

private:
	void _buildBuffers(GLBuffer<LeafNode>& leafNodes,
		const unsigned int& bitStage);
	void _computeHistogram(GLBuffer<LeafNode>& leafNodes,
		const unsigned int& bitStage);
	void _applyOffsets(GLBuffer<LeafNode>& leafNodes,
		const unsigned int& bitStage);
	void _copyElements(GLBuffer<LeafNode>& leafNodes);

	void _prefixSumBlock(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out);
	void _prefixSum(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out);

	void _2dPrefixSumBlock(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out);
	void _2dPrefixSum(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out);
	
	void _4dPrefixSum(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out);
	void _4dPrefixSumBlock(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out);

public:
	RadixSort(const unsigned int& size);
	void sort(GLBuffer<LeafNode>& leafNodes);
};
