#include "ComputeShader.h"
#include "GLBuffer.h"
#include <memory>
//#include "BVHNodes.h"

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

private:
	const unsigned int m_n;

	GLBuffer<unsigned int>& m_data;

	std::unique_ptr<GLBuffer<unsigned int>> m_temp;
	std::unique_ptr<GLBuffer<unsigned int>> m_setBits;
	std::unique_ptr<GLBuffer<unsigned int>> m_unsetBits;

	std::unique_ptr<GLBuffer<unsigned int>> m_setBitsPrefixSum;
	std::unique_ptr<GLBuffer<unsigned int>> m_unsetBitsPrefixSum;

	GLBuffer<unsigned int> m_histogram = GLBuffer<unsigned int>
		(GL_SHADER_STORAGE_BUFFER, nullptr, 2, GL_STATIC_DRAW);

private:
	void _buildBuffers(const unsigned int& bitStage);
	void _computeHistogram(const unsigned int& bitStage);
	void _applyOffsets(const unsigned int& bitStage);
	void _copyElements();

	void _prefixSumBlock(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out);
	void _prefixSum(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out);
public:
	RadixSort(GLBuffer<unsigned int>& data);
	void sort();
};
