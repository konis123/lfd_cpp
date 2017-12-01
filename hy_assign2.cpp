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

using namespace std;
using namespace cv;

typedef struct {
	double CAMERA_HEIGHT, CAMERATOBONNET, BONNETTOCHESS, CHESS_SPACE, CHESS_HEIGHT, CAR_WIDTH;
	double CAMERATOCHESS;
}BASE_INFO;

BASE_INFO b;
vector<Point2f> corners;
double vanishingLine;	//소실점 위치
double img_height, img_width;
double b2c;
double CHESS_SPACE_PIXEL;
double CHESS_SPACE_PIXEL_X;
double tempDistance;
Mat img;

BASE_INFO readData() {

	BASE_INFO b;
	string temp;
	double a[6];

	ifstream in("./data.txt");

	if (in.is_open()) {
		for(int i=0;i<6;i++){
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
	b.CAR_WIDTH = a[5];
	b.CAMERATOCHESS = b.CAMERATOBONNET + b.BONNETTOCHESS;

	return b;
}

//소실선에서 체스판 밑변까지의 픽셀수 반환하는 함수
int getVanishingLine2Chess() {
	
	double temp = corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].y + CHESS_SPACE_PIXEL -vanishingLine;

	return temp;
}

Point getIntersectPoint(Point AP1, Point AP2, Point BP1, Point BP2)
{
	Point IP;
	double t;
	double s;
	double under = (BP2.y - BP1.y)*(AP2.x - AP1.x) - (BP2.x - BP1.x)*(AP2.y - AP1.y);
	if (under == 0) return false;

	double _t = (BP2.x - BP1.x)*(AP1.y - BP1.y) - (BP2.y - BP1.y)*(AP1.x - BP1.x);
	double _s = (AP2.x - AP1.x)*(AP1.y - BP1.y) - (AP2.y - AP1.y)*(AP1.x - BP1.x);


	t = _t / under;
	s = _s / under;

	if (t<0.0 || t>1.0 || s<0.0 || s>1.0) return false;
	if (_t == 0 && _s == 0) return false;

	IP.x = AP1.x + t * (double)(AP2.x - AP1.x);
	IP.y = AP1.y + t * (double)(AP2.y - AP1.y);

	return IP;
}

//매개변수로 구하고자하는 거리(ex: 5m이면 500, 8m이면 800)를 입력받아서 해당위치에 해당하는 y좌표 픽셀위치 반환하는 함수
double getObjectLine(double obj) {

	//체스보드를 이용해서 소실선의 픽셀위치(vanishingLine)를 찾음
	CHESS_SPACE_PIXEL = abs(corners[0 + W_CORNERS].y - corners[0].y);

	double a = (b.CAMERA_HEIGHT - b.CHESS_HEIGHT) / b.CHESS_SPACE;
	vanishingLine = corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].y - CHESS_SPACE_PIXEL*(a - 1.0);

	//소실선에서 체스판바닥있는데까지의 픽셀거리 구하기
	b2c = getVanishingLine2Chess();

	//체스판 밑변이 잇는 y좌표 픽셀위치까지의 거리구하기....?
	tempDistance = (b.CAMERATOCHESS*b.CHESS_HEIGHT) / (b.CAMERA_HEIGHT - b.CHESS_HEIGHT);

	//위의 tempDistance 와 카메라의 degree 구하기
	double cAnlge = atan(b.CAMERA_HEIGHT / (tempDistance + b.CAMERATOCHESS)) * 180 / PI;
	
	//degree당 픽셀수 구하기
	double pa = b2c / cAnlge;

	//구하고자하는 거리와 카메라의 degree 구하기
	double oAngle = atan(b.CAMERA_HEIGHT / (obj+b.CAMERATOBONNET)) * 180 / PI;

	//구하고자하는 거리의 y좌표 픽셀구하기
	double objLine = vanishingLine + oAngle*pa;

	cout << obj << "cm각도 : " << oAngle << endl;

	return objLine;

	
}

