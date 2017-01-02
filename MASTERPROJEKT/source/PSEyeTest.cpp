//#include <opencv2/highgui.hpp>
//#include <opencv2/opencv.hpp>
//
//int main(int, char**)
//{
//	cv::VideoCapture cap(1);
//	
//	// open the default camera
//	if (!cap.isOpened())  // check if we succeeded
//		return -1;
//
//	cv::Mat edges;
//	cv::namedWindow("edges", 1);
//	for (;;)
//	{
//		cv::Mat frame;
//		cap >> frame; // get a new frame from camera
//		cvtColor(frame, edges, CV_BGR2GRAY);
//		imshow("edges", edges);
//		if (cv::waitKey() >= 0) break;
//	}
//	// the camera will be deinitialized automatically in VideoCapture destructor
//	return 0;
//}