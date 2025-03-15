#include "RadixSort.h"
#include <algorithm>
#include <iostream>


RadixSort::RadixSort(GLBuffer<unsigned int>& data)
	: m_data(data), m_n(data.getSize())
{
	m_temp = std::make_unique<GLBuffer<unsigned int>>
		(GL_SHADER_STORAGE_BUFFER, nullptr, m_n, GL_STATIC_DRAW);

	m_setBits = std::make_unique<GLBuffer<unsigned int>>
		(GL_SHADER_STORAGE_BUFFER, nullptr, m_n, GL_STATIC_DRAW);

	m_unsetBits = std::make_unique<GLBuffer<unsigned int>>
		(GL_SHADER_STORAGE_BUFFER, nullptr, m_n, GL_STATIC_DRAW);

	m_setBitsPrefixSum = std::make_unique<GLBuffer<unsigned int>>
		(GL_SHADER_STORAGE_BUFFER, nullptr, m_n, GL_STATIC_DRAW);

	m_unsetBitsPrefixSum = std::make_unique<GLBuffer<unsigned int>>
		(GL_SHADER_STORAGE_BUFFER, nullptr, m_n, GL_STATIC_DRAW);

}

void RadixSort::sort()
{
	for (unsigned int bitStage = 0; bitStage < 32; ++bitStage)
	{
		_buildBuffers(bitStage);
		_computeHistogram(bitStage);
		_prefixSum(*m_setBits, *m_setBitsPrefixSum);
		_prefixSum(*m_unsetBits, *m_unsetBitsPrefixSum);
		_applyOffsets(bitStage);
		_copyElements();
	}
}

void RadixSort::_buildBuffers(const unsigned int& bitStage)
{
	const unsigned int numThreads = 1024;
	const unsigned int numBlocks = (numThreads + m_n - 1) / numThreads;
	m_data.sendToGPU(0);
	m_setBits->sendToGPU(1);
	m_unsetBits->sendToGPU(2);
	m_setBitsPrefixSum->sendToGPU(3);
	m_unsetBitsPrefixSum->sendToGPU(4);
	m_histogram.sendToGPU(5);

	m_buildBuffers.bind();
	m_buildBuffers.setUInt("bitStage", bitStage);
	m_buildBuffers.setUInt("n", m_n);
	m_buildBuffers.unbind();

	m_buildBuffers.dispatch(numBlocks, 1, 1);
}

void RadixSort::_computeHistogram(const unsigned int& bitStage)
{
	m_histogram.sendToGPU(0);
	m_data.sendToGPU(1);

	const unsigned int numThreads = 1024;
	const unsigned int numBlocks = (numThreads + m_n - 1) / numThreads;

	m_computeHistogram.bind();
	m_computeHistogram.setUInt("n", m_n);
	m_computeHistogram.setUInt("bitStage", bitStage);
	m_computeHistogram.unbind();
	
	m_computeHistogram.dispatch(numBlocks, 1, 1);
}

void RadixSort::_applyOffsets(const unsigned int& bitStage)
{
	const unsigned int numThreads = 1024;
	const unsigned int numBlocks = (numThreads + m_n - 1) / 1024;

	m_data.sendToGPU(0);
	m_temp->sendToGPU(1);
	m_histogram.sendToGPU(2);
	m_setBitsPrefixSum->sendToGPU(3);
	m_unsetBitsPrefixSum->sendToGPU(4);

	m_applyOffsets.bind();
	m_applyOffsets.setUInt("n", m_n);
	m_applyOffsets.setUInt("bitStage", bitStage);
	m_applyOffsets.unbind();
	m_applyOffsets.dispatch(numBlocks, 1, 1);
}

void RadixSort::_copyElements()
{
	const unsigned int numThreads = 1024;
	const unsigned int numBlocks = (numThreads + m_n - 1) / numThreads;

	m_data.sendToGPU(0);
	m_temp->sendToGPU(1);

	m_copyElements.bind();
	m_copyElements.setUInt("n", m_n);
	m_copyElements.unbind();

	m_copyElements.dispatch(numBlocks, 1, 1);
}

void RadixSort::_prefixSumBlock(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out)
{
	data_in.sendToGPU(0);
	data_out.sendToGPU(1);
	m_prefixSumBlock.bind();
	m_prefixSumBlock.setInt("n", data_in.getSize());
	m_prefixSumBlock.unbind();
	m_prefixSumBlock.dispatch(1, 1, 1);
}

void RadixSort::_prefixSum(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out)
{
	unsigned int n = data_in.getSize();
	unsigned int numThreads = 512;
	unsigned int numBlocks = (numThreads + (n / 2) - 1) / numThreads;

	if (numBlocks == 1) return _prefixSumBlock(data_in, data_out);

	GLBuffer<unsigned int> sums = GLBuffer<unsigned int>
		(GL_SHADER_STORAGE_BUFFER, nullptr, numBlocks, GL_STATIC_DRAW);
	GLBuffer<unsigned int> incr = GLBuffer<unsigned int>
		(GL_SHADER_STORAGE_BUFFER, nullptr, numBlocks, GL_STATIC_DRAW);

	data_in.sendToGPU(0);
	data_out.sendToGPU(1);
	sums.sendToGPU(2);

	m_prefixSum.bind();
	m_prefixSum.setInt("n", n);
	m_prefixSum.unbind();
	m_prefixSum.dispatch(numBlocks, 1, 1);

	_prefixSumBlock(sums, incr);

	numThreads = 1024;
	numBlocks = (numThreads + n - 1) / numThreads;
	incr.sendToGPU(0);
	data_out.sendToGPU(1);
	m_uniformIncrement.bind();
	m_uniformIncrement.setInt("n", n);
	m_uniformIncrement.unbind();
	m_uniformIncrement.dispatch(numBlocks, 1, 1);
}