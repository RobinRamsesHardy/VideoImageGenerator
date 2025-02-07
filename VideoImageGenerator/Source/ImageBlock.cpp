#include "../Header/ImageBlock.h"

ImageBlock::ImageBlock(const std::string& name, const int& widthOffset, const int& heightOffset, const Alignment& blockAlignment, const SnapAlignment& snapSide, const int& width, const int& height)
	: BaseBlock(name, widthOffset, heightOffset, width, height, blockAlignment, snapSide)
{
	Type = BlockType::TypeImage;
}

ImageBlock::ImageBlock(const std::shared_ptr<Image>& image, const bool& retainAspectRatio, const std::string& name, const int& widthOffset, const int& heightOffset, const Alignment& blockAlignment, const SnapAlignment& snapSide, const int& width, const int& height)
	: BaseBlock(name, widthOffset, heightOffset, width, height, blockAlignment, snapSide)
{
	Type = BlockType::TypeImage;
	StoredImage = image;
	bRetainAspectRatio = retainAspectRatio;
}

ImageBlock::~ImageBlock()
{

}

int ImageBlock::GetDataWidth()
{
	return StoredImage->GetWidth();
}

int ImageBlock::GetDataHeight()
{
	return StoredImage->GetHeight();
}

void ImageBlock::SaveImage(const std::shared_ptr<Image>& image, const std::shared_ptr<Font>& font)
{
	if (StoredImage != nullptr)
	{
		//Resize the image based on the parameters we want
		if (bRetainAspectRatio)
		{
			if (Width != 0 && Width != StoredImage->GetWidth())
			{
				StoredImage->ResizeImageWidth(Width);
			}

			if (Height != 0 && Height < StoredImage->GetHeight())
			{
				StoredImage->ResizeImageHeight(Height);
			}
		}
		else
		{
			StoredImage->ResizeImage(Width, Height);
		}

		//Calculate the position from which we neeed to add the image
		int ImageWidthOffset = CalculateWidthOffset() - CalculateWidthAlignment();
		int ImageHeightOffset = CalculateHeightOffset() - CalculateHeightAlignment();
		CalculateAndAddSnapCorrection(ImageWidthOffset, ImageHeightOffset);
		image->CompositeImage(StoredImage, ImageWidthOffset, ImageHeightOffset);
	}
	else
	{
		printf("There was no image given to save for Block: %s.\n", Name.c_str());
	}

	BaseBlock::SaveImage(image, font);
}

void ImageBlock::ClearData()
{
	StoredImage = nullptr;
	BaseBlock::ClearData();
}
