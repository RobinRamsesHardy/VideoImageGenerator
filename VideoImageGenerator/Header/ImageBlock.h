#pragma once
#include "BaseBlock.h"

class ImageBlock : public BaseBlock
{
public:
	ImageBlock(const std::string& name = "", const int& widthOffset = 0, const int& heightOffset = 0, const Alignment& blockAlignment = Alignment::TopLeft, const SnapAlignment& snapSide = SnapAlignment::NoSnap, const int& width = 0, const int& height = 0);
	ImageBlock(const std::shared_ptr<Image>& image, const bool& retainAspectRatio, const std::string& name = "", const int& widthOffset = 0, const int& heightOffset = 0, const Alignment& blockAlignment = Alignment::Left, const SnapAlignment& snapSide = SnapAlignment::NoSnap, const int& width = 0, const int& height = 0);
	~ImageBlock();
	
	//BaseBlock Overridden functions
	int GetDataWidth() override;
	int GetDataHeight() override;

	void SaveImage(const std::shared_ptr<Image>& image, const std::shared_ptr<Font>& font) override;
	void ClearData() override;

	//Setters
	void SetStoredImage(const std::shared_ptr<Image>& value) { StoredImage = value; }
	void SetRetainAspectRatio(const bool& value) { bRetainAspectRatio = value; }

private:
	std::shared_ptr<Image> StoredImage;
	bool bRetainAspectRatio = true;
};
