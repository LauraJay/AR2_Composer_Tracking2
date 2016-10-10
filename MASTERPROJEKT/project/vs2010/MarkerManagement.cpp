#include "MarkerManagement.h"



bool MarkerManagement::isConstantMarker(std::vector<cv::Point2f> points, unsigned char markedCorner)
{
	//TODO

		return true;
}


std::vector<Marker*> MarkerManagement::getTrackedMarker()
{
	return trackedMarker;
}

void MarkerManagement::trackMarker(cv::RotatedRect rect, unsigned char markedCorner)
{
	//TODO
	std::vector<cv::Point2f> rectPoints = rectTOVectorPoints(rect);
	cv::Point2f center = rect.center;
	if (!isConstantMarker(rectPoints, markedCorner)) {
		registerNewMarker(rectPoints, center, markedCorner);
	}

}

MarkerManagement::MarkerManagement()
{

}


MarkerManagement::~MarkerManagement()
{
}

std::vector<cv::Point2f> MarkerManagement::rectTOVectorPoints(cv::RotatedRect rect){
	std::vector<cv::Point2f> v;
	cv::Point2f p[4];
	rect.points(p);
	for (int i = 0; i < 4; i++)
	{
		v.push_back(p[i]);
	}
	return v;
}

// checks if Marker is removed from Playfield
bool MarkerManagement::isMarkerOutOfField()
{
	//TODO
	int id;
	deleteMarker(id);
	return false;
}

// Delete Function of Marker
void MarkerManagement::deleteMarker(int id)
{
	Marker* m = trackedMarker[id];
	trackedMarker.erase(trackedMarker.begin()+id);
	delete &m;
}
// Proofs if this is moved Marker with known identity
bool MarkerManagement::isTrackedMarker(std::vector<cv::Point2f> points, unsigned char markedCorner)
{
	//TODO
	return false;
}

// inits a new trackedMarker
void MarkerManagement::registerNewMarker(std::vector<cv::Point2f> rectPoints, cv::Point2f center, unsigned char markedCorner)
{
	Marker* m = new Marker(trackedMarker.size()+1, rectPoints, center, markedCorner);
	trackedMarker.push_back(m);
}
