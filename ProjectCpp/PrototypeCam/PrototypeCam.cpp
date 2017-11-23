#include <iostream>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <conio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#pragma comment(lib, "Winmm.lib")

using namespace cv;
using namespace std;
//Init Comment
const int FRAME_WIDTH = 576;
const int FRAME_HEIGHT = 1024;
int thresh = 50, N = 5;
const char* wndname = "Square Detection Demo";

int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

void morphOps(Mat &thresh) {

Mat erodeElement = getStructuringElement(MORPH_RECT, Size(1, 1));
Mat dilateElement = getStructuringElement(MORPH_RECT, Size(12, 12));
erode(thresh, thresh, erodeElement);
erode(thresh, thresh, erodeElement);
dilate(thresh, thresh, dilateElement);
dilate(thresh, thresh, dilateElement);
}

void frameFinder(String zmiennaWejsciowa) {
Mat imgOriginal;
Mat hsvImg;
Mat binaryImg;
Mat binaryImg1;
Mat binaryImg2;

imgOriginal = imread("TestImages/FrameImages/InputTestImages/" + zmiennaWejsciowa + ".jpg", 1);
if (imgOriginal.empty()) {
cout << "error: image not read from file\n\n";
_getch();
system("PAUSE");
}

bool trackObjects = false;
bool useMorphOps = true;

cvtColor(imgOriginal, hsvImg, COLOR_BGR2HSV);
inRange(hsvImg, Scalar(168, 123, 0), Scalar(255, 256, 255), binaryImg1);
inRange(hsvImg, Scalar(35, 208, 121), Scalar(255, 255, 255), binaryImg2);
add(binaryImg1, binaryImg2, binaryImg);
resize(imgOriginal, imgOriginal, Size(FRAME_WIDTH, FRAME_HEIGHT));
resize(hsvImg, hsvImg, Size(FRAME_WIDTH, FRAME_HEIGHT));
if (useMorphOps)
morphOps(binaryImg);
waitKey(30);
imwrite("TestImages/FrameImages/OutputTestImages/" + zmiennaWejsciowa + "OutputImageTest.jpg", binaryImg);
}

String numberToString(int Number) {
stringstream ss;
ss << Number;
return ss.str();
}

bool flashFinder(Mat imgOriginal) {
Mat hsvImg;
Mat binaryImg;
Mat binaryImg1;
Mat binaryImg2;
vector<Vec3f> v3fCircles;
Point poziomXpoczatek;
Point poziomXkoniec;
Point poziomYpoczatek;
Point poziomYkoniec;
String label;

bool trackObjects = false;
bool useMorphOps = true;
bool ifFind = false;


	


	cvtColor(imgOriginal, hsvImg, COLOR_BGR2HSV);
	inRange(hsvImg, Scalar(0, 0, 251), Scalar(255, 4, 255), binaryImg1);
	inRange(hsvImg, Scalar(0, 0, 251), Scalar(255, 4, 255), binaryImg2);
	add(binaryImg1, binaryImg2, binaryImg);
	resize(imgOriginal, imgOriginal, Size(FRAME_WIDTH, FRAME_HEIGHT));
	resize(binaryImg, binaryImg, Size(FRAME_WIDTH, FRAME_HEIGHT));
	resize(hsvImg, hsvImg, Size(FRAME_WIDTH, FRAME_HEIGHT));
	if (useMorphOps)
		morphOps(binaryImg);

	HoughCircles(binaryImg, v3fCircles, CV_HOUGH_GRADIENT, 2, binaryImg.rows / 4, 40, 20, 5, 400);
	if (v3fCircles.size() == 2 || v3fCircles.size() == 3)
		ifFind = true;

	for (int i = 0; i < v3fCircles.size(); i++) {
		cout << "Point position x = " << v3fCircles[0][0] << ", y = " << v3fCircles[0][1] << "\n";
		Point p(v3fCircles[0][0], v3fCircles[0][1]);

		poziomXpoczatek.x = (int)(v3fCircles[0][0] - 10);
		poziomXpoczatek.y = (int)v3fCircles[0][1];
		poziomXkoniec.x = (int)(v3fCircles[0][0] + 10);
		poziomXkoniec.y = (int)v3fCircles[0][1];

		poziomYpoczatek.x = (int)v3fCircles[0][0];
		poziomYpoczatek.y = (int)(v3fCircles[0][1] - 10);
		poziomYkoniec.x = (int)v3fCircles[0][0];
		poziomYkoniec.y = (int)(v3fCircles[0][1] + 10);

		line(imgOriginal, poziomXpoczatek, poziomXkoniec, Scalar(0, 0, 255), 2, 8, 0);
		line(imgOriginal, poziomYpoczatek, poziomYkoniec, Scalar(0, 0, 255), 2, 8, 0);
		putText(imgOriginal, label, Point(10, 20), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255, 255, 255), 2.0);
		imshow("Elo elo", imgOriginal);
		imshow("sd", binaryImg);
		waitKey(30);
}
//imwrite("TestImages/FlashImages/OutputTestImages/Binary/" + zmiennaWejsciowa + "BinaryImageTest.jpg", binaryImg);
//imwrite("TestImages/FlashImages/OutputTestImages/Cross/" + zmiennaWejsciowa + "CrossImageTest.jpg", imgOriginal);

