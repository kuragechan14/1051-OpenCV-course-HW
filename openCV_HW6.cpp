#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <cvaux.h>
#include <fstream>
#include <sstream>

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

bool mouse_up=false;
Point mouse_point; //取滑鼠座標
void onMouse(int event, int x, int y, int flag, void *param)  
{  
    switch (event)  
    {
        //放開  
		case CV_EVENT_LBUTTONUP:
			mouse_up=true;
			mouse_point=Point(x,y);
			break;
    }
}  

int main(int argc, char* argv[])
{
	//四種vector
	vector <Point2d> L_p2d;
	vector <Point3d> L_p3d;
	vector <Point2d> R_p2d;
	vector <Point3d> R_p3d;
	//檔案讀入
	fstream fin;
	double x,y,z;
	fin.open("lcalib.p2d",ios::in);
	while(fin>>x>>y)
	{ L_p2d.push_back(Point2d(x,y)); }		//放入vector L_p2d
	fin.close();
	fin.open("lcalib.p3d",ios::in);
	while(fin>>x>>y>>z)
	{ L_p3d.push_back(Point3d(x,y,z)); }	//放入vector L_p3d
	fin.close();
	fin.open("rcalib.p2d",ios::in);
	while(fin>>x>>y)
	{ R_p2d.push_back(Point2d(x,y)); }		//放入vector R_p2d
	fin.close();
	fin.open("rcalib.p3d",ios::in);
	while(fin>>x>>y>>z)
	{ R_p3d.push_back(Point3d(x,y,z)); }	//放入vector R_p3d
	fin.close();
	//C矩陣
	vector <vector<double>> L_c;	//2*12
	vector <vector<double>> R_c;
	vector<double> c_tmp;	//1*12
	//PPT第42頁
	for (int i=0;i<L_p2d.size();i++)	//L有338筆
	{
		//第一行
		c_tmp.push_back(L_p3d[i].x);
		c_tmp.push_back(L_p3d[i].y);
		c_tmp.push_back(L_p3d[i].z);
		c_tmp.push_back(1);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(-(L_p2d[i].x)*(L_p3d[i].x));
		c_tmp.push_back(-(L_p2d[i].x)*(L_p3d[i].y));
		c_tmp.push_back(-(L_p2d[i].x)*(L_p3d[i].z));
		c_tmp.push_back(-(L_p2d[i].x));
		L_c.push_back(c_tmp);
		c_tmp.clear();
		//第二行
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(L_p3d[i].x);
		c_tmp.push_back(L_p3d[i].y);
		c_tmp.push_back(L_p3d[i].z);
		c_tmp.push_back(1);
		c_tmp.push_back(-(L_p2d[i].y)*(L_p3d[i].x));
		c_tmp.push_back(-(L_p2d[i].y)*(L_p3d[i].y));
		c_tmp.push_back(-(L_p2d[i].y)*(L_p3d[i].z));
		c_tmp.push_back(-(L_p2d[i].y));
		L_c.push_back(c_tmp);
		c_tmp.clear();
	}
	for (int i=0;i<R_p2d.size();i++)	//R有321筆
	{
		//第一行
		c_tmp.push_back(R_p3d[i].x);
		c_tmp.push_back(R_p3d[i].y);
		c_tmp.push_back(R_p3d[i].z);
		c_tmp.push_back(1);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(-(R_p2d[i].x)*(R_p3d[i].x));
		c_tmp.push_back(-(R_p2d[i].x)*(R_p3d[i].y));
		c_tmp.push_back(-(R_p2d[i].x)*(R_p3d[i].z));
		c_tmp.push_back(-(R_p2d[i].x));
		R_c.push_back(c_tmp);
		c_tmp.clear();
		//第二行
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(R_p3d[i].x);
		c_tmp.push_back(R_p3d[i].y);
		c_tmp.push_back(R_p3d[i].z);
		c_tmp.push_back(1);
		c_tmp.push_back(-(R_p2d[i].y)*(R_p3d[i].x));
		c_tmp.push_back(-(R_p2d[i].y)*(R_p3d[i].y));
		c_tmp.push_back(-(R_p2d[i].y)*(R_p3d[i].z));
		c_tmp.push_back(-(R_p2d[i].y));
		R_c.push_back(c_tmp);
		c_tmp.clear();
	}
	//算S矩陣
	double L_s[12][12],R_s[12][12];
	for (int i=0;i<12;i++)
	{
		for (int j=0;j<12;j++)
		{
			double L_tmp(0),R_tmp(0);
			for (int k=0;k<L_c.size();k++)	//L
			{L_tmp+=L_c[k][j]*L_c[k][i];}
			L_s[i][j]=L_tmp;
			for (int k=0;k<R_c.size();k++)	//R
			{R_tmp+=R_c[k][j]*R_c[k][i];}
			R_s[i][j]=R_tmp;
		}
	}
	Mat L_myMat,L_eigenvalue,L_eigenvector;
	Mat R_myMat,R_eigenvalue,R_eigenvector;
	L_myMat= Mat(12,12,CV_64F,L_s);
	R_myMat= Mat(12,12,CV_64F,R_s);
	eigen(L_myMat,L_eigenvalue,L_eigenvector);	//計算矩陣的eigenvalue和eignvector
	eigen(R_myMat,R_eigenvalue,R_eigenvector);
	//最小的那一個
	double min;
	int L_num,R_num;
	min=L_eigenvalue.at<double>(0,0);
	for(int i = 1; i < 12; i++)		//找L最小的
	{
		if(min>L_eigenvalue.at<double>(i,0))
		{
			min=L_eigenvalue.at<double>(i,0);
			L_num=i;
		}
	}
	min=R_eigenvalue.at<double>(0,0);
	for(int i = 1; i < 12; i++)		//找R最小的
	{
		if(min>R_eigenvalue.at<double>(i,0))
		{
			min=R_eigenvalue.at<double>(i,0);
			R_num=i;
		}
	}
	//換成3*4的H矩陣
	double L_h[3][4],R_h[3][4];
	for(int i=0;i<12;i++)
	{	
		L_h[i/4][i%4]=L_eigenvector.at<double>(L_num,i);
		R_h[i/4][i%4]=R_eigenvector.at<double>(R_num,i);
	}
	cout<<"H1:"<<endl;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<4;j++)
		{	cout<<setw(15)<<L_h[i][j]<<" ";	}
		cout<<endl;
	}
	cout<<"H2:"<<endl;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<4;j++)
		{	cout<<setw(15)<<R_h[i][j]<<" ";	}
		cout<<endl;
	}
	Mat H1=Mat(3,4,CV_64F,L_h);
	Mat H2=Mat(3,4,CV_64F,R_h);
	double P1[3][3],inv_P1[3][3],p1[3],C1[4];
	for(int i=0;i<3;i++)	//P1 p1
	{
		for(int j=0;j<3;j++)
		{ P1[i][j]=L_h[i][j]; }
		p1[i]=L_h[i][3];
	}
	Mat P1_mat=Mat(3,3,CV_64F,P1);
	Mat inv_P1_mat=P1_mat.inv();	//P1的反矩陣
	for (int i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
		{
			inv_P1[i][j]=inv_P1_mat.at<double>(i,j);
		}
	}
	Mat p1_mat=Mat(3,1,CV_64F,p1);
	Mat C1_tmp = (-1)*inv_P1_mat*p1_mat;	//C1 = 負的P1反矩陣 * p1
	for (int i=0;i<3;i++)
	{
		C1[i] = C1_tmp.at<double>(i,0);
	}
	C1[3]=1;
	Mat C1_mat=Mat(4,1,CV_64F,C1);
	double e2[3];
	Mat e2_mat=H2*C1_mat;	//e2=H2*C1
	for (int i=0;i<3;i++)
	{ e2[i]=e2_mat.at<double>(i,0); }
	double e2x[3][3]={
		{0,-e2[2],e2[1]},{e2[2],0,-e2[0]},{-e2[1],e2[0],0}
	};
	Mat e2x_mat=Mat(3,3,CV_64F,e2x);
	double P2[3][3];
	for(int i=0;i<3;i++)	//P2
	{
		for(int j=0;j<3;j++)
		{ P2[i][j]=R_h[i][j]; }
	}
	Mat P2_mat=Mat(3,3,CV_64F,P2);
	Mat F=e2x_mat*P2_mat*inv_P1_mat;
	cout<<"================================================="<<endl;
	cout<<"P1:"<<endl;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{	cout<<setw(15)<<P1[i][j]<<" ";	}
		cout<<endl;
	}
	cout<<"p1:"<<endl;
	for(int i=0;i<3;i++)
	{
		cout<<setw(15)<<p1[i]<<" ";
	}
	cout<<endl;
	cout<<"inv_P1:"<<endl;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{	cout<<setw(15)<<inv_P1[i][j]<<" ";	}
		cout<<endl;
	}
	cout<<"================================================="<<endl;
	cout<<"C1:"<<endl;
	for(int i=0;i<4;i++)
	{
		cout<<setw(15)<<C1[i]<<" ";
	}
	cout<<endl;
	cout<<"================================================="<<endl;
	cout<<"e2:"<<endl;
	for(int i=0;i<3;i++)
	{
		cout<<setw(15)<<e2[i]<<" ";
	}
	cout<<endl;
	cout<<"================================================="<<endl;
	cout<<"F:"<<endl;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{cout<<setw(15)<<F.at<double>(i,j)<<" ";}
		cout<<endl;
	}
	cout<<endl;
	Mat F_t=F.t();

	IplImage *imageL,*imageR;	//影像的資料結構
	imageL=cvLoadImage("L000.bmp",CV_LOAD_IMAGE_UNCHANGED);	//讀取影像的資料結構
	imageR=cvLoadImage("R000.bmp",CV_LOAD_IMAGE_UNCHANGED);
	Mat matL(imageL,0),matR(imageR,0);	//IplImage轉Mat
	Mat imgL,imgR;
	cvtColor(matL,imgL,CV_GRAY2RGB);//Mat轉彩色
	cvtColor(matR,imgR,CV_GRAY2RGB);

	imshow("HW6_L",imgL);	// 顯示影像於視窗
	imshow("HW6_R",imgR);
	bool bQuit=false;
	while (!bQuit)
	{
		switch (waitKey(1)){
			case 27: // ESC
				bQuit = true;
				break;
		}
		setMouseCallback("HW6_L",onMouse,NULL);	//讀取滑鼠座標
		if (mouse_up)  
		{
			cout<<"MP:("<<mouse_point.x<<","<<mouse_point.y<<")"<<endl;
			double m[3]={mouse_point.x,mouse_point.y,1};
			Mat m_tmp=Mat(3,1,CV_64F,m);
			Mat R_multi=F*m_tmp;
			double R_a=R_multi.at<double>(0,0);
			double R_b=R_multi.at<double>(1,0);
			double R_c=R_multi.at<double>(2,0);
			Point2d p1,p2;
			p1.x=0;		//R左端點
			p1.y=(-1)*R_c/R_b;
			p2.x=639;	//R右端點
			p2.y=(-1)*(R_c+639*R_a)/R_b;
			cout<<"R\tP1("<<p1.x<<","<<p1.y<<")\tP2("<<p2.x<<","<<p2.y<<")"<<endl;
			line(imgR,p1,p2,Scalar(0,0,255),2,CV_AA);	//在R畫圖

			double u_tmp=(-1)*R_c/R_a;	//u'
			double v_tmp=0;	//v'
			double n[3]={u_tmp,v_tmp,1};
			Mat n_tmp=Mat(3,1,CV_64F,n);
			Mat L_multi=F.t()*n_tmp;
			double L_a=L_multi.at<double>(0,0);
			double L_b=L_multi.at<double>(1,0);
			double L_c=L_multi.at<double>(2,0);
			p1.x=0;		//L左端點
			p1.y=(-1)*L_c/L_b;
			p2.x=639;	//L右端點
			p2.y=(-1)*(L_c+639*L_a)/L_b;
			cout<<"L\tP1("<<p1.x<<","<<p1.y<<")\tP2("<<p2.x<<","<<p2.y<<")"<<endl;
			line(imgL,p1,p2,Scalar(0,0,255),2,CV_AA);	//在L畫圖

			imshow("HW6_L",imgL);
			imshow("HW6_R",imgR);
			cout<<endl;
			mouse_up=false;
		}
		
	}
    cvWaitKey(0); // 停留視窗
    cvReleaseImage(&imageL); // 釋放IplImage資料結構
	cvReleaseImage(&imageR);

	return 0;
}