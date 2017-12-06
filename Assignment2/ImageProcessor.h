#pragma once
#include <iostream>
#include <map>
#include <algorithm>
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\xfeatures2d.hpp"
#include <stdlib.h>
using namespace std;
using namespace cv;
using namespace xfeatures2d;

class ImageProcessor {
private:
	enum DETECTOR_TYPE {HARRIS, BLOB, DOG, M_SIFT, LBP, MATCHING, HELP};
	int cntFrame = 0, detector = -1, descriptor = -1;
	VideoCapture vc;
	Mat frame[2], processFrame[2], cameraFrame;
	string windowName[3] = { "First Image", "Second Image", "Camera" };
	static bool cmpDMatch(const DMatch& fmatch, const DMatch& smatch);
public:
	void process(int argc, char** argv);
	void processCommand(char cmd);
	void processCamera();
	void processImage();
	//void showFrame(int index);
	void displayHelp();
	bool openImage(char* img);
	Mat harrisCornerDetector(Mat frame, int blockSize = 2, int apertureSize = 3, double k = 0.04);
	Mat blobDetector(Mat frame, int minArea = 0);
	Mat SiftDetector(Mat frame, int octaveLayer = 3);
	Mat SiftDescriptor(Mat frame, vector<KeyPoint> keypoints);
	Mat LBPDescriptor(Mat frame, vector<KeyPoint> keypoints);
	vector<KeyPoint> harrisCornerKeyPoint(Mat frame);
	vector<KeyPoint> blobKeyPoint(Mat frame);
	vector<KeyPoint> SiftKeyPoint(Mat frame);
	void performHarrisCornerDetect(int frameIndex);
	void performBlobDetect(int frameIndex);
	void performSIFTDetect(int frameIndex);
	void performMatchingFeatures();
	int getMethod(char* argv);
	void performDetect();
};