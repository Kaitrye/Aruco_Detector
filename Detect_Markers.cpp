#include <iostream>
#include <sstream>

#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


int main()
{
	// Считывание изображения.
	cv::Mat frame;
	
	const int count = 185; 

	for (int i = 1; i <= count; ++i)
	{
		std::ostringstream filename, output;
		filename << "IMG (" << i << ").jpg";

		frame = cv::imread(filename.str(), cv::IMREAD_COLOR);
		if (frame.empty ())
		{
			std::cout << "Error" << filename.str() << std::endl;
			return 1;
		}

		// Изменение размеров изображения.
		int new_height = frame.rows / 4;
		int new_width = frame.cols / 4;

		cv::resize (frame, frame, cv::Size (new_width, new_height), cv::INTER_LINEAR);

		// Обнаружение аруко-маркеров.
		std::vector<int> markerIds;
		std::vector<std::vector<cv::Point2f>> markerCorners;
		cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary (cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_5X5_250);
		cv::aruco::detectMarkers (frame, dictionary, markerCorners, markerIds);
		cv::aruco::drawDetectedMarkers (frame, markerCorners, markerIds);

		// Демонстрация либо сохранение изображения.
		output << "Out7/";
		output << markerIds.size() << '_';
		if (markerIds.size () > 0)
		{
			output << markerIds[0];
			for (int j = 1; j < markerIds.size (); ++j)
			{
				output << '-' << markerIds[j];
			}
			output << '_';
		}
		
		output << "IMG (" << i << ").jpg";

		// Сохранение в файл.
		imwrite(output.str(), frame);

		// Вывод изображения на экран.
		cv::namedWindow ("First image", cv::WINDOW_AUTOSIZE);
		imshow("First image", frame);
		cv::waitKey (0);
	}

	return 0;
}