#include "FreeTypeFont.h"

#define NOMINMAX
#include <Windows.h>

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>

#include <algorithm>

#define MAXWIDTH 1024

FreeTypeFont::FreeTypeFont()
{
	isLoaded = false;

	// Create temp shader TODO: create and set outside
	setShaderProgram(Shader::shader);
}

/*-----------------------------------------------

Name:	createChar

Params:	iIndex - character index in Unicode.

Result:	Creates one single character (its
texture).

/*---------------------------------------------*/

inline int next_p2(int n) { int res = 1; while (res < n)res <<= 1; return res; }

void FreeTypeFont::createChar(int index)
{
	FT_Load_Glyph(face, FT_Get_Char_Index(face, index), FT_LOAD_DEFAULT);

	FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
	FT_Bitmap* bitmap = &face->glyph->bitmap;

	int width = bitmap->width, height = bitmap->rows;
	int p2width = next_p2(width), p2height = next_p2(height);

	GLubyte* data = new GLubyte[p2width*p2height];
	// Copy glyph data and add dark pixels elsewhere
	for (int ch = 0; ch < p2height; ++ch) {
		for (int cw = 0; cw < p2width; ++cw) {
			data[ch*p2width + cw] = (ch >= height || cw >= width) ? 0 : bitmap->buffer[(height - ch - 1)*width + cw];
		}
	}
		
	// And create a texture from it
	charTextures[index].createFromData(data, p2width, p2height, 8, GL_DEPTH_COMPONENT, false, false);
	charTextures[index].setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);

	charTextures[index].setSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	charTextures[index].setSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Calculate glyph data
	advX[index] = face->glyph->advance.x >> 6;
	bearingX[index] = face->glyph->metrics.horiBearingX >> 6;
	charWidth[index] = face->glyph->metrics.width >> 6;

	advY[index] = (face->glyph->metrics.height - face->glyph->metrics.horiBearingY) >> 6;
	bearingY[index] = face->glyph->metrics.horiBearingY >> 6;
	charHeight[index] = face->glyph->metrics.height >> 6;

	newLine = std::max(newLine, int(face->glyph->metrics.height >> 6));

	// Rendering data, texture coordinates are always the same, so now we waste a little memory
	glm::vec3 quad[] =
	{
		glm::vec3(0.0f, float(-advY[index] + p2height), 0.0f),
		glm::vec3(0.0f, float(-advY[index]), 0.0f),
		glm::vec3(float(p2width), float(-advY[index] + p2height), 0.0f),
		glm::vec3(float(p2width), float(-advY[index]), 0.0f)
	};
	glm::vec2 texQuad[] = { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f) };

	// Add this char to VBO
	for (int i = 0; i < 4; ++i) {
		vbo.addData(&quad[i], sizeof(glm::vec3));
		vbo.addData(&texQuad[i], sizeof(glm::vec2));
	}

	delete[] data;
}

/*-----------------------------------------------

Name:	loadFont

Params:	sFile - path to font file
iPXSize - desired font pixel size

Result:	Loads whole font.

/*---------------------------------------------*/

