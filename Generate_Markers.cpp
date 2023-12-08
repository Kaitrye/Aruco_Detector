
#include <iostream>
#include <sstream>
#include <ctime>

#include <opencv2/aruco.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


int main ()
{
	cv::Mat outputMarker;

	cv::Ptr<cv::aruco::Dictionary> markerDictionary = cv::aruco::getPredefinedDictionary (cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_7X7_250);

	srand (time (0));

	for (int i = 0; i < 4; ++i)
	{
		int index = rand () % 250;
		cv::aruco::drawMarker (markerDictionary, index, 750, outputMarker, 1);
		std::ostringstream convert;
		std::string imageName = "7x7Marker_";
		convert << imageName << index << ".jpg";
		imwrite (convert.str (), outputMarker);
	}

	return 0;
}