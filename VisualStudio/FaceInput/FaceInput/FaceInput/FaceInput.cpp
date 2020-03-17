#include "stdafx.h"

//#include <opencv2\highgui\highgui.hpp>
//#include <opencv2\imgproc\imgproc.hpp>
//#include "VideoFaceDetector.h"
//#include "FaceDetect.h"
//
//namespace FaceInput
//{
//	VideoFaceDetector GetFaceDetector()
//	{
//		FaceDetect faceDetection;
//		return faceDetection.detector;		
//	}
//}

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
	int camera_device = camIndex = 0;	
	//-- 2. Read the video stream
	_capture.open(camera_device);
	if (!_capture.isOpened())
	{
		return -3;
	}

	camWidth = _capture.get(CAP_PROP_FRAME_WIDTH);
	camHeight = _capture.get(CAP_PROP_FRAME_HEIGHT);

	return 0;
}

extern "C" int __declspec(dllexport) __stdcall DetectFace(Circle facePos, Circle* eyePos, int maxEyesDetected, int maxFaces, bool showFrame)
{
	Mat frame;
	_capture >> frame;
	if (frame.empty())
		return -1;

	Mat frame_gray;
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	std::vector<Rect> faces;
	face_cascade.detectMultiScale(frame_gray, faces);

	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);

		facePos = Circle(center.x, center.y, faces[i].width + faces[i].height / 2);

		ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4);
		Mat faceROI = frame_gray(faces[i]);
		//-- In first face found, detect eyes
		std::vector<Rect> eyes;
		eyes_cascade.detectMultiScale(faceROI, eyes);
		for (size_t j = 0; j < eyes.size(); j++) //Was size_t instead of int
		{
			Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);

			eyePos[j] = Circle(faces[i].x + eyes[j].x, faces[i].y + eyes[j].y, radius);

			circle(frame, eye_center, radius, Scalar(255, 0, 0), 4);

			if ((int)j == maxEyesDetected)
			{
				break;
			}
		}

		if (i == maxFaces)
		{
			break;
		}
	}

	if (faces.size() == 0)
	{
		return -1;
	}
	
	if (showFrame) {

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

//int main(int argc, const char** argv)
//{	
//	Mat frame;
//	while (_capture.read(frame))
//	{
//		if (frame.empty())
//		{
//			break;
//		}
//		//-- 3. Apply the classifier to the frame
//		detectAndDisplay(frame);
//		if (waitKey(10) == 27)
//		{
//			break; // escape
//		}
//	}
//	return 0;
//}
//void detectAndDisplay(Mat frame)
//{
//	Mat frame_gray;
//	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
//	equalizeHist(frame_gray, frame_gray);
//	//-- Detect faces
//	std::vector<Rect> faces;
//	face_cascade.detectMultiScale(frame_gray, faces);
//	for (size_t i = 0; i < faces.size(); i++)
//	{
//		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
//		ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4);
//		Mat faceROI = frame_gray(faces[i]);
//		//-- In each face, detect eyes
//		std::vector<Rect> eyes;
//		eyes_cascade.detectMultiScale(faceROI, eyes);
//		for (size_t j = 0; j < eyes.size(); j++)
//		{
//			Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
//			int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
//			circle(frame, eye_center, radius, Scalar(255, 0, 0), 4);
//		}
//	}
//	//-- Show what you got
//	imshow("Capture - Face detection", frame);
//}