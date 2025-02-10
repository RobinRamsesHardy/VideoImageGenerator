#include "../Header/Layout.h"
#include <filesystem>

Layout::Layout(const nlohmann::json& JData)
{
	Initialize(JData.at("Layout"));
	GoThroughData(JData.at("Images"));
}

Layout::~Layout()
{

}

void Layout::Initialize(const nlohmann::json& JData) 
{
	//Go through the layout and get all the information out of it
	if (JData.contains("SaveFilePath"))
	{
		SaveFilePath = JData.at("SaveFilePath");
	}
	else
	{
		SaveFilePath = std::filesystem::current_path().string() + "\\";
		printf("The json doesn't contain a filepath to save to so it will defualt to the working directory which is: %s.\n", SaveFilePath.c_str());
	}

	//We return here because if no size is given we can't estimate what size they might want and since this isn't dynamic yet
	if (JData.contains("Background Image"))
	{
		SetBackgroundImage(JData.at("Background Image"));
	}
	else
	{
		if (JData.contains("Width") && JData.contains("Height"))
		{
			SetBackgroundImage(std::shared_ptr<Image>{new Image(JData.at("Width"), JData.at("height"))});
		}
		else
		{
			printf("The json doesn't contain a valid background image address or width and height so the layout can't be used.\n");
			return;
		}
	}

	//Initialize the Canvas which all the blocks will be added onto
	Canvas = std::shared_ptr<BaseBlock>(new BaseBlock("Canvas", 0, 0, BackgroundImage->GetWidth(), BackgroundImage->GetHeight()));

	if (JData.contains("Font"))
	{
		SetFont(std::shared_ptr<Font>{new Font(JData.at("Font"))});
	}

	if (JData.contains("BottomBottomDistanceFromLowestLayoutBlock"))
	{
		BottomDistanceFromLowestLayoutBlock = JData.at("BottomBottomDistanceFromLowestLayoutBlock");
	}

	if (JData.contains("BottomHeight"))
	{
		BottomHeight = JData.at("BottomHeight");
	}

	AddPotentialLayouts(JData, Canvas);

	if (JData.contains("Blocks"))
	{
		nlohmann::json Blocks = JData.at("Blocks");
		for (int i = 0; i < Blocks.size(); i++)
		{
			AddBlock(Blocks[i]);
		}
	}
}

void Layout::GoThroughData(const nlohmann::json& JData) 
{
	//We go though all of the imagedata and save the images
	std::string Filename = "";
	for (int i = 0; i < JData.size(); i++)
	{
		Filename = JData[i].at("Filename");

		nlohmann::json Data = JData[i].at("Data");
		for (int i = 0; i < Data.size(); i++)
		{
			AddData(Data[i]);
		}

		SaveImage(SaveFilePath + Filename + ".png");
		printf("Image saved to: %s as: %s.png\n", SaveFilePath.c_str(), Filename.c_str());

		Canvas->ClearData();
	}
}

void Layout::SetFont(const std::shared_ptr<Font>& font)
{
	TextFont = font;
}

void Layout::SaveImage(const std::string& saveLocation)
{
	//We don't want to alter the stored BackgroundImage so we will copy the data add the rest from this layout onto it and save that instead.
	std::shared_ptr<Image> Background(new Image());
	Background->CopyValue(BackgroundImage);

	//Because the Background image might have to be altered before saving we will add everything to an empty canvas and add that to the background.
	std::shared_ptr<Image> EmptyCanvas(new Image(Background->GetWidth(), Background->GetHeight()));
	
	//Go through all the blocks and add them to the canvas and calculate the LowestHeight.
	int LowestHeight = 0;
	Canvas->SaveImage(EmptyCanvas, TextFont);
	FindLowestHeight(Canvas, LowestHeight);

	//Due to what I want to do with this project I want to be able to crop the background to more accurately fit the contents of the layout.
	//Because of this I am cutting out a section of the background and moving it up a little to fit better.
	if (BottomDistanceFromLowestLayoutBlock > 0 && BottomHeight > 0)
	{
		std::shared_ptr<Image> BottomSection = Background->CopyImageSection(Background->GetWidth(), BottomHeight, 0, Background->GetHeight() - BottomHeight);
		Background->EraseImageSection(Background->GetWidth(), Background->GetHeight() - LowestHeight, 0, LowestHeight);
		Background->CompositeImage(BottomSection, 0, LowestHeight + BottomDistanceFromLowestLayoutBlock - BottomHeight);
	}

	Background->CompositeImage(EmptyCanvas);
	Background->SaveImage(saveLocation);
}

void Layout::SetBackgroundImage(const std::string& filename) 
{
	std::shared_ptr<Image> TempImage(new Image(filename));
	if (TempImage.get() != nullptr)
	{
		SetBackgroundImage(TempImage);
	}
	else
	{
		printf("Failed to load Background Image.\n");
	}
}

