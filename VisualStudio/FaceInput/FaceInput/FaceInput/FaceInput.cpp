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
CascadeClassifier profile_cascade;
CascadeClassifier eyes_cascade;
CascadeClassifier nose_cascade;
VideoCapture _capture;
String _windowName = "Capture - Face detection";

extern "C" int __declspec(dllexport) __stdcall Initialise(int& camIndex, int& camWidth, int& camHeight)
{		
	//Load the cascades
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
	if (!profile_cascade.load("haarcascade_profileface.xml"))
	{
		return -4;
	}

	//Loop through cameras to open the first available
	//Add break if unable to open at index out of bounds on camera_device
	int maxCameras = 5;

	for (size_t i = 0; i < maxCameras; i++)
	{
		int camera_device = camIndex = i;
		//Read the video stream
		_capture.open(camera_device);

		if (_capture.isOpened())
		{
			if (i == maxCameras - 1)
			{
				return -4;
			}
			else
			{
				break;
			}			
		}
	}

	camWidth = _capture.get(CAP_PROP_FRAME_WIDTH);
	camHeight = _capture.get(CAP_PROP_FRAME_HEIGHT);

	return 0;
}

extern "C" int __declspec(dllexport) __stdcall DetectFace(Circle& facePos, int minFaceSize, Circle* eyePos, int maxEyesDetected, Circle& nosePos, bool showFrame)
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

	if (facePos.Radius < minFaceSize)
	{
		return -1;
	}

	//Detect Eyes
	Mat faceROI = frame_gray(faces[selectedFaceIndex]);
	std::vector<Rect> eyes;
	eyes_cascade.detectMultiScale(faceROI, eyes);

	int offset = 0;
	int eyesDetected = 0;
	for (size_t i = 0; i < eyes.size(); i++)
	{
		Point eye_center(faces[selectedFaceIndex].x + eyes[i].x + eyes[i].width / 2, faces[selectedFaceIndex].y + eyes[i].y + eyes[i].height / 2);
		int radius = cvRound((eyes[i].width + eyes[i].height) * 0.25);

		eyesDetected++;
		eyePos[i - offset] = Circle(eye_center.x, eye_center.y, radius);		

		if ((int)i - offset == maxEyesDetected)
		{
			break;
		}

		//if (eye_center.y < faces[selectedFaceIndex].y)
		//{
		//	//Ignore eyes beneath the mid point of the face
		//	offset++;
		//}
		//else
		//{
		//	eyesDetected++;
		//	eyePos[i - offset] = Circle(eye_center.x, eye_center.y, radius);

		//	if ((int)i - offset == maxEyesDetected)
		//	{
		//		break;
		//	}
		//}
	}

	//for (size_t i = 0; i < eyesDetected; i++)
	//{
	//	//circle(frame, eye_center, radius, Scalar(0, 0, 0), 4);
	//	circle(frame, Point(eyePos[i].X, eyePos[i].Y), eyePos[i].Radius, Scalar(0, 0, 0), 4);
	//}

	//Detect the nose within the bounds of the largest face detected
	std::vector<Rect> nose;
	nose_cascade.detectMultiScale(faceROI, nose);

	int biggestNose = INT_MIN;
	int biggestNoseIndex = -1;
	//nosePos = Circle(faces[selectedFaceIndex].x, faces[selectedFaceIndex].y, 0);

	if (nose.size() == 0)
	{
		return -3;
	}

	//Use the biggest y position nose detected
	for (size_t i = 0; i < nose.size(); i++)
	{	
		if (nose[i].y > biggestNose)
		{
			int radius = cvRound((nose[i].width + nose[i].height) * 0.25);
			biggestNose = nose[i].y;
			nosePos = Circle(faces[selectedFaceIndex].x + nose[i].x, faces[selectedFaceIndex].y + nose[i].y, radius);
			biggestNoseIndex = i;
		}
	}

	Point nose_Center(faces[selectedFaceIndex].x + nose[biggestNoseIndex].x + nose[biggestNoseIndex].width / 2,
		faces[selectedFaceIndex].y + nose[biggestNoseIndex].y + nose[biggestNoseIndex].height / 2);

	//Push all drawing on frame to end of method to prevent any potential issues with haar cascades interacting with drawn components
	//Draw on the frame where the largest face is positioned
	ellipse(frame, Point(facePos.X, facePos.Y), Size(faces[selectedFaceIndex].width / 2, faces[selectedFaceIndex].height / 2), 0, 0, 360, Scalar(255, 255, 255), 4);

	//Inefficient, requires rework
	for (size_t i = 0; i < eyesDetected; i++)
	{
		int radius = cvRound((eyes[i].width + eyes[i].height) * 0.25);
		Point eye_center(faces[selectedFaceIndex].x + eyes[i].x + eyes[i].width / 2, faces[selectedFaceIndex].y + eyes[i].y + eyes[i].height / 2);
		circle(frame, eye_center, radius, Scalar(0, 0, 0), 4);
	}

	circle(frame, nose_Center, cvRound((nose[biggestNoseIndex].width + nose[biggestNoseIndex].height) * 0.25), Scalar(0, 255, 0), 4);

	if (showFrame)
	{
		imshow(_windowName, frame);
	}
	if (!showFrame)
	{
		destroyWindow(_windowName);
	}

	return 0;
}

