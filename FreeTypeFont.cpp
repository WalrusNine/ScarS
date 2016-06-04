#include "FreeTypeFont.h"

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <algorithm>

using namespace std;

FreeTypeFont::FreeTypeFont() {
	is_loaded = false;
}

inline int next_p2(int n) { int res = 1; while (res < n)res <<= 1; return res; }

void FreeTypeFont::CreateChar(int index, GLubyte* data) {
	FT_Load_Glyph(face, FT_Get_Char_Index(face, index), FT_LOAD_DEFAULT);

	FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
	FT_Bitmap* bitmap = &face->glyph->bitmap;

	int w = bitmap->width, h = bitmap->rows;

	if (w > oneCharSquareSize)
		return;
	if (h > oneCharSquareSize)
		return;

	int row = (index % CHARS_PER_TEXTURE) / CHARS_PER_TEXTUREROOT;
	int col = (index % CHARS_PER_TEXTURE) % CHARS_PER_TEXTUREROOT;
	int oneTextureByteRowSize = CHARS_PER_TEXTUREROOT * oneCharSquareSize;

	// Copy glyph data
	for (int ch = 0; ch < h; ++ch) {
		memcpy(data + row * oneTextureByteRowSize * oneCharSquareSize + col * oneCharSquareSize + ch * oneTextureByteRowSize, bitmap->buffer + (h - ch - 1)*w, w);
	}
	

	// Calculate glyph data
	advX[index] = face->glyph->advance.x >> 6;
	bearingX[index] = face->glyph->metrics.horiBearingX >> 6;
	charWidth[index] = face->glyph->metrics.width >> 6;

	advY[index] = (face->glyph->metrics.height - face->glyph->metrics.horiBearingY) >> 6;
	bearingY[index] = face->glyph->metrics.horiBearingY >> 6;
	charHeight[index] = face->glyph->metrics.height >> 6;

	newLine = max(newLine, int(face->glyph->metrics.height >> 6));

	glm::vec2 quad[] =
	{
		glm::vec2(0.0f, float(-advY[index] + oneCharSquareSize)),
		glm::vec2(0.0f, float(-advY[index])),
		glm::vec2(float(oneCharSquareSize), float(-advY[index] + oneCharSquareSize)),
		glm::vec2(float(oneCharSquareSize), float(-advY[index]))
	};
	float oneStep = 1.0f / (float(CHARS_PER_TEXTUREROOT));

	// Texture coordinates change depending on character index, which determines its position in the texture
	glm::vec2 texQuad[] =
	{
		glm::vec2(float(col) * oneStep, float(row + 1) * oneStep),
		glm::vec2(float(col) * oneStep, float(row) * oneStep),
		glm::vec2(float(col + 1) * oneStep, float(row + 1) * oneStep),
		glm::vec2(float(col + 1) * oneStep, float(row) * oneStep)
	};

	// Add this char to VBO
	for (int i = 0; i < 4; ++i) {
		vbo.AddData(&quad[i], sizeof(glm::vec2));
		vbo.AddData(&texQuad[i], sizeof(glm::vec2));
	}
}

bool FreeTypeFont::LoadFont(string file, int pxSize, int maxCharSupport) {
	int error = FT_Init_FreeType(&lib);

	error = FT_New_Face(lib, file.c_str(), 0, &face);
	if (error) return false;

	FT_Set_Pixel_Sizes(face, pxSize, pxSize);
	loadedPixelSize = pxSize;
	oneCharSquareSize = next_p2(loadedPixelSize);

	int numTextures = (maxCharSupport + CHARS_PER_TEXTURE - 1) / CHARS_PER_TEXTURE;

	// One texture will store up to CHARS_PER_TEXTURE characters
	GLubyte** textureData = new GLubyte*[numTextures];

	charTextures.resize(numTextures);

	for (int i = 0; i < numTextures; ++i) {
		int textureDataSize = oneCharSquareSize * oneCharSquareSize * CHARS_PER_TEXTURE;
		textureData[i] = new GLubyte[textureDataSize];
		memset(textureData[i], 0, textureDataSize);
	}

	advX.resize(maxCharSupport); advY.resize(maxCharSupport);
	bearingX.resize(maxCharSupport); bearingY.resize(maxCharSupport);
	charWidth.resize(maxCharSupport); charHeight.resize(maxCharSupport);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	vbo.CreateVBO({});
	vbo.BindVBO();

	for (int i = 0; i < maxCharSupport; ++i) {
		CreateChar(i, textureData[i / CHARS_PER_TEXTURE]);
	}
	is_loaded = true;

	FT_Done_Face(face);
	FT_Done_FreeType(lib);

	for (int i = 0; i < numTextures; ++i) {
		charTextures[i].CreateFromData(textureData[i], oneCharSquareSize * CHARS_PER_TEXTUREROOT, oneCharSquareSize * CHARS_PER_TEXTUREROOT, 8, GL_DEPTH_COMPONENT, false);
		charTextures[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);

		charTextures[i].SetSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		charTextures[i].SetSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) * 2, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) * 2, (void*)(sizeof(glm::vec2)));

	for (int i = 0; i < numTextures; ++i) {
		delete[] textureData[i];
	}

	delete[] textureData;

	return true;
}

