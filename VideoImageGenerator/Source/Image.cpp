#include "../Header/Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Library/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../Library/stb/stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../Library/stb/stb_image_resize2.h"

Image::Image() 
{

}

Image::Image(const std::string& filename)
{
	std::shared_ptr<unsigned char> RawImageData(stbi_load(filename.c_str(), &Width, &Height, &Components, 4));
	if (RawImageData.get() == NULL)
	{
		printf("Image: %s failed to load because %s\n", filename.c_str(), stbi_failure_reason());
	}

	UnsignedCharToImageData(RawImageData, Components);
	Components = 4; //This is done because after the data is loaded and converted it will have 4 Components
}

Image::Image(const int& width, const int& height) 
{
	Width = width;
	Height = height;
	ImageData.reset(new Pixel[Width * Height]);
	Components = 4;
}

Image::Image(const std::shared_ptr<unsigned char> imageData, const int& width, const int& height, const int& dataComponents, const bool& bIsText)
{
	Width = width;
	Height = height;
	UnsignedCharToImageData(imageData, dataComponents, bIsText);
	Components = 4; //This is done because after the data is loaded and converted it will have 4 Components
}

Image::~Image() 
{
	for (int currentPixel = 0; currentPixel < Width * Height; currentPixel++)
	{
		ImageData.get()[currentPixel].Reset();
	}
}

void Image::SaveImage(const std::string& saveLocation) 
{
	if (!stbi_write_png(saveLocation.c_str(), Width, Height, Components, ImageDataToUnsignedChar().get(), Width * Components))
	{
		printf("Image failed to save at: %s because %s\n", saveLocation.c_str(), stbi_failure_reason());
	}
}

void Image::ScaleImage(const float& factor) 
{
	if (factor) 
	{
		ResizeImage(static_cast<int>(Width * factor), static_cast<int>(Height * factor));
	}
	else 
	{
		printf("Can't scale with a factor of 0\n");
	}
}

void Image::ResizeImageHeight(const int& newHeight)
{
	float Scale = 1.0f / Height * newHeight;
	ResizeImage(static_cast<int>(roundf(Width * Scale)), newHeight);
}

void Image::ResizeImageWidth(const int& newWidth)
{
	float Scale = 1.0f / Width * newWidth;
	ResizeImage(newWidth, static_cast<int>(roundf(Height * Scale)));
}

void Image::ResizeImage(const int& newWidth, const int& newHeight) 
{
	std::shared_ptr<unsigned char> ResizedData(stbir_resize_uint8_srgb(ImageDataToUnsignedChar().get(), Width, Height, Width * Components, NULL, newWidth, newHeight, newWidth * Components, STBIR_RGBA));
	if (ResizedData) 
	{
		Width = newWidth;
		Height = newHeight;
		UnsignedCharToImageData(ResizedData, 4);
	}
	else 
	{
		printf("Failed to resize Image because %s\n", stbi_failure_reason());
	}
}

void Image::ChangeColor(const std::shared_ptr<Pixel> color, const bool& changeAlpha)
{
	for (int currentPixel = 0; currentPixel < Width * Height; currentPixel++)
	{
		ImageData.get()[currentPixel].r = color->r;
		ImageData.get()[currentPixel].g = color->g;
		ImageData.get()[currentPixel].b = color->b;
		if (changeAlpha) 
		{
			ImageData.get()[currentPixel].a = color->a;
		}
	}
}

//The offset are for the topleft corner where the image will be inserted
void Image::CompositeImage(const std::shared_ptr<Image> otherImage, const int& widthOffset, const int& heightOffset)
{
	//if the composite image would exceed the bounds of the image cut it off
	int MaxHeight = 0;
	if (heightOffset + otherImage->Height >= Height)
	{
		MaxHeight = Height - heightOffset;
	}
	else
	{
		MaxHeight = otherImage->Height;
	}

	int MinimumHeight = 0;
	if (heightOffset < 0) 
	{
		MinimumHeight -= heightOffset;
	}

	int MaxWidth = 0;
	if (widthOffset + otherImage->Width >= Width) 
	{
		MaxWidth = Width - widthOffset;
	}
	else 
	{
		MaxWidth = otherImage->Width;
	}

	int MinimumWidth = 0;
	if (widthOffset < 0)
	{
		MinimumWidth -= widthOffset;
	}

	//To make lines more readable make a currentOtherPixel to store the otherPixel
	std::shared_ptr<Pixel> currentOtherPixel(new Pixel);
	for (int currentHeight = MinimumHeight; currentHeight < MaxHeight; currentHeight++)
	{
		for (int currentWidth = MinimumWidth; currentWidth < MaxWidth; currentWidth++)
		{
			//If the alpha of the image we are overlaying is 1.0f we can just set the values
			currentOtherPixel.get()[0] = otherImage->ImageData.get()[currentHeight * otherImage->Width + currentWidth];
			if (currentOtherPixel->a == 1.0f)
			{
				ImageData.get()[(currentHeight + heightOffset) * Width + widthOffset + currentWidth] = currentOtherPixel.get()[0];
			}
			else 
			{
				ImageData.get()[(currentHeight + heightOffset) * Width + widthOffset + currentWidth].Composite(currentOtherPixel.get()[0]);
			}
		}
	}
}

