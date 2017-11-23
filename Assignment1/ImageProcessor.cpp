#include "ImageProcessor.h"

const int ImageProcessor::_MAX_ANGLE = 360;
const int ImageProcessor::_MAX_KERNEL_SIZE = 15;

ImageProcessor::ImageProcessor() {
	
}

ImageProcessor::ImageProcessor(VideoCapture vc) {
	this->vc = vc;
}

ImageProcessor::ImageProcessor(string imageName) {
	this->imageName = imageName;
	originalFrame = imread(imageName, CV_LOAD_IMAGE_COLOR);
	if (!originalFrame.data) {
		cout << "Could not load/find image" << endl;
		return;
	}
	namedWindow("Frame", CV_WINDOW_AUTOSIZE);
	imshow("Frame", originalFrame);
}

bool ImageProcessor::isOpenCamera() {
	return vc.open(0);
}

void ImageProcessor::processCamera() {
	char cmd;
	while (true) {
		vc >> originalFrame;
		if (originalFrame.empty()) break;
		namedWindow("Frame", CV_WINDOW_AUTOSIZE);
		currentFrame = applyFilter();
		imshow("Frame", currentFrame);
		if (trackBarOn) {
			if (rotateFlag) createTrackBar("Rotate");
			if (smoothFlag) createTrackBar("Smooth");
		}
		if ((cmd = waitKey(30)) == 'q') break;
		process(cmd);
	}
	vc.release();
}

void ImageProcessor::processImage() {
	currentFrame = applyFilter();
	namedWindow("Frame", CV_WINDOW_AUTOSIZE);
	imshow("Frame", currentFrame);
	if (trackBarOn) {
		if (rotateFlag) createTrackBar("Rotate");
		if (smoothFlag) createTrackBar("Smooth");
	}
}

Mat ImageProcessor::applyFilter() {
	Mat frame;
	if (useGrayscale) {
		frame = convertToGrayscaleFrame(originalFrame, useOpenCV);
		if (rotateFlag) frame = rotateImage(frame, rotateAngle);
		if (smoothFlag) frame = smoothImage(frame, useOpenCV);
		if (deriveXFilterFlag) frame = computeDerivativeX(frame);
		if (deriveYFilterFlag) frame = computeDerivativeY(frame);
		if (magnitudeGradient) frame = computerDerivative(frame);
	}
	else if (useChannel) {
		frame = cycleThroughChannel();
	}
	else frame = originalFrame;
	return frame;
}

void ImageProcessor::createTrackBar(string flag) {
	if (flag == "Smooth") createTrackbar("Smooth", "Frame", &filterKernelSz, _MAX_KERNEL_SIZE);
	else if (flag == "Rotate") createTrackbar("Angle", "Frame", &rotateAngle, _MAX_ANGLE);
}

void ImageProcessor::process(char cmd) {
	if ((int)cmd != -1) {
		if (cmd == 'w') {
			imwrite("out.jpg", currentFrame);
		}
		else {
			if (trackBarOn) destroyWindow("Frame");
			reset();
		}
	}
	switch (cmd)
	{
	case 'i':
		break;
	case 'g':
		useGrayscale = true;
		break;
	case 'G':
		useGrayscale = true;
		useOpenCV = false;
		break;
	case 'c':
		useChannel = true;
		currentChannel = (currentChannel + 1) % 3;
		break;
	case 's':
		useGrayscale = smoothFlag = trackBarOn = true;
		break;
	case 'S':
		useGrayscale = smoothFlag = trackBarOn = true;
		useOpenCV = false;
		break;
	case 'x':
		useGrayscale = useOpenCV = deriveXFilterFlag = true;
		break;
	case 'y':
		useGrayscale = useOpenCV = deriveYFilterFlag = true;
		break;
	case 'm':
		useGrayscale = useOpenCV = magnitudeGradient = true;
		break;
	case 'r':
		useGrayscale = rotateFlag = trackBarOn = true;
		break;
	case 'h':
		showHelp();
		break;
	default:
		break;
	}
}

