#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include "opencv.hpp"
#include <string>
#include <math.h>

#define PI 3.141592
//체스보드가 짝수X짝수 크기면됨
#define W_CORNERS 5	//15 홀수
#define H_CORNERS 7		//5 홀수
#define CAR_WIDTH 180

using namespace std;
using namespace cv;

typedef struct {
	double CAMERA_HEIGHT, CAMERATOBONNET, BONNETTOCHESS, CHESS_SPACE, CHESS_HEIGHT;
	double CAMERATOCHESS;
}BASE_INFO;

BASE_INFO b;
vector<Point2f> corners;
double vanishingLine;	//소실점 위치
double img_height, img_width;
//double objLine;
double b2c;
double CHESS_SPACE_PIXEL;
double tempDistance;
Mat img;

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
	/*
	double cuTheta = atan(b.CAMERA_HEIGHT / b.CAMERATOCHESS);
	double cuAngle = cuTheta * 180 / PI;
	
	return cuAngle*pa;
	*/
	
	double temp = corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].y + CHESS_SPACE_PIXEL -vanishingLine;
	//line(img, Point(0, temp), Point(1000, temp), Scalar(255, 255, 0), 5);

	return temp;
}

double getObjectLine(double obj) {

	//체스보드를 이용해서 소실선의 픽셀위치(vanishingLine)를 찾음
	CHESS_SPACE_PIXEL = abs(corners[0 + W_CORNERS].y - corners[0].y);
	double a = (b.CAMERA_HEIGHT - b.CHESS_HEIGHT) / b.CHESS_SPACE;
	vanishingLine = corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].y - CHESS_SPACE_PIXEL*(a - 1.0);
	//line(img, Point(0, int(vanishingLine)), Point(img_width, int(vanishingLine)), Scalar(255, 0, 0), 5);
	//circle(img, Point(corners[67].x, int(vanishingLine)), 10, Scalar(0, 255, 0), 5);

	//소실선에서 체스판바닥있는데까지의 픽셀거리 구하기
	b2c = getVanishingLine2Chess();


	tempDistance = (b.CAMERATOCHESS*b.CHESS_HEIGHT) / (b.CAMERA_HEIGHT - b.CHESS_HEIGHT);
	double cAnlge = atan(b.CAMERA_HEIGHT / (tempDistance + b.CAMERATOCHESS)) * 180 / PI;
	double pa = b2c / cAnlge;

	double oAngle = atan(b.CAMERA_HEIGHT / (obj+b.CAMERATOBONNET)) * 180 / PI;
	double objLine = vanishingLine + oAngle*pa;

	return objLine;

	
}

void showLane(Mat img, double carWidth, double dis1, double dis2) {

	double tempWidth = carWidth / 2;	//중앙에서 부터의 가로길이

	int tempOne = int(getObjectLine(b.BONNETTOCHESS));
	double pd = (CHESS_SPACE_PIXEL / b.CHESS_SPACE);
	//line(img, Point(int(img_width / 2 + pd*tempWidth), 300), Point(int(img_width / 2 - pd*tempWidth), 300), Scalar(0, 255, 0), 5);

	Point vPoint = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x), int(vanishingLine) };
	Point p1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*tempWidth), tempOne };
	Point p2 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - pd*tempWidth), tempOne };

	line(img, vPoint, p1, Scalar(255, 0, 0), 5);
	line(img, vPoint, p2, Scalar(255, 0, 0), 5);

}

void showHorizontalLane(Mat img, double carWidth, double objLine) {

	double tempWidth = carWidth / 2;	//중앙에서 부터의 가로길이

	//double pd = ((W_CORNERS + 1) / 2 * b.CHESS_SPACE) / (corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - corners[(W_CORNERS / 2) + W_CORNERS - 1].x);	//체스판에서의 픽셀당 센치
	double pd = ((W_CORNERS-1)* b.CHESS_SPACE) / (corners[W_CORNERS - 1].x - corners[0].x);	//체스판에서의 픽셀당 센치

	double newpd1 = b2c / (objLine - vanishingLine)*pd;	//클릭한곳에서의 픽셀당 센치미터
	double temp1 = tempWidth / newpd1;	//objLine에서 차폭의 반정도의 크기를 갖는 포인트

	Point obj_point1 = { int(img_width / 2 + temp1), (int)objLine };
	Point obj_point2 = { int(img_width / 2 - temp1), (int)objLine };

	//cout << obj_point1 << endl;
	//cout << obj_point2 << endl;

	line(img, obj_point1, obj_point2, Scalar(50, 100, 20), 5);

}