void Image::CopyValue(const std::shared_ptr<Image> otherImage) 
{
	Width = otherImage->Width;
	Height = otherImage->Height;
	Components = otherImage->Components;
	ImageData.reset(new Pixel[Width * Height]);

	for (int i = 0; i < Width * Height; i++) 
	{
		ImageData.get()[i].r = otherImage->ImageData.get()[i].r;
		ImageData.get()[i].g = otherImage->ImageData.get()[i].g;
		ImageData.get()[i].b = otherImage->ImageData.get()[i].b;
		ImageData.get()[i].a = otherImage->ImageData.get()[i].a;
	}
}

void Image::EraseImageSection(const int& sectionWidth, const int& sectionHeight, const int& widthOffset, const int& heightOffset)
{
	//Check that we aren't trying to erase outside the image bounds and correct the values if we are
	int MaxSectionHeight = sectionHeight;
	if (heightOffset + sectionHeight >= Height) 
	{
		MaxSectionHeight = Height - heightOffset;
	}

	int MinSectionHeight = heightOffset;
	if (heightOffset < 0) 
	{
		MinSectionHeight = 0;
	}

	int MaxSectionWidth = sectionWidth;
	if (widthOffset + sectionWidth >= Width)
	{
		MaxSectionWidth = Width - widthOffset;
	}

	int MinSectionWidth = widthOffset;
	if (widthOffset < 0)
	{
		MinSectionWidth = 0;
	}

	//Go through all the pixels in the section that we want to erase and call reset on the pixels
	for (int currentHeight = 0; currentHeight < MaxSectionHeight; currentHeight++)
	{
		for (int currentWidth = 0; currentWidth < MaxSectionWidth; currentWidth++)
		{
			ImageData.get()[(currentHeight + MinSectionHeight) * Width + (currentWidth + MinSectionWidth)].Reset();
		}
	}
}

std::shared_ptr<Image> Image::CopyImageSection(const int& sectionWidth, const int& sectionHeight, const int& widthOffset, const int& heightOffset)
{
	//Check that we aren't trying to copy outside the image bounds and correct the values if we are
	int MaxSectionHeight = sectionHeight;
	if (heightOffset + sectionHeight >= Height)
	{
		MaxSectionHeight = Height - heightOffset;
	}

	int MinSectionHeight = heightOffset;
	if (heightOffset < 0)
	{
		MinSectionHeight = 0;
	}

	int MaxSectionWidth = sectionWidth;
	if (widthOffset + sectionWidth >= Width)
	{
		MaxSectionWidth = Width - widthOffset;
	}

	int MinSectionWidth = widthOffset;
	if (widthOffset < 0)
	{
		MinSectionWidth = 0;
	}

	//Make a CopyImage cariable to copy the section of the image into
	std::shared_ptr<Image> CopyImage{new Image(sectionWidth, sectionHeight)};

	//Loop through the section we want to copy and than return it
	for (int currentHeight = 0; currentHeight < MaxSectionHeight; currentHeight++)
	{
		for (int currentWidth = 0; currentWidth < MaxSectionWidth; currentWidth++)
		{
			CopyImage->ImageData.get()[currentHeight * MaxSectionWidth + currentWidth] = ImageData.get()[(currentHeight + MinSectionHeight) * Width + (currentWidth + MinSectionWidth)];
		}
	}
	return CopyImage;
}

void Image::UnsignedCharToImageData(std::shared_ptr<unsigned char> image, const int& components, const bool& bIsText)
{
	//Create the pixels for the image
	ImageData.reset(new Pixel[Width * Height]);

	//Calculate changes since an image can have 1, 2, 3, 4 components and all of them should be convertable
	int GIncrease = static_cast<int>(floor((components - 1) / 2)) * 1;
	int BIncrease = static_cast<int>(floor((components - 1) / 2)) * 2;
	int AIncrease = components - 1;
	
	//Go through every pixel and assign the value
	for (int currentPixel = 0; currentPixel < Width * Height; currentPixel++)
	{
		if (bIsText) 
		{
			ImageData.get()[currentPixel].r = 1.0f;
			ImageData.get()[currentPixel].g = 1.0f;
			ImageData.get()[currentPixel].b = 1.0f;
		}
		else 
		{
			ImageData.get()[currentPixel].r = image.get()[currentPixel * components] / 255.0f;
			ImageData.get()[currentPixel].g = image.get()[currentPixel * components + GIncrease] / 255.0f;
			ImageData.get()[currentPixel].b = image.get()[currentPixel * components + BIncrease] / 255.0f;
		}

		//If components == 1 or 3 we don't have a alpha channel so we defualt it to 1
		if ((components == 1 || components == 3) && !bIsText)
		{
			ImageData.get()[currentPixel].a = 1.0f;
		}
		else 
		{
			ImageData.get()[currentPixel].a = image.get()[currentPixel * components + AIncrease] / 255.0f;
		}
	}
}

std::shared_ptr<unsigned char> Image::ImageDataToUnsignedChar()
{
	//Make all the empty unsigned chars for this image
	std::shared_ptr<unsigned char> tempImage(new unsigned char[Width * Height * Components]);
	
	//Go through every pixel and convert them back into unsigned chars which we return
	for (int currentPixel = 0; currentPixel < Width * Height; currentPixel++) 
	{
		tempImage.get()[currentPixel * 4] = static_cast<int>(ImageData.get()[currentPixel].r * 255.0f);
		tempImage.get()[currentPixel * 4 + 1] = static_cast<int>(ImageData.get()[currentPixel].g * 255.0f);
		tempImage.get()[currentPixel * 4 + 2] = static_cast<int>(ImageData.get()[currentPixel].b * 255.0f);
		tempImage.get()[currentPixel * 4 + 3] = static_cast<int>(ImageData.get()[currentPixel].a * 255.0f);
	}
	return tempImage;
}
