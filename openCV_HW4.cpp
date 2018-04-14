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
	//���vector
	vector <Point2d> p2d;
	vector <Point3d> p3d;
	//�ɮ�Ū�J
	fstream fin;
	double x,y,z;
	fin.open("lcalib.p2d",ios::in);
	while(fin>>x>>y)
	{ p2d.push_back(Point2d(x,y)); }	//��Jvector p2d
	fin.close();
	fin.open("lcalib.p3d",ios::in);
	while(fin>>x>>y>>z)
	{ p3d.push_back(Point3d(x,y,z)); }	//��Jvector p3d
	fin.close();
	//C�x�}
	vector <vector<double>> c;	//2*12
	vector<double> c_tmp;	//1*12
	//PPT��42��
	for (int i=0;i<p2d.size();i++)
	{
		//�Ĥ@��
		c_tmp.push_back(p3d[i].x);
		c_tmp.push_back(p3d[i].y);
		c_tmp.push_back(p3d[i].z);
		c_tmp.push_back(1);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(-(p2d[i].x)*(p3d[i].x));
		c_tmp.push_back(-(p2d[i].x)*(p3d[i].y));
		c_tmp.push_back(-(p2d[i].x)*(p3d[i].z));
		c_tmp.push_back(-(p2d[i].x));
		c.push_back(c_tmp);
		c_tmp.clear();
		//�ĤG��
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(0);
		c_tmp.push_back(p3d[i].x);
		c_tmp.push_back(p3d[i].y);
		c_tmp.push_back(p3d[i].z);
		c_tmp.push_back(1);
		c_tmp.push_back(-(p2d[i].y)*(p3d[i].x));
		c_tmp.push_back(-(p2d[i].y)*(p3d[i].y));
		c_tmp.push_back(-(p2d[i].y)*(p3d[i].z));
		c_tmp.push_back(-(p2d[i].y));
		c.push_back(c_tmp);
		c_tmp.clear();
	}
	//��S�x�}
	double s[12][12];
	for (int i=0;i<12;i++)
	{
		for (int j=0;j<12;j++)
		{
			double tmp(0);
			for (int k=0;k<c.size();k++)
			{tmp+=c[k][j]*c[k][i];}
			s[i][j]=tmp;
		}
	}
	Mat myMat,eigenvalue,eigenvector;
	myMat= Mat(12,12,CV_64F,s);
	eigen(myMat,eigenvalue,eigenvector);	//�p��x�}��eigenvalue�Meignvector
	//��̤peigenvalue��eigenvector
	cout<<"========================="<<endl;
	cout<<"Eigenvalue:"<<endl;
	for(int i=0;i<eigenvalue.rows;i++)
	{
		for(int j=0;j<eigenvalue.cols;j++)
		{cout<<i<<"\t"<<eigenvalue.at<double>(i,j);}	//�L�X����
		cout<<endl;
	}
	double minv=0.0,maxv=0.0;
	double* minp=&minv;
	double* maxp=&maxv;
	minMaxIdx(eigenvalue,minp,maxp);//���̤p(&�̤j)
	cout<<"========================="<<endl;
	cout<<"MIN:\t"<<minv<<endl;	//�L�X
	cout<<"MAX:\t"<<maxv<<endl;
	//�̤p�����@��
	double min;
	int num;
	min=eigenvalue.at<double>(0,0);
	for(int i = 1; i < 12; i++)
	{
		if(min>eigenvalue.at<double>(i,0))
		{
			min=eigenvalue.at<double>(i,0);
			num=i;
		}
	}
	//����3*4��H�x�}
	double h[3][4];
	for(int i=0;i<12;i++)
	{	h[i/4][i%4]=eigenvector.at<double>(num,i);	}
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<4;j++)
		{	cout<<setw(15)<<h[i][j]<<" ";	}
		cout<<endl;
	}
	IplImage *image0;	//�v������Ƶ��c
	image0=cvLoadImage("L000.bmp",CV_LOAD_IMAGE_UNCHANGED);	//Ū���v������Ƶ��c
	Mat mat0(image0,0);	//IplImage��Mat
	Mat img0;
	cvtColor(mat0,img0,CV_GRAY2RGB);//Mat��m��
	for(int i=0;i<p2d.size();i++)
	{
		//2D���I�e����
		circle(img0,Point2d(p2d[i].x,p2d[i].y),3,Scalar(0,0,255),CV_FILLED,CV_AA);
	}
	for(int i=0;i<p3d.size();i++)
	{
		//PPT��41������
		double x3,y3;
		x3=((h[0][0]*p3d[i].x)+(h[0][1]*p3d[i].y)+(h[0][2]*p3d[i].z)+h[0][3])
			/((h[2][0]*p3d[i].x)+(h[2][1]*p3d[i].y)+(h[2][2]*p3d[i].z)+h[2][3]);
		y3=((h[1][0]*p3d[i].x)+(h[1][1]*p3d[i].y)+(h[1][2]*p3d[i].z)+h[1][3])
			/((h[2][0]*p3d[i].x)+(h[2][1]*p3d[i].y)+(h[2][2]*p3d[i].z)+h[2][3]);
		circle(img0,Point2d(x3,y3),3,Scalar(0,255,255),CV_FILLED,CV_AA);	//�e����
	}
	imshow("HW4_000",img0);	// ��ܼv�������
    cvWaitKey(0); // ���d����
	//cvDestroyWindow("HW4_000"); // �P������
    cvReleaseImage(&image0); // ����IplImage��Ƶ��c
	return 0;
}