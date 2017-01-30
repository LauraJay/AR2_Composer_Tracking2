#include "uEye_input.h"

int uEye_input::inituEyeCam() {





	hCam = 1;
	char* imgMem;
	int memId;
	if (is_InitCamera(&hCam, NULL) != IS_SUCCESS) {
		return 0;
	}

	SENSORINFO sInfo;
	is_GetSensorInfo(hCam, &sInfo);
	img_width = sInfo.nMaxWidth;
	img_height = sInfo.nMaxHeight;
	int img_bpp = 24;
	is_AllocImageMem(hCam, img_width, img_height, img_bpp, &imgMem, &memId);
	is_SetImageMem(hCam, imgMem, memId);
	is_SetDisplayMode(hCam, IS_SET_DM_DIB);
	is_SetColorMode(hCam, IS_CM_BGR8_PACKED);
	is_SetImageSize(hCam, img_width, img_height);

	UINT pixelClock = 37;

	is_PixelClock(hCam, IS_PIXELCLOCK_CMD_SET, &pixelClock, sizeof(pixelClock));
	
	double FPS, NEWFPS;
	FPS = 23;
	double fps;
	is_SetFrameRate(hCam, FPS, &NEWFPS);

	double parameter = 15.;
	int error = is_Exposure(hCam, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&parameter, sizeof(parameter));
	if (error != IS_SUCCESS) {
		printf("failed Exposure");

		is_SetHWGainFactor(hCam,IS_SET_RED_GAIN_FACTOR , 0);
		is_SetHWGainFactor(hCam, IS_SET_GREEN_GAIN_FACTOR, 0);
		is_SetHWGainFactor(hCam, IS_SET_BLUE_GAIN_FACTOR, 0);
		is_SetGainBoost(hCam, IS_SET_GAINBOOST_OFF);

	}


	double factor = 0.5;
	INT Color = is_SetColorCorrection(hCam, IS_CCOR_ENABLE_NORMAL, &factor);

	INT nRet = is_SetGamma(hCam, 2200);

	img = cvCreateImage(cvSize(img_width, img_height), IPL_DEPTH_8U, 3);
		img->nChannels = 3;
		img->alphaChannel = 0;
		img->depth = 8;
		img->dataOrder = 0;
		img->origin = 0;
		img->align = 4;
		img->width = img_width;
		img->height = img_height;
		img->imageSize = 3 * img_width*img_height;
		


	return 1;

}



cv::Mat uEye_input::getCapturedFrame()
{

	/* cv::VideoCapture temp_camera;
   int maxTested = 10;
   for (int i = 0; i < maxTested; i++){
     cv::VideoCapture temp_camera(i);
     bool res = (!temp_camera.isOpened());
     temp_camera.release();
     
   }*/
  


	//INT nNumCam;
	//if (is_GetNumberOfCameras(&nNumCam) == IS_SUCCESS) {
	//	
	//	if (nNumCam >= 1) {
	//		// Liste neu mit passender Größe anlegen
	//		UEYE_CAMERA_LIST* pucl;
	//		pucl = (UEYE_CAMERA_LIST*) new BYTE[sizeof(ULONG) + nNumCam * sizeof(UEYE_CAMERA_INFO)];
	//		pucl->dwCount = nNumCam;

	//		//Kamerainformationen einlesen
	//		if (is_GetCameraList(pucl) == IS_SUCCESS) {
	//			int iCamera;
	//			for (iCamera = 0; iCamera < (int)pucl->dwCount; iCamera++) {
	//				//Kamerainformationenam am Bildschirm ausgeben
	//				printf("Camera %i Id: %d \n", iCamera,
	//					pucl->uci[iCamera].dwCameraID);
	//			}
	//		}
	//		delete[] pucl;
	//	}
	//}


	UINT ret = is_CaptureVideo(hCam, IS_WAIT);
	printf("ret of capture: %d \n",ret);
	
	if (ret==IS_SUCCESS) {
		void *pMemVoid; //pointer to where the image is stored
		is_GetImageMem(hCam, &pMemVoid);
		UINT check;
		is_PixelClock(hCam, IS_PIXELCLOCK_CMD_GET, &check, sizeof(check));
		//printf("Pixelclock: %d \n", check);
		double parameter;
		int error = is_Exposure(hCam, IS_EXPOSURE_CMD_GET_EXPOSURE, (void*)&parameter, sizeof(parameter));
		double fps;
		is_GetFramesPerSecond(hCam, &fps);
		//printf("fps cAMERA: %f \n", fps);
		img->imageData = (char*)pMemVoid;  //the pointer to imagaData
		img->widthStep = 3 * img_width;		
		img->imageDataOrigin = (char*)pMemVoid; //and again
												//										//now you can use your img just like a normal OpenCV image
												//cvNamedWindow("A", 1);
												//cvShowImage("A", img);
												//cv::waitKey(1);

		frame = cv::cvarrToMat(img);
		
	}
	
	double fps;
	is_GetFramesPerSecond(hCam, &fps);
	printf("fps cAMERA: %f \n", fps);
	return frame;
}

int uEye_input::exitCamera() {
	return is_ExitCamera(hCam);
}

uEye_input::uEye_input()
{
}

uEye_input::~uEye_input()
{
}