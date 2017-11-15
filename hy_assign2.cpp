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
#define CAR_WIDTH 180

using namespace std;
using namespace cv;

typedef struct {
	double CAMERA_HEIGHT, CAMERATOBONNET, BONNETTOCHESS, CHESS_SPACE, CHESS_HEIGHT;
	double CAMERATOCHESS;
}BASE_INFO;

BASE_INFO b;
vector<Point2f> corners;
double vanishingLine;	//�ҽ��� ��ġ
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

	//ü�����带 �̿��ؼ� �ҽǼ��� �ȼ���ġ(vanishingLine)�� ã��
	CHESS_SPACE_PIXEL = abs(corners[0 + W_CORNERS].y - corners[0].y);
	double a = (b.CAMERA_HEIGHT - b.CHESS_HEIGHT) / b.CHESS_SPACE;
	vanishingLine = corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].y - CHESS_SPACE_PIXEL*(a - 1.0);
	//line(img, Point(0, int(vanishingLine)), Point(img_width, int(vanishingLine)), Scalar(255, 0, 0), 5);
	//circle(img, Point(corners[67].x, int(vanishingLine)), 10, Scalar(0, 255, 0), 5);

	//�ҽǼ����� ü���ǹٴ��ִµ������� �ȼ��Ÿ� ���ϱ�
	b2c = getVanishingLine2Chess();


	tempDistance = (b.CAMERATOCHESS*b.CHESS_HEIGHT) / (b.CAMERA_HEIGHT - b.CHESS_HEIGHT);
	double cAnlge = atan(b.CAMERA_HEIGHT / (tempDistance + b.CAMERATOCHESS)) * 180 / PI;
	double pa = b2c / cAnlge;

	double oAngle = atan(b.CAMERA_HEIGHT / (obj+b.CAMERATOBONNET)) * 180 / PI;
	double objLine = vanishingLine + oAngle*pa;

	return objLine;

	
}

void showLane(Mat img, double carWidth, double dis1, double dis2) {

	double tempWidth = carWidth / 2;	//�߾ӿ��� ������ ���α���

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

	double tempWidth = carWidth / 2;	//�߾ӿ��� ������ ���α���

	//double pd = ((W_CORNERS + 1) / 2 * b.CHESS_SPACE) / (corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - corners[(W_CORNERS / 2) + W_CORNERS - 1].x);	//ü���ǿ����� �ȼ��� ��ġ
	double pd = ((W_CORNERS-1)* b.CHESS_SPACE) / (corners[W_CORNERS - 1].x - corners[0].x);	//ü���ǿ����� �ȼ��� ��ġ

	double newpd1 = b2c / (objLine - vanishingLine)*pd;	//Ŭ���Ѱ������� �ȼ��� ��ġ����
	double temp1 = tempWidth / newpd1;	//objLine���� ������ �������� ũ�⸦ ���� ����Ʈ

	Point obj_point1 = { int(img_width / 2 + temp1), (int)objLine };
	Point obj_point2 = { int(img_width / 2 - temp1), (int)objLine };

	//cout << obj_point1 << endl;
	//cout << obj_point2 << endl;

	line(img, obj_point1, obj_point2, Scalar(50, 100, 20), 5);

}

void showHorizontalLane2(Mat img, double carWidth) {

	double tempWidth = carWidth / 2;	//�߾ӿ��� ������ ���α���
	double objLine1 = getObjectLine(500);
	double objLine2 = getObjectLine(800);

	double pd = (CHESS_SPACE_PIXEL / b.CHESS_SPACE);

	double newpd1 = b2c / (objLine1 - vanishingLine)*pd;	//Ŭ���Ѱ������� �ȼ��� ��ġ����
	double temp1 = tempWidth / newpd1;	//objLine���� ������ �������� ũ�⸦ ���� ����Ʈ

	//Point p1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*tempWidth), tempOne };
	Point obj_point1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*tempWidth), (int)objLine1 };
	Point obj_point2 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - pd*tempWidth), (int)objLine1 };

	double newpd2 = b2c / (objLine2 - vanishingLine)*pd;	//Ŭ���Ѱ������� �ȼ��� ��ġ����
	double temp2 = tempWidth / newpd2;	//objLine���� ������ �������� ũ�⸦ ���� ����Ʈ

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
	showLane(img, CAR_WIDTH, getObjectLine(0), vanishingLine);	//���� : 180

	//�������� 180��ŭ �����ִ°� 
	//showHorizontalLane(img, 180, getObjectLine(obj));
	showHorizontalLane2(img, CAR_WIDTH);//5m,8m ���α׸���


	//�̹��� ����
	cvNamedWindow("window");
	imshow("window", img);

	//���콺 �ݹ� ���
	//setMouseCallback("window", onMouse, NULL);

	cvWaitKey(0);
	cvDestroyAllWindows();

	return 0;

}
