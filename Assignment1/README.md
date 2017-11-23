# Problem
In this assignment you need to perform simple image manipulation using OpenCV. The program should load an image by either reading it from a file or capturing it directly from a camera. When the user presses a key perform the operation corresponding to the key on the original image (not the result of the last processing step). The program should satisfy the following specifications:
1. The image to be processed by the program should be either read from a file or captured
directly from a camera. If a file name is specified in the command line, the image should
be read from it. Otherwise the program should attempt to capture an image from a
camera. When capturing an image from the camera, continue to capture and process
images continuously.
2. The read image should be read as a 3 channel color image.
3. The program should work for any size image. Make sure to test it on different size
images.
4. Special keys on the keyboard should be used to modify the displayed image as follows:
* i - reload the original image (i.e. cancel any previous processing)
* w - save the current (possibly processed) image into the file out.jpg
* g - convert the image to grayscale using the openCV conversion function.
* G - convert the image to grayscale using your implementation of conversion function.
* c - cycle through the color channels of the image showing a different channel every time the key is pressed.
* s - convert the image to grayscale and smooth it using the openCV function. Use a track bar to control the amount of smoothing.
* S - convert the image to grayscale and smooth it using your function which should perform convolution with a suitable filter. Use a track bar to control the amount of smoothing.
* x - convert the image to grayscale and perform convolution with an x derivative filter. Normalize the obtained values to the range [0,255].
* y - convert the image to grayscale and perform convolution with a y derivative filter. Normalize the obtained values to the range [0,255].
* m - show the magnitude of the gradient normalized to the range [0,255]. The gradient is computed based on the x and y derivatives of the image.
* r - convert the image to grayscale and rotate it using an angle of Q degrees. Use a track bar to control the rotation angle. The rotation of the image should be performed using an inverse map so there are no holes in it.
* h - Display a short description of the program, its command line arguments, and the keys it supports.
# Requirement
In this assignment you need to implement one or several possible applications as described below:
1. When capturing images from a camera allow the program to capture and process the
images continuously.
2. The main parameters of each algorithm should be made available for interactive manipulation through keyboard/mouse/trackbar interaction.
3. Your program must include a help key describing its functionality.