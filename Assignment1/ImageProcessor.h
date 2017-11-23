#pragma once
#include "opencv2\core\core.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\highgui\highgui.hpp"
#include <iostream>
using namespace std;
using namespace cv;


class ImageProcessor {
private:
	static const int _MAX_ANGLE;
	static const int _MAX_KERNEL_SIZE;
	int currentChannel = -1;
	string imageName = "";
	Mat originalFrame, currentFrame;
	VideoCapture vc;
	bool useOpenCV = true;
	bool useGrayscale = false;
	bool useChannel = false;
	bool rotateFlag = false;
	bool smoothFlag = false;
	bool deriveXFilterFlag = false;
	bool deriveYFilterFlag = false;
	bool magnitudeGradient = false;
	bool trackBarOn = false;
	int rotateAngle = 0, filterKernelSz = 0;
	void createTrackBar(string flag);
	//Mat performConvolution(Mat src, Mat filter);
public:
	ImageProcessor();
	ImageProcessor(VideoCapture vc);
	ImageProcessor(string imageName);
	void process(char cmd);
	void showHelp();
	bool isOpenCamera();
	void processCamera();
	void processImage();
	Mat cycleThroughChannel();
	Mat convertToGrayscaleFrame(Mat frame, bool useOpenCV = true);
	Mat applyFilter();
	Mat rotateImage(Mat frame, double angle);
	Mat smoothImage(Mat frame, bool useOpenCV = true);
	Mat computeDerivativeX(Mat frame);
	Mat computeDerivativeY(Mat frame);
	Mat computerDerivative(Mat frame);
	void reset();
	//void onTrackBarRotateProcess(int, void*);
};