void showHorizontalLane2(Mat img, double carWidth) {

	double tempWidth = carWidth / 2;	//중앙에서 부터의 가로길이
	double objLine1 = getObjectLine(500);
	double objLine2 = getObjectLine(800);

	double pd = (CHESS_SPACE_PIXEL / b.CHESS_SPACE);

	double newpd1 = b2c / (objLine1 - vanishingLine)*pd;	//클릭한곳에서의 픽셀당 센치미터
	double temp1 = tempWidth / newpd1;	//objLine에서 차폭의 반정도의 크기를 갖는 포인트

	//Point p1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*tempWidth), tempOne };
	Point obj_point1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*tempWidth), (int)objLine1 };
	Point obj_point2 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - pd*tempWidth), (int)objLine1 };

	double newpd2 = b2c / (objLine2 - vanishingLine)*pd;	//클릭한곳에서의 픽셀당 센치미터
	double temp2 = tempWidth / newpd2;	//objLine에서 차폭의 반정도의 크기를 갖는 포인트

	Point obj_point3 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*tempWidth), (int)objLine2 };
	Point obj_point4 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - pd*tempWidth), (int)objLine2 };

	cout << "5m" << endl;
	cout << obj_point1 << endl;
	cout << obj_point2 << endl;
	cout << "8m" << endl;
	cout << obj_point3 << endl;
	cout << obj_point4 << endl;

	line(img, obj_point1, obj_point2, Scalar(128, 0, 255), 5);
	line(img, obj_point3, obj_point4, Scalar(0, 0, 255), 5);


}
/*
void onMouse(int event, int x, int y, int flags, void* userdata) {

	if (event == EVENT_LBUTTONDOWN) {
		//거리구하기
		double temp = y - vanishingLine;//소실선과 y좌표 픽셀차
		double clickedAngle = temp / pa;	//이게 우리가 찍은 바닥에서 카메라와의 각도
		double objZ = b.CAMERA_HEIGHT / tan(clickedAngle*PI / 180);

		//8은 가운데에서 가로로 8칸이있다는거
		//double pd = ((W_CORNERS+1)/2 * b.CHESS_SPACE) / (corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - corners[(W_CORNERS / 2) + W_CORNERS - 1].x);	//체스판에서의 픽셀당 센치
		double pd = ((W_CORNERS + 1) / 2 * b.CHESS_SPACE) / (corners[W_CORNERS*H_CORNERS - 1].x - corners[W_CORNERS*H_CORNERS-(W_CORNERS / 2) - 1].x);	//체스판에서의 픽셀당 센치
		double newpd = b2c / temp*pd;	//클릭한곳에서의 픽셀당 센치미터
		double objX = newpd * (x - corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x);
		cout << "수직방향거리: " << objZ << " 수평방향거리: " << objX << "\n";	//픽셀당 센치에서 클릭한 곳의 좌표만큼 곱해준거
		cout << x << "," << y;
	}

}	
*/

int main() {

	double obj;
	cout << "몇 m 거리를 표시할까요?\n";
	cin >> obj;
	obj *= 100;

	string imgFilePath;
	cout << "이미지 경로 입력 : \n";
	cin >> imgFilePath;

	//Mat img = imread(imgFilePath);
	img = imread("./calibration.jpg");
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
	bool patternfound = findChessboardCorners(img_gray, Size(W_CORNERS,H_CORNERS), corners);
	if (patternfound) {
		if (corners[W_CORNERS*H_CORNERS - 1].y < corners[0].y)	reverse(corners.begin(), corners.end());
		drawChessboardCorners(img, Size(W_CORNERS, H_CORNERS), Mat(corners), patternfound);
	}
	else {
		cout << "can not find pattern." << endl;
	}

	//쇼레인
	showLane(img, CAR_WIDTH, getObjectLine(0), vanishingLine);	//차폭 : 180

	//수직으로 180만큼 보여주는거 
	//showHorizontalLane(img, 180, getObjectLine(obj));
	showHorizontalLane2(img, CAR_WIDTH);//5m,8m 라인그리기


	//이미지 띄우기
	cvNamedWindow("window");
	imshow("window", img);

	//마우스 콜백 등록
	//setMouseCallback("window", onMouse, NULL);

	cvWaitKey(0);
	cvDestroyAllWindows();

	return 0;

}
