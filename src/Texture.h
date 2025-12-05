#pragma once
#include <cstdint>
#include <glad/glad.h>
#include <vector>
#include "raymath.h"

// RGBA each channel is 8 bits --> values 0-255
struct Pixel
{
	uint8_t r = 0x0;
	uint8_t g = 0x0;
	uint8_t b = 0x0;
	uint8_t a = 0xFF;
};

// CPU-only memory
struct Image
{
	int width = -1;
	int height = -1;
	int channels = -1;
	std::vector<Pixel> pixels;
};

// GPU-only memory
struct Texture
{
	GLuint handle = GL_NONE;
	int width = -1;
	int height = -1;
	int channels = -1;
};

void LoadImage(Image* image, int width, int height);
void UnloadImage(Image* image);

void LoadImageGradient(Image* image, Vector3 uv_00/*bottom-left*/, Vector3 uv_10/*bottom-right*/, Vector3 uv_01/*top-left*/, Vector3 uv_11/*top-right*/);
void SaveImage(const char* filename, const Image& image);

void LoadTexture(Texture* texture, const Image& image);
void UnloadTexture(Texture* texture);

void BeginTexture(const Texture& texture);
void EndTexture();

// Extra practice:
// Add a LoadImageFromFile function that uses stb_image to read an image from persistant storage to memory.
// (Then you can load a ct4 texture from an image to see custom textures)!
// Note that you can use TEXTURE_GRADIENT_WARM or TEXTURE_GRADIENT_COOL for A4_CT4_TEXTURE_SHADER, I don't need to see the ct4 textures yet.