#pragma once

/**
* @file OffscreenBuffer.h
*/
#include "Texture.h"
#include <GL/glew.h>
#include <memory>

class OffscreenBuffer;
typedef std::shared_ptr<OffscreenBuffer>OffscreenBufferPtr;

/**
* オフスクリーンブッファ
*/
class OffscreenBuffer
{
public:
	static OffscreenBufferPtr Create(int w, int h);
	GLint GetFramebuffer() const { return framebuffer; }
	GLint GetTexuter() const { return tex->Id(); }

private:
	OffscreenBuffer() = default;
	OffscreenBuffer(const OffscreenBuffer&) = delete;
	OffscreenBuffer& operator=(const OffscreenBuffer&) = delete;
	~OffscreenBuffer();
private:
	TexturePtr tex;
	GLuint depthbuffer = 0;
	GLuint framebuffer = 0;
};



































