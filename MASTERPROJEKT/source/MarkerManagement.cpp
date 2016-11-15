#include "MarkerManagement.h"

std::array<Marker*, 200> MarkerManagement::getTrackedMarker()
{
	return trackedMarker;
}

std::vector<int> MarkerManagement::getTakenIDVec()
{
	return takenIDVec;
}

void MarkerManagement::trackMarker(std::vector<cv::RotatedRect> rect, std::vector<std::vector<cv::Point2f>> corners, std::vector<int> arucoIds, cv::Size size)
{


	IdMapping* im = new IdMapping();
	for each (cv::RotatedRect r in rect)
	{
		int matchID = 0;
		int arucoID = 0;
		std::vector<cv::Point2f> motionCenterVecs = im->CalculateMotionVectorCenter(r, trackedMarker, takenIDVec);

		//test if id matches MarkerRect

		if ((arucoID = im->hasArucoID(r, corners, arucoIds)) > 0) {
			matchID = findMatchID(arucoID);
			if (matchID > 0) {
				CurrentMarker(trackedMarker[matchID], r, arucoID);
			}
			else if ((matchID = im->isConstantMarker(motionCenterVecs, trackedMarker, takenIDVec)) > 0) {
				CurrentMarker(trackedMarker[matchID], r, arucoID);
			}
			else if ((matchID = im->isTranslatedMarker(motionCenterVecs, trackedMarker, takenIDVec)) > 0) {
				CurrentMarker(trackedMarker[matchID], r, arucoID);
			}
			else {
				registerNewMarker(r, arucoID);
			}
		}
		else if ((matchID = im->isConstantMarker(motionCenterVecs, trackedMarker, takenIDVec)) > 0) {
			CurrentMarker(trackedMarker[matchID], r, arucoID);
		}
		else if ((matchID = im->isTranslatedMarker(motionCenterVecs, trackedMarker, takenIDVec)) > 0) {
			CurrentMarker(trackedMarker[matchID], r, arucoID);
		}
	}

	int matchID = 0;
	for (int i = 0; i < takenIDVec.size(); i++)
	{
		int id = takenIDVec[i];
		if (trackedMarker[id]->isTracked() == 0) {
			if ((matchID = im->isMarkerOutOfField(trackedMarker[id],cv::Size(1280,1024))) > 0) {
				deleteMarker(id);
			}
			else 
				trackedMarker[id]->setVisible(0);
		}
		
		// check if devided
		// reset isTracked for next frame
		trackedMarker[id]->setTracked(0);

	}
		delete im;
}





// Delete Function of Marker
void MarkerManagement::deleteMarker(int id)
{
	Marker* m = trackedMarker[id];
	m->setId(0);
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

void MarkerManagement::registerNewMarker(cv::RotatedRect rect, int arucoID) {
	int id = openIDQueue.front();
	openIDQueue.pop();
	Marker* m = new Marker(id, rect);
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

void MarkerManagement::CurrentMarker(Marker* tm, cv::RotatedRect rect, int arucoID) {
	tm->setRect(rect);
	if (arucoID > 0)tm->setArucoID(arucoID);
	tm->setTracked(1);
	tm->setVisible(1);
}



MarkerManagement::MarkerManagement()
{
	for (size_t i = 0; i < trackedMarker.size(); i++)
	{
		if (i > 0)	openIDQueue.push(i);
		trackedMarker[i] = new Marker();
	}
}


MarkerManagement::~MarkerManagement()
{

}