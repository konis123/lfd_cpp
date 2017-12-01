#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include "opencv.hpp"
#include <string>
#include <math.h>

#define PI 3.141592
//ü�����尡 ¦��X¦�� ũ����
#define W_CORNERS 5	//15 Ȧ��
#define H_CORNERS 7		//5 Ȧ��

using namespace std;
using namespace cv;

typedef struct {
	double CAMERA_HEIGHT, CAMERATOBONNET, BONNETTOCHESS, CHESS_SPACE, CHESS_HEIGHT, CAR_WIDTH;
	double CAMERATOCHESS;
}BASE_INFO;

BASE_INFO b;
vector<Point2f> corners;
double vanishingLine;	//�ҽ��� ��ġ
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

//�ҽǼ����� ü���� �غ������� �ȼ��� ��ȯ�ϴ� �Լ�
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

//�Ű������� ���ϰ����ϴ� �Ÿ�(ex: 5m�̸� 500, 8m�̸� 800)�� �Է¹޾Ƽ� �ش���ġ�� �ش��ϴ� y��ǥ �ȼ���ġ ��ȯ�ϴ� �Լ�
double getObjectLine(double obj) {

	//ü�����带 �̿��ؼ� �ҽǼ��� �ȼ���ġ(vanishingLine)�� ã��
	CHESS_SPACE_PIXEL = abs(corners[0 + W_CORNERS].y - corners[0].y);

	double a = (b.CAMERA_HEIGHT - b.CHESS_HEIGHT) / b.CHESS_SPACE;
	vanishingLine = corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].y - CHESS_SPACE_PIXEL*(a - 1.0);

	//�ҽǼ����� ü���ǹٴ��ִµ������� �ȼ��Ÿ� ���ϱ�
	b2c = getVanishingLine2Chess();

	//ü���� �غ��� �մ� y��ǥ �ȼ���ġ������ �Ÿ����ϱ�....?
	tempDistance = (b.CAMERATOCHESS*b.CHESS_HEIGHT) / (b.CAMERA_HEIGHT - b.CHESS_HEIGHT);

	//���� tempDistance �� ī�޶��� degree ���ϱ�
	double cAnlge = atan(b.CAMERA_HEIGHT / (tempDistance + b.CAMERATOCHESS)) * 180 / PI;
	
	//degree�� �ȼ��� ���ϱ�
	double pa = b2c / cAnlge;

	//���ϰ����ϴ� �Ÿ��� ī�޶��� degree ���ϱ�
	double oAngle = atan(b.CAMERA_HEIGHT / (obj+b.CAMERATOBONNET)) * 180 / PI;

	//���ϰ����ϴ� �Ÿ��� y��ǥ �ȼ����ϱ�
	double objLine = vanishingLine + oAngle*pa;

	cout << obj << "cm���� : " << oAngle << endl;

	return objLine;

	
}

//�̹���, ����, �Ÿ�1, �Ÿ�2 �Է¹޾Ƽ� �Ÿ�1~�Ÿ�2 ���� ������ŭ ����ǥ���ϴ°� (||�䷱ �������)
//�̹����� �Ķ������� ǥ��
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

	//�̰� 5,8m ���� ������ �´� ����Ʈ����
	Point temp1 = getIntersectPoint(Point(0, five), Point(img_width, five), vPoint, p1);
	Point temp2 = getIntersectPoint(Point(0, five), Point(img_width, five), vPoint, p2);

	Point temp3 = getIntersectPoint(Point(0, eight), Point(img_width, eight), vPoint, p1);
	Point temp4 = getIntersectPoint(Point(0, eight), Point(img_width, eight), vPoint, p2);


	circle(img, temp1, 5, Scalar(128,192,45), 3);
	circle(img, temp2, 5, Scalar(128, 192, 45), 3);
	circle(img, temp3, 5, Scalar(128, 192, 45), 3);
	circle(img, temp4, 5, Scalar(128, 192, 45), 3);



	/*
	double tempWidth = b.CAR_WIDTH / 2;	//�߾ӿ��� ������ ���α���

	double pd = (CHESS_SPACE_PIXEL / b.CHESS_SPACE);	//��ġ���ʹ� �ȼ���
	int tempOne = int(getObjectLine(b.BONNETTOCHESS));

	Point obj_point1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*tempWidth*((500 - vanishingLine) / (tempOne - vanishingLine))), (int)500 };
	Point obj_point2 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - pd*tempWidth*((500 - vanishingLine) / (tempOne - vanishingLine))), (int)500 };

	Point obj_point3 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*tempWidth*((800 - vanishingLine) / (tempOne - vanishingLine))), (int)800 };
	Point obj_point4 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - pd*tempWidth*((800 - vanishingLine) / (tempOne - vanishingLine))), (int)800 };


	cout << "�ҽǼ�y" << vanishingLine << endl;
	cout << "";

	Point vPoint = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x), int(vanishingLine) };


	line(img, vPoint, obj_point3, Scalar(0, 255, 0), 5);
	line(img, vPoint, obj_point4, Scalar(0, 255, 0), 5);
	*/


}


