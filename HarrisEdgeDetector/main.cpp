
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <stdio.h>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <math.h>
#include <iostream>


#define blockS 2
#define K 0.04
#define thresh 20
using namespace cv;
using namespace std;


int counts = 0;




Mat image;
Mat gray;
Mat eig_image;
Mat temp_image;
Mat harris;
Mat res;



CvPoint3D32f *corners;


void drawCorners (Mat img, CvPoint3D32f *, int);

void generateCorners( ) {
	int rows;
    int cols;
   // double smallestEigenvalue;
	int i, j;



	// create temporary images to store the two image derivatives

	Mat dx = Mat (gray.rows, gray.cols, CV_16S);
    Mat dy = Mat (gray.rows, gray.cols, CV_16S);



    Sobel(gray, dx,CV_16S , 1, 0, 3, 1, 0, BORDER_DEFAULT );
     Sobel(gray, dy,CV_16S , 0, 1, 3, 1, 0, BORDER_DEFAULT );



CvMat dxA = dx;
CvMat dyA = dy;



	CvMat *dx1 =cvCreateMat(gray.rows, gray.cols, CV_64FC1);
	CvMat *dy1 = cvCreateMat(gray.rows, gray.cols, CV_64FC1);

	cvConvertScale( &dxA, dx1 );
	cvConvertScale( &dyA, dy1 );


    harris = image.clone();


    corners = (CvPoint3D32f*)cvAlloc (1000000);

	//Go through all pixels
	CvMat *M = cvCreateMat(2, 2, CV_64FC1);
	for (int x = 0; x < gray.rows; ++x)
	{
		for (int y = 0; y < gray.cols; ++y)
		{
		    //derivatives
			int Ix2 = 0;
			int Iy2 = 0;
			int Ixy = 0;
			for (int i = x - floor(blockS/2); i <= x + floor(blockS/2); ++i)
			{

				for (int j = y - floor(blockS/2); j <= y + floor(blockS/2); ++j)
				{
					if (i < 0 || j < 0 || i >=gray.rows || j >= gray.cols)
						continue;
					Ix2 += pow(cvmGet(dx1, i, j), 2);
					Iy2 += pow(cvmGet(dy1, i, j), 2);
					Ixy += cvmGet(dx1, i, j) * cvmGet(dy1, i, j);

				}
			}


			cvmSet(M, 0, 0, Ix2);
			cvmSet(M, 0, 1, Ixy);
			cvmSet(M, 1, 0, Ixy);
			cvmSet(M, 1, 1, Iy2);
			CvMat *vecs = cvCreateMat(2, 2, CV_64FC1);
			CvMat *vals = cvCreateMat(1, 2, CV_64FC1);
			//cout<<cvmGet(vals, 0, 0)<<"\n";
			//eigen value of the vec, sum matrix
			cvEigenVV(M, vecs, vals, -1 , -1);

			double det = (cvmGet(vals, 0, 0) * cvmGet(vals, 0, 1));
			double trace = (cvmGet(vals, 0, 0) + cvmGet(vals, 0, 1));
			double R = (det - K * pow(trace, 2));

			//leave points with diff >given threshold
			if (cvmGet(vals, 0, 0) > thresh*9170 && cvmGet(vals, 0, 1) > thresh*9170 && R > thresh*9170)
			{

				CvPoint3D32f point(x, y, 0);
				corners[counts++] = point;
			}


		}
	}





	drawCorners (harris, corners, counts);



}
void drawCorners (Mat img, CvPoint3D32f *corners, int counts) {

    int i;

    CvScalar color = cvScalar(0,0,255);

    for(i = 0; i < counts; i++){
		CvPoint pt = cvPoint(corners[i].y, corners[i].x);
		circle( res, Point(corners[i].y,corners[i].x), 1,  color, 2, 255, 0 );
	}
imshow("result",res);
waitKey(0);
	return;
}



int main() {


	 image = imread("Geneva.tif");
	 res = image.clone();
	 cvtColor(image, image, CV_BGR2GRAY);


	if(image.data == 0){
        cout<<"fail";
	}
imshow("ì",image);

gray= image.clone();




    generateCorners();



    cvWaitKey(0);


    return 0;
}
