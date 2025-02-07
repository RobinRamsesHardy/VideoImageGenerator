#pragma once
#include "ImageBlock.h"
#include "TextBlock.h"

class Layout
{
public:
	Layout(const nlohmann::json& JData);
	~Layout();

private:
	//Functions to make the layout, add the data to the blocks in the layout, and save the images
	void Initialize(const nlohmann::json& JData);
	void GoThroughData(const nlohmann::json& JData);
	void SaveImage(const std::string& saveLocation);

	//Setters
	void SetFont(const std::shared_ptr<Font>& font);
	void SetBackgroundImage(const std::string& filename);
	void SetBackgroundImage(const std::shared_ptr<Image>& image);

	//Adders
	void AddBlock(const nlohmann::json& JData, const std::shared_ptr<BaseBlock>& previousBlock = nullptr);
	void AddBaseBlockData(const nlohmann::json& JData, const std::shared_ptr<BaseBlock>& currentBlock, const bool& override = false);
	void AddData(const nlohmann::json& JData, const std::shared_ptr<BaseBlock>& previousBlock = nullptr);

	//Finders
	std::shared_ptr<BaseBlock> FindBlock(const std::vector<std::shared_ptr<BaseBlock>>& list, const std::string& name);
	nlohmann::json FindLayout(const std::vector<std::shared_ptr<PotentialLayout>>& list, const std::string& name);
	void FindLowestHeight(const std::shared_ptr<BaseBlock>& currentBlock, int& value);

	//These variables determine how tall the bottom section is and how many pixels from the bottom of the image there should be to the lowest block
	int BottomHeight = 0;
	int BottomDistanceFromLowestLayoutBlock = -1;

	//Blocks that will store the layout
	std::shared_ptr<BaseBlock> Canvas;

	std::shared_ptr<Font> TextFont;
	std::shared_ptr<Image> BackgroundImage;
	std::string SaveFilePath = "";
};
