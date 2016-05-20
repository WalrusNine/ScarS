#include <GL/glew.h>

#include "FrameBufferTexture.h"

void FrameBufferTexture::Bind(int textureUnit) {
	Texture::Bind(textureUnit);
	glGenerateMipmap(GL_TEXTURE_2D);
}
