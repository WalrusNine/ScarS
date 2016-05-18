#include <GL/glew.h>

#include "FrameBufferTexture.h"

void FrameBufferTexture::bind(GLenum) {
	Texture::bind();
	glGenerateMipmap(GL_TEXTURE_2D);
}
