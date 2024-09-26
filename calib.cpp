#include "calib.h"

const float aruco_size = 0.04f;
cv::Size chessSize = cv::Size (9, 6);
const float lengthChess = 0.0265f;

bool loadCamParams (const char* filename, cv::Mat& cameraMat, cv::Mat& distCoefs)
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
			}
		}

		inFile.close ();

		return true;
	}

	return false;
}

bool saveCamParams (const char* filename, const cv::Mat& cameraMat, const cv::Mat& distCoefs)
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
				outFile << value << std::endl;
			}
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
				outFile << value << std::endl;
			}
		}

		outFile.close ();

		return true;
	}

	return false;
}

bool Calibration (unsigned char num_camera, const std::string& path)
{
	std::vector <std::vector <cv::Point2f>> corners;

	cv::Mat frame;
	for (fs::directory_iterator cur (path), end; cur != end; ++cur)
	{
		std::string filename = cur -> path().string();
		frame = cv::imread(filename, cv::IMREAD_COLOR);

		if (frame.empty ())
		{
			return false;
		}

		std::vector <cv::Point2f> foundPoints;
		bool found = cv::findChessboardCorners (frame, chessSize, foundPoints, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
		corners.push_back (foundPoints);
	}

	// Определяем координаты углов доски в мировой системе координат.
	std::vector<cv::Point3f> chessCorners;
	for (int i = 0; i < chessSize.height; i++)
	{
		for (int j = 0; j < chessSize.width; j++)
		{
			chessCorners.push_back (cv::Point3f (j * lengthChess, i * lengthChess, 0.0f));
		}
	}

	std::vector <std::vector<cv::Point3f>> vec_chessCorners (corners.size());
	std::fill (vec_chessCorners.begin(), vec_chessCorners.end(), chessCorners);

	std::vector <cv::Mat> rVectors, tVectors;
	cv::Mat distCoefs, cameraMat;

	cv::calibrateCamera (vec_chessCorners, corners, chessSize, cameraMat, distCoefs, rVectors, tVectors);
	saveCamParams ("params.txt", cameraMat, distCoefs);

	return true;
}