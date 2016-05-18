#pragma once

#include "Texture.h"

class FrameBufferTexture : public Texture {


public:
	void bind(GLenum = 0) override;


};