void ImageProcessor::reset() {
	magnitudeGradient = trackBarOn = useChannel = useGrayscale = smoothFlag = rotateFlag = smoothFlag = deriveXFilterFlag = deriveYFilterFlag = false;
	useOpenCV = true;
	rotateAngle = filterKernelSz = 0;
}

Mat ImageProcessor::computerDerivative(Mat frame) {
	Mat ans;
	Mat deriveX = computeDerivativeX(frame);
	Mat deriveY = computeDerivativeY(frame);
	ans = (deriveX + deriveY) / 2.0;
	return ans;
}

Mat ImageProcessor::cycleThroughChannel() {
	Mat channelFrame = Mat::zeros(originalFrame.rows, originalFrame.cols, CV_8UC3);
	unsigned char* input = (unsigned char*)(channelFrame.data);
	unsigned char* sample = (unsigned char*)(originalFrame.data);
	for (int i = 0; i < originalFrame.rows; i++) {
		for (int j = 0; j < originalFrame.cols; j++) {
			int index = originalFrame.step * i + originalFrame.channels() * j + currentChannel;
			input[index] = sample[index];
		}
	}
	return channelFrame;
}

Mat ImageProcessor::smoothImage(Mat frame, bool useOpenCV) {
	Mat smoothImage;
	int kernelSz = (filterKernelSz << 1) + 1;
	if (useOpenCV) blur(frame, smoothImage, Size(kernelSz, kernelSz));
	else {
		Mat filter = Mat::ones(kernelSz, kernelSz, CV_32F) / (float)(kernelSz * kernelSz);
		filter2D(frame, smoothImage, CV_8UC3, filter);

		// Self-implement convolutional but failed (T_T)

		//smoothImage = performConvolution(frame, filter);
		/*smoothImage = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
		unsigned char* srcPt = (unsigned char*)(frame.data);
		unsigned char* ansPt = (unsigned char*)(smoothImage.data);
		for (int i = 0; i < smoothImage.rows - kernelSz + 1; i++) {
			for (int j = 0; j < smoothImage.cols - kernelSz + 1; j++) {
				int resultIndex = smoothImage.step * i + smoothImage.channels() * j;
				for (int t = 0; t < kernelSz; t++) {
					for (int u = 0; u < kernelSz; u++) {
						int srcIndex = smoothImage.step * (i + t) + smoothImage.channels() * (j + u);
						for (int k = 0; k < smoothImage.channels(); k++) {
							ansPt[resultIndex + k] += srcPt[srcIndex + k];
						}
					}
				}
				for (int k = 0; k < smoothImage.channels(); k++) {
					ansPt[resultIndex + k] = (unsigned char)((float) ansPt[resultIndex + k] / (float) (kernelSz * kernelSz));
				}
			}
		}*/
	}
	return smoothImage;
}

Mat ImageProcessor::computeDerivativeX(Mat frame) {
	float tempFilter[9] = {
		-1, 0, 1,
		-2, 0, 2,
		-1, 0, 1
	};
	Mat filter = Mat(3, 3, CV_32F, tempFilter);
	Mat deriveX, ans;
	filter2D(frame, deriveX, CV_32F, filter);
	deriveX.convertTo(ans, CV_8UC3);
	return ans;
}

Mat ImageProcessor::computeDerivativeY(Mat frame) {
	float tempFilter[9] = {
		-1, -2, -1,
		0, 0, 0,
		1, 2, 1
	};
	Mat filter = Mat(3, 3, CV_32F, tempFilter);
	Mat deriveY, ans;
	filter2D(frame, deriveY, CV_32F, filter);
	deriveY.convertTo(ans, CV_8UC3);
	return ans;
}



// Got confused of the Mat type and behavior of OpenCV (T_T)

