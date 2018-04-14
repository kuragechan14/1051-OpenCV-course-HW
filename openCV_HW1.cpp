#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <cvaux.h>

using namespace std;

//#define _CAPTURE_FROM_AVI_FILE_

#define OPENCV2411
#ifdef _MSC_VER
#	ifdef OPENCV210
#		pragma comment(lib, "cv210.lib")
#		pragma comment(lib, "cxcore210.lib")
#		pragma comment(lib, "cvaux210.lib")
#		pragma comment(lib, "highgui210.lib")
#	endif
#	ifdef OPENCV220
#		pragma comment(lib, "opencv_core220.lib")
#		pragma comment(lib, "opencv_highgui220.lib")
#	endif
#	ifdef OPENCV231
#		pragma comment(lib, "opencv_core231.lib")
#		pragma comment(lib, "opencv_highgui231.lib")
#		pragma comment(lib, "opencv_imgproc231.lib")
#		pragma comment(lib, "opencv_objdetect231d.lib")
#	endif
#	ifdef OPENCV2411
#		pragma comment(lib, "opencv_core2411d.lib")
#		pragma comment(lib, "opencv_highgui2411d.lib")
#		pragma comment(lib, "opencv_imgproc2411d.lib")
#	endif
#endif
using namespace std;
using namespace cv;

Mat src1, src2;
int sliderValue;

bool g_bDrawing=false;
Point g_nCurrPoint,mp; //取滑鼠座標
int g_radius=5;	//預設畫筆大小
void onMouse(int event, int x, int y, int flag, void *param)  
{  
    //Mat &img = *(cv::Mat*)param;
    switch (event)  
    {
		//按下 
		case CV_EVENT_LBUTTONDOWN:  
			g_bDrawing =true;
			break;  
        //MOVE
		case CV_EVENT_MOUSEMOVE:
			mp=g_nCurrPoint;
			g_nCurrPoint=Point(x,y);
			break;  
        //放開  
		case CV_EVENT_LBUTTONUP:
			g_bDrawing = false;
			break;  
    }
	cout<<event<<"\t"<<x<<","<<y<<endl;
}  


int main(int argc, char* argv[])
{
	VideoCapture capture;
	int width = 0, height = 0;
	Mat frame,frame1; //兩個source
	Mat src1;	//畫圖用
	bool bCapture;
	string win_in("Input"), win_out("Output");
	int flag=0;	//判斷是否有截圖的flag

#ifdef _CAPTURE_FROM_AVI_FILE_
	if (argc != 2) {
		fprintf(stderr, "Usage: %s input-avi-filename\n", *argv);
		exit(1);
	}
	char *AviFileName = * ++argv;
	bCapture = capture.open(AviFileName);
#else
	bCapture = capture.open(0);
#endif
	 
	namedWindow(win_in, cv::WINDOW_NORMAL);
	namedWindow(win_out, cv::WINDOW_NORMAL);
	createTrackbar("radius",win_out,&g_radius, 30, 0); //建立trackbar,最大值30

	bool bQuit = false, bInit = false;
	
	if(bCapture)
	{
		while(!bQuit)
		{
			if(capture.grab()) //捕捉下一帪
			{
				capture.retrieve(frame);

				if (!bInit) {
					width = frame.cols;
					height = frame.rows;

					moveWindow(win_in, 0, 0);
					resizeWindow(win_in,width,height);
					moveWindow(win_out, width+20, 0);
					resizeWindow(win_out,width,height);
					bInit = true;
				}
				imshow(win_in, frame);	//win_in即時imageshow
				switch (waitKey(1)) {
					case 27: // ESC
						bQuit = true;
						break;
					case 'q':
						frame1=frame.clone();		//複製
						imshow(win_out,frame1);		//SHOW
						frame1.copyTo(src1);		//將frame1複製到src1等等畫圖用
						flag=1;
						break;
				}  
				setMouseCallback(win_out,onMouse,NULL);	//讀取滑鼠座標
				if (g_bDrawing && flag==1)  
				{
					line(src1,g_nCurrPoint,mp,Scalar(0,0,255),g_radius,CV_AA);	//在src1上畫線
					//circle(src1,g_nCurrPoint,g_radius,Scalar(0,0,255),CV_FILLED,CV_AA);	//在src1上畫圓
					imshow(win_out,src1);	//將src1立即imageshow出來
				}
				
			}
		}
	}
	return 0;
}