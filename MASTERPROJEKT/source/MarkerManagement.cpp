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
	nr.center.x =(int)(r.center.x * 10 + 0.5) / 10.0;
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
		cv::RotatedRect nr = reduceRectJitter(r);

		int matchID = 0;
		int arucoID = 0;
		int rep = 0;

		std::vector<cv::Point2f> motionCenterVecs = im->CalculateMotionVectorCenter(r, trackedMarker, takenIDVec);

		if ((arucoID = im->hasArucoID(r, corners, arucoIds)) >= 0) {
			matchID = findMatchID(arucoIds[arucoID]);
			if (matchID > 0) {
				if ((rep = im->isMarkerOutOfField(trackedMarker[matchID], pcd)) > 0) {
					deleteMarker(matchID);
				}
				else { 
					CurrentMarkerWAruco(trackedMarker[matchID], nr, arucoIds[arucoID], corners[arucoID].at(0)); 
				}
			}
			else if ((matchID = im->isConstantMarker(motionCenterVecs, trackedMarker, takenIDVec, arucoIds[arucoID])) > 0) {
				CurrentMarkerWAruco(trackedMarker[matchID], nr, arucoIds[arucoID], corners[arucoID].at(0));
			}
			else if ((matchID = im->isTranslatedMarker(motionCenterVecs, trackedMarker, takenIDVec, arucoIds[arucoID])) > 0) {
				CurrentMarkerWAruco(trackedMarker[matchID], nr, arucoIds[arucoID], corners[arucoID].at(0));
			}
			else {
				if ((rep = im->isRectOutOfField(r, pcd)) == 0) {
					registerNewMarker(nr, arucoIds[arucoID], corners[arucoID].at(0));
				}
			}
		}
		else if ((rep = im->isRectOutOfField(nr, pcd)) > 0) {
			
		}
		else if ((matchID = im->isConstantMarker(motionCenterVecs, trackedMarker, takenIDVec, -1)) > 0) {
			CurrentMarker(trackedMarker[matchID], nr);
		}
		else if ((matchID = im->isTranslatedMarker(motionCenterVecs, trackedMarker, takenIDVec, -1)) > 0) {
			CurrentMarker(trackedMarker[matchID], nr);
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

void MarkerManagement::CurrentMarkerWAruco(Marker* tm, cv::RotatedRect rect, int arucoID, cv::Point2f anglePoint) {
	tm->setRectWithAngle(rect, anglePoint);
	if (arucoID > 0)tm->setArucoID(arucoID);
	tm->setTracked(1);
	tm->setVisible(1);
}

void MarkerManagement::CurrentMarker(Marker* tm, cv::RotatedRect rect) {
	//rect.angle = tm->getAngle();
	tm->setRect(rect);
	tm->setTracked(1);
	tm->setVisible(1);
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