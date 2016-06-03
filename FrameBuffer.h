#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.h"


class FrameBuffer {
public:
	bool CreateFramebufferWithTexture(int width, int height);
	bool CreateFrameBufferForDepthShadow(int width, int height);

	bool AddDepthBuffer();
	void BindFramebuffer(bool setFullViewport = true);

	void SetFramebufferTextureFiltering(int magnification, int minification);
	void BindFramebufferTexture(int textureUnit = 0, bool regenMipMaps = false);

	glm::mat4 CalculateProjectionMatrix(float fFOV, float fNear, float fFar);
	glm::mat4 CalculateOrthoMatrix();

	void DeleteFramebuffer();

	int GetWidth();
	int GetHeight();

	FrameBuffer();

	Texture* GetFrameBufferTexture();
private:
	int width, height;
	unsigned int framebuffer;
	unsigned int depthRenderbuffer;
	Texture* framebufferTex;
};