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

bool saveCamParams (const std::string& filename, const cv::Mat& cameraMat, const cv::Mat& distCoefs)
{
	std::ofstream outFile (filename);
	if (outFile)
	{
		// Матрица камеры.
		int rows = cameraMat.rows;
		int columns = cameraMat.cols;

		outFile << rows << std::endl;
		outFile << columns << std::endl;

		for (int row = 0; row < rows; row++)
		{
			for (int col = 0; col < columns; col++)
			{
				double value = cameraMat.at <double> (row, col);
				outFile << value << " ";
			}

			outFile << std::endl;
		}

		// Вектор коэффициентов.
		rows = distCoefs.rows;
		columns = distCoefs.cols;

		outFile << rows << std::endl;
		outFile << columns << std::endl;

		for (int row = 0; row < rows; row++)
		{
			for (int col = 0; col < columns; col++)
			{
				double value = distCoefs.at <double> (row, col);
				outFile << value << " ";
			}

			outFile << std::endl;
		}

		outFile.close ();

		return true;
	}

	return false;
}



int dmain ()
{
	cv::Mat frame;
	std::string indir = "chessboard";
	cv::Size chessSize = cv::Size (9, 6);
	const float lengthChess = 0.0265f;
	std::vector<cv::Mat> images;
	std::vector<std::vector<cv::Point2f>> corners;

	for (int i = 1; i <= 12; ++i)
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

		std::vector<cv::Point2f> foundPoints;
		bool found = findChessboardCorners(frame, chessSize, foundPoints, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
		images.push_back(frame);
		corners.push_back(foundPoints);

		/*drawChessboardCorners(frame, chessSize, foundPoints, found);
		imshow("Калибровка камеры", frame);
		cv::waitKey (0);*/
	}

	// Находим координаты углов шахматной доски в мировой системе. Проделываем поиск один раз и дублируем для каждого изображения.
	std::vector<std::vector<cv::Point3f>> chessCorners (1);
	for (int i = 0; i < chessSize.height; i++)
	{
		for (int j = 0; j < chessSize.width; j++)
		{
			chessCorners[0].push_back (cv::Point3f (j * lengthChess, i * lengthChess, 0.0f));
		}
	}
	chessCorners.resize (corners.size (), chessCorners[0]);

	std::vector<cv::Mat> rVectors;
	std::vector<cv::Mat> tVectors;
	cv::Mat distCoefs = cv::Mat::zeros (8, 1, CV_64F);
	cv::Mat cameraMat = cv::Mat::eye (3, 3, CV_64F);

	calibrateCamera (chessCorners, corners, chessSize, cameraMat, distCoefs, rVectors, tVectors);
	saveCamParams("params.txt", cameraMat, distCoefs);

	return 0;
}