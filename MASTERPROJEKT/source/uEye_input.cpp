#include "uEye_input.h"

int uEye_input::inituEyeCam() {

	hCam = 1;
	
	
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

	double parameter = 40.;
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
		
        UINT ret = is_CaptureVideo(hCam, IS_DONT_WAIT);
        if (ret != IS_SUCCESS && ret != IS_CAPTURE_RUNNING)
           // printf("ret of capture: %d \r; ", ret);

	return 1;

}



cv::Mat uEye_input::getCapturedFrame()
{
	UINT ret = is_CaptureVideo(hCam, IS_DONT_WAIT);
    if (ret != IS_SUCCESS && ret != IS_CAPTURE_RUNNING)
        printf("ret of capture: %d \r; ", ret);
	
	if (ret==IS_SUCCESS || ret==IS_CAPTURE_RUNNING) {
		void *pMemVoid; //pointer to where the image is stored
       is_GetImageMem(hCam, &pMemVoid);
      
		/*UINT check;
		is_PixelClock(hCam, IS_PIXELCLOCK_CMD_GET, &check, sizeof(check));
		printf("Pixelclock: %d \n", check);*/
		/*double parameter;
		int error = is_Exposure(hCam, IS_EXPOSURE_CMD_GET_EXPOSURE, (void*)&parameter, sizeof(parameter));
		double fps;*/
		//is_GetFramesPerSecond(hCam, &fps);
		//printf("fps camera IN DER SUCCESS: %f \n", fps);
   		img->imageData = (char*)pMemVoid;  //the pointer to imagaData
		img->widthStep = 3 * img_width;		
		img->imageDataOrigin = (char*)pMemVoid; 
        frame = cv::cvarrToMat(img);
        
	}
	
	double fps;
	is_GetFramesPerSecond(hCam, &fps);
    //printf("fps: %f \n", fps);
	return frame;
}

int uEye_input::exitCamera() {
    is_FreeImageMem(hCam, imgMem, memId);
	return is_ExitCamera(hCam);
}

uEye_input::uEye_input()
{
}

uEye_input::~uEye_input()
{
}