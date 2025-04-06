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
	const float aruco_size = 0.15f;
	cv::Mat frame;
	cv::VideoCapture cam (4);

	std::ofstream outFile("distance_1.txt");
	if (!outFile.is_open ())
	{
		std::cout << "Can't open file for writing.";
		return -2;
	}

	if (!cam.isOpened ())
	{
		std::cerr << "Error: can not open camera." << std::endl;
		return -1;
	}

	// çàãðóçêà ïàðàìåòðîâ êàìåðû.
	cv::Mat distCoefs;
	cv::Mat cameraMat;
	loadCamParams ("params.txt", cameraMat, distCoefs);

	outFile << "Distance" << std::endl;

	// îáðàáîòêà âèäåî, îáíàðóæåíèå ìàðêåðîâ.
	int cnt = 0;
	while (cnt < 10000)
	{
		if (!cam.read (frame))
		{
			std::cerr << "Error: can not read frame." << std::endl;
			return -2;
		}

		std::vector<int> markerIds;
		std::vector<std::vector<cv::Point2f>> markerCorners;
		cv::aruco::Dictionary markerDictionary = cv::aruco::getPredefinedDictionary (cv::aruco::DICT_7X7_50);
		cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters ();
		cv::aruco::ArucoDetector detector (markerDictionary, detectorParams);
		detector.detectMarkers (frame, markerCorners, markerIds);

		if (markerIds.size () == 1)
		{
			++cnt;
			std::vector<cv::Vec3d> tVectors, rVectors;
			cv::aruco::estimatePoseSingleMarkers (markerCorners, aruco_size, cameraMat, distCoefs, rVectors, tVectors);

			std::cout << "Marker was detected:" << std::endl;

			// Âûâîäèì ðàññòîÿíèå îò ìàðêåðà äî êàìåðû.
			double dist = sqrt (tVectors[0] (0) * tVectors[0] (0) + tVectors[0] (1) * tVectors[0] (1) + tVectors[0] (2) * tVectors[0] (2));
			std::cout << "Distance: " << dist << std::endl;

			outFile << dist << std::endl;
		} 
	}

	outFile.close();
	return 0;
}
