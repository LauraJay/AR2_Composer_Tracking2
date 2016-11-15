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
	for (int i = 0; i < rect.size(); i++)
	{
		cv::RotatedRect r = rect[i];
	
		int matchID = 0;
		int arucoID = 0;
		cv::Size rs = r.size;

		/*if(arucoIds.size()>rect.size())
		 {
			 if (rs.width / rs.height > 0.4 ||
				 rs.height / rs.width > 0.4
				 ) {
				 if (rs.width > rs.height) {
					 cv::Point2f newCenter = cv::Point2f(r.center.x - r.size.width / 4, r.center.y);
					 cv::Size2f newSize = cv::Size2f(markerSize.height, markerSize.height);
					 cv::RotatedRect newOne = cv::RotatedRect(newCenter, newSize, r.angle);
					 r.center.x = r.center.x + r.size.width / 4;
					 r.size.height = markerSize.height;
					 r.size.width = markerSize.height;
					 rect.push_back(newOne);
				 }
				 else {
					 cv::Point2f newCenter = cv::Point2f(r.center.x, r.center.y - r.size.height / 4);
					 cv::Size2f newSize = cv::Size2f(markerSize.height, markerSize.height);
					 cv::RotatedRect newOne = cv::RotatedRect(newCenter, newSize, r.angle);
					 r.center.y = r.center.y + r.size.height / 4;
					 r.size.height = markerSize.height;
					 r.size.width = markerSize.height;
					 rect.push_back(newOne);
				 }
			 }
		}*/

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
			if ((matchID = im->isMarkerOutOfField(trackedMarker[id],frameSize)) > 0) {
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



MarkerManagement::MarkerManagement(cv::Size markerSize,cv::Size frameSize)
{
	for (size_t i = 0; i < trackedMarker.size(); i++)
	{
		if (i > 0)	openIDQueue.push(i);
		trackedMarker[i] = new Marker();
	}
	MarkerManagement::markerSize = markerSize;
	MarkerManagement::frameSize = frameSize;
}


MarkerManagement::~MarkerManagement()
{

}