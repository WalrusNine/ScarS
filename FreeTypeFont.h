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
	bool LoadFont(std::string sFile, int iPXSize, int iMaxCharSupport = 128);
	bool LoadSystemFont(std::string sName, int iPXSize, int iMaxCharSupport = 128);

	int GetTextWidth(std::string sText, int iPXSize);

	void Print(std::string sText, int x, int y, int iPXSize = -1);
	//void Print(wstring sText, int x, int y, int iPXSize = -1);
	void PrintFormatted(int x, int y, int iPXSize, char* sText, ...);
	void PrintFormatted(int x, int y, int iPXSize, wchar_t* sText, ...);

	void DeleteFont();

	void SetShaderProgram(Shader* a_shShaderProgram);

	FreeTypeFont();
private:
	void CreateChar(int iIndex, GLubyte* bData);

	std::vector<Texture> tCharTextures;
	std::vector<int> iAdvX, iAdvY;
	std::vector<int> iBearingX, iBearingY;
	std::vector<int> iCharWidth, iCharHeight;
	int iLoadedPixelSize, iNewLine;
	int iOneCharSquareSize;

	bool bLoaded;

	unsigned int vao;
	VertexBufferObject vbo;

	FT_Library ftLib;
	FT_Face ftFace;
	Shader* shShaderProgram;
};