//Mat ImageProcessor::performConvolution(Mat src, Mat filter) {
//	Mat ans = Mat::zeros(src.rows, src.cols, CV_32F);
//	unsigned char* srcPt = (unsigned char*)(src.data);
//	unsigned char* ansPt = (unsigned char*)(ans.data);
//	unsigned char* filterPt = (unsigned char*)(filter.data);
//	for (int i = 0; i < src.rows - filter.rows + 1; i++) {
//		for (int j = 0; j < src.cols - filter.cols + 1; j++) {
//			int resultIndex = ans.step * i + ans.channels() * j;
//			for (int t = 0; t < filter.rows; t++) {
//				for (int u = 0; u < filter.cols; u++) {
//					int srcIndex = src.step * (i + t) + src.channels() * (j + u);
//					int filterIndex = filter.step * t + filter.channels() * u;
//					for (int k = 0; k < src.channels(); k++) {
//						ansPt[resultIndex + k] += filterPt[filterIndex + k] * srcPt[srcIndex + k];
//					}
//				}
//			}
//		}
//	}
//	return ans;
//}

Mat ImageProcessor::rotateImage(Mat frame, double angle) {
	Point center = Point(frame.cols / 2.0, frame.rows / 2.0);
	Mat rotationMat = getRotationMatrix2D(center, angle, 1);
	Mat rotationFrame;
	warpAffine(frame, rotationFrame, rotationMat, frame.size(), WARP_INVERSE_MAP);
	return rotationFrame;
}

Mat ImageProcessor::convertToGrayscaleFrame(Mat frame, bool useOpenCV) {
	Mat grayscaleFrame;
	if (useOpenCV)  cvtColor(frame, grayscaleFrame, CV_BGR2GRAY);
	else {
		grayscaleFrame = frame.clone();
		unsigned char* input = (unsigned char*)(grayscaleFrame.data);
		for (int i = 0; i < frame.rows; i++) {
			for (int j = 0; j < frame.cols; j++) {
				int index = frame.step * i + j * frame.channels();
				input[index] = input[index + 1] = input[index + 2] = (uchar)input[index] * 0.0722 + input[index + 1] * 0.7152 + input[index + 2] * 0.2126;
			}
		}
	}
	return grayscaleFrame;
}

//void ImageProcessor::onTrackBarRotateProcess(int, void*) {
//	
//}

void ImageProcessor::showHelp() {
	int x = 10, y = 20, thickness = 1, baseline = 0;
	const char* helpString[] = {
		"i - reload the original image (i.e. cancel any previous processing)",
		"w - save the current (possibly processed) image into the file out.jpg",
		"g - convert the image to grayscale using the openCV conversion function.",
		"G - convert the image to grayscale using your implementation of conversion function.",
		"c - cycle through the color channels of the image showing a different channel every", 
		"time the key is pressed.",
		"s - convert the image to grayscale and smooth it using the openCV function.Use",
		"a track bar to control the amount of smoothing.",
		"S - convert the image to grayscale and smooth it using your function which should",
		"perform convolution with a suitable filter.Use a track bar to control the amount",
		"of smoothing.",
		"x - convert the image to grayscale and perform convolution with an x derivative",
		"filter.Normalize the obtained values to the range[0,255].",
		"y - convert the image to grayscale and perform convolution with a y derivative",
		"filter.Normalize the obtained values to the range[0,255].",
		"m - show the magnitude of the gradient normalized to the range [0,255]. The"
		"gradient is computed based on the x and y derivatives of the image.",
		"r - convert the image to grayscale and rotate it using an angle of Q degrees. Use",
		"a track bar to control the rotation angle.The rotation of the image should be",
		"performed using an inverse map so there are no holes in it.",
		"h - Display a short description of the program, its command line arguments, and",
		"the keys it supports.",
		"q - Quit the program"
	};
	Mat helpFrame = Mat(400, 1000, CV_8U, double(0));
	CvFont* font = new CvFont();
	cvInitFont(font, CV_FONT_VECTOR0, 0.5f, 0.5f);
	for (int i = 0; i < sizeof(helpString) / sizeof(char*); i++) {
		putText(helpFrame, helpString[i], cvPoint(x, y), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, Scalar(255, 255, 255), thickness, 1);
		Size textSize = getTextSize(helpString[i], CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, thickness, &baseline);
		y += textSize.height  + baseline;
	}
	imshow("HELP", helpFrame);
}