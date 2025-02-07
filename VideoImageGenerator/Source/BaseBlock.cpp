#include "../Header/BaseBlock.h"
#define WidthOffsetMask		 1
#define HeightOffsetMask	 2
#define WidthMask			 4
#define HeightMask			 8
#define SnapSideMask		16
#define AlignmentMask		32

PotentialLayout::PotentialLayout(const std::string& name, const nlohmann::json& JData)
{
	LayoutName = name;
	LayoutJData = JData;
}

PotentialLayout::~PotentialLayout()
{

}

BaseBlock::BaseBlock(const std::string& name, const int& widthOffset, const int& heightOffset, const int& width, const int& height, 
					 const Alignment& blockAlignment, const SnapAlignment& snapSide)
{
	Name = name;
	WidthOffset = widthOffset;
	HeightOffset = heightOffset;
	Width = width;
	Height = height;
	BlockAlignment = blockAlignment;
	SnapSide = snapSide;
}

BaseBlock::~BaseBlock()
{

}

void BaseBlock::AddBlock(const std::shared_ptr<BaseBlock> newBlock)
{
	LinkedBlocks.push_back(newBlock);
}

void BaseBlock::AddPotentialLayout(const std::shared_ptr<PotentialLayout> newLayout) 
{
	PotentialLayouts.push_back(newLayout);
}

void BaseBlock::RemoveBlock(const std::string& name)
{
	int BlockIter = -1;
	for (int i = 0; i < LinkedBlocks.size(); i++) 
	{
		if (LinkedBlocks[i]->GetName() == name)
		{
			BlockIter = i;
			break;
		}
	}

	if (BlockIter > -1)
	{
		LinkedBlocks.erase(LinkedBlocks.begin() + BlockIter);
	}
	else 
	{
		printf("Couldn't find a block with the name %s to remove from block %s linked blocks.\n", name.c_str(), Name.c_str());
	}
}

int BaseBlock::CalculateWidthOffset()
{
	//Make the TotalWidthOffset variable and add the current blocks WidthOffset
	int TotalWidthOffset = WidthOffset;
	std::shared_ptr<BaseBlock> CurrentBlock = PreviousBlock;

	//Go throuh the previous Blocks in the chain until we hit the root Block and add the WidthOffsets and SnapWidthCorrections to the TotalWidthOffset
	while (CurrentBlock != nullptr)
	{
		TotalWidthOffset += CurrentBlock->WidthOffset;
		TotalWidthOffset += CurrentBlock->SnapWidthCorrection;
		CurrentBlock = CurrentBlock->PreviousBlock;
	}

	return TotalWidthOffset;
}

int BaseBlock::CalculateHeightOffset()
{
	//Make the TotalHeightOffset variable and add the current blocks HeightOffset
	int TotalHeightOffset = HeightOffset;
	std::shared_ptr<BaseBlock> CurrentBlock = PreviousBlock;

	//Go throuh the previous Blocks in the chain until we hit the root Block and add the HeightOffsets and SnapHeightCorrections to the TotalHeightOffset
	while (CurrentBlock != nullptr)
	{
		TotalHeightOffset += CurrentBlock->HeightOffset;
		TotalHeightOffset += CurrentBlock->SnapHeightCorrection;
		CurrentBlock = CurrentBlock->PreviousBlock;
	}
	return TotalHeightOffset;
}

//The BaseBlock doesn't contain any data with a Width so it returns 0 and the values should come from the child classes
int BaseBlock::GetDataWidth()
{
	return 0;
}

//The BaseBlock doesn't contain any data with a Height so it returns 0 and the values should come from the child classes
int BaseBlock::GetDataHeight()
{
	return 0;
}

//Expanded all the setters for the values that can be overriden and set a flag and the previous value if they are
void BaseBlock::SetWidthOffset(const int& value, const bool& override) 
{ 
	if (override) 
	{
		OverrideFlags |= WidthOffsetMask;
		DefualtWidthOffset = WidthOffset;
	}

	WidthOffset = value;
}

void BaseBlock::SetHeightOffset(const int& value, const bool& override) 
{ 
	if (override)
	{
		OverrideFlags |= HeightOffsetMask;
		DefualtHeightOffset = HeightOffset;
	}

	HeightOffset = value;
}

void BaseBlock::SetWidth(const int& value, const bool& override) 
{ 
	if (override)
	{
		OverrideFlags |= WidthMask;
		DefualtWidth = Width;
	}

	Width = value;
}

void BaseBlock::SetHeight(const int& value, const bool& override) 
{ 
	if (override)
	{
		OverrideFlags |= HeightMask;
		DefualtHeight = Height;
	}

	Height = value;
}

void BaseBlock::SetSnapSide(const SnapAlignment& value, const bool& override) 
{ 
	if (override)
	{
		OverrideFlags |= SnapSideMask;
		DefualtSnapSide = SnapSide;
	}

	SnapSide = value;
}

void BaseBlock::SetBlockAlignment(const Alignment& value, const bool& override) 
{ 
	if (override)
	{
		OverrideFlags |= AlignmentMask;
		DefualtBlockAlignment = BlockAlignment;
	}

	BlockAlignment = value;
}

//To Calculate the WidthAligment we first modules 3 the BlockAlignment flatting the values from the TopLeft(0) to BottomRight(8) into 
//Left(0), Middle(1), and Right(2). We want a value between 0 and 1 so we need to divide the result by 2 to get the value to multiple with the Width
//of the Data to know how much it needs to shift for the alignment to be correct 
int BaseBlock::CalculateWidthAlignment()
{
	return static_cast<int>(GetDataWidth() * (static_cast<float>(BlockAlignment % 3) / 2.0f));
}

