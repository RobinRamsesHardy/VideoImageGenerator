#pragma once
#include <iostream>
#include <string>

struct Pixel
{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 0.0f;
	
	void Reset() 
	{
		r = 0.0f;
		g = 0.0f;
		b = 0.0f;
		a = 0.0f;
	}

	//Composite the Pixel with another one using Alpha Blending
	void Composite(const Pixel& otherPixel) 
	{
		float FinalAlpha = a + (1.0f - a) * otherPixel.a;
		if (FinalAlpha > 0.0f) 
		{
			r = (otherPixel.r * otherPixel.a + (1.0f - otherPixel.a) * r * a) / FinalAlpha;
			g = (otherPixel.g * otherPixel.a + (1.0f - otherPixel.a) * g * a) / FinalAlpha;
			b = (otherPixel.b * otherPixel.a + (1.0f - otherPixel.a) * b * a) / FinalAlpha;
			a = FinalAlpha;
		}
	}

	bool operator== (const Pixel& Other) const
	{
		if (fabs(r - Other.r) < FLT_EPSILON && fabs(g - Other.g) < FLT_EPSILON && fabs(b - Other.b) < FLT_EPSILON && fabs(a - Other.a) < FLT_EPSILON)
		{
			return true;
		}
		return false;
	}
};

class Image
{
public:
	//The empty constructor should only be used when creating an empty shared pointer which will have it's value assigned from another
	Image();

	Image(const std::string& filename);
	Image(const int& width, const int& height);

	//This is the function used by the Font class to turn the text into an image so we add a IsText bool since they have to be loaded in a different way
	Image(const std::shared_ptr<unsigned char> imageData, const int& width, const int& height, const int& dataComponents = 4, const bool& bIsText = false);
	~Image();

	void SaveImage(const std::string& saveLocation);
	void ScaleImage(const float& factor);

	//Ascept Ratio will remain the same
	void ResizeImageHeight(const int& newHeight);

	//Ascept Ratio will remain the same
	void ResizeImageWidth(const int& newWidth);

	void ResizeImage(const int& newWidth, const int& newHeight);
	void CompositeImage(const std::shared_ptr<Image> otherImage, const int& widthOffset = 0, const int& heightOffset = 0);
	
	//Copies the value from another shared Image pointer into this one
	void CopyValue(const std::shared_ptr<Image> otherImage);

	//This function is mainly used to change the color of generated text images but can be used for other purposes
	void ChangeColor(const std::shared_ptr<Pixel> color, const bool& changeAlpha = false);

	//Functions to manipulate a section of the image
	void EraseImageSection(const int& sectionWidth, const int& sectionHeight, const int& widthOffset = 0, const int& heightOffset = 0);
	std::shared_ptr<Image> CopyImageSection(const int& sectionWidth, const int& sectionHeight, const int& widthOffset = 0, const int& heightOffset = 0);

	//Getters
	int GetWidth() { return Width; }
	int GetHeight() { return Height; }
	const std::shared_ptr<Pixel> GetData() { return ImageData; }

	bool operator== (const Image& Other) const
	{
		if (Width != Other.Width && Height != Other.Height)
		{
			return false;
		}

		for(int i = 0; i < Width * Height; i++)
		{
			if (!(ImageData.get()[i] == Other.ImageData.get()[i]))
			{
				return false;
			}
		}

		return true;
	}

private:
	//Convert between the Pixel struct and unsigned char
	void UnsignedCharToImageData(const std::shared_ptr<unsigned char> image, const int& components = 4, const bool& bIsText = false);
	std::shared_ptr<unsigned char> ImageDataToUnsignedChar();

	//Image data, the width and height will have their origin in the top left corner of the image with the bottom right corner being their highest values
	int Width = 0;
	int Height = 0;
	int Components = 0; //Amount of channels each pixel has with the option being Grey, Grey & Alpha, RGB, and RGBA
	std::shared_ptr<Pixel> ImageData;
};