return ifFind;
}

static double angle(Point pt1, Point pt2, Point pt0) {
double dx1 = pt1.x - pt0.x;
double dy1 = pt1.y - pt0.y;
double dx2 = pt2.x - pt0.x;
double dy2 = pt2.y - pt0.y;
return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

static void findSquares(const Mat& image, vector<vector<Point> >& squares) {
squares.clear();
Mat timg(image);
medianBlur(image, timg, 9);
Mat gray0(timg.size(), CV_8U), gray;

vector<vector<Point> > contours;

for (int c = 0; c < 3; c++) {
int ch[] = { c, 0 };
mixChannels(&timg, 1, &gray0, 1, ch, 1);

for (int l = 0; l < N; l++) {
if (l == 0) {
Canny(gray0, gray, 5, thresh, 5);
dilate(gray, gray, Mat(), Point(-1, -1));
}
else {
gray = gray0 >= (l + 1) * 255 / N;
}

findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

vector<Point> approx;

for (size_t i = 0; i < contours.size(); i++) {
approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
if (approx.size() == 4 && fabs(contourArea(Mat(approx))) > 1000 && isContourConvex(Mat(approx))) {
double maxCosine = 0;

for (int j = 2; j < 5; j++) {
double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
maxCosine = MAX(maxCosine, cosine);
}
if (maxCosine < 0.3) {
squares.push_back(approx);
}
}
}
}
}
}

//Przerobic na funkcje przyjmujaca 3 punkty i zwracaj�ca index


//	(d/D)*3
//	d - odleglosc punktu testowyPunkcik od poczatku
//	D - szerokosc ramki
// tak samo z wysokoscia (d/D)*4
//Dodac rozciaganie ramki z 4 punktow nie z dwoch ;)
//Zapisanie sciezek audio w tablicy

static void drawSquares(Mat& image, const vector<vector<Point> >& squares, Point testowyPunkcik, String zmiennaWejsciowa) {
const Point* p = &squares[1][0];
int n = (int)squares[1].size();
if (p->x > 3 && p->y > 3)
polylines(image, &p, &n, 1, true, Scalar(0, 255, 0), 3, LINE_AA);

int wysokosc;
int szerokosc;

wysokosc = abs(squares[1][0].y - squares[4][2].y);
szerokosc = abs(squares[4][2].x - squares[1][0].x);

int szynaczy = szerokosc / 3;
int wynary = wysokosc / 4;

if (testowyPunkcik.x > squares[1][0].x && testowyPunkcik.x < squares[1][0].x + szynaczy) {
if (testowyPunkcik.y > squares[1][0].y && testowyPunkcik.y < squares[1][0].y + wynary) {
cout << "PIERWSZY KWADRAT" << endl;
PlaySound(TEXT("C:/opencv/ProjectAppsBinaries/testSounds/1.wav"), NULL, SND_ASYNC);
}
else if (testowyPunkcik.y > squares[1][0].y + wynary && testowyPunkcik.y < squares[1][0].y + (2 * wynary)) {
cout << "CZWARTY KWADRAT" << endl;
PlaySound(TEXT("C:/opencv/ProjectAppsBinaries/testSounds/4.wav"), NULL, SND_ASYNC);
}
else if (testowyPunkcik.y > squares[1][0].y + 2 * wynary && testowyPunkcik.y < squares[1][0].y + (3 * wynary)) {
cout << "SIODMY KWADRAT" << endl;
PlaySound(TEXT("C:/opencv/ProjectAppsBinaries/testSounds/7.wav"), NULL, SND_ASYNC);

}
else if (testowyPunkcik.y > squares[1][0].y + 3 * wynary && testowyPunkcik.y < squares[1][0].y + (4 * wynary)) {
cout << "DZIESIATY KWADRAT" << endl;
PlaySound(TEXT("C:/opencv/ProjectAppsBinaries/testSounds/10.wav"), NULL, SND_ASYNC);
}

}
else if (testowyPunkcik.x > squares[1][0].x + szynaczy && testowyPunkcik.x < squares[1][0].x + (2 * szynaczy)) {
if (testowyPunkcik.y > squares[1][0].y && testowyPunkcik.y < squares[1][0].y + wynary) {
cout << "DRUGI KWADRAT" << endl;
PlaySound(TEXT("C:/opencv/ProjectAppsBinaries/testSounds/2.wav"), NULL, SND_ASYNC);
}
else if (testowyPunkcik.y > squares[1][0].y + wynary && testowyPunkcik.y < squares[1][0].y + (2 * wynary)) {
cout << "PIATY KWADRAT" << endl;
PlaySound(TEXT("C:/opencv/ProjectAppsBinaries/testSounds/5.wav"), NULL, SND_ASYNC);
}
else if (testowyPunkcik.y > squares[1][0].y + 2 * wynary && testowyPunkcik.y < squares[1][0].y + (3 * wynary)) {
cout << "OSMY KWADRAT" << endl;
PlaySound(TEXT("C:/opencv/ProjectAppsBinaries/testSounds/8.wav"), NULL, SND_ASYNC);
}
else if (testowyPunkcik.y > squares[1][0].y + 3 * wynary && testowyPunkcik.y < squares[1][0].y + (4 * wynary)) {
cout << "JEDENASTY KWADRAT" << endl;
PlaySound(TEXT("C:/opencv/ProjectAppsBinaries/testSounds/11.wav"), NULL, SND_ASYNC);
}
}
else if (testowyPunkcik.x > squares[1][0].x + (2 * szynaczy) && testowyPunkcik.x < squares[1][0].x + (3 * szynaczy)) {
if (testowyPunkcik.y > squares[1][0].y && testowyPunkcik.y < squares[1][0].y + wynary) {
cout << "TRZECI KWADRAT" << endl;
PlaySound(TEXT("C:/opencv/ProjectAppsBinaries/testSounds/3.wav"), NULL, SND_ASYNC);
}
else if (testowyPunkcik.y > squares[1][0].y + wynary && testowyPunkcik.y < squares[1][0].y + (2 * wynary)) {
cout << "SZOSTY KWADRAT" << endl;
PlaySound(TEXT("C:/opencv/ProjectAppsBinaries/testSounds/6.wav"), NULL, SND_ASYNC);
}
else if (testowyPunkcik.y > squares[1][0].y + 2 * wynary && testowyPunkcik.y < squares[1][0].y + (3 * wynary)) {
cout << "DZIEWIATY KWADRAT" << endl;
PlaySound(TEXT("C:/opencv/ProjectAppsBinaries/testSounds/9.wav"), NULL, SND_ASYNC);
}
else if (testowyPunkcik.y > squares[1][0].y + 3 * wynary && testowyPunkcik.y < squares[1][0].y + (4 * wynary)) {
cout << "DWUNASTY KWADRAT" << endl;
PlaySound(TEXT("C:/opencv/ProjectAppsBinaries/testSounds/12.wav"), NULL, SND_ASYNC);
}
}
else {
cout << "Cos sie popsulo po calosci ;)" << endl;
cout << "Mati ma�y programista c++" << endl;
}

imshow(wndname, image);
imwrite("TestImages/FrameImages/OutputTestImages/" + zmiennaWejsciowa + "OutputImageTest.jpg", image);
}


int main(int argc, char** argv) {
/*
if (argc != 2) {
cout << "Blad";
return 0;
}
String zmiennaWejsciowa = argv[1];
*/
//Point point = flashFinder();

	Mat imgOriginal;
	VideoCapture vCapture;
	vCapture.open(0);
	vCapture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	vCapture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	bool ss = false;

	while (1) {
		vCapture.read(imgOriginal);
		ss = flashFinder(imgOriginal);
		if (ss) {
			system("PAUSE");
		}
	}


/*
frameFinder(zmiennaWejsciowa);

namedWindow(wndname, 1);
vector<vector<Point> > squares;

Mat image = imread("TestImages/FrameImages/OutputTestImages/" + zmiennaWejsciowa + "OutputImageTest.jpg", 1);
if (image.empty()) {
cout << "Couldn't load " << "TestImages/FrameImages/InputTestImages/" + zmiennaWejsciowa + ".jpg" << endl;
}

findSquares(image, squares);
drawSquares(image, squares, point, zmiennaWejsciowa);

waitKey(10000);*/
return 0;
}