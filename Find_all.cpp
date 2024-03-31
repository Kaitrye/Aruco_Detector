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
#define _USE_MATH_DEFINES
#include <math.h>

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
	const char* result = "characters.txt";
	std::ofstream outFile (result);
	if (!outFile)
	{
		std::cout << "Can not open file " << result << std::endl;
		return 1;
	}

	const char* sizes = "currentASize.txt";
	std::ifstream inFile (sizes);
	if (!inFile)
	{
		std::cout << "Can not open file " << sizes << std::endl;
		return 2;
	}

	cv::Mat distCoefs = cv::Mat::zeros (8, 1, CV_64F);
	cv::Mat cameraMat = cv::Mat::eye (3, 3, CV_64F);
	const double aruco_size[4] = {0.033f, 0.076f, 0.105f, 0.156f};

	loadCamParams("params.txt", cameraMat, distCoefs);

	cv::Mat frame;
	std::string indir = "dataset";
	std::string outdir = "axis";
	size_t ar_index;

	for (int i = 1; i <= 2540; ++i)
	{
		std::string filename = "IMG_" + std::to_string (i) + ".jpg"; // имя изображения.
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

		inFile >> ar_index;

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
			cv::aruco::estimatePoseSingleMarkers (markerCorners, aruco_size[ar_index], cameraMat, distCoefs, rVectors, tVectors);
			cv::aruco::drawDetectedMarkers (frame, markerCorners, markerIds);
			cv::aruco::drawAxis (frame, cameraMat, distCoefs, rVectors[0], tVectors[0], 0.05f);
			
			cv::Mat rMatrix;
			cv::Rodrigues(rVectors[0], rMatrix);

			/*std::cout << filename << "\nMatrix:\n";

			for (int row = 0; row < 3; ++row)
			{
				for (int col = 0; col < 3; ++col)
				{
					std::cout << rMatrix.at <double> (row, col) << " ";
				}
				std::cout << "\n";
			}*/

			double beta = std::asin(rMatrix.at <double> (2, 0));
			double alpha = std::asin(rMatrix.at <double> (1, 0) / std::cos(beta));
			double gamma = std::asin(rMatrix.at <double> (2, 1) / std::cos(beta));
			double z = tVectors[0] (2);

			/**std::cout << "Angles:\n";
			std::cout << gamma * 180 / M_PI << "\n" << beta * 180 / M_PI << "\n" << alpha * 180 / M_PI << std::endl;

			std::cout << "Size: " << z << std::endl;*/

			outFile << i << "," << z << "," << gamma * 180 / M_PI << "," << beta * 180 / M_PI << "," << alpha * 180 / M_PI << "," << 1 << "," << markerIds[0] << "," << aruco_size[ar_index] << std::endl;
		}
		else
		{
			outFile << i << "," << 0 << "," << 0 << "," << 0 << "," << 0 << "," << 0 << "," << 0 << "," << aruco_size[ar_index] << std::endl;
		}

		std::string output = outdir + "/" + filename;

		imwrite (output, frame);
		
		/*imshow ("Маркеры и оси", frame);
		cv::waitKey (0);*/
	}

	outFile.close();
	return 0;
}
