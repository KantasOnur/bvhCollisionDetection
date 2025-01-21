#pragma once
#include "OpenGL.h"
#include <cassert>
#include <stdexcept>

template <typename T>
class GLBuffer
{
private:
	unsigned int m_id;
	GLenum m_type;
	size_t m_n;

public:
	GLBuffer(GLenum type, T* data, const size_t& n, GLenum usage)
	: m_type(type), m_n(n)
	{
		glGenBuffers(1, &m_id);
		glBindBuffer(type, m_id);
		glBufferData(type, sizeof(T)* n, data, usage);
		glBindBuffer(type, 0);
		m_n = n;
	};
	~GLBuffer()
	{
		glDeleteBuffers(1, &m_id);
	};
	unsigned int getID() const { return m_id; };
	void sendToGPU(const size_t& index) const
	{
		glBindBufferBase(m_type, index, m_id);
	}
	void retrieveBuffer(const size_t& offset, T* data, const size_t& n = m_n) const
	{
		if (n > m_n) {
			throw std::out_of_range("Error: Requested size (n = " + std::to_string(n) +
				") exceeds buffer size (m_n = " + std::to_string(m_n) + ").");
		}
		glGetNamedBufferSubData(m_id, offset, n * sizeof(T), data);
	}

	void updateBuffer(T* data, const size_t& n = m_n)
	{
		if (n > m_n) {
			throw std::out_of_range("Error: Requested size (n = " + std::to_string(n) +
				") exceeds buffer size (m_n = " + std::to_string(m_n) + ").");
		}
		glBindBuffer(m_type, m_id);
		glBufferSubData(m_type, 0, sizeof(T) * n, data);
		glBindBuffer(0, m_id);
	}

	void bind() const
	{
		glBindBuffer(m_type, m_id);
	}
	void unbind()
	{
		glBindBuffer(m_type, 0);
	}
};