#include "pch.h"
#include "CppUnitTest.h"
#include <fstream>
#include "../VideoImageGenerator/Header/Image.h"
#include "../VideoImageGenerator/Header/Font.h"
#include "../VideoImageGenerator/Header/Layout.h"
#include "../VideoImageGenerator/Header/ImageBlock.h"
#include "../VideoImageGenerator/Header/TextBlock.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
//To get the classes to be properly linked this has to be followed: https://learn.microsoft.com/en-us/visualstudio/test/how-to-use-microsoft-test-framework-for-cpp?view=vs-2022#object_files

namespace VideoImageGeneratorUnitTests
{
	TEST_CLASS(ImageUnitTests)
	{
	public:
		TEST_METHOD_INITIALIZE(SaveAnImage)
		{
			int Width = 4;
			int Height = 4;
			int Components = 4;

			std::shared_ptr<unsigned char> TestImageData(new unsigned char[Width * Height * Components]);
			for (int i = 0; i < Width * Height; i++)
			{
				TestImageData.get()[Components * i] = 255;
				TestImageData.get()[Components * i + 1] = 0;
				TestImageData.get()[Components * i + 2] = 0;
				TestImageData.get()[Components * i + 3] = 255;
			}

			Image RedSquare(TestImageData, Width, Height, Components);
			RedSquare.SaveImage("../../UnitTestImages/Test.png");
		}

		TEST_METHOD(EmptyImageConstructorTest)
		{
			int Width = 4;
			int Height = 4;
			Image Test(Width, Height);
			Pixel EmptyPixel;
			bool Correct = true;

			for (int i = 0; i < Test.GetWidth() * Test.GetHeight(); i++) 
			{
				if (Test.GetData().get()[i] != EmptyPixel)
				{
					Correct = false;
				}
			}
			Assert::IsTrue(Correct, L"The empty constructor didn't generate empty pixels");
		}

		TEST_METHOD(DataImageConstructorTest)
		{
			int Width = 4;
			int Height = 4;
			int Components = 4;
			std::shared_ptr<unsigned char> TestImageData(new unsigned char[Width * Height * Components]);
			for (int i = 0; i < Width * Height; i++)
			{
				TestImageData.get()[Components * i] = 255;
				TestImageData.get()[Components * i + 1] = 0;
				TestImageData.get()[Components * i + 2] = 0;
				TestImageData.get()[Components * i + 3] = 255;
			}

			Image Test(TestImageData, Width, Height, Components);
			Pixel RedPixel{ 1.0f, 0.0f, 0.0f, 1.0f };
			bool Correct = true;

			for (int i = 0; i < Test.GetWidth() * Test.GetHeight(); i++)
			{
				if (Test.GetData().get()[i] != RedPixel)
				{
					Correct = false;
				}
			}
			Assert::IsTrue(Correct, L"The constructor didn't generate pixels according to the given data");
		}

		TEST_METHOD(PathImageConstructorTest) 
		{
			Image Test("../../UnitTestImages/Test.png");
			Pixel RedPixel{ 1.0f, 0.0f, 0.0f, 1.0f };
			bool Correct = true;

			for (int i = 0; i < Test.GetWidth() * Test.GetHeight(); i++)
			{
				if (Test.GetData().get()[i] != RedPixel)
				{
					Correct = false;
				}
			}
			Assert::IsTrue(Correct, L"The constructor didn't generate the pixels according to the file given");
		}

		TEST_METHOD(CopyValueTest)
		{
			Image Test("../../UnitTestImages/Test.png");
			Image CopyTest;
			CopyTest.CopyValue(std::make_shared<Image>(Test));
			Pixel RedPixel{ 1.0f, 0.0f, 0.0f, 1.0f };
			bool Correct = true;

			for (int i = 0; i < CopyTest.GetWidth() * CopyTest.GetHeight(); i++)
			{
				if (CopyTest.GetData().get()[i] != RedPixel)
				{
					Correct = false;
				}
			}
			Assert::IsTrue(Correct, L"The data wasn't properly copied");
		}

		TEST_METHOD(ImageGetWidthTest) 
		{
			Image Test("../../UnitTestImages/Test.png");
			Assert::AreEqual(Test.GetWidth(), 4, L"Didn't get the correct width");
		}

		TEST_METHOD(ImageGetHeightTest)
		{
			Image Test("../../UnitTestImages/Test.png");
			Assert::AreEqual(Test.GetHeight(), 4, L"Didn't get the correct height");
		}

		TEST_METHOD(ResizeImageTest)
		{
			Image Test("../../UnitTestImages/Test.png");
			Test.ResizeImage(8, 4);
			Assert::AreEqual(Test.GetWidth(), 8, L"Didn't resize to the correct width");
			Assert::AreEqual(Test.GetHeight(), 4, L"Didn't resize to the correct height");
		}

		TEST_METHOD(ScaleImageTest)
		{
			Image Test("../../UnitTestImages/Test.png");
			Test.ScaleImage(2.0f);
			Assert::AreEqual(Test.GetWidth(), 8, L"Didn't scale to the correct width");
			Assert::AreEqual(Test.GetHeight(), 8, L"Didn't scale to the correct height");
		}

		TEST_METHOD(ResizeImageHeightTest)
		{
			Image Test("../../UnitTestImages/Test.png");
			Test.ResizeImageWidth(8);
			Assert::AreEqual(Test.GetWidth(), 8, L"Didn't resize to the correct width");
			Assert::AreEqual(Test.GetHeight(), 8, L"Didn't resize to the correct height");
		}

		TEST_METHOD(ResizeImageWidthTest)
		{
			Image Test("../../UnitTestImages/Test.png");
			Test.ResizeImageWidth(8);
			Assert::AreEqual(Test.GetWidth(), 8, L"Didn't resize to the correct width");
			Assert::AreEqual(Test.GetHeight(), 8, L"Didn't resize to the correct height");
		}

		TEST_METHOD(EraseImageSectionTest)
		{
			//We erase the bottom half and check if it is still empty
			Image Test("../../UnitTestImages/Test.png");
			Test.EraseImageSection(4, 2, 0, 2);
			Pixel RedPixel{ 1.0f, 0.0f, 0.0f, 1.0f };
			Pixel EmptyPixel;
			bool Correct = true;

			for (int i = 0; i < Test.GetWidth() * Test.GetHeight(); i++)
			{
				if (i < 8) 
				{
					if (Test.GetData().get()[i] != RedPixel)
					{
						Correct = false;
					}
				}
				else 
				{
					if (Test.GetData().get()[i] != EmptyPixel)
					{
						Correct = false;
					}
				}
			}
			Assert::IsTrue(Correct, L"The section wasn't properly erased");
		}

		TEST_METHOD(CopyImageSectionTest)
		{
			//We erase the bottom half of the image and than copy a section in the enter to check
			Image Test("../../UnitTestImages/Test.png");
			Test.ResizeImageWidth(8);
			Test.EraseImageSection(4, 2, 0, 2);
			std::shared_ptr<Image> CopiedSection = Test.CopyImageSection(2, 2, 1, 1);

			Pixel RedPixel{ 1.0f, 0.0f, 0.0f, 1.0f };
			Pixel EmptyPixel;
			bool Correct = true;
			for (int i = 0; i < CopiedSection->GetWidth() * CopiedSection->GetHeight(); i++)
			{
				if (i < 2)
				{
					if (CopiedSection->GetData().get()[i] != RedPixel)
					{
						Correct = false;
					}
				}
				else
				{
					if (CopiedSection->GetData().get()[i] != EmptyPixel)
					{
						Correct = false;
					}
				}
			}
			Assert::IsTrue(Correct, L"The section wasn't properly copied");
		}

		TEST_METHOD(ChangeColorTest)
		{
			//emove the bottom half of the image before changing the color to see if it properly works with the alpha channel
			Image Test("../../UnitTestImages/Test.png");
			Pixel GreenPixel{ 0.0f, 1.0f, 0.0f, 1.0f };
			Pixel InvisibleGreenPixel{ 0.0f, 1.0f, 0.0f, 0.0f};

			Test.EraseImageSection(4, 2, 0, 2);
			Test.ChangeColor(std::make_shared<Pixel>(GreenPixel));

			bool Correct = true;
			for (int i = 0; i < Test.GetWidth() * Test.GetHeight(); i++)
			{
				if (i < 8)
				{
					if (Test.GetData().get()[i] != GreenPixel)
					{
						Correct = false;
					}
				}
				else
				{
					if (Test.GetData().get()[i] != InvisibleGreenPixel)
					{
						Correct = false;
					}
				}
			}
			Assert::IsTrue(Correct, L"The section wasn't properly erased");
		}

		TEST_METHOD(ChangeColorAlphaTest)
		{
			Image Test("../../UnitTestImages/Test.png");
			Pixel GreenPixel{ 0.0f, 1.0f, 0.0f, 1.0f };

			Test.EraseImageSection(4, 2, 0, 2);
			Test.ChangeColor(std::make_shared<Pixel>(GreenPixel), true);
			bool Correct = true;

			for (int i = 0; i < Test.GetWidth() * Test.GetHeight(); i++)
			{
				if (Test.GetData().get()[i] != GreenPixel)
				{
					Correct = false;
				}
			}
			Assert::IsTrue(Correct, L"The color didn't properly get changed");
		}

		TEST_METHOD(CompositeImageTest)
		{
			Image Test("../../UnitTestImages/Test.png");
			int Width = 4;
			int Height = 2;
			int Components = 4;

			std::shared_ptr<unsigned char> TestImageData(new unsigned char[Width * Height * Components]);
			for (int i = 0; i < Width * Height; i++)
			{
				TestImageData.get()[Components * i] = 0;
				TestImageData.get()[Components * i + 1] = 255;
				TestImageData.get()[Components * i + 2] = 0;
				TestImageData.get()[Components * i + 3] = 255;
			}
			Image GreenRectangle(TestImageData, Width, Height, Components);

			Test.CompositeImage(std::make_shared<Image>(GreenRectangle), 0, 2);
			Pixel RedPixel{ 1.0f, 0.0f, 0.0f, 1.0f };
			Pixel GreenPixel{ 0.0f, 1.0f, 0.0f, 1.0f };
			bool Correct = true;

			for (int i = 0; i < Test.GetWidth() * Test.GetHeight(); i++)
			{
				if (i < 8)
				{
					if (Test.GetData().get()[i] != RedPixel)
					{
						Correct = false;
					}
				}
				else
				{
					if (Test.GetData().get()[i] != GreenPixel)
					{
						Correct = false;
					}
				}
			}
			Assert::IsTrue(Correct, L"The images didn't composite together");
		}
	};

