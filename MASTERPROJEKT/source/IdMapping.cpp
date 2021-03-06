#include "IdMapping.h"




// tests if a point p is inside the triangle of a, b and c by a BarycentricTest
//@param cv::Point2f a: triangle Point
//@param cv::Point2f b: triangle Point
//@param cv::Point2f c: triangle Point
//@param cv::Point2f p: point to test
//@return bool: Point is inside
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

// tests if marker is inside game plane
// @param cv::RotatedRect r: rect of current Marker
// @param PlaneAndAffineCalibration::planeCalibData pcd: game plane data
// @return: 1 if out of field and 0 if not
int IdMapping::isMarkerOutOfField(Marker * m, PlaneAndAffineCalibration::planeCalibData pcd) {
	std::vector<cv::Point2f> ps = m->getPoints();
	cv::Rect r = cv::Rect(pcd.upperCorner, pcd.lowerCorner);

	for each (cv::Point2f p in ps)
	{
		cv::Point2i pi = cv::Point2i(p.x, p.y);
		if (!r.contains(pi))
			return 1;
	}
	return 0;
}

// Calculates the motion vectors between the current green rectangle of frame n and all tracked markers of frame n-1
// @param cv::RotatedRect normRect: rect of marker in frame n
// @param std::array<Marker*, 100>  allMarkers: array of size 100 with all current markers
// @param std::vector<int> takenIdVec: vector of all valid marker positions in allMarkers array
// @return std::vector<cv::Point2f> all caluculated motion vectors
std::vector<cv::Point2f> IdMapping::CalculateMotionVectorCenter(cv::RotatedRect normRect, std::array<Marker*, 100> trackedMarker, std::vector<int> takenIDVec)
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


// Proofs whether the tracked marker is a known marker, whos positions has rarely changed. 
// A marker is a constant marker even its angle has changed. This allows to rotate the marker on the same position
// @param std::vector<cv::Point2f> motionCenterVecs: motionsVector of 
// @param std::array<Marker*, 100>  allMarkers: array of size 100 with all current markers
// @param std::vector<int> takenIdVec: vector of all valid marker positions in allMarkers array
// @param int arucoId: id of current marker
// @return: id of matched marker (not aruco id)

int IdMapping::isConstantMarker(std::vector<cv::Point2f> motionCenterVecs, std::array<Marker*, 100> trackedMarker, std::vector<int> takenIDVec, int arucoID)
{
	bool isConstant = false;
	int  matchID = 0;
	float tCenterConstant = 3;

	for (int i = 0; i < takenIDVec.size(); i++)
	{
		int id = takenIDVec[i];
		Marker* m = trackedMarker[id];
		cv::Point2f mvC = motionCenterVecs[i];
		if (mvC.x* mvC.x + mvC.y*mvC.y <= tCenterConstant*tCenterConstant) {
			isConstant = true;
			}
		else {
			isConstant = false;
		}
		if (isConstant && arucoID == -1) {
			isConstant = true;
			matchID = m->getId();
			break;
		}
		else if (isConstant && m->getArucoID()==arucoID) {
			isConstant = true;
			matchID = m->getId();
			break;
		}
	}
	return matchID;
}


// If the Aruco marker can't be recognized because of occlusion or motion blur, only the green rectangle is detected.
// This function return the estimated marker id. 
// @param std::vector<cv::Point2f> motionCenterVecs: motionsVector of 
// @param std::array<Marker*, 100>  allMarkers: array of size 100 with all current markers
// @param std::vector<int> takenIdVec: vector of all valid marker positions in allMarkers array
// @param int arucoId: id of current marker
// @return: id of matched marker (not aruco id)
int IdMapping::isTranslatedMarker(std::vector<cv::Point2f> motionCenterVecs, std::array<Marker*, 100> trackedMarker, std::vector<int> takenIDVec, int arucoID)
{
	bool isTranslated = false;
	int matchID = 0;
	for (int i = 0; i < takenIDVec.size(); i++)
	{
		int id = takenIDVec[i];
		Marker* m = trackedMarker[id];
		cv::Point2f mvC = motionCenterVecs[i];
		cv::Point2f mvCMarker=	m->getMotionCenterVec();
		cv::Point2f centerMarker = m->getCenter();
		float tTranslation = 100;
		if (abs(mvC.x) + abs(mvC.y) <= tTranslation) {
			isTranslated = true;
		}
		else {
			isTranslated = false;
		}
		if (isTranslated && arucoID == -1) {
			isTranslated = true;
			matchID = m->getId();
			break;
		}
		else if (isTranslated && m->getArucoID()==arucoID) {
			isTranslated = true;
			matchID = m->getId();
			break;
		}
	}
	return matchID;
}


// If the Aruco marker can't be recognized because of occlusion or motion blur, only the green rectangle is detected.
// This function return the estimated marker id. 
// @param cv::RotatedRect normRect: rect of marker in frame n// @param std::array<Marker*, 100>  allMarkers: array of size 100 with all current markers
// @param std::vector<std::vector<cv::Point2f>> corners: four corners of each detected aruco marker 
// @return: 1 if has arcuo id and -1 if not
int IdMapping::hasArucoID(cv::RotatedRect normRect, std::vector<std::vector<cv::Point2f>> corners)
{
	for (int i = 0; i < corners.size(); i++)
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
			return i;
	}
	return -1;
}
// tests if marker is inside game plane
// @param cv::RotatedRect r: rect of current Marker
// @param PlaneAndAffineCalibration::planeCalibData pcd: game plane data
// @return: 1 if out of field and 0 if not
int IdMapping::isRectOutOfField(cv::RotatedRect r, PlaneAndAffineCalibration::planeCalibData pcd)
{
	cv::Point2f ps[4];
	r.points(ps);
	cv::Rect br = cv::Rect(pcd.upperCorner, pcd.lowerCorner);

	for each (cv::Point2f p in ps)
	{
		if (!br.contains(p))
			return 1;
	}
	return 0;
}


