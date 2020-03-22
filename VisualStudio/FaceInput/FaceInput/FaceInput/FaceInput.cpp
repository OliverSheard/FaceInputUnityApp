#include "stdafx.h"

using namespace std;
using namespace cv;

// Declare structure to be used to pass data from C++ to Mono.
struct Circle
{
	Circle(int x, int y, int radius) : X(x), Y(y), Radius(radius) {}
	int X, Y, Radius;
};

//void detectAndDisplay(Mat frame);
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
CascadeClassifier nose_cascade;
VideoCapture _capture;
String _windowName = "Capture - Face detection";

extern "C" int __declspec(dllexport) __stdcall Initialise(int& camIndex, int& camWidth, int& camHeight)
{	
	//-- 1. Load the cascades
	if (!face_cascade.load("haarcascade_frontalface_alt.xml"))
	{
		return -1;
	};
	if (!eyes_cascade.load("haarcascade_eye_tree_eyeglasses.xml"))
	{
		return -2;
	};
	if (!nose_cascade.load("haarcascade_mcs_nose.xml"))
	{
		return -3;
	}

	int camera_device = camIndex = 0;	
	//-- 2. Read the video stream
	_capture.open(camera_device);
	if (!_capture.isOpened())
	{
		return -4;
	}

	camWidth = _capture.get(CAP_PROP_FRAME_WIDTH);
	camHeight = _capture.get(CAP_PROP_FRAME_HEIGHT);

	return 0;
}

extern "C" int __declspec(dllexport) __stdcall DetectFace(Circle& facePos, Circle* eyePos, Circle& nosePos, int maxEyesDetected, bool showFrame)
{
	//Detect the largest face visible via the haar cascade
	Mat frame;
	_capture >> frame;
	if (frame.empty())
		return -1;

	Mat frame_gray;
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	std::vector<Rect> faces;
	face_cascade.detectMultiScale(frame_gray, faces);

	if (faces.size() == 0)
	{
		//No face found
		return -1;
	}

	int biggestFace = 0;
	int selectedFaceIndex = 0;

	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);

		int faceSize = faces[i].width + faces[i].height / 2;

		if (faceSize > biggestFace)
		{
			facePos = Circle(center.x, center.y, faceSize);
			biggestFace = faceSize;
			selectedFaceIndex = (int)i;
		}				
	}	

	//Draw on the frame where the largest face is positioned
	ellipse(frame, Point(facePos.X, facePos.Y), Size(faces[selectedFaceIndex].width / 2, faces[selectedFaceIndex].height / 2), 0, 0, 360, Scalar(255, 255, 255), 4);

	//Detect the eyes within the bounds of the largest face detected
	Mat faceROI = frame_gray(faces[selectedFaceIndex]);
	std::vector<Rect> eyes;
	eyes_cascade.detectMultiScale(faceROI, eyes);

	for (size_t j = 0; j < eyes.size(); j++)
	{
		Point eye_center(faces[selectedFaceIndex].x + eyes[j].x + eyes[j].width / 2, faces[selectedFaceIndex].y + eyes[j].y + eyes[j].height / 2);
		int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);

		eyePos[j] = Circle(faces[selectedFaceIndex].x + eyes[j].x, faces[selectedFaceIndex].y + eyes[j].y, radius);

		circle(frame, eye_center, radius, Scalar(0, 0, 0), 4);

		if ((int)j == maxEyesDetected)
		{
			break;
		}
	}

	//Detect the nose within the bounds of the largest face detected
	std::vector<Rect> nose;
	nose_cascade.detectMultiScale(faceROI, nose);

	int biggestNose = 0;
	int biggestNoseIndex = -1;
	nosePos = Circle(faces[selectedFaceIndex].x, faces[selectedFaceIndex].y, 0);

	for (size_t i = 0; i < nose.size(); i++)
	{		
		int radius = cvRound((nose[i].width + nose[i].height) * 0.25);

		if (radius > biggestNose)
		{
			biggestNose = radius;
			nosePos = Circle(faces[selectedFaceIndex].x + nose[i].x, faces[selectedFaceIndex].y + nose[i].y, radius);
			biggestNoseIndex = i;
			Point nose_Center(faces[selectedFaceIndex].x + nose[biggestNoseIndex].x + nose[biggestNoseIndex].width / 2, faces[selectedFaceIndex].y + nose[biggestNoseIndex].y + nose[biggestNoseIndex].height / 2);
			circle(frame, nose_Center, cvRound((nose[biggestNoseIndex].width + nose[biggestNoseIndex].height) * 0.25), Scalar(0, 255, 0), 4);
		}
	}
	
	/*if (biggestNoseIndex >= 0)
	{
		
	}*/

	//Possibly remove at later date
	if (showFrame)
	{
		// Display debug output.
		imshow(_windowName, frame);
	}
	if (!showFrame)
	{
		destroyWindow(_windowName);
	}

	return 0;
}

extern "C" void __declspec(dllexport) __stdcall Release()
{	
	_capture.release();
}