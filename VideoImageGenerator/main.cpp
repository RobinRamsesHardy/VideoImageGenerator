#include <iostream>
#include "Header/Layout.h"
#include "Header/Font.h"
#include <fstream>

int main() 
{
	std::string Filepath = "";
	bool bCorrectFilepath = false;

	printf("Enter the filepath to start the generation.\n");
	while (bCorrectFilepath == false) 
	{
		std::cin >> Filepath;
		if (Filepath.size() > 5) 
		{
			if (Filepath.substr(Filepath.size() - 5) == ".json")
			{
				bCorrectFilepath = true;
			}
			else
			{
				printf("The file must be a .json.\n");
			}
		}
		else 
		{
			printf("Incorrect filepath try again.\n");
		}
	}

	std::shared_ptr<Layout> LayoutTest(new Layout(nlohmann::json::parse(std::ifstream(Filepath))));
	return 0;
}