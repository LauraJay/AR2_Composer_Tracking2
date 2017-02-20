#include "MarkerManagement.h"

std::array<Marker*, 100> MarkerManagement::getTrackedMarker()
{
	return trackedMarker;
}

std::vector<int> MarkerManagement::getTakenIDVec()
{
	return takenIDVec;
}

cv::RotatedRect reduceRectJitter(cv::RotatedRect r) {
	cv::RotatedRect nr;
	nr.center.x = (int)(r.center.x * 10 + 0.5) / 10.0;
	nr.center.y = (int)(r.center.y * 10 + 0.5) / 10.0;
	nr.size.width = (int)(r.size.width * 10 + 0.5) / 10.0;
	nr.size.height = (int)(r.size.width * 10 + 0.5) / 10.0;
	nr.angle = r.angle;
	return nr;
}

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

		if ((arucoID = im->hasArucoID(r, corners, arucoIds)) >= 0) {
			matchID = findMatchID(arucoIds[arucoID]);
			if (matchID > 0)
			 {
				if ((rep = im->isMarkerOutOfField(trackedMarker[matchID], pcd)) > 0) {
					deleteMarker(matchID);
				}
				else {
					if ((im->isConstantMarker(motionCenterVecs, trackedMarker, takenIDVec, arucoIds[arucoID])) > 0)
						CurrentMarkerWAruco(trackedMarker[matchID], trackedMarker[matchID]->getRect(), arucoIds[arucoID], corners[arucoID].at(0), true);
					else
						CurrentMarkerWAruco(trackedMarker[matchID], r, arucoIds[arucoID], corners[arucoID].at(0), false);
					//printf("HAS id %d \n",matchID);
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
			//printf("isTranslated \n");
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







// Delete Function of Marker
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

void MarkerManagement::registerNewMarker(cv::RotatedRect rect, int arucoID, cv::Point2f anglePoint) {
	int id = openIDQueue.front();
	openIDQueue.pop();
	Marker* m = new Marker(id, rect, anglePoint);
	if (arucoID > 0)m->setArucoID(arucoID);
	m->setTracked(1);
	trackedMarker[id] = m;
	takenIDVec.push_back(id);
}


int MarkerManagement::findMatchID(int arucoID)
{
	for each (int i in takenIDVec)
	{
		if (trackedMarker[i]->getArucoID() == arucoID)
			return trackedMarker[i]->getId();
	}
	return 0;
}

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
	}
}

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

void MarkerManagement::CurrentMarker(Marker* tm, cv::RotatedRect rect) {
	if (tm->isTracked()==0) {
		tm->setRect(rect);
		tm->setTracked(1);
		tm->setVisible(1);
	}
}






MarkerManagement::MarkerManagement(cv::Size frameSize, PlaneCalibration::planeCalibData pcd)
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