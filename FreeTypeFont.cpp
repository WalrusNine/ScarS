#include "FreeTypeFont.h"

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>


using namespace std;

FreeTypeFont::FreeTypeFont() {
	bLoaded = false;
}

/*
inline int next_p2(int n) { int res = 1; while (res < n)res <<= 1; return res; }

void FreeTypeFont::CreateChar(int iIndex, GLubyte* bData)
{
	FT_Load_Glyph(ftFace, FT_Get_Char_Index(ftFace, iIndex), FT_LOAD_DEFAULT);

	FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL);
	FT_Bitmap* pBitmap = &ftFace->glyph->bitmap;

	int iW = pBitmap->width, iH = pBitmap->rows;

	// Some characters when rendered, are somehow just bigger than our desired pixel size
	// In this case, I just ignore them - another solution is to set iOneCharSquareSize in LoadFont function
	// to twice the size (just multiply by 2 and you're safe)
	if (iW > iOneCharSquareSize)
		return;
	if (iH > iOneCharSquareSize)
		return;

	int iRow = (iIndex%CHARS_PER_TEXTURE) / CHARS_PER_TEXTUREROOT;
	int iCol = (iIndex%CHARS_PER_TEXTURE) % CHARS_PER_TEXTUREROOT;
	int iOneTextureByteRowSize = CHARS_PER_TEXTUREROOT*iOneCharSquareSize;

	// Copy glyph data
	FOR(ch, iH)memcpy(bData + iRow*iOneTextureByteRowSize*iOneCharSquareSize + iCol*iOneCharSquareSize + ch*iOneTextureByteRowSize, pBitmap->buffer + (iH - ch - 1)*iW, iW);

	// Calculate glyph data
	iAdvX[iIndex] = ftFace->glyph->advance.x >> 6;
	iBearingX[iIndex] = ftFace->glyph->metrics.horiBearingX >> 6;
	iCharWidth[iIndex] = ftFace->glyph->metrics.width >> 6;

	iAdvY[iIndex] = (ftFace->glyph->metrics.height - ftFace->glyph->metrics.horiBearingY) >> 6;
	iBearingY[iIndex] = ftFace->glyph->metrics.horiBearingY >> 6;
	iCharHeight[iIndex] = ftFace->glyph->metrics.height >> 6;

	iNewLine = max(iNewLine, int(ftFace->glyph->metrics.height >> 6));

	glm::vec2 vQuad[] =
	{
		glm::vec2(0.0f, float(-iAdvY[iIndex] + iOneCharSquareSize)),
		glm::vec2(0.0f, float(-iAdvY[iIndex])),
		glm::vec2(float(iOneCharSquareSize), float(-iAdvY[iIndex] + iOneCharSquareSize)),
		glm::vec2(float(iOneCharSquareSize), float(-iAdvY[iIndex]))
	};
	float fOneStep = 1.0f / (float(CHARS_PER_TEXTUREROOT));
	// Texture coordinates change depending on character index, which determines its position in the texture
	glm::vec2 vTexQuad[] =
	{
		glm::vec2(float(iCol)*fOneStep, float(iRow + 1)*fOneStep),
		glm::vec2(float(iCol)*fOneStep, float(iRow)*fOneStep),
		glm::vec2(float(iCol + 1)*fOneStep, float(iRow + 1)*fOneStep),
		glm::vec2(float(iCol + 1)*fOneStep, float(iRow)*fOneStep)
	};

	// Add this char to VBO
	FOR(i, 4)
	{
		vboData.AddData(&vQuad[i], sizeof(glm::vec2));
		vboData.AddData(&vTexQuad[i], sizeof(glm::vec2));
	}
}

bool FreeTypeFont::LoadFont(string sFile, int iPXSize, int iMaxCharSupport)
{
	BOOL bError = FT_Init_FreeType(&ftLib);

	bError = FT_New_Face(ftLib, sFile.c_str(), 0, &ftFace);
	if (bError)return false;
	FT_Set_Pixel_Sizes(ftFace, iPXSize, iPXSize);
	iLoadedPixelSize = iPXSize;
	iOneCharSquareSize = next_p2(iLoadedPixelSize);

	// Neat trick - we need to calculate ceil(iMaxCharSupport/CHARS_PER_TEXTURE) and that calculation does it, more in article
	int iNumTextures = (iMaxCharSupport + CHARS_PER_TEXTURE - 1) / CHARS_PER_TEXTURE;

	// One texture will store up to CHARS_PER_TEXTURE characters
	GLubyte** bTextureData = new GLubyte*[iNumTextures];

	tCharTextures.resize(iNumTextures);

	FOR(i, iNumTextures)
	{
		int iTextureDataSize = iOneCharSquareSize*iOneCharSquareSize*CHARS_PER_TEXTURE;
		bTextureData[i] = new GLubyte[iTextureDataSize];
		memset(bTextureData[i], 0, iTextureDataSize);
	}

	iAdvX.resize(iMaxCharSupport); iAdvY.resize(iMaxCharSupport);
	iBearingX.resize(iMaxCharSupport); iBearingY.resize(iMaxCharSupport);
	iCharWidth.resize(iMaxCharSupport); iCharHeight.resize(iMaxCharSupport);

	glGenVertexArrays(1, &uiVAO);
	glBindVertexArray(uiVAO);
	vboData.CreateVBO();
	vboData.BindVBO();

	FOR(i, iMaxCharSupport)CreateChar(i, bTextureData[i / CHARS_PER_TEXTURE]);
	bLoaded = true;

	FT_Done_Face(ftFace);
	FT_Done_FreeType(ftLib);

	FOR(i, iNumTextures)
	{
		tCharTextures[i].CreateFromData(bTextureData[i], iOneCharSquareSize*CHARS_PER_TEXTUREROOT, iOneCharSquareSize*CHARS_PER_TEXTUREROOT, 8, GL_DEPTH_COMPONENT, false);
		tCharTextures[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);

		tCharTextures[i].SetSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		tCharTextures[i].SetSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	vboData.UploadDataToGPU(GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) * 2, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) * 2, (void*)(sizeof(glm::vec2)));

	FOR(i, iNumTextures)
		delete[] bTextureData[i];

	delete[] bTextureData;

	return true;
}


bool FreeTypeFont::LoadSystemFont(string sName, int iPXSize, int iMaxCharSupport)
{
	char buf[512]; GetWindowsDirectory(buf, 512);
	string sPath = buf;
	sPath += "\\Fonts\\";
	sPath += sName;

	return LoadFont(sPath, iPXSize, iMaxCharSupport);
}


int FreeTypeFont::GetTextWidth(string sText, int iPXSize)
{
	int iResult = 0;
	FOR(i, ESZ(sText))iResult += iAdvX[sText[i]];

	return iResult*iPXSize / iLoadedPixelSize;
}


void FreeTypeFont::Print(string sText, int x, int y, int iPXSize)
{
	if (!bLoaded)return;
	int iLastBoundTexture = -1;

	glBindVertexArray(vao);
	shShaderProgram->setUniform("gSampler", 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int iCurX = x, iCurY = y;
	if (iPXSize == -1)iPXSize = iLoadedPixelSize;
	float fScale = float(iPXSize) / float(iLoadedPixelSize);
	for (int i = 0; i < (int)sText.size(); ++i) {
		if (sText[i] == '\n')
		{
			iCurX = x;
			iCurY -= iNewLine*iPXSize / iLoadedPixelSize;
			continue;
		}
		int iIndex = int(sText[i]);
		int iTextureNeeded = iIndex / CHARS_PER_TEXTURE;
		if (iTextureNeeded != iLastBoundTexture)
		{
			iLastBoundTexture = iTextureNeeded;
			tCharTextures[iTextureNeeded].bind();
		}
		iCurX += iBearingX[iIndex] * iPXSize / iLoadedPixelSize;
		if (sText[i] != ' ')
		{
			glm::mat4 mModelView = glm::translate(glm::mat4(1.0f), glm::vec3(float(iCurX), float(iCurY), 0.0f));
			mModelView = glm::scale(mModelView, glm::vec3(fScale));
			shShaderProgram->setUniform("matrices.modelViewMatrix", mModelView);
			// Draw character
			glDrawArrays(GL_TRIANGLE_STRIP, iIndex * 4, 4);
		}

		iCurX += (iAdvX[iIndex] - iBearingX[iIndex])*iPXSize / iLoadedPixelSize;
	}
	glDisable(GL_BLEND);
}

void FreeTypeFont::Print(wstring sText, int x, int y, int iPXSize)
{
	if (!bLoaded)return;
	int iLastBoundTexture = -1;

	glBindVertexArray(vao);
	shShaderProgram->setUniform("gSampler", 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int iCurX = x, iCurY = y;
	if (iPXSize == -1)iPXSize = iLoadedPixelSize;
	float fScale = float(iPXSize) / float(iLoadedPixelSize);
	for (int i = 0; i < (int)sText.size(); ++i) {
		if (sText[i] == '\n') {
			iCurX = x;
			iCurY -= iNewLine*iPXSize / iLoadedPixelSize;
			continue;
		}
		int iIndex = int(sText[i]);
		int iTextureNeeded = iIndex / CHARS_PER_TEXTURE;
		if (iTextureNeeded < 0 || iTextureNeeded >= ESZ(tCharTextures))
			continue;
		if (iTextureNeeded != iLastBoundTexture) {
			iLastBoundTexture = iTextureNeeded;
			tCharTextures[iTextureNeeded].bind();
		}
		iCurX += iBearingX[iIndex] * iPXSize / iLoadedPixelSize;
		if (sText[i] != ' ') {
			glm::mat4 mModelView = glm::translate(glm::mat4(1.0f), glm::vec3(float(iCurX), float(iCurY), 0.0f));
			mModelView = glm::scale(mModelView, glm::vec3(fScale));
			shShaderProgram->setUniform("matrices.modelViewMatrix", mModelView);
			// Draw character
			glDrawArrays(GL_TRIANGLE_STRIP, iIndex * 4, 4);
		}

		iCurX += (iAdvX[iIndex] - iBearingX[iIndex])*iPXSize / iLoadedPixelSize;
	}
	glDisable(GL_BLEND);
}


void FreeTypeFont::PrintFormatted(int x, int y, int iPXSize, char* sText, ...)
{
	char buf[512];
	va_list ap;
	va_start(ap, sText);
	vsprintf(buf, sText, ap);
	va_end(ap);
	Print(buf, x, y, iPXSize);
}

void FreeTypeFont::PrintFormatted(int x, int y, int iPXSize, wchar_t* sText, ...)
{
	wchar_t buf[512];
	va_list ap;
	va_start(ap, sText);
	vswprintf(buf, sText, ap);
	va_end(ap);
	Print(buf, x, y, iPXSize);
}


void FreeTypeFont::DeleteFont()
{
	FOR(i, ESZ(tCharTextures))
		tCharTextures[i].DeleteTexture();
	vboData.DeleteVBO();
	glDeleteVertexArrays(1, &uiVAO);
}


void FreeTypeFont::SetShaderProgram(Shader* a_shShaderProgram) {
	shShaderProgram = a_shShaderProgram;
}*/