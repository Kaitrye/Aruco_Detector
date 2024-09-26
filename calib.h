#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>

#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

#ifndef CAM_CALIB_
#define CAM_CALIB_

extern const float aruco_size;
extern cv::Size chessSize;
extern const float lengthChess;

bool loadCamParams (const char* filename, cv::Mat& cameraMat, cv::Mat& distCoefs);
bool saveCamParams (const char* filename, const cv::Mat& cameraMat, const cv::Mat& distCoefs);
bool Calibration (unsigned char num_camera = 0, , const std::string& path = "./calib_photos");

#endif //CAM_CALIB_
