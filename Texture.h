#pragma once

//Include GLFW  
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

enum ETextureFiltering
{
	TEXTURE_FILTER_MAG_NEAREST = 0, // Nearest criterion for magnification
	TEXTURE_FILTER_MAG_BILINEAR, // Bilinear criterion for magnification
	TEXTURE_FILTER_MIN_NEAREST, // Nearest criterion for minification
	TEXTURE_FILTER_MIN_BILINEAR, // Bilinear criterion for minification
	TEXTURE_FILTER_MIN_NEAREST_MIPMAP, // Nearest criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, // Bilinear criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_TRILINEAR, // Bilinear criterion for minification on two closest mipmaps, then averaged
};

class Texture {

public:
	Texture(std::string name = "");
	~Texture();

	void createFromData(GLubyte* data, int width, int height, int bpp, GLenum format, bool generateMipMaps, bool pixelStore);
	bool LoadTexture2D(std::string filepath, bool generateMipMaps = false);
	void UnloadTexture();

	int getMinificationFilter();
	int getMagnificationFilter();

	void setFiltering(int in_magnification, int in_minification);
	void setSamplerParameter(GLenum parameter, GLenum value);

	GLuint getTextureHandle();
	void bind(GLenum = 0);

	std::string getPath();
	std::string getName();

	static void addTexture(Texture* t);
	static Texture* createTexture(std::string filepath, int magnification, int minification, std::string name, bool generateMipMaps = false);
	static Texture* getTexture(std::string name);

private:
	std::string filepath;
	std::string name;

	GLuint textureHandle;
	GLuint samplerHandle;

	int width, height, bpp;
	bool mipMapsGenerated;

	int minification, magnification;

	static std::vector<Texture*> textures;
};