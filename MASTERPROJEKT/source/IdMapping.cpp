#include "IdMapping.h"


bool IdMapping::computeBarycentricTest(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f p)
{
	// Compute vectors        
	cv::Point2f v0 = c - a;
	cv::Point2f	v1 = b - a;
	cv::Point2f v2 = p - a;

	// Compute dot products
	float dot00=  v0.dot(v0);
	float dot01 = v0.dot(v1);
	float dot02 = v0.dot(v2);
	float dot11 = v1.dot(v1);
	float dot12 = v1.dot(v2);

	// Compute barycentric coordinates
	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v < 1);

}

std::vector<cv::Point2f> IdMapping::CalculateMotionVectorCenter(cv::RotatedRect normRect, std::array<Marker*, 200> trackedMarker, std::vector<int> takenIDVec)
{
	std::vector<cv::Point2f> motionVectors;
	for each (int id in takenIDVec)
	{
		Marker* tm = trackedMarker[id];
		cv::Point2f mvC;
		mvC.x = normRect.center.x - tm->getCenter().x;
		mvC.y = normRect.center.y - tm->getCenter().y;
		motionVectors.push_back(mvC);
	}
	return motionVectors;
}

int IdMapping::isConstantMarker(std::vector<cv::Point2f> motionCenterVecs, std::array<Marker*, 200> trackedMarker, std::vector<int> takenIDVec)
{
	/*myfile2.open("isConstant.txt", std::ios::out | std::ios::app);
	myfile2 << "Next Frame " << "\n";*/
	bool isConstant = false;
	int  matchID = 0;
	float tCenterConstant = 0.03f;

	for (int i = 0; i < takenIDVec.size(); i++)
	{
		int id = takenIDVec[i];
		Marker* m = trackedMarker[id];
		cv::Point2f mvC = motionCenterVecs[i];
		if (abs(mvC.x) + abs(mvC.y) <= tCenterConstant) {
			isConstant = true;
			/*myfile2 << "\t Schleifenabbruch bei " << c << "\n";
			myfile2 << "\t isConstant auf true gesetzt \n";*/
		}
		else {
			isConstant = false;
			//myfile2 << "\t Schleifendurchlauf " << c << "\n";
		}
		if (isConstant) {
			isConstant = true;
			matchID = m->getId();
			break;
		}
	}
	//myfile2.close();
	return matchID;
}

int IdMapping::isTranslatedMarker(std::vector<cv::Point2f> motionCenterVecs, std::array<Marker*, 200> trackedMarker, std::vector<int> takenIDVec)
{
	bool isTranslated = false;
	int matchID = 0;
	float tTranslation = 2.*(50./1024.);
	for (int i = 0; i < takenIDVec.size(); i++)
	{
		int id = takenIDVec[i];
		Marker* m = trackedMarker[id];
		cv::Point2f mvC = motionCenterVecs[i];
		if (abs(mvC.x) + abs(mvC.y) <= tTranslation) {
			isTranslated = true;
		}
		else {
			isTranslated = false;
		}
		if (isTranslated) {
			isTranslated = true;
			matchID = m->getId();
			break;
		}
	}
	return matchID;
}

bool IdMapping::isMarkerOutOfField() {


	return false;
}

int IdMapping::hasArucoID(cv::RotatedRect normRect, std::vector<std::vector<cv::Point2f>> corners, std::vector<int> arucoIds)
{
	for (int i = 0; i < arucoIds.size(); i++)
	{
		std::vector<cv::Point2f> rectPoints = corners[i];
		bool isInsideOfRect = true;
		for (int j = 0; j < rectPoints.size(); j++)
		{
			//split into triangle and test if point is inside both
			cv::Point2f points[4];
			normRect.points(points);
			bool isInsideOfFirstTri = computeBarycentricTest(points[0], points[1], points[2], rectPoints[j]);
			bool isInsideOfSecondTri = computeBarycentricTest(points[0], points[2], points[3], rectPoints[j]);
			isInsideOfRect &= isInsideOfFirstTri | isInsideOfSecondTri;
			if (!isInsideOfRect)break;
		}
		if (isInsideOfRect)
			return arucoIds[i];
	}
	return 0;
}


