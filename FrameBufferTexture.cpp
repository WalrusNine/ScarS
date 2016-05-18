#include <GL/glew.h>

#include "FrameBufferTexture.h"

void FrameBufferTexture::bind(int textureUnit) {
	Texture::bind(textureUnit);
	glGenerateMipmap(GL_TEXTURE_2D);
}