void Layout::SetBackgroundImage(const std::shared_ptr<Image>& image)
{
	BackgroundImage = image;
}

void Layout::AddBlock(const nlohmann::json& JData, const std::shared_ptr<BaseBlock>& previousBlock)
{
	//Add a block and all it's data, because a block can be multiple types we have to make sure we create the correct one which is the first thing we do
	std::string Type = JData.at("Type");
	std::shared_ptr<BaseBlock> TempBlock = nullptr;
	if (Type != "ImageBlock" && Type != "TextBlock")
	{
		printf("No valid Block Type given.\n");
	}
	else 
	{
		if (Type == "ImageBlock")
		{
			TempBlock = std::shared_ptr<BaseBlock>(new ImageBlock());
		}
		else
		{
			TempBlock = std::shared_ptr<BaseBlock>(new TextBlock());
		}

		if (JData.contains("Name"))
		{
			TempBlock->SetName(JData.at("Name"));
		}
		else 
		{
			printf("A block must be given a name.\n");
		}

		AddBaseBlockData(JData, TempBlock);

		//Once all the data has been added we will either add the block to the Blocks vector in the layout or the LinkedBlocks vector from the previousBlock
		TempBlock->SetPreviousBlock(previousBlock);
		if (previousBlock == nullptr)
		{
			Canvas->AddBlock(TempBlock);
		}
		else
		{
			previousBlock->AddBlock(TempBlock);
		}
	}

	AddPotentialLayouts(JData, TempBlock);

	if (JData.contains("Blocks"))
	{
		nlohmann::json Block = JData.at("Blocks");
		for (int i = 0; i < Block.size(); i++)
		{
			AddBlock(Block[i], TempBlock);
		}
	}
}

void Layout::AddPotentialLayouts(const nlohmann::json& JData, const std::shared_ptr<BaseBlock>& currentBlock) 
{
	//Go through the PotentialLayouts if they are there and store the .json of this section so it can be constructed later if it is called on
	if (JData.contains("PotentialLayouts") && currentBlock != nullptr)
	{
		nlohmann::json Layouts = JData.at("PotentialLayouts");
		for (int i = 0; i < Layouts.size(); i++)
		{
			nlohmann::json Layout = Layouts[i];
			if (Layout.contains("LayoutName"))
			{
				currentBlock->AddPotentialLayout(std::shared_ptr<PotentialLayout>(new PotentialLayout(Layout.at("LayoutName"), Layout)));
			}
			else
			{
				printf("Couldn't find a LayoutName for a PotentialLayout in Block: %s.", currentBlock->GetName().c_str());
			}
		}
	}
}

void Layout::AddBaseBlockData(const nlohmann::json& JData, const std::shared_ptr<BaseBlock>& currentBlock, const bool& override)
{
	if (currentBlock == nullptr) 
	{
		printf("You can't call AddBaseBlockData without a Block.\n");
		return;
	}

	if (JData.contains("WidthOffset"))
	{
		currentBlock->SetWidthOffset(JData.at("WidthOffset"), override);
	}
	if (JData.contains("HeightOffset"))
	{
		currentBlock->SetHeightOffset(JData.at("HeightOffset"), override);
	}
	if (JData.contains("Width"))
	{
		currentBlock->SetWidth(JData.at("Width"), override);
	}
	if (JData.contains("Height"))
	{
		currentBlock->SetHeight(JData.at("Height"), override);
	}
	if (JData.contains("Alignment"))
	{
		currentBlock->SetBlockAlignment(JData.at("Alignment"), override);
	}
	if (JData.contains("SnapSide"))
	{
		currentBlock->SetSnapSide(JData.at("SnapSide"), override);
	}
}

