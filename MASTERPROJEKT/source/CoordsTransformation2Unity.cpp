#include "CoordsTransformation2Unity.h"

// Uses the affine transformation matrix in order to transform all 
// tracked marker positions to Unity Space
// std::array<Marker*, 100>  allMarkers: array of size 100 with all current markers
// std::vector<int> takenIdVec: vector of all valid marker positions in allMarkers array
//@return int: 1 is sucess and -1 not sucessful
int CoordsTransformtion2Untiy::computeTransformation2Unity(std::array<Marker*, 100>  allMarkers, std::vector<int> takenIdVec)
{
	for (int i = 0; i < takenIdVec.size(); i++)
	{
		int id = takenIdVec[i];
		Marker* m = allMarkers[id];
		std::vector<cv::Point3d> tVecs;
		cv::Point3d estiCenter;
		tVecs = m->getTvecs();
		cv::Point3d tVec = tVecs[0];
		estiCenter.x = affineTransform.at<double>(0, 0) * tVec.x + affineTransform.at<double>(0, 1) * tVec.y + affineTransform.at<double>(0, 2)*tVec.z + affineTransform.at<double>(0, 3) * 1;
		estiCenter.y = affineTransform.at<double>(1, 0) * tVec.x + affineTransform.at<double>(1, 1) * tVec.y + affineTransform.at<double>(1, 2)*tVec.z + affineTransform.at<double>(1, 3) * 1;
		estiCenter.z = affineTransform.at<double>(2, 0) * tVec.x + affineTransform.at<double>(2, 1) * tVec.y + affineTransform.at<double>(2, 2)*tVec.z + affineTransform.at<double>(2, 3) * 1;
		m->setEstimatedCenter(estiCenter);
	}
	return 1;
}
