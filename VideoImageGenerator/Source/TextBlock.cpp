#include "../Header/TextBlock.h"

TextBlock::TextBlock(const std::string& name, const int& widthOffset, const int& heightOffset, const Alignment& blockAlignment, const SnapAlignment& snapSide, const int& width, const int& height)
	:BaseBlock(name, widthOffset, heightOffset, width, height, blockAlignment, snapSide)
{
	Type = BlockType::TypeText;
}

TextBlock::TextBlock(const std::string& text, const int& pixelHeight, const std::shared_ptr<Pixel> color, const std::string& name, const int& widthOffset, const int& heightOffset,
	const Alignment& blockAlignment, const SnapAlignment& snapSide, const int& width, const int& height)
	: BaseBlock(name, widthOffset, heightOffset, width, height, blockAlignment, snapSide)
{
	Type = BlockType::TypeText;
	Text = text;
	PixelHeight = pixelHeight;
	Color = color;
}

TextBlock::~TextBlock()
{

}

int TextBlock::GetDataWidth()
{
	return CalculatedWidth;
}

int TextBlock::GetDataHeight()
{
	return PixelHeight;
}

void TextBlock::SaveImage(const std::shared_ptr<Image>& image, const std::shared_ptr<Font>& font)
{
	if (font != nullptr || Text == "")
	{
		//Turn the text into an image and change it's color if we set a different one
		std::shared_ptr<Image> TextImage = font->GetTextImage(Text, PixelHeight);
		if (Color.get())
		{
			TextImage->ChangeColor(Color);
		}
		
		//Calculate the position from which we neeed to add the image
		int TextWidthOffset = CalculateWidthOffset() - CalculateWidthAlignment();
		int TextHeightOffset = CalculateHeightOffset() - CalculateHeightAlignment();
		CalculateAndAddSnapCorrection(TextWidthOffset, TextHeightOffset);
		image->CompositeImage(TextImage, TextWidthOffset, TextHeightOffset);
	}
	else
	{
		printf("There was no font or text given so no text can be saved in Block: %s.\n", Name.c_str());
	}

	BaseBlock::SaveImage(image, font);
}

void TextBlock::ClearData()
{
	Text = "";
	PixelHeight = 0;
	Color = nullptr;
	BaseBlock::ClearData();
}
