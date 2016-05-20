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

class Texture
{

public:
	Texture(std::string name = "");
	~Texture();

	void CreateFromData(GLubyte* data, int width, int height, int bpp, GLenum format, bool generateMipMaps, bool pixelStore = false);
	void CreateEmptyTexture(int width, int height, int bpp, GLenum format);
	bool LoadTexture2D(std::string filepath, bool generateMipMaps = false);
	void UnloadTexture();

	int GetMinificationFilter();
	int GetMagnificationFilter();

	void SetFiltering(int in_magnification, int in_minification);
	void SetSamplerParameter(GLenum parameter, GLenum value);

	GLuint GetTextureHandle();
	virtual void Bind(int = 0);

	std::string GetPath();
	std::string GetName();

	static void AddTexture(Texture* t);
	static Texture* CreateTexture(std::string filepath, int magnification, int minification, std::string name, bool generateMipMaps = false);
	static Texture* GetTexture(std::string name);

	static std::vector<Texture*> textures;

private:
	std::string filepath;
	std::string name;

	GLuint textureHandle;
	GLuint samplerHandle;

	int width, height, bpp;
	bool mipMapsGenerated;

	int minification, magnification;
};