	TEST_CLASS(FontUnitTests)
	{
	public:
		TEST_METHOD(GetCharacterImageTest)
		{
			Image Character("../../UnitTestImages/ExpectedResults/ExpectedFontCharacterImage.png");
			std::shared_ptr<Font> TestFont(new Font("C:/Windows/Fonts/arial.ttf"));
			Assert::IsTrue(Character == TestFont->GetCharacterImage("A", 128).get()[0], L"The images weren't the same");
		}

		TEST_METHOD(GetTextImageTest)
		{
			Image Text("../../UnitTestImages/ExpectedResults/ExpectedFontTextImage.png");
			std::shared_ptr<Font> TestFont(new Font("C:/Windows/Fonts/arial.ttf"));
			Assert::IsTrue(Text == TestFont->GetTextImage("Paradise Lost", 128).get()[0], L"The images weren't the same");
		}

		TEST_METHOD(GetStringLengthTest)
		{
			Image Text("../../UnitTestImages/ExpectedResults/ExpectedFontTextImage.png");
			std::shared_ptr<Font> TestFont(new Font("C:/Windows/Fonts/arial.ttf"));
			Assert::AreEqual(Text.GetWidth(), TestFont->GetStringLength("Paradise Lost", 128), L"The images aren't of the same length");
		}
	};