bool FreeTypeFont::loadFont2(std::string file, int pxSize) {
	bool error = FT_Init_FreeType(&lib);

	error = FT_New_Face(lib, file.c_str(), 0, &face);
	if (error) return false;

	// Set font size, like 32, 48
	FT_Set_Pixel_Sizes(face, pxSize, pxSize);

	// Go through chars to get whole size
	// Get face's glyph
	FT_GlyphSlot g = face->glyph;

	// w = width of longest line
	// h = sum of the height of all lines
	// roww = current line's width
	// rowh = current line's height
	int w = 0, roww = 0;
	int h = 0, rowh = 0;

	for (int i = 32; i < 128; ++i) {
		FT_Load_Char(face, i, FT_LOAD_RENDER);

		// If reached line limit, create new line
		if (roww + g->bitmap.width + 1 >= MAXWIDTH) {
			w = std::max(w, roww);
			h += rowh;
			roww = 0;
			rowh = 0;
		}
		roww += g->bitmap.width + 1;
		rowh = std::max(rowh, (int) g->bitmap.rows);
	}
	w = std::max(w, roww);
	h += rowh;

	// Create texture with nothing
	Texture* t = new Texture();
	// Set its size to w x h = width of longest line x sum of the height of the lines
	t->createFromData(NULL, w, h, 8, GL_ALPHA, false, true);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	t->setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);

	// Create atlas
	atlas = new Atlas();

	// ox = total width (limited by MAXWIDTH)
	// oy = total height (all lines)
	int ox = 0;
	int oy = 0;
	// rowh = current line's height
	rowh = 0;

	glm::vec2 texQuad[] = { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f) };
	vbo.bindVBO();

	// Add subimages to the texture created
	int x = 0;
	for (int i = 0; i < 128; ++i) {
		CharacterInfo ci;
		if (i < 32) {
			ci.ax = 0;
			ci.ay =0;

			ci.bw = 0;
			ci.bh = 0;

			ci.bl = 0;
			ci.bt = 0;

			ci.tx = 0;
			ci.ty = 0;
		}
		else {
			// Load character
			FT_Load_Char(face, i, FT_LOAD_RENDER);

			// If reached line limit, add a line
			if (ox + g->bitmap.width + 1 >= MAXWIDTH) {
				// Increase total height
				oy += rowh;
				// Set current line's height to 0
				rowh = 0;
				// Set current line's width to 0
				ox = 0;
			}

			// Add character to texture
			glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
			
			// Divide by 64 to get in pixels
			ci.ax = g->advance.x >> 6;
			ci.ay = g->advance.y >> 6;

			// Get char size
			ci.bw = g->bitmap.width;
			ci.bh = g->bitmap.rows;

			ci.bl = g->bitmap_left;
			ci.bt = g->bitmap_top;

			// ox and oy = position in texture of letter.
			// texture's size = w x h
			// If it's 2 x 2, and 1 per letter,
			// first is		0  , 0		, getting 1/2 of first line		= 1
			// second is	1/2, 0		, getting 1/2 of first line		= 2
			// third is		0  , 1/2	, getting 1/2 of second line	= 3
			// fourth is	1/2, 1/2	, getting 1/2 of second line	= 4
			ci.tx = ox / (float)w;
			ci.ty = oy / (float)h;

			// rowh = highest line
			rowh = std::max(rowh, (int)g->bitmap.rows);

			glm::vec3 position[] = { glm::vec3(ox, oy, 0),
				glm::vec3(ox, oy + ci.ay, 0),
				glm::vec3(ox + ci.ax, oy, 0),
				glm::vec3(ox + ci.ax, oy + ci.ay, 0) };

			// Add character to vbo (keep vertex values from the char)
			for (int j = 0; j < 4; ++j) {
				// Add position
				vbo.addData(&position[i], sizeof(glm::vec3));
				// Add texture coord
				vbo.addData(&texQuad[i], sizeof(glm::vec2));
			}

			// ox = increase total width with the addition of new char
			ox += g->bitmap.width + 1;
		}
		// Add character to list
		atlas->charInfo.push_back(ci);
	}

	isLoaded = true;

	FT_Done_Face(face);
	FT_Done_FreeType(lib);

	vbo.uploadDataToGPU(GL_STATIC_DRAW);
	int loc = shader->getAttribLocation("a_position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	loc = shader->getAttribLocation("a_coord");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), (void*)(sizeof(glm::vec3)));
	return true;
}

void FreeTypeFont::print(std::string text, float x, float y, float sx, float sy) {
	int loc = 0;

	if (!isLoaded) return;

	/*
	int loc = 0;

	// Use the texture containing the atlas
	glBindTexture(GL_TEXTURE_2D, atlas->tex);
	// set sampler
	// glUniform1i(uniform_tex, 0);

	// Set up the VBO for our vertex data
	loc = Shader::shader->getAttribLocation("a_coord");
	glEnableVertexAttribArray(loc);
	vbo.bindVBO();
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, 0);

	int size = 6 * text.size();

	std::vector<glm::vec4> coords;
	coords.reserve(size);

	int c = 0;

	// Loop through all characters
	for (int p = 0; p < text.size(); ++p) {
	char cur_c = text[p];
	// Calculate the vertex and texture coordinates
	float x2 = x + atlas->charInfo[p].bl * sx;
	float y2 = -y - atlas->charInfo[p].bt * sy;
	float w = atlas->charInfo[p].bw * sx;
	float h = atlas->charInfo[p].bh * sy;

	// Advance the cursor to the start of the next character
	x += atlas->charInfo[p].ax * sx;
	y += atlas->charInfo[p].ay * sy;

	// Skip glyphs that have no pixels
	if (!w || !h)
	continue;

	coords[c++] = {
	x2, -y2, atlas->charInfo[p].tx, atlas->charInfo[p].ty
	};
	coords[c++] = {
	x2 + w, -y2, atlas->charInfo[p].tx + atlas->charInfo[p].bw / atlas->w, atlas->charInfo[p].ty
	};
	coords[c++] = {
	x2, -y2 - h, atlas->charInfo[p].tx, atlas->charInfo[p].ty + atlas->charInfo[p].bh / atlas->h
	};
	coords[c++] = {
	x2 + w, -y2, atlas->charInfo[p].tx + atlas->charInfo[p].bw / atlas->w, atlas->charInfo[p].ty
	};
	coords[c++] = {
	x2, -y2 - h, atlas->charInfo[p].tx, atlas->charInfo[p].ty + atlas->charInfo[p].bh / atlas->h
	};
	coords[c++] = {
	x2 + w, -y2 - h, atlas->charInfo[p].tx + atlas->charInfo[p].bw / atlas->w, atlas->charInfo[p].ty + atlas->charInfo[p].bh / atlas->h
	};
	}

	// Draw all the character on the screen in one go
	vbo.addData(&coords, sizeof(coords));
	//glBufferData(GL_ARRAY_BUFFER, sizeof (coords), &coords, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, c);

	glDisableVertexAttribArray(loc);
	*/
}

