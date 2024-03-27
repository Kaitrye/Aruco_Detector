#include<opencv2/core.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/aruco.hpp>
#include<opencv2/calib3d.hpp>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>

bool loadCamParams (const std::string& filename, cv::Mat& cameraMat, cv::Mat& distCoefs)
{
	std::ifstream inFile (filename);

	if (inFile)
	{
		int rows;
		int columns;

		// Матрица камеры.
		inFile >> rows;
		inFile >> columns;

		cameraMat = cv::Mat (cv::Size (columns, rows), CV_64F);

		for (int row = 0; row < rows; row++)
		{
			for (int col = 0; col < columns; col++)
			{
				double num;
				inFile >> num;
				cameraMat.at <double> (row, col) = num;
				std::cout << cameraMat.at <double> (row, col) << "\n";
			}
		}

		// Вектор коэффициентов.
		inFile >> rows;
		inFile >> columns;

		distCoefs = cv::Mat::zeros (rows, columns, CV_64F);

		for (int row = 0; row < rows; row++)
		{
			for (int col = 0; col < columns; col++)
			{
				double num;;
				inFile >> num;
				distCoefs.at <double> (row, col) = num;
				std::cout << distCoefs.at <double> (row, col) << "\n";
			}
		}

		inFile.close ();

		return true;
	}

	return false;
}

int main ()
{
	const char* filename = "sizes2.txt";
	std::ofstream outFile (filename);
	if (!outFile)
	{
		std::cout << "Can not open file " << filename << std::endl;
		return 1;
	}

	cv::Mat distCoefs = cv::Mat::zeros (8, 1, CV_64F);
	cv::Mat cameraMat = cv::Mat::eye (3, 3, CV_64F);
	const double aruco_size = 0.033f;

	loadCamParams("params.txt", cameraMat, distCoefs);

	cv::Mat frame;
	std::string indir = "Exp_1";

	for (int i = 1; i <= 10; ++i)
	{
		std::string filename = "IMG (" + std::to_string (i) + ").jpg"; // имя изображения.
		std::string path = indir + "/" + filename;

		frame = cv::imread (path, cv::IMREAD_COLOR);
		if (frame.empty ())
		{
			std::cout << "Error: " << filename << std::endl;
			return 2;
		}

		int new_height = frame.rows / 4;
		int new_width = frame.cols / 4;

		cv::resize (frame, frame, cv::Size (new_width, new_height), cv::INTER_LINEAR);

		// обнаружение аруко-маркеров.
		std::vector<int> markerIds;
		std::vector<std::vector<cv::Point2f>> markerCorners;
		std::vector<cv::Ptr<cv::aruco::Dictionary>> array_dict (4);

		array_dict[0] = cv::aruco::getPredefinedDictionary (cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_250);
		array_dict[1] = cv::aruco::getPredefinedDictionary (cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_5X5_250);
		array_dict[2] = cv::aruco::getPredefinedDictionary (cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_6X6_250);
		array_dict[3] = cv::aruco::getPredefinedDictionary (cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_7X7_250);

		int j = 0;
		cv::aruco::detectMarkers (frame, array_dict[j], markerCorners, markerIds);
		while (markerIds.size () == 0 && ++j < 4)
		{
			cv::aruco::detectMarkers (frame, array_dict[j], markerCorners, markerIds);
		}
		
		if (markerIds.size () > 0)
		{
			std::vector<cv::Vec3d> tVectors, rVectors;
			cv::aruco::estimatePoseSingleMarkers (markerCorners, aruco_size, cameraMat, distCoefs, rVectors, tVectors);
			cv::aruco::drawDetectedMarkers (frame, markerCorners, markerIds);
			cv::aruco::drawAxis (frame, cameraMat, distCoefs, rVectors[0], tVectors[0], 0.05f);
			double x = tVectors[0] (0);
			double y = tVectors[0] (1);
			double z = tVectors[0] (2);

			outFile << filename << ": " << std::sqrt (x * x + y * y + z * z) << std::endl;
			outFile << "x: " << x << std::endl;
			outFile << "y: " << y << std::endl;
			outFile << "z: " << z << std::endl;
			outFile << std::endl;
		}
		
		imshow ("Маркеры и оси", frame);
		cv::waitKey (0);
	}

	outFile.close();
	return 0;
}