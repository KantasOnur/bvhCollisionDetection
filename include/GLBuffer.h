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
	};
	GLBuffer(const GLenum& type, const std::vector<T>& data, const GLenum& usage)
		: m_type(type), m_n(data.size())
	{
		glGenBuffers(1, &m_id);
		glBindBuffer(type, m_id);
		glBufferData(type, sizeof(T) * m_n, data.data(), usage);
		glBindBuffer(type, 0);
	}
	GLBuffer() = default;
	~GLBuffer()
	{
		glDeleteBuffers(1, &m_id);
	};
	unsigned int getID() const { return m_id; };
	void sendToGPU(const size_t& index) const
	{
		glBindBuffer(m_type, m_id);
		glBindBufferBase(m_type, index, m_id);
		glBindBuffer(m_type, 0);

	}
	std::vector<T> retrieveBuffer() const
	{
		std::vector<T> data(m_n);
		glGetNamedBufferSubData(m_id, 0, m_n * sizeof(T), data.data());
		return data;
	}

	void clearBuffer(T val)
	{
		glClearNamedBufferData(m_id, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &val);
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

	unsigned int getID()
	{
		return m_id;
	}

	unsigned int& getIDByRef()
	{
		return m_id;
	}

	unsigned int getSize() const
	{
		return m_n;
	}
};