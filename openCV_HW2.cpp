#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <cvaux.h>
#include <fstream>

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

int main(int argc, char* argv[])
{
	VideoCapture capture;
	int width = 0, height = 0;
	Mat frame,frame1; //兩個source
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
	bool bQuit = false, bInit = false;

	Mat m1=(Mat_<char>(3,3)<<1,0,-1,  
                              2,0,-2,  
                              1,0,-1);
	Mat m2=(Mat_<char>(3,3)<<1,2,1,  
                              0,0,0,  
                              -1,-2,-1);
	Mat m3=(Mat_<char>(3,3)<<0,1,0,  
                              1,-4,1,  
                              0,1,0);
	Mat m4=(Mat_<char>(3,3)<<1,1,1,  
                              1,1,1,  
                              1,1,1);
	Mat m5=(Mat_<char>(3,3)<<-1,-1,0,  
                              -1,0,1,  
                              0,1,1);
	Mat B;

	if(bCapture)
	{
		while(!bQuit)
		{
			if(capture.grab()) //捕捉下一?
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
						frame1=frame.clone();
						flag=1;
						filter2D (frame1,B,frame1.depth(),m1);
						break;
					case 'w':
						frame1=frame.clone();
						flag=2;
						filter2D (frame1,B,frame1.depth(),m2);
						break;
					case 'e':
						frame1=frame.clone();
						flag=3;
						filter2D (frame1,B,frame1.depth(),m3);
						break;
					case 'r':
						frame1=frame.clone();
						flag=4;
						filter2D (frame1,B,frame1.depth(),m4);
						imshow(win_out,B);
						break;
					case 't':
						frame1=frame.clone();
						flag=5;
						filter2D (frame1,B,frame1.depth(),m5);
						break;
				}
				if (flag!=0)
				{imshow(win_out,B);}
			}
		}
	}
	return 0;
}