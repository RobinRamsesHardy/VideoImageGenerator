#pragma once
#include <iostream>
#include <map>
#include <string>
#include "Image.h"
#include "../library/stb/stb_truetype.h"

struct CharacterInfo 
{
	int CharacterWidth = 0;
	int LeftSideOffset = 0;
};

class Font
{
public:
	Font(const std::string& filePath);
	~Font();

	//Getters
	std::shared_ptr<Image> GetTextImage(const std::string& text, const int& characterPixelHeight);
	std::shared_ptr<Image> GetCharacterImage(const char* text, const int& characterPixelHeight);
	int GetStringLength(const std::string& text, const int& characterPixelHeight);

private:
	void Init(const std::string& filePath);
	void MakeCharacterWidthMap();
	float GetScale(const int& characterPixelHeight);

	//The FontInfo and the Buffer where the font will be loaded into
	stbtt_fontinfo Info;
	std::shared_ptr<unsigned char> Buffer;
	
	//Character information so we can calculate the width of the image when we need to
	std::map<const char, const CharacterInfo> CharacterWidthMap;
	
	int Ascent = 0;
	int Descent = 0;
	int LineGap = 0;
};
