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


int main(int argc, char* argv[])
{
	//2種vector
	vector <Point2d> L_p2d;
	vector <Point3d> L_p3d;
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
	//C矩陣
	vector <vector<double>> L_c;	//2*12
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
	//算S矩陣
	double L_s[12][12];
	for (int i=0;i<12;i++)
	{
		for (int j=0;j<12;j++)
		{
			double L_tmp(0);
			for (int k=0;k<L_c.size();k++)	//L
			{L_tmp+=L_c[k][j]*L_c[k][i];}
			L_s[i][j]=L_tmp;
		}
	}
	Mat L_myMat,L_eigenvalue,L_eigenvector;
	L_myMat= Mat(12,12,CV_64F,L_s);
	eigen(L_myMat,L_eigenvalue,L_eigenvector);	//計算矩陣的eigenvalue和eignvector
	//最小的那一個
	double min;
	int L_num;
	min=L_eigenvalue.at<double>(0,0);
	for(int i = 1; i < 12; i++)		//找L最小的
	{
		if(min>L_eigenvalue.at<double>(i,0))
		{
			min=L_eigenvalue.at<double>(i,0);
			L_num=i;
		}
	}
	//換成3*4的H矩陣
	double L_h[3][4];
	for(int i=0;i<12;i++)
	{ L_h[i/4][i%4]=L_eigenvector.at<double>(L_num,i); }
	cout<<"H1:"<<endl;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<4;j++)
		{	cout<<setw(15)<<L_h[i][j]<<" ";	}
		cout<<endl;
	}
	Mat H1=Mat(3,4,CV_64F,L_h);
	cout<<"================================================="<<endl;
	double w=abs(sqrt(pow(L_h[2][0],2.0)+pow(L_h[2][1],2.0)+pow(L_h[2][2],2.0)));
	cout<<"w = "<<w<<endl;
	double h3[4]={L_h[2][0],L_h[2][1],L_h[2][2],L_h[2][3]};
	double h2[4]={L_h[1][0],L_h[1][1],L_h[1][2],L_h[1][3]};
	double h1[4]={L_h[0][0],L_h[0][1],L_h[0][2],L_h[0][3]};
	double r3[3]={h3[0]/w,h3[1]/w,h3[2]/w};
	double t3=h3[3]/w;
	//P48
	double t_r3[4][1]={{r3[0]},{r3[1]},{r3[2]},{0}};
	Mat t_r3_mat=Mat(4,1,CV_64F,t_r3);
	Mat h1_w_mat=Mat(1,4,CV_64F,h1)/w;
	Mat u0_mat=h1_w_mat*t_r3_mat;
	double u0=u0_mat.at<double>(0,0);
	//P49
	Mat h2_w_mat=Mat(1,4,CV_64F,h2)/w;
	Mat v0_mat=h2_w_mat*t_r3_mat;
	double v0=v0_mat.at<double>(0,0);
	//P50
	Mat h3_w_mat=Mat(1,4,CV_64F,h3)/w;
	Mat fu_tmp=h1_w_mat-u0*(h3_w_mat);	//1*4
	double fu=sqrt(pow(fu_tmp.at<double>(0,0),2.0)
					+pow(fu_tmp.at<double>(0,1),2.0)
					+pow(fu_tmp.at<double>(0,2),2.0));	
	double r1[3]={fu_tmp.at<double>(0,0)/fu,
					fu_tmp.at<double>(0,1)/fu,
					fu_tmp.at<double>(0,2)/fu};
	double t1=fu_tmp.at<double>(0,3)/fu;
	//P51
	Mat fv_tmp=h2_w_mat-v0*(h3_w_mat);	//1*4
	double fv=sqrt(pow(fv_tmp.at<double>(0,0),2.0)
					+pow(fv_tmp.at<double>(0,1),2.0)
					+pow(fv_tmp.at<double>(0,2),2.0));
	double r2[3]={fv_tmp.at<double>(0,0)/fv,
					fv_tmp.at<double>(0,1)/fv,
					fv_tmp.at<double>(0,2)/fv};
	double t2=fv_tmp.at<double>(0,3)/fv;
	
	cout<<"r1 = ";
	for (int i=0;i<3;i++)
	{ cout<<setw(12)<<r1[i]<<" "; }
	cout<<endl;
	cout<<"r2 = ";
	for (int i=0;i<3;i++)
	{ cout<<setw(12)<<r2[i]<<" "; }
	cout<<endl;
	cout<<"r3 = ";
	for (int i=0;i<3;i++)
	{ cout<<setw(12)<<r3[i]<<" "; }
	cout<<endl;
	cout<<"u0 = "<<u0<<endl;
	cout<<"v0 = "<<v0<<endl;
	cout<<"fu = "<<fu<<endl;
	cout<<"fv = "<<fv<<endl;
	cout<<"t1 = "<<t1<<endl;
	cout<<"t2 = "<<t2<<endl;
	cout<<"t3 = "<<t3<<endl;

	return 0;
}