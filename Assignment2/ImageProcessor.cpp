#include "ImageProcessor.h"

void ImageProcessor::process(int argc, char** argv) {
	if (argc < 2) goto UNKNOWN_COMMAND_ERROR;
	int method = getMethod(argv[1]);
	if (method != MATCHING) {
		if (method == HELP) {
			displayHelp();
			waitKey(0);
			return;
		}
		detector = method;
		if (argc == 2) {
			if (vc.open(0)) processCamera();
			else goto CAMERA_ERROR;
		}
		else {
			if (!openImage(argv[2])) goto UNKNOWN_COMMAND_ERROR;;
			processImage();
		}
	}
	else {
		if (argc < 4) goto UNKNOWN_COMMAND_ERROR;
		detector = getMethod(argv[2]);
		descriptor = getMethod(argv[3]);
		if (detector == -1 || descriptor == -1) goto UNKNOWN_COMMAND_ERROR;
		if (argc >= 6) {
			if (!openImage(argv[4])) goto UNKNOWN_COMMAND_ERROR;
			if (!openImage(argv[5])) goto UNKNOWN_COMMAND_ERROR;
			processImage();
		}
		else if (argc == 4) {
			if (vc.open(0)) processCamera();
			else goto CAMERA_ERROR;
		}
	}
	return;
CAMERA_ERROR:
	cout << "Cannot open camera" << endl;
	return;
UNKNOWN_COMMAND_ERROR:
	cout << "Unknown command" << endl;
	return;
}

void ImageProcessor::processCommand(char cmd) {
	if (cmd == -1) return;
	switch (cmd)
	{
	case 'h':
		displayHelp();
		break;
	case 'q':
		break;
	default:
		if (vc.isOpened()) {
			if (cntFrame < 2) {
				if (descriptor == -1) {
					if (cntFrame < 1) {
						cout << "Captured " << cntFrame + 1 << " frames" << endl;
						frame[cntFrame++] = cameraFrame.clone();
					}
					else {
						cout << "Updated frame " << cntFrame << endl;
						frame[cntFrame - 1] = cameraFrame.clone();
					}
					performDetect();
				}
				else {
					cout << "Captured " << cntFrame + 1 << " frames" << endl;
					frame[cntFrame++] = cameraFrame.clone();
					if (cntFrame == 2) performDetect();
				}
			}
			else if (cntFrame == 2) {
				int randomFactor = rand() % 2;
				frame[randomFactor] = cameraFrame.clone();
				cout << "Updated frame " << cntFrame + 1 << endl;
				performDetect();
			}
		}
		else performDetect();
		break;
	}
}


void ImageProcessor::processCamera() {
	char cmd = -1;
	while (true) {
		vc >> cameraFrame;
		if (!cameraFrame.data) break;
		namedWindow(windowName[2], CV_WINDOW_AUTOSIZE);
		processCommand(cmd);
		imshow(windowName[2], cameraFrame);
		if ((cmd = waitKey(30)) == 'q' || cntFrame == 2 || (cntFrame == 1 && descriptor == -1)) break;
	}
	processCommand(cmd);
	vc.release();
}

void ImageProcessor::processImage() {
	performDetect();
}

//void ImageProcessor::showFrame(int index) {
//	namedWindow(windowName[index], CV_WINDOW_AUTOSIZE);
//	imshow(windowName[index], processFrame[index]);
//}

bool ImageProcessor::openImage(char* img) {
	frame[cntFrame] = imread(img, CV_LOAD_IMAGE_COLOR);
	if (!frame[cntFrame].data) {
		cout << "Cannot open image" << endl;
		return false;
	}
	cntFrame++;
	return true;
}

Mat ImageProcessor::harrisCornerDetector(Mat frame, int blockSize, int apertureSize, double k) {
	int threshold = 200;
	Mat grayscaleFrame, result, temp;
	cvtColor(frame, grayscaleFrame, CV_BGR2GRAY);
	cornerHarris(grayscaleFrame, temp, blockSize, apertureSize, k, BORDER_DEFAULT);
	normalize(temp, grayscaleFrame, 0, 255, CV_MINMAX, CV_32FC1);
	convertScaleAbs(grayscaleFrame, result);
	float* pt = (float*)grayscaleFrame.data;
	size_t elemStep = grayscaleFrame.step / sizeof(float);
	for (int i = 0; i < grayscaleFrame.rows; i++) {
		for (int j = 0; j < grayscaleFrame.cols; j++) {
			if ((int)pt[i*elemStep + j] > threshold) {
				circle(result, Point(j, i), 5, Scalar(0));
			}
		}
	}
	return result;
}

