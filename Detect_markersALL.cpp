#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


int main ()
{
	const char* filename = "result.txt";
	std::ofstream outFile (filename);
	if (!outFile)
	{
		std::cout << "Can not open file " << filename << std::endl;
		return 1;
	}

	// Считывание изображения.
	cv::Mat frame;
	const int count = 2540;
	std::string indir = "dataset";
	std::string outdir = "result";

	for (int i = 1; i <= count; ++i)
	{
		std::string filename = "IMG_" + std::to_string(i) + ".jpg"; // имя изображения.
		std::string path = indir + "/" + filename;

		frame = cv::imread (path, cv::IMREAD_COLOR);
		if (frame.empty ())
		{
			std::cout << "Error: " << filename << std::endl;
			return 2;
		}

		// Изменение размеров изображения.
		int new_height = frame.rows / 4;
		int new_width = frame.cols / 4;

		cv::resize (frame, frame, cv::Size (new_width, new_height), cv::INTER_LINEAR);

		// Обнаружение аруко-маркеров.
		std::vector<int> markerIds;
		std::vector<std::vector<cv::Point2f>> markerCorners;
		std::vector<cv::Ptr<cv::aruco::Dictionary>> array_dict(4);

		array_dict[0] = cv::aruco::getPredefinedDictionary (cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_250);
		array_dict[1] = cv::aruco::getPredefinedDictionary (cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_5X5_250);
		array_dict[2] = cv::aruco::getPredefinedDictionary (cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_6X6_250);
		array_dict[3] = cv::aruco::getPredefinedDictionary (cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_7X7_250);

		int j = 0;
		cv::aruco::detectMarkers (frame, array_dict[j], markerCorners, markerIds);
		while (markerIds.size() == 0 && ++j < 4)
		{
			cv::aruco::detectMarkers (frame, array_dict[j], markerCorners, markerIds);
		}
		cv::aruco::drawDetectedMarkers (frame, markerCorners, markerIds);

		// Сохранение в файл.
		std::string answer = (markerIds.size() > 0) ? "detected " + std::to_string(markerIds[0]) + " marker in DICT_"
			+ std::to_string(j + 4) + "X" + std::to_string (j + 4) + "_250" : "not detected";

		outFile << filename << " - " << answer << std::endl;
		std::string output = outdir + "/" + filename;

		imwrite (output, frame);
	}

	outFile.close();

	return 0;
}