int FreeTypeFont::GetTextWidth(string text, int pxSize) {
	int result = 0;
	for (int i = 0; i < (int)text.size(); ++i)
		result += advX[text[i]];

	return result*pxSize / loadedPixelSize;
}


void FreeTypeFont::Print(string text, int x, int y, int pxSize) {
	if (!is_loaded) return;
	int lastBoundTexture = -1;

	glBindVertexArray(vao);
	shader->SetUniform("gSampler", 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int curX = x, curY = y;
	if (pxSize == -1) pxSize = loadedPixelSize;
	float scale = float(pxSize) / float(loadedPixelSize);
	for (int i = 0; i < (int)text.size(); ++i) {
		if (text[i] == '\n') {
			curX = x;
			curY -= newLine*pxSize / loadedPixelSize;
			continue;
		}
		int index = int(text[i]);
		int textureNeeded = index / CHARS_PER_TEXTURE;
		if (textureNeeded != lastBoundTexture) {
			lastBoundTexture = textureNeeded;
			charTextures[textureNeeded].Bind();
		}
		curX += bearingX[index] * pxSize / loadedPixelSize;
		if (text[i] != ' ') {
			glm::mat4 modelView = glm::translate(glm::mat4(1.0f), glm::vec3(float(curX), float(curY), 0.0f));
			modelView = glm::scale(modelView, glm::vec3(scale));
			shader->SetUniform("modelMatrix", modelView);
			// Draw character
			glDrawArrays(GL_TRIANGLE_STRIP, index * 4, 4);
		}

		curX += (advX[index] - bearingX[index])*pxSize / loadedPixelSize;
	}
	glDisable(GL_BLEND);
}

void FreeTypeFont::Print(wstring text, int x, int y, int pxSize) {
	if (!is_loaded) return;
	int lastBoundTexture = -1;

	glBindVertexArray(vao);
	shader->SetUniform("gSampler", 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int curX = x, curY = y;
	if (pxSize == -1) pxSize = lastBoundTexture;
	float scale = float(pxSize) / float(lastBoundTexture);
	for (int i = 0; i < (int)text.size(); ++i) {
		if (text[i] == '\n') {
			curX = x;
			curY -= newLine*pxSize / lastBoundTexture;
			continue;
		}
		int index = int(text[i]);
		int textureNeeded = index / CHARS_PER_TEXTURE;
		if (textureNeeded < 0 || textureNeeded >= (int)charTextures.size())
			continue;
		if (textureNeeded != lastBoundTexture) {
			lastBoundTexture = textureNeeded;
			charTextures[textureNeeded].Bind();
		}
		curX += bearingX[index] * pxSize / lastBoundTexture;
		if (text[i] != ' ') {
			glm::mat4 mModelView = glm::translate(glm::mat4(1.0f), glm::vec3(float(curX), float(curY), 0.0f));
			mModelView = glm::scale(mModelView, glm::vec3(scale));
			shader->SetUniform("modelMatrix", mModelView);
			// Draw character
			glDrawArrays(GL_TRIANGLE_STRIP, index * 4, 4);
		}

		curX += (advX[index] - bearingX[index]) * pxSize / loadedPixelSize;
	}
	glDisable(GL_BLEND);
}


void FreeTypeFont::PrintFormatted(int x, int y, int pxSize, char* text, ...) {
	char buf[512];
	va_list ap;
	va_start(ap, text);
	vsprintf_s(buf, text, ap);
	va_end(ap);
	Print(buf, x, y, pxSize);
}

void FreeTypeFont::PrintFormatted(int x, int y, int pxSize, wchar_t* text, ...) {
	wchar_t buf[512];
	va_list ap;
	va_start(ap, text);
	vswprintf(buf, (size_t)std::char_traits<wchar_t>::length(text), text, ap);
	va_end(ap);
	Print(buf, x, y, pxSize);
}


void FreeTypeFont::DeleteFont() {
	for (int i = 0; i < (int)charTextures.size(); ++i)
		charTextures[i].UnloadTexture();
	vbo.DeleteVBO();
	glDeleteVertexArrays(1, &vao);
}


void FreeTypeFont::SetShaderProgram(Shader* s) {
	shader = s;
}