//Attempted to divide detection into multiple components per section, caused issues when drawing on frames for capture data and referencing
extern "C" int __declspec(dllexport) __stdcall DetectEyes(Circle* eyePos, int maxEyesDetected)
{
	return 0;
	//if (faceDectectStatus == 0)
	//{
	//	Mat frame;
	//	_capture >> frame;

	//	if (frame.empty())
	//		return -2;

	//	std::vector<Rect> eyes;
	//	eyes_cascade.detectMultiScale(faceROI, eyes);

	//	int offset = 0;
	//	int eyesDetected = 0;
	//	for (size_t i = 0; i < eyes.size(); i++)
	//	{
	//		Point eye_center(facePos.x + eyes[i].x + eyes[i].width / 2, facePos.y + eyes[i].y + eyes[i].height / 2);
	//		int radius = cvRound((eyes[i].width + eyes[i].height)*0.25);

	//		if (eye_center.y < facePos.y)
	//		{
	//			//Ignore eyes beneath the mid point of the face
	//			offset++;
	//		}
	//		else
	//		{
	//			eyesDetected++;
	//			eyePos[i - offset] = Circle(facePos.x + eyes[i].x, facePos.y + eyes[i].y, radius);
	//			
	//			if ((int)i - offset == maxEyesDetected)
	//			{
	//				break;
	//			}
	//		}			
	//	}

	//	for (size_t i = 0; i < eyesDetected; i++)
	//	{
	//		//circle(frame, eye_center, radius, Scalar(0, 0, 0), 4);
	//		circle(frame, Point(eyePos[i].X, eyePos[i].Y), eyePos[i].Radius, Scalar(0, 0, 0), 4);
	//	}

	//	return 0;
	//}
	//else
	//{
	//	return -1;
	//}	
}

extern "C" int __declspec(dllexport) __stdcall DetectNose(Circle& nosePos)
{

	return 0;
	//if (faceDectectStatus == 0)
	//{
	//	Mat frame;
	//	_capture >> frame;

	//	if (frame.empty())
	//		return -2;

	//	//Detect the nose within the bounds of the largest face detected
	//	std::vector<Rect> nose;
	//	nose_cascade.detectMultiScale(faceROI, nose);

	//	int biggestNose = 0;
	//	int biggestNoseIndex = -1;
	//	nosePos = Circle(facePos.x, facePos.y, 0);

	//	for (size_t i = 0; i < nose.size(); i++)
	//	{
	//		int radius = cvRound((nose[i].width + nose[i].height) * 0.25);

	//		if (radius > biggestNose)
	//		{
	//			biggestNose = radius;
	//			nosePos = Circle(facePos.x + nose[i].x, facePos.y + nose[i].y, radius);
	//			biggestNoseIndex = i;
	//		}
	//	}

	//	Point nose_Center(facePos.x + nose[biggestNoseIndex].x + nose[biggestNoseIndex].width / 2, facePos.y + nose[biggestNoseIndex].y + nose[biggestNoseIndex].height / 2);
	//	//circle(frame, nose_Center, cvRound((nose[biggestNoseIndex].width + nose[biggestNoseIndex].height) * 0.25), Scalar(0, 255, 0), 4);

	//	return 0;
	//}
	//else
	//{
	//	return -1;
	//}	
}

extern "C" void __declspec(dllexport) __stdcall Release()
{	
	destroyWindow(_windowName);
	_capture.release();
}