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
	double CAMERA_HEIGHT, CAMERATOBONNET, BONNETTOCHESS, CHESS_SPACE, CHESS_HEIGHT;
	double CAMERATOCHESS;
}BASE_INFO;

BASE_INFO b;
vector<Point2f> corners;
double vanishingLine;	//�ҽ��� ��ġ
double pa;	//1���� ��Ÿ���� �ȼ���. ü������ĭ*8�� �ٴڿ��� ü���ǰ�������� �ȼ���
double img_height, img_width;
//double objLine;
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

	double tempWidth = carWidth / 2;	//�߾ӿ��� ������ ���α���

	//double pd = ((W_CORNERS + 1) / 2 * b.CHESS_SPACE) / (corners[W_CORNERS*H_CORNERS-(W_CORNERS/2)-1].x - corners[(W_CORNERS/2)+W_CORNERS-1].x);	//ü���ǿ����� �ȼ��� ��ġ
	double pd = ((W_CORNERS-1) * b.CHESS_SPACE) / (corners[W_CORNERS-1].x - corners[0].x);	//ü���ǿ����� �ȼ��� ��ġ


	double newpd1 = b2c / (dis1 - vanishingLine)*pd;	//Ŭ���Ѱ������� �ȼ��� ��ġ����
	double temp1 = tempWidth / newpd1;	//objLine���� ������ �������� ũ�⸦ ���� ����Ʈ

	Point dis1_point1 = { int(img_width / 2 + temp1), (int)dis1 };
	Point dis1_point2 = { int(img_width / 2 - temp1), (int)dis1 };


	double newpd2 = b2c / (dis2 - vanishingLine)*pd;	//Ŭ���Ѱ������� �ȼ��� ��ġ����
	double temp2 = tempWidth / newpd2;	//objLine���� ������ �������� ũ�⸦ ���� ����Ʈ

	Point dis2_point1 = { int(img_width / 2 + temp2), (int)dis2 };
	Point dis2_point2 = { int(img_width / 2 - temp2), (int)dis2 };

	//circle(img, Point(img_width / 2 + objPoint, int(objLine)), 10, Scalar(128, 128, 0), 5);
	//circle(img, Point(img_width / 2 - objPoint, int(objLine)), 10, Scalar(128, 128, 0), 5);

	line(img, dis1_point1, dis2_point1, Scalar(255, 0, 0), 5);
	line(img, dis1_point2, dis2_point2, Scalar(255, 0, 0), 5);

}

void showHorizontalLane(Mat img, double carWidth, double objLine) {

	double tempWidth = carWidth / 2;	//�߾ӿ��� ������ ���α���

	//double pd = ((W_CORNERS + 1) / 2 * b.CHESS_SPACE) / (corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].x - corners[(W_CORNERS / 2) + W_CORNERS - 1].x);	//ü���ǿ����� �ȼ��� ��ġ
	double pd = ((W_CORNERS-1)* b.CHESS_SPACE) / (corners[W_CORNERS - 1].x - corners[0].x);	//ü���ǿ����� �ȼ��� ��ġ

	double newpd1 = b2c / (objLine - vanishingLine)*pd;	//Ŭ���Ѱ������� �ȼ��� ��ġ����
	double temp1 = tempWidth / newpd1;	//objLine���� ������ �������� ũ�⸦ ���� ����Ʈ

	Point obj_point1 = { int(img_width / 2 + temp1), (int)objLine };
	Point obj_point2 = { int(img_width / 2 - temp1), (int)objLine };

	line(img, obj_point1, obj_point2, Scalar(50, 100, 20), 5);



}

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



double getObjectLine(double obj) {

	//ü�����带 �̿��ؼ� �ҽǼ��� �ȼ���ġ(vanishingLine)�� ã��
	double CHESS_SPACE_PIXEL = abs(corners[0+W_CORNERS].y - corners[0].y);
	double a = (b.CAMERA_HEIGHT - b.CHESS_HEIGHT) / b.CHESS_SPACE;
	vanishingLine = corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].y - CHESS_SPACE_PIXEL*(a - 1.0);
	//line(img, Point(0, int(vanishingLine)), Point(img_width, int(vanishingLine)), Scalar(255, 0, 0), 5);
	//circle(img, Point(corners[67].x, int(vanishingLine)), 10, Scalar(0, 255, 0), 5);

	//ü������� ī�޶���� ���� cTheta
	double cTheta = atan((b.CAMERA_HEIGHT - b.CHESS_HEIGHT) / b.CAMERATOCHESS);
	double cAngle = cTheta * 180.0 / PI;
	cout << "ü������� ī�޶���� ���� : " << cAngle << "��\n";

	//1���� ��Ÿ���� �ȼ���. ü������ĭ*8�� �ٴڿ��� ü���ǰ�������� �ȼ���
	pa = (corners[W_CORNERS*H_CORNERS - (W_CORNERS / 2) - 1].y + CHESS_SPACE_PIXEL - vanishingLine) / cAngle;

	//input���� ���� ���� �ҽ����� �ȼ��Ÿ� ���̱��ϱ�
	double oTheta = atan(b.CAMERA_HEIGHT / obj);
	double oAngle = oTheta * 180.0 / PI;
	cout << "���ϰ����ϴ� �Ÿ��� ī�޶���� ���� : " << oAngle << "��\n";

	//�ҽǼ����� ü���ǹٴ��ִµ������� �ȼ��Ÿ� ���ϱ�
	b2c = getVanishingLine2Chess();

	//���ϰ����ϴ� ������ x�ȼ� ��ġ
	double objLine = vanishingLine + oAngle*pa;
	//line(img, Point(0, int(objLine)), Point(img_width, int(objLine)), Scalar(0, 0, 255), 5);


	return objLine;
}

int main() {

	double obj;
	cout << "�� m �Ÿ��� ǥ���ұ��?\n";
	cin >> obj;
	obj *= 100;

	string imgFilePath;
	cout << "�̹��� ��� �Է� : \n";
	cin >> imgFilePath;

	//Mat img = imread(imgFilePath);
	Mat img = imread("./calibration_new.jpg");
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
	showLane(img, 180, getObjectLine(10), vanishingLine);	//���� : 180

	//�������� 180��ŭ �����ִ°� 
	showHorizontalLane(img, 180, getObjectLine(obj));

	//�̹��� ����
	cvNamedWindow("window");
	imshow("window", img);

	//���콺 �ݹ� ���
	setMouseCallback("window", onMouse, NULL);

	cvWaitKey(0);
	cvDestroyAllWindows();

	return 0;

}
