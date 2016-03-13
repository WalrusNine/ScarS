#include <GL/glew.h>

#include "Texture.h"
#include <FreeImage.h>
#pragma comment (lib, "FreeImage.lib")


//Include GLFW
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <iostream>

std::vector<Texture*> Texture::textures;

Texture::Texture(std::string name) {
	textureHandle = 0;
	mipMapsGenerated = false;
	this->name = name;
}

void Texture::createFromData(GLubyte* data, int inWidth, int inHeight, int inBpp, GLenum format, bool generateMipMaps, bool pixelStore) {
	// Generate an OpenGL texture ID for this texture
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	if (pixelStore)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (format == GL_RGB || format == GL_BGR) // We must handle this because of internal format parameter
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, inWidth, inHeight, 0, format, GL_UNSIGNED_BYTE, data);
		if (generateMipMaps)glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, inWidth, inHeight, 0, format, GL_UNSIGNED_BYTE, data);
		if (generateMipMaps)glGenerateMipmap(GL_TEXTURE_2D);
	}

	glGenSamplers(1, &samplerHandle);

	filepath = "";
	mipMapsGenerated = generateMipMaps;
	width = inWidth;
	height = inHeight;
	bpp = inBpp;
}

bool Texture::LoadTexture2D(std::string in_filepath, bool generateMipMaps) {
	// Load image
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(in_filepath.c_str(), 0); // Check the file signature and deduce its format

	if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(in_filepath.c_str());

	if (fif == FIF_UNKNOWN) // If still unkown, return failure
		return false;

	if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, in_filepath.c_str());
	if (!dib)
		return false;

	BYTE* dataPointer = FreeImage_GetBits(dib); // Retrieve the image data

	width	= FreeImage_GetWidth(dib); // Get the image width and height
	height	= FreeImage_GetHeight(dib);
	bpp		= FreeImage_GetBPP(dib);

	// If somehow one of these failed (they shouldn't), return failure
	if (dataPointer == NULL || width == 0 || height == 0)
		return false;

	// Generate an OpenGL texture ID for this texture
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	int format = bpp == 24 ? GL_BGR_EXT : bpp == 8 ? GL_LUMINANCE : 0;
	int internalFormat = bpp == 24 ? GL_RGB : GL_DEPTH_COMPONENT;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, dataPointer);

	if (generateMipMaps) glGenerateMipmap(GL_TEXTURE_2D);

	FreeImage_Unload(dib);

	glGenSamplers(1, &samplerHandle);

	filepath = in_filepath;
	mipMapsGenerated = generateMipMaps;

	return true; // Success
}

void Texture::UnloadTexture() {
	glDeleteSamplers(1, &samplerHandle);
	glDeleteTextures(1, &textureHandle);
}

GLuint Texture::getTextureHandle(){
	return textureHandle;
}

Texture::~Texture() {
}

void Texture::bind(GLenum textureUnit) {
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glBindSampler(textureUnit, samplerHandle);
}

std::string Texture::getPath() {
	return filepath;
}

void Texture::setFiltering(int in_magnification, int in_minification) {
	// Set magnification filter
	if (in_magnification == TEXTURE_FILTER_MAG_NEAREST)
		glSamplerParameteri(samplerHandle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else if (in_magnification == TEXTURE_FILTER_MAG_BILINEAR)
		glSamplerParameteri(samplerHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set minification filter
	if (in_minification == TEXTURE_FILTER_MIN_NEAREST)
		glSamplerParameteri(samplerHandle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	else if (in_minification == TEXTURE_FILTER_MIN_BILINEAR)
		glSamplerParameteri(samplerHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	else if (in_minification == TEXTURE_FILTER_MIN_NEAREST_MIPMAP)
		glSamplerParameteri(samplerHandle, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	else if (in_minification == TEXTURE_FILTER_MIN_BILINEAR_MIPMAP)
		glSamplerParameteri(samplerHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	else if (in_minification == TEXTURE_FILTER_MIN_TRILINEAR)
		glSamplerParameteri(samplerHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	minification = in_minification;
	magnification = in_magnification;
}

void Texture::setSamplerParameter(GLenum parameter, GLenum value) {
	glSamplerParameteri(samplerHandle, parameter, value);
}

int Texture::getMinificationFilter() {
	return minification;
}

int Texture::getMagnificationFilter() {
	return magnification;
}

void Texture::addTexture(Texture* t) {
	textures.push_back(t);
}

Texture* Texture::createTexture(std::string filepath, int mag, int min, std::string name, bool generateMipMaps) {
	Texture* t = new Texture(name);

	bool success = t->LoadTexture2D(filepath, generateMipMaps);
	if (!success) {
		std::cout << "Error: couldn't load texture " << filepath << std::endl;
	}

	t->setFiltering(mag, min);

	return t;
}

std::string Texture::getName() {
	return name;
}

Texture* Texture::getTexture(std::string name) {
	int len = Texture::textures.size();
	for (int i = 0; i < len; ++i) {
		if (Texture::textures[i]->getName() == name) {
			return Texture::textures[i];
		}
	}

	return NULL;
}