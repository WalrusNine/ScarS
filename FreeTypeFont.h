#pragma once
#include <string>
//#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"
#include "Texture.h"
#include "VertexBufferObject.h"

class CharacterInfo
{

public:
	// Advance is the whole space occupied by the letter, so that
	// the next letter starts at the end of advance
	float ax; // advance.x
	float ay; // advance.y

	// Width and height are the actual size of the letter
	float bw; // bitmap.width;
	float bh; // bitmap.rows;

	// Distance from the glyph origin
	float bl; // bitmap_left;
	float bt; // bitmap_top;

	float tx; // x offset of glyph in texture coordinates
	float ty; // y offset of glyph in texture coordinates
};

class Atlas
{

public:
	GLuint tex;

	unsigned int w;
	unsigned int h;

	std::vector<CharacterInfo> charInfo;

	Atlas();
};

class FreeTypeFont
{

public:

	bool loadFont(std::string filepath, int pxSize);
	bool loadFont2(std::string filepath, int pxSize);
	bool loadSystemFont(std::string name, int pxSize);

	int getTextWidth(std::string text, int pxSize);

	void print(std::string text, int x, int y, int pxSize = 32);
	//void print(std::string text, float x, float y, float sx, float sy);

	void releaseFont();

	void setShaderProgram(Shader* shader);

	FreeTypeFont();

private:

	void createChar(int index);

	Texture charTextures[256];
	int advX[256], advY[256];
	int bearingX[256], bearingY[256];
	int charWidth[256], charHeight[256];
	int loadedPixelSize, newLine;

	bool isLoaded;

	unsigned int vao;
	VertexBufferObject vbo;

	FT_Library lib;
	FT_Face face;
	Shader* shader;

	Atlas* atlas;

	Texture* mainTexture;
};