void Layout::AddData(const nlohmann::json& JData, const std::shared_ptr<BaseBlock>& previousBlock)
{
	//If no name is given we won't be able to find this block to add data to so the function will return right there.
	std::string TempName = "";
	if (JData.contains("Name"))
	{
		TempName = JData.at("Name");
	}
	else 
	{
		printf("There was no Name in the json for the AddData function so no Block can be found.\n");
		return;
	}

	//Now that we have a name for a block we have to find it. 
	std::shared_ptr<BaseBlock> TempBlock = nullptr;
	if (previousBlock == nullptr) 
	{
		FindBlock(TempBlock, Canvas, TempName);
	}
	else 
	{
		FindBlock(TempBlock, previousBlock, TempName);
	}

	if (TempBlock == nullptr) 
	{
		if (previousBlock == nullptr) 
		{
			printf("Tried adding data to block %s that could not be found and has no known previous block.\n", TempName.c_str());
		}
		else 
		{
			printf("Tried adding data to block %s that could not be found in the linked blocks of block %s.\n", TempName.c_str(), previousBlock->GetName().c_str());
		}
		return;
	}

	//Depending on the BlockType we want to add different data into it
	if (TempBlock->GetBlockType() == BlockType::TypeImage)
	{
		std::shared_ptr<ImageBlock> TempImageBlock = std::dynamic_pointer_cast<ImageBlock>(TempBlock);
		if (JData.contains("StoredImage"))
		{
			TempImageBlock->SetStoredImage(std::shared_ptr<Image>(new Image{ JData.at("StoredImage") }));
		}
		else 
		{
			printf("Block %s hasn't been given a StoredImage.\n", TempBlock->GetName().c_str());
			return;
		}

		if (JData.contains("RetainAspectRatio"))
		{
			TempImageBlock->SetRetainAspectRatio(JData.at("RetainAspectRatio"));
		}
	}
	else if(TempBlock->GetBlockType() == BlockType::TypeText)
	{
		std::shared_ptr<TextBlock> TempTextBlock = std::dynamic_pointer_cast<TextBlock>(TempBlock);
		if (JData.contains("Text"))
		{
			TempTextBlock->SetText(JData.at("Text"));
		}
		else 
		{
			printf("Block %s hasn't been given Text.\n", TempBlock->GetName().c_str());
			return;
		}

		if (JData.contains("PixelHeight"))
		{
			TempTextBlock->SetPixelHeight(JData.at("PixelHeight"));
		}
		else 
		{
			printf("Block %s hasn't been given a PixelHeight so it will default to 16.\n", TempBlock->GetName().c_str());
		}
		
		std::shared_ptr<Pixel> TempPixel(new Pixel{ 1.0f, 1.0f, 1.0f, 1.0f });
		if (JData.contains("ColorR"))
		{
			TempPixel->r = JData.at("ColorR");
		}
		if (JData.contains("ColorG"))
		{
			TempPixel->g = JData.at("ColorG");
		}
		if (JData.contains("ColorB"))
		{
			TempPixel->b = JData.at("ColorB");
		}
		if (JData.contains("ColorA"))
		{
			TempPixel->a = JData.at("ColorA");
		}
		if(TextFont != nullptr) 
		{
			TempTextBlock->SetCalculatedWidth(TextFont->GetStringLength(TempTextBlock->GetText(), TempTextBlock->GetPixelHeight()));
		}

		TempTextBlock->SetColor(TempPixel);
	}

	//For a specific image in a group we might want to change the paramaters so they can be overwritten here
	if (JData.contains("Override"))
	{
		nlohmann::json Data = JData.at("Override");
		AddBaseBlockData(Data, TempBlock, true);
	}

	if (JData.contains("Blocks"))
	{
		nlohmann::json Data = JData.at("Blocks");
		for (int i = 0; i < Data.size(); i++)
		{
			AddData(Data[i], TempBlock);
		}
	}
}

void Layout::FindBlock(std::shared_ptr<BaseBlock>& foundBlock, const std::shared_ptr<BaseBlock>& currentBlock, const std::string& name)
{
	//If the name is the same as a PotentialLayout from the previousBlock it will be constructed.
	if (!FindLayout(currentBlock->GetPotentialLayouts(), name).empty())
	{
		AddBlock(FindLayout(currentBlock->GetPotentialLayouts(), name), currentBlock);
		foundBlock = currentBlock->GetLinkedBlocks()[currentBlock->GetLinkedBlocks().size() - 1];
		foundBlock->SetCreatedThroughPossibleLayout(true);
	}
	else if (FindBlock(currentBlock->GetLinkedBlocks(), name) != nullptr)
	{
		foundBlock = FindBlock(currentBlock->GetLinkedBlocks(), name);
	}
	else
	{
		printf("Couldn't find the layout or potential layout with the name %s in the lists of block %s.\n", name.c_str(), currentBlock->GetName().c_str());
		return;
	}
}

std::shared_ptr<BaseBlock> Layout::FindBlock(const std::vector<std::shared_ptr<BaseBlock>>& list, const std::string& name)
{
	for (std::shared_ptr<BaseBlock> CurrentBlock : list)
	{
		if (CurrentBlock->GetName() == name)
		{
			return CurrentBlock;
		}
	}

	return nullptr;
}

nlohmann::json Layout::FindLayout(const std::vector<std::shared_ptr<PotentialLayout>>& list, const std::string& name)
{
	if (list.size() > 0) 
	{
		for (std::shared_ptr<PotentialLayout> CurrentLayout : list)
		{
			if (CurrentLayout->GetName() == name)
			{
				return CurrentLayout->GetJData();
			}
		}
	}

	return nullptr;
}

//Recursively go througj every block until we have gone through all of them and found the lowest one
void Layout::FindLowestHeight(const std::shared_ptr<BaseBlock>& currentBlock, int& lowestValue)
{
	if (currentBlock->CalculateBottomHeight() > lowestValue)
	{
		lowestValue = currentBlock->CalculateBottomHeight();
	}

	for (std::shared_ptr<BaseBlock> NextBlock : currentBlock->GetLinkedBlocks())
	{
		FindLowestHeight(NextBlock, lowestValue);
	}
}
