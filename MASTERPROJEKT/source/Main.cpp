
#include <iostream>
#include <opencv2/opencv.hpp>


cv::Mat colorThreshold(cv::Mat image) {
	cv::Mat output;
	cv::Mat outputYellow;
	cv::inRange(image, cv::Scalar(100, 0, 0), cv::Scalar(255, 150, 150), output);
	cv::inRange(image, cv::Scalar(0, 200, 200), cv::Scalar(30, 255, 255), outputYellow);
	cv::bitwise_or(output, outputYellow, output);
	return output;
}

std::vector<cv::RotatedRect>  getOBB(cv::Mat image, cv::Mat rgb) {
	std::vector<cv::Point> points;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	/// Approximate contours to polygons + get bounding rects and circles
	std::vector<std::vector<cv::Point> > contours_poly(contours.size());
	std::vector<cv::RotatedRect> boundRect(contours.size());
	std::vector<cv::Point2f>center(contours.size());
	std::vector<float>radius(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = cv::minAreaRect(cv::Mat(contours_poly[i]));

	}


	/*cv::Mat_<uchar>::iterator it = image.begin<uchar>();
	cv::Mat_<uchar>::iterator end = image.end<uchar>();
	for (; it != end; ++it)
		if (*it)
			points.push_back(it.pos());

	cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));
	double angle = box.angle;*/
	// TODO Koordinaten der Box können negativ sein!!!!
	//For Debugging

	for (int i = 0; i < contours.size(); i++) {

		cv::Point2f vertices[4];
		cv::RotatedRect box = boundRect[i];
		box.points(vertices);
		cv::Mat image1 = image.clone();
		image1.setTo(0);
		for (int i = 0; i < 4; ++i) {
			cv::line(rgb, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 255, 255), 1, CV_AA);
			cv::line(image, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 0, 0), 1, CV_AA);
		}
	}
	cv::waitKey(0);

	return boundRect;
}

float getOrientation(cv::RotatedRect rect, cv::Mat rgbImage) {
	cv::Size2f tempSize = rect.size;
	rect.size.height = rect.size.height*0.8f;
	rect.size.width = rect.size.width*0.8f;
	cv::Point2f center = rect.center;
	cv::Size2f size = rect.size;
	cv::Point2f upperLeft;
	cv::Point2f upperRight;
	cv::Point2f bottomLeft;
	cv::Point2f bottomRight;
	cv::Point2f cornerPoints[4];
	rect.points(cornerPoints);

	//order of corners

	//0=bottomLeft
	//1=upperLeft
	//2=upperRight
	//3=bottomRight

	float cornerAngle;
	for (unsigned int i = 0; i < 4; i++)
	{
		cv::circle(rgbImage, cornerPoints[0], 10, (255, 255, 255), 1, 8, 0);
		bool isMarkedCorner = false;
		cv::Vec3b intensity = rgbImage.at<cv::Vec3b>(cornerPoints[i].y,cornerPoints[i].x);
		uchar blue = intensity.val[0];
		uchar green = intensity.val[1];
		uchar red = intensity.val[2];

		if (blue >=0 && blue < 35 && green <= 255 && green>200 && red <= 255 && red>200)
			isMarkedCorner = true;

		if (isMarkedCorner) {
			cornerAngle = i*90.f;
			break;
		}
	}
	rect.size = tempSize;
	return cornerAngle + std::abs(rect.angle);

}

int main()
{
	std::string filename1 = "C:/Users/Vera/Dropbox/MP/Testaufnahmen/TestBild_03.jpg";
	cv::Mat image1 = cv::imread(filename1);
	std::string filename2 = "C:/Users/Vera/Dropbox/MP/Testaufnahmen/TestBild_06.jpg";
	cv::Mat image2 = cv::imread(filename2);
	//cv::imshow( "Test Window1", image1 );
	//cv::imshow("Test Window2", image2);
	//cv::Mat colorthre1 = colorThreshold(image1);
	cv::Mat colorthre2 = colorThreshold(image2);
	// image nur zum Debuggen
	//cv::RotatedRect box1 = getOBB(colorthre1, image1);
	std::vector<cv::RotatedRect>  box2 = getOBB(colorthre2, image2);
	//float ori = getOrientation(box2, image2);
	//printf("Angle %f", ori);
	cv::waitKey();
	return EXIT_SUCCESS;
}