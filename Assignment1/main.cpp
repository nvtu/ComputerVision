#include "ImageProcessor.h"

ImageProcessor* ip = NULL;

int main(int argc, char** argv) {
	if (argc > 1) {
		char cmd;
		ip = new ImageProcessor(argv[1]);
		while ((cmd = waitKey(30)) != 'q'){
			ip->process(cmd);
			ip->processImage();
		}
	}
	else {
		VideoCapture vc;
		ip = new ImageProcessor(vc);
		if (ip->isOpenCamera()) ip->processCamera();
	}
	return 0;
}
