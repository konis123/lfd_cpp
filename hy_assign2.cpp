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

//�ҽǼ����� ü���� �غ������� �ȼ��� ��ȯ�ϴ� �Լ�
int getVanishingLine2Chess() {
	
	double temp = corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].y + CHESS_SPACE_PIXEL -vanishingLine;

	return temp;
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

	return objLine;

	
}

//�̹���, ����, �Ÿ�1, �Ÿ�2 �Է¹޾Ƽ� �Ÿ�1~�Ÿ�2 ���� ������ŭ ����ǥ���ϴ°� (||�䷱ �������)
//�̹����� �Ķ������� ǥ��
void showLane(Mat img, double carWidth, double dis1, double dis2) {

	double tempWidth = carWidth / 2;	//�߾ӿ��� ������ ���α���

	int tempOne = int(getObjectLine(b.BONNETTOCHESS));
	double pd = (CHESS_SPACE_PIXEL / b.CHESS_SPACE);

	Point vPoint = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x), int(vanishingLine) };
	Point p1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*tempWidth), tempOne };
	Point p2 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - pd*tempWidth), tempOne };

	line(img, vPoint, p1, Scalar(255, 0, 0), 5);
	line(img, vPoint, p2, Scalar(255, 0, 0), 5);

}


//�ܼ�â���� ǥ���ϰ����ϴ� �Ÿ� �Է¹����� ������������ �ȼ���ġ ǥ���ϴ°�
//�̹����� �ʷϻ����� �׸�
void showHorizontalLane(Mat img, double carWidth, double objLine) {

	double tempWidth = carWidth / 2;	//�߾ӿ��� ������ ���α���

	double pd = (CHESS_SPACE_PIXEL / b.CHESS_SPACE);	//��ġ���ʹ� �ȼ���
	int tempOne = int(getObjectLine(b.BONNETTOCHESS));

	Point obj_point1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*tempWidth*((objLine - vanishingLine) / (tempOne - vanishingLine))), (int)objLine };
	Point obj_point2 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - pd*tempWidth*((objLine - vanishingLine) / (tempOne - vanishingLine))), (int)objLine };

	line(img, obj_point1, obj_point2, Scalar(0, 255, 0), 5);

}

//5m�� 8m�Ÿ��� ������������ ǥ���Ѱ�
//�̹����� ���������� �׸�
void showHorizontalLane2(Mat img, double carWidth) {

	double tempWidth = carWidth / 2;	//�߾ӿ��� ������ ���α���
	double objLine1 = getObjectLine(500);	//5m �Ÿ��� �ȼ���ǥ���ϱ�
	double objLine2 = getObjectLine(800);	//8m �Ÿ��� �ȼ���ǥ���ϱ�

	double pd = (CHESS_SPACE_PIXEL / b.CHESS_SPACE);	//��ġ���ʹ� �ȼ���

	int tempOne = int(getObjectLine(b.BONNETTOCHESS));

	Point obj_point1 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*tempWidth*((objLine1 - vanishingLine) / (tempOne - vanishingLine))), (int)objLine1 };
	Point obj_point2 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - pd*tempWidth*((objLine1 - vanishingLine) / (tempOne - vanishingLine))), (int)objLine1 };

	Point obj_point3 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x + pd*tempWidth*((objLine2 - vanishingLine) / (tempOne - vanishingLine))), (int)objLine2 };
	Point obj_point4 = { int(corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - pd*tempWidth*((objLine2 - vanishingLine) / (tempOne - vanishingLine))), (int)objLine2 };

	cout << "5m" << endl;
	cout << obj_point1 << endl;
	cout << obj_point2 << endl;
	cout << "8m" << endl;
	cout << obj_point3 << endl;
	cout << obj_point4 << endl;

	line(img, obj_point1, obj_point2, Scalar(0, 0, 255), 5);
	line(img, obj_point3, obj_point4, Scalar(0, 0, 255), 5);


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
	showLane(img, CAR_WIDTH, getObjectLine(0), vanishingLine);	//���� : 180

	//�������� 180��ŭ �����ִ°� 
	showHorizontalLane(img, 180, getObjectLine(obj));	//�Է��� �Ÿ��� ���α��ϱ�
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
