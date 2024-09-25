#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/aruco.hpp>
#include<opencv2/calib3d.hpp>

#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;


int main ()
{
	char result[512];
	std::cout << "Enter filename for result: ";
	std::cin.getline (result, 512);

	std::ofstream outFile (result);
	if (!outFile)
	{
		std::cout << "Can not open file " << result << std::endl;
		return -1;
	}

	std::vector <double> prob(20);

	std::string path;
	std::cout << "Enter name of directory: ";
	getline(std::cin, path);

	fs::directory_iterator dir (path);

	cv::Mat frame;
	for (int i = 0; i < 50; ++i)
	{
		for (int j = 0; j < 20; ++j, ++dir)
		{
			std::string filename = dir->path ().string ();

			frame = cv::imread (filename, cv::IMREAD_COLOR);
			if (frame.empty ())
			{
				std::cout << "Error of reading: " << filename << std::endl;
				return 2;
			}

			std::vector<int> markerIds;
			std::vector<std::vector<cv::Point2f>> markerCorners;
			cv::Ptr<cv::aruco::Dictionary> array_dict = cv::aruco::getPredefinedDictionary (cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_7X7_250);

			cv::aruco::detectMarkers (frame, array_dict, markerCorners, markerIds);

			if (markerIds.size () > 0)
			{
				if (markerIds[0] != i)
				{
					std::cout << "NO" << " " << markerIds[0] << " " << i << std::endl;
					return -2;
				}

				++prob[j];
			}

			std::cout << "Process: marker " << i << ", distance " << j << std::endl;
		}
	}

	std::cout << "Probability: " << std::endl;
	for (int i = 0; i < 19; ++i)
	{
		prob[i] /= 50.;

		if (i > 0)
		{
			outFile << ", ";
			std::cout << ", ";
		}

		outFile << prob[i];
		std::cout << prob[i];
	}

	outFile.close();

	return 0;
}