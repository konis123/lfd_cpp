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
double banishingLine;	//소실점 위치
double pa;	//1도당 나타내는 픽셀수. 체스판한칸*8은 바닥에서 체스판가운데까지의 픽셀수

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

int getBanishingLine2Chess() {

	double oTheta = atan(b.CAMERA_HEIGHT / b.CAMERATOCHESS);
	double oAngle = oTheta * 180 / PI;
	double objLine = banishingLine + oAngle*pa;

	return objLine - banishingLine;
}

void onMouse(int event, int x, int y, int flags, void* userdata) {

	if (event == EVENT_LBUTTONDOWN) {
		//거리구하기
		double temp = y - banishingLine;//소실선과 y좌표 픽셀차
		double clickedAngle = temp / pa;	//이게 우리가 찍은 바닥에서 카메라와의 각도
		double objZ = b.CAMERA_HEIGHT / tan(clickedAngle*PI / 180);

		double b2c = getBanishingLine2Chess();

		//8은 가운데에서 가로로 8칸이있다는거
		double pd = (8 * b.CHESS_SPACE) / (corners[44].x - corners[37].x);	//체스판에서의 픽셀당 센치
		double newpd = b2c / temp*pd;	//클릭한곳에서의 픽셀당 센치미터
		double objX = newpd * (x - corners[67].x);
		cout << "수직방향거리: " << objZ << " 수평방향거리: " << objX << "\n";	//픽셀당 센치에서 클릭한 곳의 좌표만큼 곱해준거
	
	}

}

int main() {

	double obj;
	cout << "몇 m 거리를 표시할까요?\n";
	cin >> obj;
	obj *= 100;

	string imgFilePath;
	cout << "이미지 경로 입력 : \n";
	cin >> imgFilePath;

	Mat img = imread(imgFilePath);
	//Mat img = imread("./iphone3_white0.jpg");
	if (!img.data) {
		cout << "이미지 읽기 실패\n";
		return -1;
	}
	
	//가로,세로 구하기
	b = readData();
	double img_height = img.size().width;
	double img_width = img.size().height;

	//체스판 코너검출 및 그리기
	Mat img_gray;
	cvtColor(img, img_gray, CV_BGR2GRAY);
	bool patternfound = findChessboardCorners(img_gray, Size(15,5), corners);
	drawChessboardCorners(img, Size(15, 5), Mat(corners), patternfound);

	//체스보드를 이용해서 소실선의 픽셀위치(banishingLine)를 찾음
	double CHESS_SPACE_PIXEL = abs(corners[7].y - corners[22].y);
	double a = (b.CAMERA_HEIGHT - b.CHESS_HEIGHT) / b.CHESS_SPACE;
	banishingLine = corners[67].y - CHESS_SPACE_PIXEL*(a - 1);
	line(img, Point(0, int(banishingLine)), Point(img_width, int(banishingLine)), Scalar(255,0,0), 5);
	circle(img, Point(corners[67].x, int(banishingLine)), 10, Scalar(0, 255, 0), 5);

	//체스보드와 카메라와의 각도 cTheta
	double cTheta = atan((b.CAMERA_HEIGHT - b.CHESS_HEIGHT) / b.CAMERATOCHESS);
	double cAngle = cTheta * 180.0 / PI;
	cout << "체스보드와 카메라와의 각도 : " << cAngle << "도\n";

	//1도당 나타내는 픽셀수. 체스판한칸*8은 바닥에서 체스판가운데까지의 픽셀수
	pa = (corners[67].y + CHESS_SPACE_PIXEL - banishingLine) / cAngle;

	//input으로 받은 값과 소실점의 픽셀거리 차이구하기
	double oTheta = atan(b.CAMERA_HEIGHT / obj);
	double oAngle = oTheta * 180.0 / PI;
	cout << "구하고자하는 거리와 카메라와의 각도 : " << oAngle << "도\n";

	//구하고자하는 라인의 x픽셀 위치
	double objLine = banishingLine + oAngle*pa;
	line(img, Point(0, int(objLine)), Point(img_width, int(objLine)), Scalar(0, 0, 255), 5);

	//이미지 띄우기
	cvNamedWindow("window");
	imshow("window", img);

	//마우스 콜백 등록
	setMouseCallback("window", onMouse, NULL);

	cvWaitKey(0);
	cvDestroyAllWindows();
	return 0;
}
