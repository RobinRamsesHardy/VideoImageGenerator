#pragma once
#include "BaseBlock.h"

class TextBlock : public BaseBlock
{
public:
	TextBlock(const std::string& name = "", const int& widthOffset = 0, const int& heightOffset = 0, const Alignment& blockAlignment = Alignment::TopLeft, const SnapAlignment& snapSide = SnapAlignment::NoSnap, const int& width = 0, const int& height = 0);
	TextBlock(const std::string& text, const int& pixelHeight, const std::shared_ptr<Pixel> color, const std::string& name = "", const int& widthOffset = 0, const int& heightOffset = 0,
		const Alignment& blockAlignment = Alignment::Left, const SnapAlignment& snapSide = SnapAlignment::NoSnap, const int& width = 0, const int& height = 0);
	~TextBlock();

	//BaseBlock Overridden functions
	int GetDataWidth() override;
	int GetDataHeight() override;

	void SaveImage(const std::shared_ptr<Image>& image, const std::shared_ptr<Font>& font) override;
	void ClearData() override;

	//Setters
	void SetText(const std::string& value) { Text = value; }
	void SetPixelHeight(const int& value) { PixelHeight = value; }
	void SetCalculatedWidth(const int& value) { CalculatedWidth = value; }
	void SetColor(const std::shared_ptr<Pixel>& value) { Color = value; }

	//Getters
	const std::string GetText() { return Text; }
	const int GetPixelHeight() { return PixelHeight; }

private:
	std::string Text;
	int PixelHeight = 16;
	int CalculatedWidth = 0;
	std::shared_ptr<Pixel> Color;
};
