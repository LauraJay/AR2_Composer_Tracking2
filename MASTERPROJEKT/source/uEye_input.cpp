#include "uEye_input.h"

int uEye_input::inituEyeCam() {

	hCam = 0;
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
	
	double pixelClock = 37;
	
	is_PixelClock(hCam,IS_PIXELCLOCK_CMD_SET , &pixelClock, sizeof(pixelClock));

	double FPS, NEWFPS;
	FPS = 23;
	double fps;
	is_SetFrameRate(hCam, FPS, &NEWFPS);

	double parameter = 21;
	int error = is_Exposure(hCam, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&parameter, sizeof(parameter));
	if (error != IS_SUCCESS) {
	printf("failed Exposure");
	}

	double factor = 0.5;
	INT Color = is_SetColorCorrection(hCam, IS_CCOR_ENABLE_NORMAL,&factor );
	INT nRet = is_SetGamma(hCam,1800);
	return 1;

}

cv::Mat uEye_input::getCapturedFrame()
{
	if (is_FreezeVideo(hCam, IS_WAIT) == IS_SUCCESS) {
		void *pMemVoid; //pointer to where the image is stored
		is_GetImageMem(hCam, &pMemVoid);

		IplImage * img;
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
		img->imageData = (char*)pMemVoid;  //the pointer to imagaData
		img->widthStep = 3 * img_width;
		img->imageDataOrigin = (char*)pMemVoid; //and again
		//										//now you can use your img just like a normal OpenCV image
		//cvNamedWindow("A", 1);
		//cvShowImage("A", img);
		//cv::waitKey(1);

		frame = cv::cvarrToMat(img);
	}

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