Mat ImageProcessor::blobDetector(Mat frame, int minArea) {
	Mat grayscaleFrame, result;
	cvtColor(frame, grayscaleFrame, CV_BGR2GRAY);

	SimpleBlobDetector::Params params;
	// Change thresholds
	params.minThreshold = 10;
	params.maxThreshold = 200;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = (float)minArea;
	params.maxArea = 100;

	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.1f;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.87f;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.01f;
	
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	vector<KeyPoint> keypoints;
	detector->detect(grayscaleFrame, keypoints);
	drawKeypoints(grayscaleFrame, keypoints, result, Scalar(0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	return result;
}

Mat ImageProcessor::SiftDetector(Mat frame, int octaveLayer) {
	Mat grayscaleFrame, result;
	cvtColor(frame, grayscaleFrame, CV_BGR2GRAY);
	Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create(0, octaveLayer);
	vector<KeyPoint> keypoints;
	detector->detect(grayscaleFrame, keypoints);
	drawKeypoints(grayscaleFrame, keypoints, result, Scalar(0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	return result;
}

Mat ImageProcessor::SiftDescriptor(Mat frame, vector<KeyPoint> keypoints) {
	Mat grayscaleFrame, result;
	cvtColor(frame, grayscaleFrame, CV_BGR2GRAY);
	Ptr<SiftDescriptorExtractor> extractor = SiftDescriptorExtractor::create();
	extractor->compute(grayscaleFrame, keypoints, result);
	return result;
}

Mat ImageProcessor::LBPDescriptor(Mat frame, vector<KeyPoint> keypoints) {
	Mat grayscaleFrame;
	cvtColor(frame, grayscaleFrame, CV_BGR2GRAY);
	Mat lbp = Mat::zeros(grayscaleFrame.rows - 2, grayscaleFrame.cols - 2, CV_8UC1);
	unsigned char* pt = (unsigned char*)grayscaleFrame.data;
	unsigned char* ptLBP = (unsigned char*)lbp.data;
	const int dx[] = { -1,-1,-1,0,1,1,1,0 };
	const int dy[] = { -1,0,1,1,1,0,-1,-1 };
	size_t elemStep = grayscaleFrame.step;
	for (int i = 1; i < grayscaleFrame.rows - 1; i++) {
		for (int j = 1; j < grayscaleFrame.cols - 1; j++) {
			unsigned char code = 0;
			int center = i * elemStep + j;
			for (int k = 0; k < 8; k++) {
				int pos = (i + dx[k]) * elemStep + j + dy[k];
				code |= (pt[center] < pt[pos]) << (7 - k);
			}
			int mapCenter = (i - 1) * lbp.step + (j - 1);
			ptLBP[mapCenter] = code;
		}
	}
	Mat result = Mat((int)keypoints.size(), 256, CV_32FC1);
	float* ptRes = (float*)result.data;
	int cntBucket[256];
	elemStep = result.step / sizeof(float);
	for (int i = 0; i < (int)keypoints.size(); i++) {
		int x = (int)keypoints[i].pt.y, y = (int)keypoints[i].pt.x, radius = (int)(keypoints[i].size / 2.0);
		int gridTopLeftX = max(0, x - radius), gridBottomRightY = min(lbp.cols - 1, y + radius);
		pair<int, int> gridTopLeft = make_pair(gridTopLeftX, max(0, gridBottomRightY - (radius << 1)));
		pair<int, int> gridBottomRight = make_pair(min(lbp.rows, gridTopLeftX + (radius << 1) + 1), gridBottomRightY + 1);
		memset(cntBucket, 0, sizeof(cntBucket));
		for (int j = gridTopLeft.first; j < gridBottomRight.first; j++) {
			for (int k = gridTopLeft.second; k < gridBottomRight.second; k++) {
				int index = j * lbp.step + k;
				cntBucket[ptLBP[index]]++;
			}
		}
		for (int k = 0; k < 256; k++) {
			int index = i * elemStep + k;
			ptRes[index] = (float)cntBucket[k];
		}
	}
	return result;
}

vector<KeyPoint> ImageProcessor::harrisCornerKeyPoint(Mat frame) {
	Mat temp, norm;
	vector<KeyPoint> keypoints;
	int blockSize = 4, apertureSize = 3, threshold = 200;
	double k = 0.04;
	cornerHarris(frame, temp, blockSize, apertureSize, k, BORDER_DEFAULT);
	normalize(temp, norm, 0, 255, CV_MINMAX, CV_32FC1);
	size_t elemStep = norm.step / sizeof(float);
	float* pt = (float*)norm.data;
	for (int i = 0; i < norm.rows; i++) {
		for (int j = 0; j < norm.cols; j++) {
			if ((int)pt[i * elemStep + j] > threshold) {
				keypoints.push_back(KeyPoint(Point2d(i, j), 10));
			}
		}
	}
	return keypoints;
}

vector<KeyPoint> ImageProcessor::blobKeyPoint(Mat frame) {
	SimpleBlobDetector::Params params;
	// Change thresholds
	params.minThreshold = 10;
	params.maxThreshold = 200;
	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 20;
	params.maxArea = 100;

	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.1f;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.87f;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.01f;

	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	vector<KeyPoint> keypoints;
	detector->detect(frame, keypoints);
	return keypoints;
}

vector<KeyPoint> ImageProcessor::SiftKeyPoint(Mat frame) {
	vector<KeyPoint> keypoints;
	Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create();
	detector->detect(frame, keypoints);
	return keypoints;
}


void ImageProcessor::performHarrisCornerDetect(int frameIndex) {
	char cmd = -1;
	int blockSize = 0, apertureSize = 0, oldBlockSize = -1, oldApertureSize = -1;
	namedWindow(windowName[frameIndex], CV_WINDOW_AUTOSIZE);
	createTrackbar("Block Size", windowName[frameIndex], &blockSize, 8);
	createTrackbar("Aperture Size", windowName[frameIndex], &apertureSize, 3);
	while (true) {
		if (vc.isOpened()) {
			vc >> cameraFrame;
			processCommand(cmd);
			imshow(windowName[2], cameraFrame);
		}
		if (oldBlockSize != blockSize || apertureSize != oldApertureSize) {
			processFrame[frameIndex] = harrisCornerDetector(frame[frameIndex], blockSize + 2, 2 * apertureSize + 3);
			imshow(windowName[frameIndex], processFrame[frameIndex]);
			oldBlockSize = blockSize;
			oldApertureSize = apertureSize;
		}
		if (vc.isOpened()) {
			if ((cmd = waitKey(30)) == 'q') break;
		}
		else {
			if ((cmd = waitKey(30)) != -1) break;
		}
	}
}


void ImageProcessor::performBlobDetect(int frameIndex) {
	char cmd = -1;
	int oldMinArea = -1, minArea = 0;
	namedWindow(windowName[frameIndex], CV_WINDOW_AUTOSIZE);
	createTrackbar("Min Area", windowName[frameIndex], &minArea, 80);
	while (true) {
		if (vc.isOpened()) {
			vc >> cameraFrame;
			processCommand(cmd);
			imshow(windowName[2], cameraFrame);
		}
		if (minArea != oldMinArea) {
			processFrame[frameIndex] = blobDetector(frame[frameIndex], minArea + 20);
			imshow(windowName[frameIndex], processFrame[frameIndex]);
			oldMinArea = minArea;
		}
		if (vc.isOpened()) {
			if ((cmd = waitKey(30)) == 'q') break;
		}
		else {
			if ((cmd = waitKey(30)) != -1) break;
		}
	}
}

void ImageProcessor::performSIFTDetect(int frameIndex) {
	char cmd = -1;
	int octaveLayer = 0, oldOctaveLayer = -1;
	namedWindow(windowName[frameIndex], CV_WINDOW_AUTOSIZE);
	createTrackbar("Octave Layer", windowName[frameIndex], &octaveLayer, 7);
	while (true) {
		if (vc.isOpened()) {
			vc >> cameraFrame;
			processCommand(cmd);
			imshow(windowName[2], cameraFrame);
		}
		if (oldOctaveLayer != octaveLayer) {
			processFrame[frameIndex] = SiftDetector(frame[frameIndex], octaveLayer + 3);
			imshow(windowName[frameIndex], processFrame[frameIndex]);
			oldOctaveLayer = octaveLayer;
		}
		if (vc.isOpened()) {
			if ((cmd = waitKey(30)) == 'q') break;
		}
		else {
			if ((cmd = waitKey(30)) != -1) break;
		}
	}
}

void ImageProcessor::performMatchingFeatures() {
	Mat grayscaleFrame[2], descriptors[2];
	char cmd = -1;
	int cnt = 0;
	bool firstUpdate = true;
	while (true) {
		if (vc.isOpened()) {
			vc >> cameraFrame;
			processCommand(cmd);
			imshow(windowName[2], cameraFrame);
		}
		if (cmd != -1 || firstUpdate) {
			firstUpdate = false;
			vector<KeyPoint> keypoints[2];
			for (int i = 0; i < cntFrame; i++) {
				cvtColor(frame[i], grayscaleFrame[i], CV_BGR2GRAY);
			}
			switch (detector) {
			case HARRIS:
				for (int i = 0; i < cntFrame; i++) {
					const int MAXIMUM_KEYPOINTS_ACCEPTED = 1000;
					keypoints[i] = harrisCornerKeyPoint(grayscaleFrame[i]);
					if ((int)keypoints[i].size() > MAXIMUM_KEYPOINTS_ACCEPTED) keypoints[i].resize(MAXIMUM_KEYPOINTS_ACCEPTED);
				}
				break;
			case BLOB:
				for (int i = 0; i < cntFrame; i++) {
					keypoints[i] = blobKeyPoint(grayscaleFrame[i]);
				}
				break;
			case DOG:
				for (int i = 0; i < cntFrame; i++) {
					keypoints[i] = SiftKeyPoint(grayscaleFrame[i]);
				}
				break;
			}
			switch (descriptor) {
			case M_SIFT:
				for (int i = 0; i < cntFrame; i++) {
					descriptors[i] = SiftDescriptor(frame[i], keypoints[i]);
				}
				break;
			case LBP:
				for (int i = 0; i < cntFrame; i++) {
					descriptors[i] = LBPDescriptor(frame[i], keypoints[i]);
				}
				break;
			}
			Ptr<BFMatcher> bfmatcher = BFMatcher::create(NORM_L2);
			vector<vector<DMatch> > matches;
			vector<DMatch> match;
			Mat imgMatch;
			bfmatcher->knnMatch(descriptors[0], descriptors[1], matches, 2);
			for (int i = 0; i < (int)matches.size(); i++) {
				match.push_back(matches[i][0]);
			}
			int minMatchesSize = min(50, (int)match.size());
			std::sort(match.begin(), match.end(), cmpDMatch);
			match.resize(minMatchesSize);
			drawMatches(frame[0], keypoints[0], frame[1], keypoints[1], match, imgMatch);
			namedWindow("Matching Feature", CV_WINDOW_AUTOSIZE);
			imshow("Matching Feature", imgMatch);
		}
		if (vc.isOpened()) {
			if ((cmd = waitKey(30)) == 'q') break;
		}
		else {
			if ((cmd = waitKey(30)) != -1) break;
		}
	}
}


int ImageProcessor::getMethod(char* argv) {
	for (int i = 0; i < strlen(argv); i++) {
		argv[i] = tolower(argv[i]);
	}
	if (!strcmp(argv, "harris")) return HARRIS;
	else if (!strcmp(argv, "blob")) return BLOB;
	else if (!strcmp(argv, "dog")) return DOG;
	else if (!strcmp(argv, "sift")) return M_SIFT;
	else if (!strcmp(argv, "lbp")) return LBP;
	else if (!strcmp(argv, "m")) return MATCHING;
	else if (!strcmp(argv, "h")) return HELP;
	return -1;
}

void ImageProcessor::performDetect() {
	if (descriptor == -1) {
		switch (detector) {
		case HARRIS:
			performHarrisCornerDetect(cntFrame - 1);
			break;
		case BLOB:
			performBlobDetect(cntFrame - 1);
			break;
		case DOG:
			performSIFTDetect(cntFrame - 1);
			break;
		}
	}
	else {
		//Perform matching
		performMatchingFeatures();
	}
}

bool ImageProcessor::cmpDMatch(const DMatch& fmatch, const DMatch& smatch) {
	return fmatch.distance < smatch.distance;
}

void ImageProcessor::displayHelp() {
	int x = 10, y = 20, baseline = 0, thickness = 1;
	const char* helpString[] = {
		"1 - harris image.jpg detect key points using harris algorithm and show the keypoints",
		"in original image.",
		"2 - blob image.jpg - detect key points using blob algorithm and show the keypoints",
		"in original image.",
		"3 - dog image.jpg detect key points using DoG Algorithm and show keypoints in",
		"original image.",
		"4 - m harris sift image1.jpg image2.jpg match and show results of image1 and image2",
		"using Harris detector and SIFT descriptor.",
		"5 - m dog sift image1.jpg image2.jpg - match and show results of image1 and image2",
		"using DoG detector and SIFT descriptor.",
		"6 - m blob sift image1.jpg image2.jpg - match and show results of image1 and image2",
		"using using Blob detector and SIFT descriptor.",
		"7 - m harris lbp image1.jpg image2.jpg - match and show results of image1 and image2",
		"using Harris detector and LBP descriptor.",
		"8 - m dog lbp image1.jpg image2.jpg - match and show results of image1 and image2",
		"using DoG detector and LBP descriptor.",
		"9 - m blob lbp image1.jpg image2.jpg - match and show results of image1 and image2",
		"using Blob detector and LBP descriptor.",
		"10 - h - Display a short description of the program, its command line arguments, and",
		"the keys it supports.",
		"Press q to quit the program"
	};
	Mat helpFrame = Mat(400, 1000, CV_8U, double(0));
	for (int i = 0; i < sizeof(helpString) / sizeof(char*); i++) {
		putText(helpFrame, helpString[i], Point(x, y), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, Scalar(255, 255, 255), thickness, 1);
		Size textSize = getTextSize(helpString[i], CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, thickness, &baseline);
		y += textSize.height + baseline;
	}
	imshow("Help Window", helpFrame);
}