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

	if (corners.size() > 0) {
		for (int i = 0; i < corners.size(); i++)
		{
			std::vector<cv::Point2f> v = corners[i];
			for (int i = 0; i < v.size(); i++)
			{
				cv::Point2f p = v[i];
				p.x = p.x / size.width;
				p.y = p.y / size.height;
				v[i] = p;
			}
			corners[i] = v;
		}
	}

	for each (cv::RotatedRect r in rect)
	{
		cv::RotatedRect normRect = normalizeCoord(r, size);
		IdMapping* im = new IdMapping();
		int matchID = 0;
		int arucoID = 0;
		std::vector<cv::Point2f> motionCenterVecs = im->CalculateMotionVectorCenter(normRect, trackedMarker, takenIDVec);
		//test if id matches MarkerRect
		if ((arucoID = im->hasArucoID(normRect, corners, arucoIds)) > 0) {
			matchID = findMatchID(arucoID);
			if (matchID > 0) {
				CurrentMarker(trackedMarker[matchID], normRect);
				if (arucoID > 0)trackedMarker[matchID]->setArucoID(arucoID);
			}
			else if ((matchID = im->isConstantMarker(motionCenterVecs, trackedMarker, takenIDVec)) > 0) {
				CurrentMarker(trackedMarker[matchID], normRect);
				if (arucoID > 0)trackedMarker[matchID]->setArucoID(arucoID);
			}
			else if ((matchID = im->isTranslatedMarker(motionCenterVecs, trackedMarker, takenIDVec)) > 0) {
				CurrentMarker(trackedMarker[matchID], normRect);
				if (arucoID > 0)trackedMarker[matchID]->setArucoID(arucoID);
			}
			/*else if ((matchID = im->isRotatedMarker(normRect, motionCenterVecs, trackedMarker, takenIDVec)) > 0) {
			CurrentMarker(trackedMarker[matchID], normRect);
			}*/
			else {
				registerNewMarker(normRect, arucoID);
			}
		}
		else if ((matchID = im->isConstantMarker(motionCenterVecs, trackedMarker, takenIDVec)) > 0) {
			CurrentMarker(trackedMarker[matchID], normRect);
			if (arucoID > 0)trackedMarker[matchID]->setArucoID(arucoID);
		}
		else if ((matchID = im->isTranslatedMarker(motionCenterVecs, trackedMarker, takenIDVec)) > 0) {
			CurrentMarker(trackedMarker[matchID], normRect);
			if (arucoID > 0)trackedMarker[matchID]->setArucoID(arucoID);
		}
		/*else if ((matchID = im->isRotatedMarker(normRect, motionCenterVecs, trackedMarker, takenIDVec)) > 0) {
			CurrentMarker(trackedMarker[matchID], normRect);
		}*/
		else {
			registerNewMarker(normRect, arucoID);
			}
		delete im;
	}
}


cv::RotatedRect MarkerManagement::normalizeCoord(cv::RotatedRect r, cv::Size size) {
	cv::Point2f center = r.center;
	center.x = center.x / size.width;
	center.y = center.y / size.height;
	cv::Size2f normSize = cv::Size2f((r.size.width / size.width), (r.size.height / size.height));
	cv::RotatedRect normRect = cv::RotatedRect(center, normSize, r.angle);
	return normRect;
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
	//delete &m;
}


// inits a new trackedMarker

void MarkerManagement::registerNewMarker(cv::RotatedRect normRect, int arucoID) {
	int id = openIDQueue.front();
	openIDQueue.pop();
	Marker* m = new Marker(id, normRect);
	if(arucoID>0)m->setArucoID(arucoID);
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

void MarkerManagement::CurrentMarker(Marker* tm, cv::RotatedRect normRect) {
	//std::vector<cv::Point2f> rectPoints;
	//cv::Point2f p[4];
	//normRect.points(p);
	//for (int i = 0; i < 4; i++)
	//{
	//	rectPoints.push_back(p[i]);
	//}
	/*tm->setPoints(rectPoints, normRect.center);
	tm->setAngle(normRect.angle);*/
	tm->setNormRect(normRect);
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