//�ܼ�â���� ǥ���ϰ����ϴ� �Ÿ� �Է¹����� ������������ �ȼ���ġ ǥ���ϴ°�
//�̹����� �ʷϻ����� �׸�
void showHorizontalLane(Mat img, double objLine) {

	double pd = (CHESS_SPACE_PIXEL / b.CHESS_SPACE);	//��ġ���ʹ� �ȼ���
	int tempOne = int(getObjectLine(b.BONNETTOCHESS));

	Point obj_point1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*(b.CAR_WIDTH/2)*((objLine - vanishingLine) / (tempOne - vanishingLine))), (int)objLine };
	Point obj_point2 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - pd*(b.CAR_WIDTH/2)*((objLine - vanishingLine) / (tempOne - vanishingLine))), (int)objLine };


	line(img, obj_point1, obj_point2, Scalar(0, 255, 0), 5);

}

//5m�� 8m�Ÿ��� ������������ ǥ���Ѱ�
//�̹����� ���������� �׸�
void showHorizontalLane2(Mat img) {
	/*
	double objLine1 = getObjectLine(500);	//5m �Ÿ��� �ȼ���ǥ���ϱ�
	double objLine2 = getObjectLine(800);	//8m �Ÿ��� �ȼ���ǥ���ϱ�

	double pd = (CHESS_SPACE_PIXEL / b.CHESS_SPACE);	//��ġ���ʹ� �ȼ���

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

/*	�̰� ���ݺ��ϱ� ���� ���϶�� �ϽŰ��� �𸣰�°� �����Ʊ���� ����
void onMouse(int event, int x, int y, int flags, void* userdata) {

	if (event == EVENT_LBUTTONDOWN) {
		//�Ÿ����ϱ�
		double temp = y - vanishingLine;//�ҽǼ��� y��ǥ �ȼ���
		double clickedAngle = temp / pa;	//�̰� �츮�� ���� �ٴڿ��� ī�޶���� ����
		double objZ = b.CAMERA_HEIGHT / tan(clickedAngle*PI / 180);

		//8�� ������� ���η� 8ĭ���ִٴ°�
		//double pd = ((W_CORNERS+1)/2 * b.CHESS_SPACE) / (corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - corners[(W_CORNERS / 2) + W_CORNERS - 1].x);	//ü���ǿ����� �ȼ��� ��ġ
		double pd = ((W_CORNERS + 1) / 2 * b.CHESS_SPACE) / (corners[W_CORNERS*H_CORNERS - 1].x - corners[W_CORNERS*H_CORNERS-(W_CORNERS / 2) - 1].x);	//ü���ǿ����� �ȼ��� ��ġ
		double newpd = b2c / temp*pd;	//Ŭ���Ѱ������� �ȼ��� ��ġ����
		double objX = newpd * (x - corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x);
		cout << "��������Ÿ�: " << objZ << " �������Ÿ�: " << objX << "\n";	//�ȼ��� ��ġ���� Ŭ���� ���� ��ǥ��ŭ �����ذ�
		cout << x << "," << y;
	}

}	
*/

int main() {

	double obj;
	cout << "�� m �Ÿ��� ǥ���ұ��?\n";
	cin >> obj;
	obj *= 100;

	string imgFilePath;
	cout << "�̹��� ��� �Է� : \n";
	cin >> imgFilePath;

	//Mat img = imread(imgFilePath);
	img = imread("./calibration.jpg");
	if (!img.data) {
		cout << "�̹��� �б� ����\n";
		return -1;
	}
	//����,���� ���ϱ�
	b = readData();
	img_height = img.size().height;
	img_width = img.size().width;

	//ü���� �ڳʰ��� �� �׸���
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

	//���
	showLane(img, getObjectLine(0), vanishingLine);	//���� : 180

	//�������� 180��ŭ �����ִ°� 
	showHorizontalLane(img, getObjectLine(obj));	//�Է��� �Ÿ��� ���α��ϱ�
	showHorizontalLane2(img);//5m,8m ���α׸���


	//�̹��� ����
	cvNamedWindow("window");
	imshow("window", img);

	//���콺 �ݹ� ���
	//setMouseCallback("window", onMouse, NULL);

	cvWaitKey(0);
	cvDestroyAllWindows();

	return 0;

}
