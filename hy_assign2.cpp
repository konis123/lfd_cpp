#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include "opencv.hpp"
#include <string>
#include <math.h>

#define PI 3.141592

using namespace std;
using namespace cv;

typedef struct {
	double CAMERA_HEIGHT, CAMERATOBONNET, BONNETTOCHESS, CHESS_SPACE, CHESS_HEIGHT;
	double CAMERATOCHESS;
}BASE_INFO;

BASE_INFO b;
vector<Point2f> corners;
double vanishingLine;	//소실점 위치
double pa;	//1도당 나타내는 픽셀수. 체스판한칸*8은 바닥에서 체스판가운데까지의 픽셀수
double img_height, img_width;
double objLine;
double b2c;

BASE_INFO readData() {

	BASE_INFO b;
	string temp;
	double a[5];

	ifstream in("./data.txt");

	if (in.is_open()) {
		for(int i=0;i<5;i++){
			getline(in, temp);
			double pos = temp.find('=');
			a[i] = atoi(temp.substr(pos+1, 10).c_str());
		}
	}
	
	b.CAMERA_HEIGHT = a[0];
	b.CAMERATOBONNET = a[1];
	b.BONNETTOCHESS = a[2];
	b.CHESS_SPACE = a[3];
	b.CHESS_HEIGHT = a[4];
	b.CAMERATOCHESS = b.CAMERATOBONNET + b.BONNETTOCHESS;

	return b;
}

int getVanishingLine2Chess() {

	double cuTheta = atan(b.CAMERA_HEIGHT / b.CAMERATOCHESS);
	double cuAngle = cuTheta * 180 / PI;
	//double chess_under = vanishingLine + oAngle*pa;

	//return chess_under - vanishingLine;
	return cuAngle*pa;
}

void showLane(Mat img, double carWidth, double dis1, double dis2) {

	double tempWidth = carWidth / 2;	//중앙에서 부터의 가로길이

	double pd = (8 * b.CHESS_SPACE) / (corners[44].x - corners[37].x);	//체스판에서의 픽셀당 센치
	

	double newpd1 = b2c / (dis1 - vanishingLine)*pd;	//클릭한곳에서의 픽셀당 센치미터
	double temp1 = tempWidth / newpd1;	//objLine에서 차폭의 반정도의 크기를 갖는 포인트

	Point dis1_point1 = { int(img_width / 2 + temp1), (int)dis1 };
	Point dis1_point2 = { int(img_width / 2 - temp1), (int)dis1 };


	double newpd2 = b2c / (dis2 - vanishingLine)*pd;	//클릭한곳에서의 픽셀당 센치미터
	double temp2 = tempWidth / newpd2;	//objLine에서 차폭의 반정도의 크기를 갖는 포인트

	Point dis2_point1 = { int(img_width / 2 + temp2), (int)dis2 };
	Point dis2_point2 = { int(img_width / 2 - temp2), (int)dis2 };

	//circle(img, Point(img_width / 2 + objPoint, int(objLine)), 10, Scalar(128, 128, 0), 5);
	//circle(img, Point(img_width / 2 - objPoint, int(objLine)), 10, Scalar(128, 128, 0), 5);

	line(img, dis1_point1, dis2_point1, Scalar(255, 0, 0), 5);
	line(img, dis1_point2, dis2_point2, Scalar(255, 0, 0), 5);



}

void onMouse(int event, int x, int y, int flags, void* userdata) {

	if (event == EVENT_LBUTTONDOWN) {
		//거리구하기
		double temp = y - vanishingLine;//소실선과 y좌표 픽셀차
		double clickedAngle = temp / pa;	//이게 우리가 찍은 바닥에서 카메라와의 각도
		double objZ = b.CAMERA_HEIGHT / tan(clickedAngle*PI / 180);

		//8은 가운데에서 가로로 8칸이있다는거
		double pd = (8 * b.CHESS_SPACE) / (corners[44].x - corners[37].x);	//체스판에서의 픽셀당 센치
		double newpd = b2c / temp*pd;	//클릭한곳에서의 픽셀당 센치미터
		double objX = newpd * (x - corners[67].x);
		cout << "수직방향거리: " << objZ << " 수평방향거리: " << objX << "\n";	//픽셀당 센치에서 클릭한 곳의 좌표만큼 곱해준거
	
	}

}



