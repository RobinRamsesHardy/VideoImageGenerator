#include "../Header/Font.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "../library/stb/stb_truetype.h"
//Code took heavy inspiration from: https://github.com/justinmeiners/stb-truetype-example/blob/master/main.c 

Font::Font(const std::string& filePath)
{
	Init(filePath);
	MakeCharacterWidthMap();
}

Font::~Font()
{

}

void Font::Init(const std::string& filePath)
{
	//Initialize variables
	//I am using a pointer and not a shared pointer because I would need to make the shared_ptr<FILE*> to work with fopen_s
	//but this causes it to not work with the rest of the code below
	FILE* FontFile = NULL;
	long Size = 0;

	//Open the FontFile
	fopen_s(&FontFile, filePath.c_str(), "rb");

	//Read the file and copy the files data to the buffer
	if (FontFile)
	{
		//Get the size of the file
		fseek(FontFile, 0, SEEK_END);
		Size = ftell(FontFile);
		fseek(FontFile, 0, SEEK_SET);

		//Make a buffer the size of the file so we can load it in
		Buffer = std::shared_ptr<unsigned char>(new unsigned char[Size]);

		//Read the Fontfile and write all of it to the Info variable
		fread(Buffer.get(), Size, 1, FontFile);
		stbtt_InitFont(&Info, Buffer.get(), 0);

		//Close the FontFile
		fclose(FontFile);
		FontFile = NULL;

		//Get the values which are for how high above and below the baseline the characters go,
		//LineGap being the difference between the lines in the font image
		stbtt_GetFontVMetrics(&Info, &Ascent, &Descent, &LineGap);
	}
	else 
	{
		printf("Font %s failed to load\n", filePath.c_str());
	}
}

void Font::MakeCharacterWidthMap() 
{
	//Make a string with all the characters we will initialize for this map
	//Note: If a character outside of these gets used the code will break
	std::string Alphabet{"aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ 0123456789+-_=:.,'\""};
	CharacterInfo temp;

	//Go throught every letter in the alphabet along with " ", 0-9, and some special characters and load all the width and offsets
	for (int i = 0; i < Alphabet.size(); ++i)
	{
		//Get the Characters width and how far away it needs to be placed and add them to the map
		stbtt_GetCodepointHMetrics(&Info, Alphabet[i], &temp.CharacterWidth, &temp.LeftSideOffset);
		CharacterWidthMap.insert(std::pair<char, CharacterInfo>(Alphabet[i], temp));
	}
}

std::shared_ptr<Image> Font::GetTextImage(const std::string& text, const int& characterPixelHeight)
{
	//Create a canvas where the character images will be printed onto
	std::shared_ptr<Image> TextImage(new Image{ GetStringLength(text, characterPixelHeight), characterPixelHeight });

	//Initialze variables we are going to need
	float Scale = GetScale(characterPixelHeight);
	int XOffset = 0;
	int ScaledAscent = static_cast<int>(roundf(Ascent * Scale));
	int YOffsetRoundingErrorCorrection = 0;

	for (int i = 0; i < text.size(); ++i)
	{
		//Get the YOffset because the characters shouldn't be added at the top,
		//we will need to correct some of the rounding errors as the offset can result in -1
		int YOffset;
		stbtt_GetCodepointBitmapBox(&Info, text[i], Scale, Scale, 0, &YOffset, 0, 0);
		YOffset += ScaledAscent;
		if (YOffset < 0) 
		{
			YOffsetRoundingErrorCorrection = -1 * YOffset;
			YOffset = 0;
		}
		else 
		{
			YOffset += YOffsetRoundingErrorCorrection;
		}

		//Add the character at the right location
		TextImage->CompositeImage(GetCharacterImage(&text[i], characterPixelHeight), XOffset, YOffset);

		//Update the XOffset by adding the new character and the spacing between it and the next character for this font 
		XOffset += static_cast<int>(roundf(CharacterWidthMap.at(text[i]).CharacterWidth * Scale));
		if (i != text.size())
		{
			int Kern;
			Kern = stbtt_GetCodepointKernAdvance(&Info, text[i], text[i + 1]);
			XOffset += static_cast<int>(roundf(Kern * Scale));
		}
	}

	return TextImage;
}

std::shared_ptr<Image> Font::GetCharacterImage(const char* text, const int& characterPixelHeight) 
{
	float Scale = GetScale(characterPixelHeight);
	
	//Get the bounding box around the letter
	int Left = 0;
	int Bottom = 0;
	int Right = 0;
	int Top = 0;
	stbtt_GetCodepointBitmapBox(&Info, *text, Scale, Scale, &Left, &Bottom, &Right, &Top);

	int CharacterWidth = Right - Left;
	int CharacterHeight = Top - Bottom;
	
	//Create a bitmap to write the character into. This bitmap will be turned into an image and returned
	std::shared_ptr<unsigned char> Bitmap(new unsigned char[CharacterHeight * CharacterWidth]);
	stbtt_MakeCodepointBitmap(&Info, Bitmap.get(), Right - Left, Top - Bottom, CharacterWidth, Scale, Scale, *text);
	return std::shared_ptr<Image>(new Image(Bitmap, CharacterWidth, CharacterHeight, 1, true));
}

int Font::GetStringLength(const std::string& text, const int& characterPixelHeight) 
{
	float Scale = GetScale(characterPixelHeight);
	int Length = 0;

	for (int i = 0; i < text.size(); i++)
	{
		//Get the Characters width and how far away it needs to be placed and add them to the map
		Length += static_cast<int>(roundf(CharacterWidthMap.at(text[i]).CharacterWidth * Scale));

		//Add spacing
		if (i != text.size()) 
		{
			int Kern;
			Kern = stbtt_GetCodepointKernAdvance(&Info, text[i], text[i + 1]);
			Length += static_cast<int>(roundf(Kern * Scale));
		}
	}

	return Length;
}

float Font::GetScale(const int& characterPixelHeight)
{
	return stbtt_ScaleForPixelHeight(&Info, static_cast<float>(characterPixelHeight));
}
