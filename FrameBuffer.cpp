#include "FrameBuffer.h"

#include <glm/gtc/matrix_transform.hpp>
#include "FrameBufferTexture.h"

FrameBuffer::FrameBuffer() {
	framebuffer = 0;
	depthRenderbuffer = 0;
}

Texture* FrameBuffer::GetFrameBufferTexture() {
	return framebufferTex;
}

/*-----------------------------------------------

Name:	CreateFramebufferWithTexture

Params:	a_iWidth - framebuffer width
a_iHeight - framebuffer height

Result:	Creates a framebuffer and a texture to
render to.

/*---------------------------------------------*/

bool FrameBuffer::CreateFramebufferWithTexture(int w, int h) {
	if (framebuffer != 0) return false;

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	framebufferTex = new FrameBufferTexture();
	framebufferTex->CreateEmptyTexture(w, h, 24, GL_RGB);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTex->GetTextureHandle(), 0);

	width = w;
	height = h;

	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

/*-----------------------------------------------

Name:	AddDepthBuffer

Params:	none

Result:	Adds depth renderbuffer to framebuffer,
so rendering can perform depth testing.

/*---------------------------------------------*/

bool FrameBuffer::AddDepthBuffer() {
	if (framebuffer == 0) return false;

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenRenderbuffers(1, &depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

/*-----------------------------------------------

Name:	BindFramebuffer

Params:	bSetFullViewport - set full framebuffer
viewport, default is true

Result:	Binds this framebuffer.

/*---------------------------------------------*/

void FrameBuffer::BindFramebuffer(bool bSetFullViewport) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	if (bSetFullViewport)glViewport(0, 0, width, height);
}

/*-----------------------------------------------

Name:	BindFramebufferTexture

Params:	iTextureUnit - texture unit to bind to (defaul 0)
bRegenMipMaps - regenerate mipmaps? (default false)

Result:	Binds framebuffer texture, where renderings
are stored.

/*---------------------------------------------*/

void FrameBuffer::BindFramebufferTexture(int iTextureUnit, bool bRegenMipMaps) {
	framebufferTex->Bind(iTextureUnit);
	if (bRegenMipMaps) glGenerateMipmap(GL_TEXTURE_2D);
}

/*-----------------------------------------------

Name:	SetFramebufferTextureFiltering

Params:	a_tfMagnification - magnification filter
a_tfMinification - miniification filter

Result:	Sets filtering of framebuffer texture,
poor texturing here results in greater
performance.

/*---------------------------------------------*/

void FrameBuffer::SetFramebufferTextureFiltering(int a_tfMagnification, int a_tfMinification) {
	framebufferTex->SetFiltering(a_tfMagnification, a_tfMinification);
}

/*-----------------------------------------------

Name:	DeleteFramebuffer

Params:	none

Result:	Deletes framebuffer and frees memory.

/*---------------------------------------------*/

void FrameBuffer::DeleteFramebuffer() {
	if (framebuffer) {
		glDeleteFramebuffers(1, &framebuffer);
		framebuffer = 0;
	}
	if (depthRenderbuffer) {
		glDeleteRenderbuffers(1, &depthRenderbuffer);
		depthRenderbuffer = 0;
	}
	framebufferTex->UnloadTexture();
}

/*-----------------------------------------------

Name:	CalculateProjectionMatrix

Params:	fFOV - field of view angle
fNear, fFar - distance of near and far clipping plane

Result:	Calculates projection matrix.

/*---------------------------------------------*/

glm::mat4 FrameBuffer::CalculateProjectionMatrix(float fov, float near, float far) {
	return glm::perspective(fov, float(width) / float(height), near, far);
}

/*-----------------------------------------------

Name:	CalculateOrthoMatrix

Params:	none

Result:	Calculates ortho 2D projection matrix.

/*---------------------------------------------*/

glm::mat4 FrameBuffer::CalculateOrthoMatrix()
{
	return glm::ortho(0.0f, float(width), 0.0f, float(height));
}

/*-----------------------------------------------

Name:	Getters

Params:	none

Result:	... They get something :D

/*---------------------------------------------*/

int FrameBuffer::GetWidth() {
	return width;
}

int FrameBuffer::GetHeight() {
	return height;
}