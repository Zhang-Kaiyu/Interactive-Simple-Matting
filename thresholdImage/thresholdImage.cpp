/******OpenCV交互式抠图，鼠标左键擦除，中键填补，右键撤销最近一次修改，按键盘任意键保存退出********
***************************************************************************************************/

#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<iostream>
using namespace std;
using namespace cv;
Mat dstImage, tmpImage;
//拖动条回调函数
void onChangeTrackBar(int pos, void *data)
{
	//强制类型转换
	Mat srcImage = *(cv::Mat*)(data);
//	Mat dstImage;
	//根据拖动条的值对传入图像进行二值化
	threshold(srcImage, dstImage, pos, 255, 0);
//	threshold(srcImage, dstImage, pos, 255, THRESH_BINARY | THRESH_OTSU);
	/**********************/
	Mat k = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	morphologyEx(dstImage, dstImage, MORPH_OPEN, k);
	morphologyEx(dstImage, dstImage, MORPH_CLOSE, k);
	/**********************/
	cvtColor(dstImage, dstImage, COLOR_GRAY2BGR);
	imshow("threshold", dstImage);
}

bool leftButtonDown = false, rightButtonDown = false, middleButtonDown = false;
vector<Point> g_vpLinePoints, m_vpts;
Point g_pPrevious; // previous point position
Point g_pCurrent; // current mouse position
void onMouse(int event, int x, int y, int flags, void* params)
{
//	Mat& img = *(Mat*)params;
	switch (event)
	{
	case EVENT_MOUSEMOVE:
		if (leftButtonDown)
		{
			g_pCurrent = Point(x, y);
			g_vpLinePoints.push_back(g_pCurrent);
//			line(dstImage, g_pPrevious, g_pCurrent, Scalar(255, 0, 0), 8, 8);
			line(dstImage, g_pPrevious, g_pCurrent, Scalar(0, 0, 0), 8, 8);
			g_pPrevious = g_pCurrent;
			imshow("threshold", dstImage);
		}
		else if (middleButtonDown)
		{
			g_pCurrent = Point(x, y);
			g_vpLinePoints.push_back(g_pCurrent);
			line(dstImage, g_pPrevious, g_pCurrent, Scalar(255, 255, 255), 8, 8);
			g_pPrevious = g_pCurrent;
			imshow("threshold", dstImage);
		}
		break;
	case EVENT_LBUTTONDOWN:
		tmpImage = dstImage.clone();
		leftButtonDown = true;
		g_pPrevious = Point(x, y);
		g_vpLinePoints.push_back(g_pPrevious);
		break;
	case EVENT_LBUTTONUP:
		leftButtonDown = false;
		break;
	case EVENT_RBUTTONDOWN:
		rightButtonDown = true;
		dstImage = tmpImage.clone();
		rightButtonDown = false;
		imshow("threshold", dstImage);
		break;

	case EVENT_MBUTTONDOWN:
		tmpImage = dstImage.clone();
		middleButtonDown = true;
		g_pPrevious = Point(x, y);
		m_vpts.push_back(g_pPrevious);
		break;
	case EVENT_MBUTTONUP:
		middleButtonDown = false;
	}
}
int main()
{
	//读取图像
	Mat srcImage = imread("../convertedImage/008703011130ROI.png");
	imshow("original", srcImage);
	if (!srcImage.data) {
		cout << "read failed" << endl;
		system("pause");
		return -1;
	}
	resize(srcImage, srcImage, Size(srcImage.cols * 2, srcImage.rows * 2));
	//原图像转换灰度图
	Mat srcGray;
	cvtColor(srcImage, srcGray, COLOR_RGB2GRAY);
	namedWindow("threshold");//创建窗口
	imshow("threshold", srcGray);
	//创建滑动条createTrackbar
	createTrackbar("pos", "threshold", 0, 255, onChangeTrackBar, &srcGray);
	setMouseCallback("threshold", onMouse, (void*)&srcGray);
	waitKey(0);
	imwrite("../preProcessed/tmp130.png", dstImage);
	return 0;
}