bool FreeTypeFont::loadFont(std::string file, int pxSize)
{
	bool error = FT_Init_FreeType(&lib);

	error = FT_New_Face(lib, file.c_str(), 0, &face);
	if (error) return false;

	FT_Set_Pixel_Sizes(face, pxSize, pxSize);
	loadedPixelSize = pxSize;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	vbo.createVBO();
	vbo.bindVBO();

	for (int i = 0; i < 128; ++i) {
		createChar(i);
	}
	isLoaded = true;

	FT_Done_Face(face);
	FT_Done_FreeType(lib);

	vbo.uploadDataToGPU(GL_STATIC_DRAW);
	int loc = shader->getAttribLocation("a_position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	loc = shader->getAttribLocation("a_coord");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), (void*)(sizeof(glm::vec3)));
	return true;
}

/*-----------------------------------------------

Name:	loadSystemFont

Params:	sName - system font name
iPXSize - desired font pixel size

Result:	Loads system font (from system Fonts
directory).

/*---------------------------------------------*/

bool FreeTypeFont::loadSystemFont(std::string name, int pxSize)
{
	char buf[512]; GetWindowsDirectory(buf, 512);
	std::string path = buf;
	path += "\\Fonts\\";
	path += name;

	return loadFont2(path, pxSize);
}

/*-----------------------------------------------

Name:	print

Params:	sText - text to print
x, y - 2D position
iPXSize - printed text size

Result:	Prints text at specified position
with specified pixel size.

/*---------------------------------------------*/
/*
void FreeTypeFont::print(std::string text, int x, int y, int pxSize)
{
	if (!isLoaded) return;

	glBindVertexArray(vao);
	shader->setUniform("gSampler", 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int curX = x, curY = y;
	if (pxSize == -1) pxSize = loadedPixelSize;
	float scale = float(pxSize) / float(loadedPixelSize);
	for (int i = 0; i < (int)text.size(); ++i) {
		if (text[i] == '\n') {
			curX = x;
			curY -= newLine * pxSize / loadedPixelSize;
			continue;
		}
		int index = int(text[i]);
		curX += bearingX[index] * pxSize / loadedPixelSize;
		if (text[i] != ' ') {
			charTextures[index].bind();
			glm::mat4 mModelView = glm::translate(glm::mat4(1.0f), glm::vec3(float(curX), float(curY), 0.0f));
			mModelView = glm::scale(mModelView, glm::vec3(scale));
			shader->setUniform("modelMatrix", mModelView);
			// Draw character
			glDrawArrays(GL_TRIANGLE_STRIP, index * 4, 4);
		}

		curX += (advX[index] - bearingX[index])*pxSize / loadedPixelSize;
	}
	glDisable(GL_BLEND);
}
*/
/*-----------------------------------------------

Name:	releaseFont

Params:	none

Result:	Deletes all font textures.

/*---------------------------------------------*/

void FreeTypeFont::releaseFont()
{
	for (int i = 0; i < 128; ++i) {
		charTextures[i].UnloadTexture();
	}
	vbo.deleteVBO();
	glDeleteVertexArrays(1, &vao);
}

/*-----------------------------------------------

Name:	setShaderProgram

Params:	a_shShaderProgram - shader program

Result:	Sets shader program that font uses.

/*---------------------------------------------*/

void FreeTypeFont::setShaderProgram(Shader* sh)
{
	shader = sh;
}

Atlas::Atlas() {
	tex = 0;
	w = 0;
	h = 0;
	
	charInfo.reserve(128);
}