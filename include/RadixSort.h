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

	ComputeShader m_prefixSumBlock2d	= ComputeShader("RadixSort/PrefixSum2D/prefixSumBlock");
	ComputeShader m_prefixSum2d			= ComputeShader("RadixSort/PrefixSum2D/prefixSum");
	ComputeShader m_uniformIncrement2d	= ComputeShader("RadixSort/PrefixSum2D/uniformIncrement");
private:
	const unsigned int m_n;

	GLBuffer<unsigned int>& m_data;

	std::unique_ptr<GLBuffer<unsigned int>> m_temp;

	std::unique_ptr<GLBuffer<unsigned int>> m_is0;
	std::unique_ptr<GLBuffer<unsigned int>> m_is1;

	std::unique_ptr<GLBuffer<unsigned int>> m_relativeOffset_0;
	std::unique_ptr<GLBuffer<unsigned int>> m_relativeOffset_1;

	std::unique_ptr<GLBuffer<unsigned int>> m_relativeOffsets;
	std::unique_ptr<GLBuffer<unsigned int>> m_isBelonging;

	GLBuffer<unsigned int> m_histogram = GLBuffer<unsigned int>
		(GL_SHADER_STORAGE_BUFFER, nullptr, 2, GL_STATIC_DRAW);
	GLBuffer<unsigned int> m_offsets = GLBuffer<unsigned int>
		(GL_SHADER_STORAGE_BUFFER, nullptr, 2, GL_STATIC_DRAW);

private:
	void _buildBuffers(const unsigned int& bitStage);
	void _computeHistogram(const unsigned int& bitStage);
	void _applyOffsets(const unsigned int& bitStage);
	void _copyElements();

	void _prefixSumBlock(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out);
	void _prefixSum(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out);

	void _2dPrefixSumBlock(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out);
	void _2dPrefixSum(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out);
	
public:
	RadixSort(GLBuffer<unsigned int>& data);
	void sort();
};