double getObjectLine(double obj) {

	//체스보드를 이용해서 소실선의 픽셀위치(vanishingLine)를 찾음
	double CHESS_SPACE_PIXEL = abs(corners[7].y - corners[22].y);
	double a = (b.CAMERA_HEIGHT - b.CHESS_HEIGHT) / b.CHESS_SPACE;
	vanishingLine = corners[67].y - CHESS_SPACE_PIXEL*(a - 1);
	//line(img, Point(0, int(vanishingLine)), Point(img_width, int(vanishingLine)), Scalar(255, 0, 0), 5);
	//circle(img, Point(corners[67].x, int(vanishingLine)), 10, Scalar(0, 255, 0), 5);

	//체스보드와 카메라와의 각도 cTheta
	double cTheta = atan((b.CAMERA_HEIGHT - b.CHESS_HEIGHT) / b.CAMERATOCHESS);
	double cAngle = cTheta * 180.0 / PI;
	cout << "체스보드와 카메라와의 각도 : " << cAngle << "도\n";

	//1도당 나타내는 픽셀수. 체스판한칸*8은 바닥에서 체스판가운데까지의 픽셀수
	pa = (corners[67].y + CHESS_SPACE_PIXEL - vanishingLine) / cAngle;

	//input으로 받은 값과 소실점의 픽셀거리 차이구하기
	double oTheta = atan(b.CAMERA_HEIGHT / obj);
	double oAngle = oTheta * 180.0 / PI;
	cout << "구하고자하는 거리와 카메라와의 각도 : " << oAngle << "도\n";

	//소실선에서 체스판바닥있는데까지의 픽셀거리 구하기
	b2c = getVanishingLine2Chess();

	//구하고자하는 라인의 x픽셀 위치
	objLine = vanishingLine + oAngle*pa;
	//line(img, Point(0, int(objLine)), Point(img_width, int(objLine)), Scalar(0, 0, 255), 5);


	return objLine;
}

int main() {

	double obj;
	cout << "몇 m 거리를 표시할까요?\n";
	cin >> obj;
	obj *= 100;

	string imgFilePath;
	cout << "이미지 경로 입력 : \n";
	cin >> imgFilePath;

	//Mat img = imread(imgFilePath);
	Mat img = imread("./iphone150.jpeg");
	if (!img.data) {
		cout << "이미지 읽기 실패\n";
		return -1;
	}
	
	//가로,세로 구하기
	b = readData();
	img_height = img.size().height;
	img_width = img.size().width;


	//체스판 코너검출 및 그리기
	Mat img_gray;
	cvtColor(img, img_gray, CV_BGR2GRAY);
	bool patternfound = findChessboardCorners(img_gray, Size(15,5), corners);
	drawChessboardCorners(img, Size(15, 5), Mat(corners), patternfound);
	
	/*
	//체스보드를 이용해서 소실선의 픽셀위치(vanishingLine)를 찾음
	double CHESS_SPACE_PIXEL = abs(corners[7].y - corners[22].y);
	double a = (b.CAMERA_HEIGHT - b.CHESS_HEIGHT) / b.CHESS_SPACE;
	vanishingLine = corners[67].y - CHESS_SPACE_PIXEL*(a - 1);
	line(img, Point(0, int(vanishingLine)), Point(img_width, int(vanishingLine)), Scalar(255,0,0), 5);
	circle(img, Point(corners[67].x, int(vanishingLine)), 10, Scalar(0, 255, 0), 5);

	//체스보드와 카메라와의 각도 cTheta
	double cTheta = atan((b.CAMERA_HEIGHT - b.CHESS_HEIGHT) / b.CAMERATOCHESS);
	double cAngle = cTheta * 180.0 / PI;
	cout << "체스보드와 카메라와의 각도 : " << cAngle << "도\n";

	//1도당 나타내는 픽셀수. 체스판한칸*8은 바닥에서 체스판가운데까지의 픽셀수
	pa = (corners[67].y + CHESS_SPACE_PIXEL - vanishingLine) / cAngle;

	//input으로 받은 값과 소실점의 픽셀거리 차이구하기
	double oTheta = atan(b.CAMERA_HEIGHT / obj);
	double oAngle = oTheta * 180.0 / PI;
	cout << "구하고자하는 거리와 카메라와의 각도 : " << oAngle << "도\n";

	//소실선에서 체스판바닥있는데까지의 픽셀거리 구하기
	b2c = getVanishingLine2Chess();

	//구하고자하는 라인의 x픽셀 위치
	objLine = vanishingLine + oAngle*pa;
	line(img, Point(0, int(objLine)), Point(img_width, int(objLine)), Scalar(0, 0, 255), 5);
	*/

	//double dis1 = getObjectLine(obj);
	//double dis2 = getObjectLine(obj + 300);
	


	//쇼레인
	showLane(img, 180, getObjectLine(10), vanishingLine);	//차폭 : 180

	//이미지 띄우기
	cvNamedWindow("window");
	imshow("window", img);

	//마우스 콜백 등록
	setMouseCallback("window", onMouse, NULL);

	cvWaitKey(0);
	cvDestroyAllWindows();

	return 0;

}
