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

int main ()
{
	const float aruco_size = 0.04f;
	cv::Mat frame;
	//cv::VideoCapture cam("https://192.168.25.250:8080/video");
	cv::VideoCapture cam(0);

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
		cv::Ptr<cv::aruco::Dictionary> markerDictionary = cv::aruco::getPredefinedDictionary (cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);
		cv::aruco::detectMarkers (frame, markerDictionary, markerCorners, markerIds);
		
		if (markerIds.size () > 0)
		{
			cv::aruco::drawDetectedMarkers (frame, markerCorners, markerIds);

			std::vector<cv::Vec3d> tVectors, rVectors;
			cv::aruco::estimatePoseSingleMarkers (markerCorners, aruco_size, cameraMat, distCoefs, rVectors, tVectors);

			for (int j = 0; j < markerIds.size (); ++j)
			{
				cv::Point2f center = { (markerCorners[j][0].x + markerCorners[j][2].x) / 2.0f, (markerCorners[j][0].y + markerCorners[j][2].y) / 2.0f };

				double dist = sqrt(tVectors[j] (0) * tVectors[j] (0) + tVectors[j] (1) * tVectors[j] (1) + tVectors[j] (2) * tVectors[j] (2));
				cv::putText (frame, "dist: " + std::to_string (dist) + ",\t center: " + std::to_string (center.x) + " " + std::to_string (center.y), cv::Point (markerCorners[j][0].x, markerCorners[j][0].y - 20), cv::FONT_ITALIC, 0.35, cv::Scalar (255, 0, 255));

				cv::putText (frame, "X: " + std::to_string (tVectors[j] (0)), cv::Point (markerCorners[j][1].x + 10, markerCorners[j][1].y), cv::FONT_ITALIC, 0.35, cv::Scalar (0, 0, 255));
				cv::putText (frame, "Y: " + std::to_string (tVectors[j] (1)), cv::Point (markerCorners[j][1].x + 10, markerCorners[j][1].y + 20), cv::FONT_ITALIC, 0.35, cv::Scalar (0, 255, 0));
				cv::putText (frame, "Z: " + std::to_string (tVectors[j] (2)), cv::Point (markerCorners[j][1].x + 10, markerCorners[j][1].y + 40), cv::FONT_ITALIC, 0.35, cv::Scalar (255, 0, 0));

				cv::Mat rMatrix;
				cv::Rodrigues (rVectors[j], rMatrix);
				double beta = std::asin (rMatrix.at <double> (2, 0));
				double alpha = std::asin (rMatrix.at <double> (1, 0) / std::cos (beta));
				double gamma = std::asin (rMatrix.at <double> (2, 1) / std::cos (beta));

				cv::putText (frame, "Oyz (roll): " + std::to_string (gamma * 180 / M_PI), cv::Point (markerCorners[j][2].x + 10, markerCorners[j][2].y + 40), cv::FONT_ITALIC, 0.35, cv::Scalar (255, 0, 0));
				cv::putText (frame, "Oxz (pitch): " + std::to_string (beta * 180 / M_PI), cv::Point (markerCorners[j][2].x + 10, markerCorners[j][2].y), cv::FONT_ITALIC, 0.35, cv::Scalar (0, 0, 255));
				cv::putText (frame, "Oxy (yaw): " + std::to_string (alpha * 180 / M_PI), cv::Point (markerCorners[j][2].x + 10, markerCorners[j][2].y + 20), cv::FONT_ITALIC, 0.35, cv::Scalar (0, 255, 0));

				cv::aruco::drawAxis (frame, cameraMat, distCoefs, rVectors[j], tVectors[j], aruco_size * 1.5f);
			}
		}

		cv::putText (frame, "Number of markers: " + std::to_string (markerIds.size ()), cv::Point (10, frame.rows - 20), cv::FONT_ITALIC, 0.5, cv::Scalar (0, 0, 255));

		imshow ("Grayscale", frame);

		if (cv::waitKey (5) >= 0)
		{
			break;
		}
	 }

	return 0;
}