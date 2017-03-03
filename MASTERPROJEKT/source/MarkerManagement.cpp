#include "MarkerManagement.h"

std::array<Marker*, 100> MarkerManagement::getTrackedMarker()
{
	return trackedMarker;
}

std::vector<int> MarkerManagement::getTakenIDVec()
{
	return takenIDVec;
}

// marker tracking
//@param std::vector<cv::RotatedRect> rect: current green rectangles
// @param std::vector<std::vector<cv::Point2f>> corners: four corners of current detected aruco marker 
//@param std::vector<int> arucoIds: vector with all current detected aruco ids
//@param cv::Size size: image size
void MarkerManagement::trackMarker(std::vector<cv::RotatedRect> rect, std::vector<std::vector<cv::Point2f>> corners, std::vector<int> arucoIds, cv::Size size)
{

	IdMapping* im = new IdMapping();
	for (int i = 0; i < rect.size(); i++)
	{
		cv::RotatedRect r = rect[i];

		int matchID = 0;
		int arucoID = 0;
		int rep = 0;

		
		std::vector<cv::Point2f> motionCenterVecs = im->CalculateMotionVectorCenter(r, trackedMarker, takenIDVec);

		if ((arucoID = im->hasArucoID(r, corners)) >= 0) {
			matchID = findMatchID(arucoIds[arucoID]); //is the marker id already known?
			if (matchID > 0)
			 {
				if ((rep = im->isMarkerOutOfField(trackedMarker[matchID], pcd)) > 0) {
					deleteMarker(matchID); //delete marker when leaving the workspace
				}
				else {
					if ((im->isConstantMarker(motionCenterVecs, trackedMarker, takenIDVec, arucoIds[arucoID])) > 0)
						CurrentMarkerWAruco(trackedMarker[matchID], trackedMarker[matchID]->getRect(), arucoIds[arucoID], corners[arucoID].at(0), true);
					else
						CurrentMarkerWAruco(trackedMarker[matchID], r, arucoIds[arucoID], corners[arucoID].at(0), false);
				}
			}
			else if ((rep = im->isRectOutOfField(r, pcd)) == 0) {
					registerNewMarker(r, arucoIds[arucoID], corners[arucoID].at(0));
				}
			
			else if ((matchID = im->isConstantMarker(motionCenterVecs, trackedMarker, takenIDVec, arucoIds[arucoID])) > 0) {
					CurrentMarkerWAruco(trackedMarker[matchID], trackedMarker[matchID]->getRect(), arucoIds[arucoID], corners[arucoID].at(0),true);
			}
			else {
				if ((matchID = im->isTranslatedMarker(motionCenterVecs, trackedMarker, takenIDVec, arucoIds[arucoID])) > 0) {

					CurrentMarkerWAruco(trackedMarker[matchID], r, arucoIds[arucoID], corners[arucoID].at(0),false);
				}
			}
		}
		else if ((rep = im->isRectOutOfField(r, pcd)) > 0) {
			if ((matchID = im->isTranslatedMarker(motionCenterVecs, trackedMarker, takenIDVec, -1)) > 0) {
				deleteMarker(matchID);
			}
		}
		else if ((matchID = im->isConstantMarker(motionCenterVecs, trackedMarker, takenIDVec, -1)) > 0) {
			CurrentMarker(trackedMarker[matchID], trackedMarker[matchID]->getRect());
		}
		else if ((matchID = im->isTranslatedMarker(motionCenterVecs, trackedMarker, takenIDVec, -1)) > 0) {
			CurrentMarker(trackedMarker[matchID], r);
		}
	}

	int matchID = 0;
	for (int i = 0; i < takenIDVec.size(); i++)
	{
		int id = takenIDVec[i];
		if (trackedMarker[id]->isTracked() == 0) {

			trackedMarker[id]->setVisible(0);
		}
		trackedMarker[id]->setTracked(0);
	}
	delete im;
}







// Delete all properties of a Marker identified by its id 
// @param int id: id in trackedMarker
void MarkerManagement::deleteMarker(int id)
{
	Marker* m = trackedMarker[id];
	m->clear();
	openIDQueue.push(id);
	for (int i = 0; i < takenIDVec.size(); i++)
	{
		if (takenIDVec[i] == id) {
			takenIDVec.erase(takenIDVec.begin() + i);
			break;
		}
	}
}


