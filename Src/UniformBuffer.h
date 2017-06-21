#pragma once

#include <GL/glew.h>
#include<string>
#include <memory>

class UniformBuffer;
typedef std::shared_ptr<UniformBuffer>UniformBufferPtr;

/**
* UBOƒNƒ‰ƒX
*/
class UniformBuffer {
public:
	static UniformBufferPtr Create(
		GLsizeiptr size, GLuint bindingPoint, const char*name);
	bool BufferSubData(const GLvoid*data, GLintptr offset = 0, GLsizeiptr size = 0);
	GLsizeiptr Size() const { return size; }

private:
	UniformBuffer() = default;
	~UniformBuffer();
	UniformBuffer(const UniformBuffer&) = delete;
	UniformBuffer& operator=(const UniformBuffer&) = delete;

private:
	GLuint ubo = 0;
	GLsizeiptr size = 0;
	GLuint bindingPoint;
	std::string name;
};





