//To Calculate the heightAligment we first divide and floor the BlockAlignment by 3 flatting the values from the TopLeft(0) to BottomRight(8) into 
//Top(0), Middle(1), and Bottom(2). We want a value between 0 and 1 so we need to divide the result by 2 to get the value to multiple with the Height 
//of the Data to know how much it needs to shift for the alignment to be correct 
int BaseBlock::CalculateHeightAlignment()
{
	return static_cast<int>(GetDataHeight() * (floorf(static_cast<float>(BlockAlignment) / 3.0f) / 2.0f));
}

//Based on the side of the previous Block we are snapping too we need to calculate the correction which is done differently per side
void BaseBlock::CalculateAndAddSnapCorrection(int& widthOffset, int& heightOffset)
{
	if (PreviousBlock != nullptr)
	{
		SnapWidthCorrection = 0;
		SnapHeightCorrection = 0;

		switch (SnapSide)
		{
		case SnapAlignment::NoSnap:
			break;
		case SnapAlignment::SnapTop:
			//To calculate the difference in height that needs to be corrected we need to get the current Block's negative height as it is on top
			//of the previous Block, now we need to lower the amount based on the previous Block's height alignment and add this Block's.
			//By doing this we will be able to have the Block properly snap even if the Block's have a different alignment from each other
			SnapHeightCorrection = -1 * GetDataHeight() - PreviousBlock->CalculateHeightAlignment() + CalculateHeightAlignment();
			break;
		case SnapAlignment::SnapBottom:
			//To calculate the difference in height that needs to be corrected we just needs the previous Block's height as this Block will sit beneath it,
			//now we need to lower the amount based on the previous Block's height alignment and add this Block's.
			//By doing this we will be able to have the Block properly snap even if the Block's have a different alignment from each other
			SnapHeightCorrection = PreviousBlock->GetDataHeight() - PreviousBlock->CalculateHeightAlignment() + CalculateHeightAlignment();
			break;
		case SnapAlignment::SnapLeft:
			//To calculate the difference in width that needs to be corrected we need to get the current Block's negative width as it is to the left
			//of the previous Block, now we need to lower the amount based on the previous Block's width alignment and add this Block's.
			//By doing this we will be able to have the Block properly snap even if the Block's have a different alignment from each other
			SnapWidthCorrection = -1 * GetDataWidth() - PreviousBlock->CalculateWidthAlignment() + CalculateWidthAlignment();
			break;
		case SnapAlignment::SnapRight:
			//To calculate the difference in width that needs to be corrected we need to get the previous Block's width as this Block will be to the right of it,
			//of the previous Block, now we need to lower the amount based on the previous Block's width alignment and add this Block's.
			//By doing this we will be able to have the Block properly snap even if the Block's have a different alignment from each other
			SnapWidthCorrection = PreviousBlock->GetDataWidth() - PreviousBlock->CalculateWidthAlignment() + CalculateWidthAlignment();
			break;
		}

		heightOffset += SnapHeightCorrection;
		widthOffset += SnapWidthCorrection;
	}
}

int BaseBlock::CalculateTopHeight()
{
	//To calculate the height of the top of the Block we need to calculate and subtract the height alignment as it can only be moved upwards and the top is 0
	return CalculateHeightOffset() - CalculateHeightAlignment();
}

int BaseBlock::CalculateBottomHeight()
{
	//To calculate the height of the bottom of the Block we calculate the top height and just add the data's height since the top is 0
	return CalculateTopHeight() + GetDataHeight();
}

int BaseBlock::CalculateLeftWidth()
{
	//To calculate the width of the left of the Block we need to calculate and subtract the width alignment as it can only be moved to the left and the left is 0
	return CalculateWidthOffset() - CalculateWidthAlignment();
}

int BaseBlock::CalculateRightWidth()
{
	//To calculate the width of the right of the Block we calculate the left width and just add the data's width since the left is 0
	return CalculateLeftWidth() + GetDataWidth();
}

void BaseBlock::SaveImage(const std::shared_ptr<Image>& image, const std::shared_ptr<Font>& font)
{
	for (std::shared_ptr<BaseBlock> CurrentBlock : LinkedBlocks)
	{
		CurrentBlock->SaveImage(image, font);
	}
}

void BaseBlock::ClearData()
{
	SnapWidthCorrection = 0;
	SnapHeightCorrection = 0;
	
	//Set the values whether an override happened or not if the flag has been set
	if ((OverrideFlags & WidthOffsetMask) == WidthOffsetMask)
	{
		WidthOffset = DefualtWidthOffset;
	}

	if ((OverrideFlags & HeightOffsetMask) == HeightOffsetMask)
	{
		HeightOffset = DefualtHeightOffset;
	}

	if ((OverrideFlags & WidthMask) == WidthMask)
	{
		Width = DefualtWidth;
	}

	if ((OverrideFlags & HeightMask) == HeightMask)
	{
		Height = DefualtHeight;
	}

	if ((OverrideFlags & SnapSideMask) == SnapSideMask)
	{
		SnapSide = DefualtSnapSide;
	}

	if ((OverrideFlags & AlignmentMask) == AlignmentMask)
	{
		BlockAlignment = DefualtBlockAlignment;
	}
	
	//We clear all the flags as they should only exist for 1 image
	OverrideFlags = 0;

	//If the block is created through a possible layout it should be deleted as these will be image dependent and we don't want a redundant data
	//that never gets freed
	if (bCreatedThroughPossibleLayout)
	{
		PreviousBlock->RemoveBlock(Name);
	}
	else
	{
		//We have to go through the vector in reverse since blocks can be removed here causing an out of bounds crash
		for (int i = LinkedBlocks.size() - 1; i > -1; i--)
		{
			LinkedBlocks[i]->ClearData();
		}
	}
}
