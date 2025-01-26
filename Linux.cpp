#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/aruco.hpp>

#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include "calib.h"

#define _USE_MATH_DEFINES
#include <math.h>

int main (int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Wrong format. Input: \n\t Linux.cpp image_name" << std::endl;
		return 1;
	}

	const float aruco_size = 0.04f;
	cv::Mat frame;
	//cv::VideoCapture cam("https://192.168.25.250:8080/video");
	cv::VideoCapture cam (0);

	if (!cam.isOpened ())
	{
		std::cerr << "Error: can not open camera." << std::endl;
		return -1;
	}

	// загрузка параметров камеры.
	cv::Mat distCoefs;
	cv::Mat cameraMat;
	loadCamParams ("params.txt", cameraMat, distCoefs);

	// обработка видео, обнаружение маркеров.
	while (true)
	{
		if (!cam.read (frame))
		{
			std::cerr << "Error: can not read frame." << std::endl;
			return -2;
		}

		std::vector<int> markerIds;
		std::vector<std::vector<cv::Point2f>> markerCorners;
		cv::Ptr<cv::aruco::Dictionary> markerDictionary = cv::aruco::getPredefinedDictionary (cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_7X7_50);
		cv::aruco::detectMarkers (frame, markerDictionary, markerCorners, markerIds);

		if (markerIds.size () == 1)
		{
			cv::aruco::drawDetectedMarkers (frame, markerCorners, markerIds);

			std::vector<cv::Vec3d> tVectors, rVectors;
			cv::aruco::estimatePoseSingleMarkers (markerCorners, aruco_size, cameraMat, distCoefs, rVectors, tVectors);

			std::cout << "Marker was detected:" << std::endl;

			// Выводим расстояние от маркера до камеры.
			double dist = sqrt (tVectors[0] (0) * tVectors[0] (0) + tVectors[0] (1) * tVectors[0] (1) + tVectors[0] (2) * tVectors[0] (2));
			std::cout << "Distance: " << dist << std::endl;

			// Выводим координаты центра камеры.
			std::cout << "X: " + std::to_string (tVectors[0] (0)) << std::endl;
			std::cout << "Y: " + std::to_string (tVectors[0] (1)) << std::endl;
			std::cout << "Z: " + std::to_string (tVectors[0] (2)) << std::endl;

			cv::Mat rMatrix;
			cv::Rodrigues (rVectors[0], rMatrix);
			double beta = std::asin (rMatrix.at <double> (2, 0));
			double alpha = std::asin (rMatrix.at <double> (1, 0) / std::cos (beta));
			double gamma = std::asin (rMatrix.at <double> (2, 1) / std::cos (beta));

			// Выводим углы наклона камеры.
			std::cout << "Oyz (roll): " + std::to_string (gamma * 180 / M_PI) << std::endl;
			std::cout << "Oxz (pitch): " + std::to_string (beta * 180 / M_PI) << std::endl;
			std::cout << "Oxy (yaw): " + std::to_string (alpha * 180 / M_PI) << std::endl;

			cv::aruco::drawAxis (frame, cameraMat, distCoefs, rVectors[0], tVectors[0], aruco_size * 1.5f);

			imwrite (argv[1], frame);
			std::cout << "Image was saved." << std::endl;

			break;
		}
		else if (markerIds.size () > 1)
		{
			std::cout << "There are extra markers" << std::endl;
		}


		if (cv::waitKey (5) >= 0)
		{
			break;
		}
	}

	return 0;
}