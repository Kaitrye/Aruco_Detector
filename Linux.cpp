#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/objdetect/aruco_detector.hpp> 

#include <wiringSerial.h>

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

	int serial = serialOpen ("/dev/ttyUSB0", 9600);
	if (serial < 0)
	{
	    std::cout << "Unable to open serial device" << std::endl;
	    return 2;
	}

	const float aruco_size = 0.04f;
	cv::Mat frame;
	cv::VideoCapture cam (4);

	if (!cam.isOpened ())
	{
		std::cerr << "Error: can not open camera." << std::endl;
		return -1;
	}

	// çàãðóçêà ïàðàìåòðîâ êàìåðû.
	cv::Mat distCoefs;
	cv::Mat cameraMat;
	loadCamParams ("params.txt", cameraMat, distCoefs);

	// îáðàáîòêà âèäåî, îáíàðóæåíèå ìàðêåðîâ.
	while (true)
	{
		if (!cam.read (frame))
		{
			std::cerr << "Error: can not read frame." << std::endl;
			return -2;
		}

		std::vector<int> markerIds;
		std::vector<std::vector<cv::Point2f>> markerCorners;
		cv::aruco::Dictionary markerDictionary = cv::aruco::getPredefinedDictionary (cv::aruco::DICT_7X7_50);
		cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
		cv::aruco::ArucoDetector detector(markerDictionary, detectorParams);
		detector.detectMarkers (frame, markerCorners, markerIds);

		if (markerIds.size () == 1)
		{
			serialPutchar(serial, '1');
			cv::aruco::drawDetectedMarkers (frame, markerCorners, markerIds);

			std::vector<cv::Vec3d> tVectors, rVectors;
			cv::aruco::estimatePoseSingleMarkers (markerCorners, aruco_size, cameraMat, distCoefs, rVectors, tVectors);

			std::cout << "Marker was detected:" << std::endl;

			// Âûâîäèì ðàññòîÿíèå îò ìàðêåðà äî êàìåðû.
			double dist = sqrt (tVectors[0] (0) * tVectors[0] (0) + tVectors[0] (1) * tVectors[0] (1) + tVectors[0] (2) * tVectors[0] (2));
			std::cout << "Distance: " << dist << std::endl;

			// Âûâîäèì êîîðäèíàòû öåíòðà êàìåðû.
			std::cout << "X: " + std::to_string (tVectors[0] (0)) << std::endl;
			std::cout << "Y: " + std::to_string (tVectors[0] (1)) << std::endl;
			std::cout << "Z: " + std::to_string (tVectors[0] (2)) << std::endl;

			cv::Mat rMatrix;
			cv::Rodrigues (rVectors[0], rMatrix);
			double beta = std::asin (rMatrix.at <double> (2, 0));
			double alpha = std::asin (rMatrix.at <double> (1, 0) / std::cos (beta));
			double gamma = std::asin (rMatrix.at <double> (2, 1) / std::cos (beta));

			// Âûâîäèì óãëû íàêëîíà êàìåðû.
			std::cout << "Oyz (roll): " + std::to_string (gamma * 180 / M_PI) << std::endl;
			std::cout << "Oxz (pitch): " + std::to_string (beta * 180 / M_PI) << std::endl;
			std::cout << "Oxy (yaw): " + std::to_string (alpha * 180 / M_PI) << std::endl;

			cv::drawFrameAxes (frame, cameraMat, distCoefs, rVectors[0], tVectors[0], aruco_size * 1.5f, 2);

			//imwrite (argv[1], frame);
			//std::cout << "Image was saved." << std::endl;
		}
		else if (markerIds.size () > 1)
		{
			std::cout << "There are extra markers" << std::endl;
		}
		else {
			serialPutchar(serial, '0');
		}
	}

	return 0;
}
