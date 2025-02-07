#pragma once
#include <vector>
#include <string>
#include "Image.h"
#include "Font.h"
#include "../Library/json/json.hpp"

//The alignment of the Block relative to the point where it is located
enum Alignment
{
	TopLeft = 0,
	TopMiddle = 1,
	TopRight = 2,
	Left = 3,
	Center = 4,
	Right = 5,
	BottomLeft = 6,
	BottomMiddle = 7,
	BottomRight = 8
};

//The side from the previous Block the current Block should snap to
enum SnapAlignment
{
	NoSnap = 0,
	SnapTop = 1,
	SnapLeft = 2,
	SnapBottom = 3,
	SnapRight = 4
};

//The type of Block which we are dealing with
enum BlockType
{
	Non = 0,
	TypeImage = 1,
	TypeText = 2
};

//Class to store the PotentialLayout which is used to store the json data
class PotentialLayout
{
public:
	PotentialLayout(const std::string& name, const nlohmann::json& JData);
	~PotentialLayout();

	std::string GetName() { return LayoutName; }
	nlohmann::json GetJData() { return LayoutJData; }

private:
	std::string LayoutName = "";
	nlohmann::json LayoutJData;
};

//The BaseBlock which is used for everything in the Layout
class BaseBlock
{
public:
	BaseBlock(const std::string& name, const int& widthOffset = 0, const int& heightOffset = 0, const int& width = 0, const int& height = 0, const Alignment& blockAlignment = Alignment::Left, const SnapAlignment& snapSide = SnapAlignment::NoSnap);
	~BaseBlock();

	//Adders
	void AddBlock(const std::shared_ptr<BaseBlock> newBlock);
	void AddPotentialLayout(const std::shared_ptr<PotentialLayout> newLayout);

	//Virtual functions used Save and ClearData which will be called in every child
	virtual void SaveImage(const std::shared_ptr<Image>& image, const std::shared_ptr<Font>& font);
	virtual void ClearData();

	//Functions for finding the position of a specific side of the Block
	int CalculateTopHeight();
	int CalculateBottomHeight();
	int CalculateLeftWidth();
	int CalculateRightWidth();

	//Getters and Setters
	void SetWidthOffset(const int& value, const bool& override = false);
	void SetHeightOffset(const int& value, const bool& override = false);
	void SetWidth(const int& value, const bool& override = false);
	void SetHeight(const int& value, const bool& override = false);

	//Virtual functions to get data from the child classes to use in the calculations
	virtual int GetDataWidth();
	virtual int GetDataHeight();

	void SetCreatedThroughPossibleLayout(const bool value) { bCreatedThroughPossibleLayout = value; };
	void SetSnapSide(const SnapAlignment& value, const bool& override = false);
	void SetBlockAlignment(const Alignment& value, const bool& override = false);
	const BlockType GetBlockType() { return Type; };

	void SetName(const std::string& value) { Name = value; };
	const std::string& GetName() { return Name; };
	const std::vector<std::shared_ptr<PotentialLayout>>& GetPotentialLayouts() { return PotentialLayouts; };
	const std::vector<std::shared_ptr<BaseBlock>>& GetLinkedBlocks() { return LinkedBlocks; };
	void SetPreviousBlock(const std::shared_ptr<BaseBlock> value) { PreviousBlock = value; };

protected:
	//Functions that will backpropagate through the Blocks to calculate the Blocks current offset
	int CalculateWidthOffset();
	int CalculateHeightOffset();

	//Functions to calculate how much the Aligment enum should correct the position
	int CalculateWidthAlignment();
	int CalculateHeightAlignment();

	//Function that calculates the correction that needs to be applied to have the Block correctly snap which is stored and applied
	void CalculateAndAddSnapCorrection(int& widthOffset, int& heightOffset);

	//Function to remove one of the linked blocks, this is only used to remove a block that has been added through a possible layout
	void RemoveBlock(const std::string& name);

	//The Offset is relative to the previous Block making it easier to move things around
	int WidthOffset = 0;
	int HeightOffset = 0;

	//If Width, Height, or both are 0 that axis will be scaled with the image resolution and if both are 0 the image won't be scaled at all
	int Width = 0;
	int Height = 0;

	//The correction applied to the block when calculating the correction that needs to be applied to a later block
	int SnapWidthCorrection = 0;
	int SnapHeightCorrection = 0;

	//If the block was created using a PotentialLayout it shouldn't remain for the next image
	bool bCreatedThroughPossibleLayout = false;

	//When snapping to a side it will pick the side of the previous block
	SnapAlignment SnapSide = SnapAlignment::NoSnap;

	//Determines which part the image should be drawn from when given a location 
	Alignment BlockAlignment = Alignment::TopLeft;
	BlockType Type = BlockType::Non;

	//This will be used to tell which block this is
	std::string Name = "";

	std::vector<std::shared_ptr<PotentialLayout>> PotentialLayouts;
	std::vector<std::shared_ptr<BaseBlock>> LinkedBlocks;
	std::shared_ptr<BaseBlock> PreviousBlock = nullptr;

	//Store the values so we can revert them if they are overriden for a specific image
	char OverrideFlags = 0;
	int DefualtWidthOffset = 0;
	int DefualtHeightOffset = 0;
	int DefualtWidth = 0;
	int DefualtHeight = 0;
	SnapAlignment DefualtSnapSide = SnapAlignment::NoSnap;
	Alignment DefualtBlockAlignment = Alignment::TopLeft;
};