	TEST_CLASS(layoutUnitTests)
	{
	public:
		TEST_METHOD_INITIALIZE(SaveAnImage)
		{
			Image RedSquare(100, 100);
			RedSquare.ChangeColor(std::shared_ptr<Pixel>(new Pixel{ 1.0f, 0.0f, 0.0f, 1.0f }), true);
			RedSquare.SaveImage("../../UnitTestImages/Red.png");

			Image BlueRectangle(100, 50);
			BlueRectangle.ChangeColor(std::shared_ptr<Pixel>(new Pixel{ 0.0f, 0.0f, 1.0f, 1.0f }), true);
			BlueRectangle.SaveImage("../../UnitTestImages/Blue.png");

			Image GreenRectangle(50, 25);
			GreenRectangle.ChangeColor(std::shared_ptr<Pixel>(new Pixel{ 0.0f, 1.0f, 0.0f, 1.0f }), true);
			GreenRectangle.SaveImage("../../UnitTestImages/Green.png");
		}

		TEST_METHOD(InitializeTest) 
		{
			std::ifstream File("../../UnitTestImages/ExpectedResults/LayoutUnitTests.json");
			nlohmann::json Data = nlohmann::json::parse(File);
			std::shared_ptr<Layout> Test(new Layout(Data.at("InitializeTest")));

			Image Original("../../UnitTestImages/Red.png");
			Image LayoutGenerated("../../UnitTestImages/InitializeTest.png");
			Assert::IsTrue(Original == LayoutGenerated, L"The layout didn't save an image of just the background");
		}

		TEST_METHOD(ImageBlockTest)
		{
			std::ifstream File("../../UnitTestImages/ExpectedResults/LayoutUnitTests.json");
			nlohmann::json Data = nlohmann::json::parse(File);
			std::shared_ptr<Layout> Test(new Layout(Data.at("ImageBlockTest")));

			Image Original("../../UnitTestImages/ExpectedResults/ExpectedImageBlockTest.png");
			Image LayoutGenerated("../../UnitTestImages/ImageBlockTest.png");
			Assert::IsTrue(Original == LayoutGenerated, L"The layout didn't composite the image blocks properly");
		}

		TEST_METHOD(TextBlockTest)
		{
			std::ifstream File("../../UnitTestImages/ExpectedResults/LayoutUnitTests.json");
			nlohmann::json Data = nlohmann::json::parse(File);
			std::shared_ptr<Layout> Test(new Layout(Data.at("TextBlockTest")));

			Image Original("../../UnitTestImages/ExpectedResults/ExpectedTextBlockTest.png");
			Image LayoutGenerated("../../UnitTestImages/TextBlockTest.png");
			Assert::IsTrue(Original == LayoutGenerated, L"The layout didn't composite the text blocks properly");
		}

		TEST_METHOD(SnapBlockTest)
		{
			std::ifstream File("../../UnitTestImages/ExpectedResults/LayoutUnitTests.json");
			nlohmann::json Data = nlohmann::json::parse(File);
			std::shared_ptr<Layout> Test(new Layout(Data.at("SnapBlockTest")));

			Image Original("../../UnitTestImages/ExpectedResults/ExpectedSnapBlockTest.png");
			Image LayoutGenerated("../../UnitTestImages/SnapBlockTest.png");
			Assert::IsTrue(Original == LayoutGenerated, L"The layout didn't snap the blocks properly");
		}

		TEST_METHOD(PotentialLayoutTest)
		{
			std::ifstream File("../../UnitTestImages/ExpectedResults/LayoutUnitTests.json");
			nlohmann::json Data = nlohmann::json::parse(File);
			std::shared_ptr<Layout> Test(new Layout(Data.at("PotentialLayoutTest")));

			Image OriginalLeft("../../UnitTestImages/ExpectedResults/ExpectedSnapBlockTest.png");
			Image LayoutGeneratedLeft("../../UnitTestImages/PotentialLayoutTestLeft.png");
			
			Image OriginalRight("../../UnitTestImages/ExpectedResults/ExpectedPotentialLayoutTestRight.png");
			Image LayoutGeneratedRight("../../UnitTestImages/PotentialLayoutTestRight.png");
			Assert::IsTrue(OriginalLeft == LayoutGeneratedLeft && OriginalRight == LayoutGeneratedRight, L"The layout didn't composite the text blocks properly");
		}

		TEST_METHOD(OverrideTest)
		{
			std::ifstream File("../../UnitTestImages/ExpectedResults/LayoutUnitTests.json");
			nlohmann::json Data = nlohmann::json::parse(File);
			std::shared_ptr<Layout> Test(new Layout(Data.at("OverrideTest")));

			Image OriginalOverride("../../UnitTestImages/ExpectedResults/ExpectedOverrideOverrideTest.png");
			Image LayoutGeneratedOverride("../../UnitTestImages/OverrideOverrideTest.png");

			Image Original("../../UnitTestImages/ExpectedResults/ExpectedOverrideTest.png");
			Image LayoutGenerated("../../UnitTestImages/OverrideTest.png");
			Assert::IsTrue(OriginalOverride == LayoutGeneratedOverride && Original == LayoutGenerated, L"The layout didn't composite the text blocks properly");
		}
	};
}