//이미지, 차폭, 거리1, 거리2 입력받아서 거리1~거리2 까지 차폭만큼 라인표시하는거 (||요런 모양으로)
//이미지에 파랑색으로 표시
void showLane(Mat img, double dis1, double dis2) {
	
	/*
	double F = b.CAMERATOCHESS*CHESS_SPACE_PIXEL / b.CHESS_SPACE;
	double P = F*b.CAR_WIDTH / 500;// * img_height / img_width;

	int bP = int(P / 2);

	Point vPoint = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x), int(vanishingLine) };
	Point p1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + bP), int(getObjectLine(500)) };
	Point p2 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - bP), int(getObjectLine(500)) };

	line(img, vPoint, p1, Scalar(255, 0, 0), 5);
	line(img, vPoint, p2, Scalar(255, 0, 0), 5);

	cout << "5m" << endl;
	cout << p1 << endl;
	cout << p2 << endl;
	*/

	
	double a = getObjectLine(int(b.BONNETTOCHESS));
	int half_width = b.CAR_WIDTH / 2;
	int aa = half_width / b.CHESS_SPACE;

	Point vPoint = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x), int(vanishingLine) };
	Point p1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + CHESS_SPACE_PIXEL*(b.CAMERATOCHESS/sqrt(b.CHESS_HEIGHT*b.CHESS_HEIGHT + b.CAMERATOCHESS*b.CAMERATOCHESS))*aa), int(a) };
	Point p2 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - CHESS_SPACE_PIXEL*(b.CAMERATOCHESS / sqrt(b.CHESS_HEIGHT*b.CHESS_HEIGHT + b.CAMERATOCHESS*b.CAMERATOCHESS))*aa), int(a) };

	line(img, vPoint, p1, Scalar(255, 0, 0), 5);
	line(img, vPoint, p2, Scalar(255, 0, 0), 5);

	double five = getObjectLine(500);
	double eight = getObjectLine(800);

	//이게 5,8m 에서 차폭에 맞는 포인트들임
	Point temp1 = getIntersectPoint(Point(0, five), Point(img_width, five), vPoint, p1);
	Point temp2 = getIntersectPoint(Point(0, five), Point(img_width, five), vPoint, p2);

	Point temp3 = getIntersectPoint(Point(0, eight), Point(img_width, eight), vPoint, p1);
	Point temp4 = getIntersectPoint(Point(0, eight), Point(img_width, eight), vPoint, p2);


	circle(img, temp1, 5, Scalar(128,192,45), 3);
	circle(img, temp2, 5, Scalar(128, 192, 45), 3);
	circle(img, temp3, 5, Scalar(128, 192, 45), 3);
	circle(img, temp4, 5, Scalar(128, 192, 45), 3);



	/*
	double tempWidth = b.CAR_WIDTH / 2;	//중앙에서 부터의 가로길이

	double pd = (CHESS_SPACE_PIXEL / b.CHESS_SPACE);	//센치미터당 픽셀수
	int tempOne = int(getObjectLine(b.BONNETTOCHESS));

	Point obj_point1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*tempWidth*((500 - vanishingLine) / (tempOne - vanishingLine))), (int)500 };
	Point obj_point2 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - pd*tempWidth*((500 - vanishingLine) / (tempOne - vanishingLine))), (int)500 };

	Point obj_point3 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*tempWidth*((800 - vanishingLine) / (tempOne - vanishingLine))), (int)800 };
	Point obj_point4 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - pd*tempWidth*((800 - vanishingLine) / (tempOne - vanishingLine))), (int)800 };


	cout << "소실선y" << vanishingLine << endl;
	cout << "";

	Point vPoint = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x), int(vanishingLine) };


	line(img, vPoint, obj_point3, Scalar(0, 255, 0), 5);
	line(img, vPoint, obj_point4, Scalar(0, 255, 0), 5);
	*/


}


