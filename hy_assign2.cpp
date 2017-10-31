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
double banishingLine;	//�ҽ��� ��ġ
double pa;	//1���� ��Ÿ���� �ȼ���. ü������ĭ*8�� �ٴڿ��� ü���ǰ�������� �ȼ���

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
		//�Ÿ����ϱ�
		double temp = y - banishingLine;//�ҽǼ��� y��ǥ �ȼ���
		double clickedAngle = temp / pa;	//�̰� �츮�� ���� �ٴڿ��� ī�޶���� ����
		double objZ = b.CAMERA_HEIGHT / tan(clickedAngle*PI / 180);

		double b2c = getBanishingLine2Chess();

		//8�� ������� ���η� 8ĭ���ִٴ°�
		double pd = (8 * b.CHESS_SPACE) / (corners[44].x - corners[37].x);	//ü���ǿ����� �ȼ��� ��ġ
		double newpd = b2c / temp*pd;	//Ŭ���Ѱ������� �ȼ��� ��ġ����
		double objX = newpd * (x - corners[67].x);
		cout << "��������Ÿ�: " << objZ << " �������Ÿ�: " << objX << "\n";	//�ȼ��� ��ġ���� Ŭ���� ���� ��ǥ��ŭ �����ذ�
	
	}

}

int main() {

	double obj;
	cout << "�� m �Ÿ��� ǥ���ұ��?\n";
	cin >> obj;
	obj *= 100;

	string imgFilePath;
	cout << "�̹��� ��� �Է� : \n";
	cin >> imgFilePath;

	Mat img = imread(imgFilePath);
	//Mat img = imread("./iphone3_white0.jpg");
	if (!img.data) {
		cout << "�̹��� �б� ����\n";
		return -1;
	}
	
	//����,���� ���ϱ�
	b = readData();
	double img_height = img.size().width;
	double img_width = img.size().height;

	//ü���� �ڳʰ��� �� �׸���
	Mat img_gray;
	cvtColor(img, img_gray, CV_BGR2GRAY);
	bool patternfound = findChessboardCorners(img_gray, Size(15,5), corners);
	drawChessboardCorners(img, Size(15, 5), Mat(corners), patternfound);

	//ü�����带 �̿��ؼ� �ҽǼ��� �ȼ���ġ(banishingLine)�� ã��
	double CHESS_SPACE_PIXEL = abs(corners[7].y - corners[22].y);
	double a = (b.CAMERA_HEIGHT - b.CHESS_HEIGHT) / b.CHESS_SPACE;
	banishingLine = corners[67].y - CHESS_SPACE_PIXEL*(a - 1);
	line(img, Point(0, int(banishingLine)), Point(img_width, int(banishingLine)), Scalar(255,0,0), 5);
	circle(img, Point(corners[67].x, int(banishingLine)), 10, Scalar(0, 255, 0), 5);

	//ü������� ī�޶���� ���� cTheta
	double cTheta = atan((b.CAMERA_HEIGHT - b.CHESS_HEIGHT) / b.CAMERATOCHESS);
	double cAngle = cTheta * 180.0 / PI;
	cout << "ü������� ī�޶���� ���� : " << cAngle << "��\n";

	//1���� ��Ÿ���� �ȼ���. ü������ĭ*8�� �ٴڿ��� ü���ǰ�������� �ȼ���
	pa = (corners[67].y + CHESS_SPACE_PIXEL - banishingLine) / cAngle;

	//input���� ���� ���� �ҽ����� �ȼ��Ÿ� ���̱��ϱ�
	double oTheta = atan(b.CAMERA_HEIGHT / obj);
	double oAngle = oTheta * 180.0 / PI;
	cout << "���ϰ����ϴ� �Ÿ��� ī�޶���� ���� : " << oAngle << "��\n";

	//���ϰ����ϴ� ������ x�ȼ� ��ġ
	double objLine = banishingLine + oAngle*pa;
	line(img, Point(0, int(objLine)), Point(img_width, int(objLine)), Scalar(0, 0, 255), 5);

	//�̹��� ����
	cvNamedWindow("window");
	imshow("window", img);

	//���콺 �ݹ� ���
	setMouseCallback("window", onMouse, NULL);

	cvWaitKey(0);
	cvDestroyAllWindows();
	return 0;
}
