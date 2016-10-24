#include "uEye_input.h"

uEye_input::uEye_input()
{
}

uEye_input::~uEye_input()
{
}
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

	int img_bpp = 24, img_step, img_data_size;
	is_AllocImageMem(hCam, img_width, img_height, img_bpp, &imgMem, &memId);
	is_SetImageMem(hCam, imgMem, memId);
	is_SetDisplayMode(hCam, IS_SET_DM_DIB);
	is_SetColorMode(hCam, IS_CM_BGR8_PACKED);
	is_SetImageSize(hCam, img_width, img_height);


	/*double enable = 1;
	double disable = 0;
	is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_GAIN, &enable, 0);
	is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_WHITEBALANCE, &enable, 0);
	is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_FRAMERATE, &disable, 0);
	is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SHUTTER, &disable, 0);
	is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SENSOR_GAIN, &enable, 0);
	is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SENSOR_WHITEBALANCE, &enable, 0);
	is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SENSOR_SHUTTER, &disable, 0);*/

	double FPS, NEWFPS;
	FPS = 24;
	double fps;
	is_SetFrameRate(hCam, FPS, &NEWFPS);

	double parameter = 40;
	int error = is_Exposure(hCam, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&parameter, sizeof(parameter));
	if (error != IS_SUCCESS) {
	printf("failed Exposure");
	}

	error = is_Exposure(hCam, IS_EXPOSURE_CMD_GET_EXPOSURE, (void*)&parameter, sizeof(parameter));

	UINT uiCaps = 0;

	INT nRet = is_Focus(hCam, FDT_CMD_GET_CAPABILITIES, &uiCaps, sizeof(uiCaps));

	if (nRet == IS_SUCCESS && (uiCaps & FOC_CAP_AUTOFOCUS_SUPPORTED))
	{
		printf("If supported, enable auto focus");
		nRet = is_Focus(hCam, FOC_CMD_SET_DISABLE_AUTOFOCUS, NULL, 0);
	}

	nRet = is_SetGamma(hCam,2200);
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