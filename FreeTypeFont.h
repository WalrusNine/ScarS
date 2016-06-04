#pragma once
#include <string>
//#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"
#include "Texture.h"
#include "VertexBufferObject.h"

#define CHARS_PER_TEXTURE 1024
#define CHARS_PER_TEXTUREROOT 32

class FreeTypeFont
{
public:
	bool LoadFont(std::string file, int pxSize, int maxCharSupport = 128);

	int GetTextWidth(std::string text, int pxSize);

	void Print(std::string text, int x, int y, int pxSize = -1);
	void Print(std::wstring text, int x, int y, int pxSize = -1);
	void PrintFormatted(int x, int y, int pxSize, char* text, ...);
	void PrintFormatted(int x, int y, int pxSize, wchar_t* text, ...);

	void DeleteFont();

	void SetShaderProgram(Shader* shader);

	FreeTypeFont();
private:
	void CreateChar(int index, GLubyte* data);

	std::vector<Texture> charTextures;
	std::vector<int> advX, advY;
	std::vector<int> bearingX, bearingY;
	std::vector<int> charWidth, charHeight;
	int loadedPixelSize, newLine;
	int oneCharSquareSize;

	bool is_loaded;

	unsigned int vao;
	VertexBufferObject vbo;

	FT_Library lib;
	FT_Face face;
	Shader* shader;
};
