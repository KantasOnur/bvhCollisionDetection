#include "RadixSort.h"
#include <algorithm>
#include <iostream>
#include <chrono>


#define TIME_AND_ACCUMULATE(func, key, ...) \
    { \
        auto start = std::chrono::high_resolution_clock::now(); \
        func(__VA_ARGS__); \
        auto end = std::chrono::high_resolution_clock::now(); \
        accumulatedTimes[key] += std::chrono::duration<double, std::milli>(end - start).count(); \
    }

RadixSort::RadixSort(GLBuffer<unsigned int>& data)
	: m_data(data), m_n(data.getSize())
{
	m_temp = std::make_unique<GLBuffer<unsigned int>>
		(GL_SHADER_STORAGE_BUFFER, nullptr, m_n, GL_STATIC_DRAW);

	m_is0 = std::make_unique<GLBuffer<unsigned int>>
		(GL_SHADER_STORAGE_BUFFER, nullptr, m_n, GL_STATIC_DRAW);

	m_is1 = std::make_unique<GLBuffer<unsigned int>>
		(GL_SHADER_STORAGE_BUFFER, nullptr, m_n, GL_STATIC_DRAW);

	m_relativeOffset_0 = std::make_unique<GLBuffer<unsigned int>>
		(GL_SHADER_STORAGE_BUFFER, nullptr, m_n, GL_STATIC_DRAW);

	m_relativeOffset_1 = std::make_unique<GLBuffer<unsigned int>>
		(GL_SHADER_STORAGE_BUFFER, nullptr, m_n, GL_STATIC_DRAW);


	std::vector<unsigned int> values(pow(2, 20));
	for (int y = 0; y < 2; ++y)
	{
		for (int x = 0; x < values.size() / 2; ++x)
		{
			values[y * values.size()/2 + x] = y + 1;
		}
	}

	m_relativeOffsets = std::make_unique<GLBuffer<unsigned int>>
		(GL_SHADER_STORAGE_BUFFER, nullptr, values.size(), GL_STATIC_DRAW);

	m_isBelonging = std::make_unique<GLBuffer<unsigned int>>
		(GL_SHADER_STORAGE_BUFFER, values, GL_STATIC_DRAW);

	//_prefixSumBlock(*m_isBelonging, *m_relativeOffsets);
	_2dPrefixSum(*m_isBelonging, *m_relativeOffsets);

	auto a = m_relativeOffsets->retrieveBuffer();
	std::vector<unsigned int> expected(values.size());
	expected[0] = 0;
	expected[values.size() / 2] = 0;
	for (int i = 1; i < values.size() / 2; i++)
	{
		expected[i] = expected[i - 1] + values[i - 1];
	}

	for (int i = values.size() / 2 + 1; i < values.size(); ++i)
	{
		expected[i] = expected[i - 1] + values[i - 1];
	}
	for (int i = 0; i < values.size(); ++i)
	{
		if (a[i] != expected[i])
		{
			auto ai = a[i];
			auto ei = expected[i];
			std::cout << "not eq" << std::endl;
		}
	}
}

void RadixSort::sort()
{
	std::unordered_map<std::string, double> accumulatedTimes;
	
	for (unsigned int bitStage = 0; bitStage < 32; ++bitStage)
	{
		_buildBuffers(bitStage);
		_prefixSum(*m_is0, *m_relativeOffset_0);
		_prefixSum(*m_is1, *m_relativeOffset_1);
		_prefixSum(m_histogram, m_offsets);
		_applyOffsets(bitStage);

		std::swap(m_data.getIDByRef(), m_temp->getIDByRef());

		m_histogram.clearBuffer(0);
		m_relativeOffset_0->clearBuffer(0);
		m_relativeOffset_1->clearBuffer(0);
		m_offsets.clearBuffer(0);
	}

	auto a = m_data.retrieveBuffer();

}

void RadixSort::_buildBuffers(const unsigned int& bitStage)
{
	const unsigned int numThreads = 1024;
	const unsigned int numBlocks = (numThreads + m_n - 1) / numThreads;

	m_data.sendToGPU(0);
	m_is0->sendToGPU(1);
	m_is1->sendToGPU(2);
	m_histogram.sendToGPU(3);

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
	m_relativeOffset_0->sendToGPU(2);
	m_relativeOffset_1->sendToGPU(3);
	m_offsets.sendToGPU(4);

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

void RadixSort::_2dPrefixSumBlock(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out)
{
	unsigned int n = data_in.getSize() / 2;
	unsigned int numThreads = 256;
	unsigned int numBlocksX = 1;
	unsigned int numBlocksY = 2;

	data_in.sendToGPU(0);
	data_out.sendToGPU(1);
	m_prefixSumBlock2d.bind();
	m_prefixSumBlock2d.setInt("n", n);
	m_prefixSumBlock2d.unbind();
	m_prefixSumBlock2d.dispatch(1, 2, 1);
}

void RadixSort::_2dPrefixSum(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out)
{
	unsigned int n = data_in.getSize() / 2;
	unsigned int numThreads = 256;
	unsigned int numBlocksX = (numThreads + (n / 2) - 1) / numThreads;
	unsigned int numBlocksY = 2;

	if (numBlocksX == 1) return _2dPrefixSumBlock(data_in, data_out);

	GLBuffer<unsigned int> sums = GLBuffer<unsigned int>
		(GL_SHADER_STORAGE_BUFFER, nullptr, 2 * numBlocksX, GL_STATIC_DRAW);
	GLBuffer<unsigned int> incr = GLBuffer<unsigned int>
		(GL_SHADER_STORAGE_BUFFER, nullptr, 2 * numBlocksX, GL_STATIC_DRAW);

	data_in.sendToGPU(0);
	data_out.sendToGPU(1);
	sums.sendToGPU(2);

	m_prefixSum2d.bind();
	m_prefixSum2d.setInt("n", n);
	m_prefixSum2d.setInt("n_incr", numBlocksX);
	m_prefixSum2d.unbind();
	m_prefixSum2d.dispatch(numBlocksX, numBlocksY, 1);

	//if (numBlocksX > 512) _2dPrefixSum(sums, incr);
	//else _2dPrefixSumBlock(sums, incr);
	_2dPrefixSumBlock(sums, incr);

	numThreads = 512;
	numBlocksX = (numThreads + n - 1) / numThreads;
	incr.sendToGPU(0);
	data_out.sendToGPU(1);
	m_uniformIncrement2d.bind();
	m_uniformIncrement2d.setInt("n", n);
	m_uniformIncrement2d.setInt("n_incr", numBlocksX);
	m_uniformIncrement2d.unbind();
	m_uniformIncrement2d.dispatch(numBlocksX, 2, 1);
}