// inits a new trackedMarker
//@param cv::RotatedRect rect: current green rectangle
//@param int arucoID: current aruco id
//cv::Point2f anglePoint of first aruco corner
void MarkerManagement::registerNewMarker(cv::RotatedRect rect, int arucoID, cv::Point2f anglePoint) {
	int id = openIDQueue.front();
	openIDQueue.pop();
	Marker* m = new Marker(id, rect, anglePoint);
	if (arucoID > 0)m->setArucoID(arucoID);
	m->setTracked(1);
	trackedMarker[id] = m;
	takenIDVec.push_back(id);
	std::vector< cv::Point3d > tvecs=estimateGreenRects(rect);
	m->setTvecs(tvecs);
	//std::vector< cv::Point3d > rvecs = estimateGreenRectsForRvec(rect);
	//m->setRvecs(rvecs);

}

// estimate center of current green rectangle
//@param cv::RotatedRect rect: current green rectangle
std::vector< cv::Point3d > MarkerManagement::estimateGreenRects(cv::RotatedRect r) {
	cv::Point2f vert[4];
	r.points(vert);
	std::vector<std::vector<cv::Point2f>> corners;
	std::vector<cv::Point2f> temp;
	for (int i = 0; i < 4; i++)
	{
		temp.push_back(vert[i]);
	}
	corners.push_back(temp);
	std::vector< cv::Point3d > rvecs, tvecs;
	cv::aruco::estimatePoseSingleMarkers(corners,0.04, camMat, distMat,rvecs,tvecs);
	return tvecs;
}


// search matching id in trackedMarker to aruco id
//@param int arucoID: aruco id
// @return matching id
int MarkerManagement::findMatchID(int arucoID)
{
	for each (int i in takenIDVec)
	{
		if (trackedMarker[i]->getArucoID() == arucoID)
			return trackedMarker[i]->getId();
	}
	return 0;
}


// updates a current marker object with matched Aruco id
// @param Marker* tm: current marker object
//@param cv::RotatedRect rect: current green rectangle
//@param int arucoID: current aruco id
//@param cv::Point2f anglePoint of first aruco corner
//@param bool isConstant: is constant marker
void MarkerManagement::CurrentMarkerWAruco(Marker* tm, cv::RotatedRect rect, int arucoID, cv::Point2f anglePoint, bool isConstant) {
	if (tm->isTracked()==0) {
		if (!isConstant)
			tm->setRectWithAngle(rect, anglePoint);
		else {
			float currentAngle = tm->getAngle();
			tm->setRectWithAngle(rect, anglePoint);
			if (std::abs(tm->getAngle() - currentAngle) <= getAngleThreshold(rect.center)) 	tm->setAngle(currentAngle);
		}
		if (arucoID > 0)tm->setArucoID(arucoID);
		tm->setTracked(1);
		tm->setVisible(1);
		std::vector< cv::Point3d > tvecs = estimateGreenRects(rect);
		tm->setTvecs(tvecs);

	}
}
// gives angle threshold in order to border distance
// @param cv::Point2f center: center of current marker
//@param int: threshold
int MarkerManagement::getAngleThreshold(cv::Point2f center) {
	cv::Rect r;
	int x1 = frameSize.width*(1. / 6);
	int y1 = frameSize.height*(1. / 6);
	int x2 = frameSize.width*(2. / 6);
	int y2 = frameSize.height*(2. / 6);
	r = cv::Rect(cv::Point(x2, y2), cv::Point(frameSize.width - x2, frameSize.height - y2));
	if (r.contains(center)) { /*printf("treshold = 2 \r");*/ return 2; }
	r = cv::Rect(cv::Point(x1, y1), cv::Point(frameSize.width - x1, frameSize.height - y1));
	if (r.contains(center)) {/* printf("treshold = 4 \r");*/ return 3; }
	//printf("treshold = 5 \r");
	return 4;
}

// updates a current marker object with matched Aruco id
// @param Marker* tm: current marker object
//@param cv::RotatedRect rect: current green rectangle

void MarkerManagement::CurrentMarker(Marker* tm, cv::RotatedRect rect) {
	if (tm->isTracked()==0) {
		tm->setRect(rect);
		tm->setTracked(1);
		tm->setVisible(1);
	}
	std::vector< cv::Point3d > tvecs = estimateGreenRects(rect);
	tm->setTvecs(tvecs);
}



MarkerManagement::MarkerManagement(cv::Size frameSize, PlaneAndAffineCalibration::planeCalibData pcd)
{
	for (size_t i = 0; i < trackedMarker.size(); i++)
	{
		if (i > 0)	openIDQueue.push(i);
		trackedMarker[i] = new Marker();
	}
	trackedMarker[0]->setId(0);
	MarkerManagement::frameSize = frameSize;
	MarkerManagement::pcd = pcd;
}


MarkerManagement::~MarkerManagement()
{

}