//콘솔창에서 표시하고자하는 거리 입력받은거 수직방향으로 픽셀위치 표시하는거
//이미지에 초록색으로 그림
void showHorizontalLane(Mat img, double objLine) {

	double pd = (CHESS_SPACE_PIXEL / b.CHESS_SPACE);	//센치미터당 픽셀수
	int tempOne = int(getObjectLine(b.BONNETTOCHESS));

	Point obj_point1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*(b.CAR_WIDTH/2)*((objLine - vanishingLine) / (tempOne - vanishingLine))), (int)objLine };
	Point obj_point2 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - pd*(b.CAR_WIDTH/2)*((objLine - vanishingLine) / (tempOne - vanishingLine))), (int)objLine };


	line(img, obj_point1, obj_point2, Scalar(0, 255, 0), 5);

}

//5m와 8m거리를 수직방향으로 표시한거
//이미지에 빨강색으로 그림
void showHorizontalLane2(Mat img) {
	/*
	double objLine1 = getObjectLine(500);	//5m 거리의 픽셀좌표구하기
	double objLine2 = getObjectLine(800);	//8m 거리의 픽셀좌표구하기

	double pd = (CHESS_SPACE_PIXEL / b.CHESS_SPACE);	//센치미터당 픽셀수

	int tempOne = int(getObjectLine(b.BONNETTOCHESS));

	double F1 = b.BONNETTOCHESS*CHESS_SPACE_PIXEL / b.CHESS_SPACE;
	double P1 = F1*b.CAR_WIDTH / 500;
	int bP1 = int(P1 / 2);

	double F2 = b.BONNETTOCHESS*CHESS_SPACE_PIXEL / b.CHESS_SPACE;
	double P2 = F2*b.CAR_WIDTH / 800;
	int bP2 = int(P2 / 2);

	Point obj_point1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + bP1), (int)objLine1 };
	Point obj_point2 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - bP1), (int)objLine1 };

	Point obj_point3 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + bP2), (int)objLine2 };
	Point obj_point4 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - bP2), (int)objLine2 };

	*/

	double a = getObjectLine(int(b.BONNETTOCHESS));
	int half_width = b.CAR_WIDTH / 2;
	int aa = half_width / b.CHESS_SPACE;

	Point vPoint = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x), int(vanishingLine) };
	Point p1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + CHESS_SPACE_PIXEL * aa), int(a) };
	Point p2 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - CHESS_SPACE_PIXEL * aa), int(a) };

	double five = getObjectLine(500);
	double eight = getObjectLine(800);

	Point temp1 = getIntersectPoint(Point(0, five), Point(img_width, five), vPoint, p1);
	Point temp2 = getIntersectPoint(Point(0, five), Point(img_width, five), vPoint, p2);

	Point temp3 = getIntersectPoint(Point(0, eight), Point(img_width, eight), vPoint, p1);
	Point temp4 = getIntersectPoint(Point(0, eight), Point(img_width, eight), vPoint, p2);
	

	line(img, temp1, temp2, Scalar(0, 0, 255), 5);
	line(img, temp3, temp4, Scalar(0, 0, 255), 5);


}

/*	이건 지금보니까 궂이 왜하라고 하신건지 모르곗는거 지우기아까워서 냅둠
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
	showLane(img, getObjectLine(0), vanishingLine);	//차폭 : 180

	//수직으로 180만큼 보여주는거 
	showHorizontalLane(img, getObjectLine(obj));	//입력한 거리의 라인구하기
	showHorizontalLane2(img);//5m,8m 라인그리기


	//이미지 띄우기
	cvNamedWindow("window");
	imshow("window", img);

	//마우스 콜백 등록
	//setMouseCallback("window", onMouse, NULL);

	cvWaitKey(0);
	cvDestroyAllWindows();

	return 0;

}
