# Problem
In this assignment you need to perform simple image manipulation using OpenCV. The
program should load an image by either reading it from a file or capturing it directly from a camera. When the user presses a key perform the operation corresponding to the key on the original image (not the result of the last processing step). The program should satisfy the following specifications:
1. The image to be processed by the program should be either read from a file or captured directly from a camera. If a file name is specified in the command line, the image should be read from it. Otherwise the program should attempt to capture an image from a camera. When capturing an image from the camera, continue to capture and process images continuously.
2. The read image should be read as a 3 channel color image.
3. The program should work for any size image. Make sure to test it on different size images.
4. The matcher algorithm is KNN matcher in OpenCV. (see samples directory in OpenCV)
5. The programs are written by using command line arguments as follows:
• (a) harris image.jpg detect key points using harris algorithm and show the keypoints in original image.
• (b) blob image.jpg - detect key points using blob algorithm and show the keypoints
in original image.
• (c) dog image.jpg detect key points using DoG Algorithm and show keypoints in
original image.
• (d) m harris sift image1.jpg image2.jpg match and show results of image1 and
image2 using Harris detector and SIFT descriptor.
• (e) m dog sift image1.jpg image2.jpg - match and show results of image1 and
image2 using DoG detector and SIFT descriptor.
1• (f) m blob sift image1.jpg image2.jpg - match and show results of image1 and
image2 using using Blob detector and SIFT descriptor.
• (g) m harris lbp image1.jpg image2.jpg - match and show results of image1 and image2 using Harris detector and LBP descriptor.
• (h) m dog lbp image1.jpg image2.jpg - match and show results of image1 and
image2 using DoG detector and LBP descriptor.
• (i) m blob lbp image1.jpg image2.jpg - match and show results of image1 and
image2 using Blob detector and LBP descriptor.
• (j) h - Display a short description of the program, its command line arguments,
and the keys it supports.
# Requirement
In this assignment you need to implement one or several possible applications as described below:
1. When capturing images from a camera allow the program to capture and process the
images continuously.
2. The main parameters of each algorithm should be made available for interactive manipulation through keyboard/mouse/trackbar interaction.
3. The student can use the OpenCV functions or implement the algorithms by yourself.
If you implement the algorithms by yourself, you can only do (a) ,(b) ,(d), (f), and (j) options.
4. Your program must include a help key describing its functionality.
5. You need to evaluate the performance of the algorithms using test data (each student gathers 5 objects and 3 images/object). The results of your evaluation should be included in your report. Try to determine the strengths and weaknesses of the algorithm.
6. Your report must include a description of the algorithm you implemented.
7. Your submission must organize into 3 folder and compressed in 1 file StudentID.zip
• Document
• Source Code
• Release (includes only an .exe file and a .txt file shows dll requirements)