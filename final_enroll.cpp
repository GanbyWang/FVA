//���С��project
//��������ʦ��
//enroll����
//����� 1400012795
//��Ƶ� 1400012862
//������ 1400012880

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cmath>
#include "bitmap.h"
#include <opencv2\opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

#define maxn 600			//����ͼƬ�������
#define srcLength 512		//Դͼ����
#define srcWidth 384
#define dstLength 128		//Ŀ��ͼ����
#define dstWidth 96
#define compressRatio 0.25	//ѹ����
#define lowThreshold 80		//����Canny����ʱ����ֵ�½磬Խ������Խ��
#define thresholdRatio 3	//����Canny����ʱ��ֵ�Ͻ���½��ֵ
#define kernelSize 3		//����Canny����ʱ�ں����ӵĴ�С
#define impArea 12			//���µĲ�����������Ϊ�����������������������
#define lredunArea 10		//ȥ����߲���ͼ����Ϊ�������
#define rredunArea 4		//ȡ���ұ߲���ͼ����Ϊ�������
#define contourTol 5		//��������ֵ�����������������ص����ڴ�֮�������
#define startArea 10		//��ͼƬ�м����¶������ص㿪ʼѰ������
#define Rad2Deg(Rad) (Rad)* R2D		// ���ȵ��Ƕ�
#define R2D    57.295779513082323

int upContour[maxn];		//��¼������������
int downContour[maxn];		//��¼������������

unsigned char color[maxn][maxn];	//��ʼ�ĻҶȾ���
unsigned char transV[maxn][maxn];	//��ֵ����Ľ��

std::vector<Point> midPoints;       //���ߵ㼯
Vec4f midLine;                      //�洢�����������ĸ�������ǰ����Ϊֱ�ߵķ���ĵ�λ������������ֵ�������Ǹ�ֱ��ͨ����һ���㡣
int midP[2];

int dis = 8;				//����dis��ʾ���Ӧ�����뾶
int t = 1;					//����t��ʾԲ����ͻ��ֵ����Ϊt��Խ���ͻ��Ҫ��Խ�ߣ��ҶȲ�Խ��
int g = 108;				//����g��ʾԲ����ͻ�������ԽС��ͻ��Ҫ��Խ�ߣ�ͻ����Խ�ࣩ

/* ѹ��ͼƬ������opencv������ */
Mat Compress(Mat & img)
{
	Mat tmp;
	resize(img, tmp, Size(0, 0), compressRatio, compressRatio, CV_INTER_LINEAR);
#ifdef DEBUG
	namedWindow("compressed");
	imshow("compressed", tmp);
	waitKey(0);
#endif // DEBUG
	Mat cmpImg = tmp;
	return cmpImg;
}

/* �ú������ڱ�������������������ΪMat���ͼƬ�����ս����¼��upContour��downContour���������� */
void Contour(Mat & img)
{
	Mat bwimg;				//���ڴ洢������Canny�������ͼƬ

	/* ����Canny���� */
	Canny(img, bwimg, lowThreshold, lowThreshold*thresholdRatio, kernelSize);

	/* ���ڵ��ԣ����Canny�������ú�Ĵ����� */
	/*#ifdef DEBUG
	namedWindow("edge");
	imshow("edge", bwimg);
	//imwrite("edge.bmp", bwimg);
	waitKey(0);
	#endif // DEBUG*/

	/* ��ʼ���������� */
	for (int i = 0; i < dstLength; i++)
	{
		upContour[i] = downContour[i] = 0;
	}

	bool edge[maxn][maxn];	//edge�������ڴ洢������Canny����������ص㣨1��ʾ��ɫ��0��ʾ��ɫ��
	for (int i = 0; i < dstWidth; i++)
	{
		for (int j = 0; j < dstLength; j++)
		{
			if (bwimg.at<uchar>(i, j) == 0) edge[i][j] = 0;	//��������Canny��ͼ��ת�浽edge����
			else
			{
				if (i > impArea && i < dstWidth - impArea)	//ֻ�������м�����İ�ɫ���ص�ſ���������
					edge[i][j] = 1;
			}
		}
	}

	/* �öγ�������Ѱ�������������������� */
	for (int j = dstLength - 1; j >= 0; j--)
	{
		for (int i = dstWidth / 2 - startArea; i >= 0; i--)
		{
			if (edge[i][j] == 1)
			{
				if (upContour[j] == 0)
				{
					upContour[j] = i;

					if (j < dstLength - 1)
					{
						int k = j + 1;
						while (k < dstLength && upContour[k] == 0)
							k++;

						if (k < dstLength &&
							abs(upContour[j] - upContour[k]) > contourTol)
							upContour[j] = 0;
					}
				}
				else
				{
					int k = j + 1;
					while (k < dstLength && upContour[k] == 0)
						k++;

					if (k < dstLength &&
						abs(i - upContour[k]) < abs(upContour[j] - upContour[k]))
						upContour[j] = i;
				}
			}
		}

		for (int i = dstWidth / 2 + startArea; i < dstWidth; i++)
		{
			if (edge[i][j] == 1)
			{
				if (downContour[j] == 0)
				{
					downContour[j] = i;

					if (j < dstLength - 1)
					{
						int k = j + 1;
						while (k < dstLength && downContour[k] == 0)
							k++;

						if (k < dstLength &&
							abs(downContour[j] - downContour[k]) > contourTol)
							downContour[j] = 0;
					}
				}
				else
				{
					int k = j + 1;
					while (k < dstLength && downContour[k] == 0)
						k++;

					if (k < dstLength &&
						abs(i - downContour[k]) < abs(downContour[j] - downContour[k]))
						downContour[j] = i;
				}
			}
		}
	}

	/*#ifdef DEBUG
	Mat Contr = bwimg;
	for (int i = 0; i < dstWidth; i++)
	{
	for (int j = 0; j < dstLength; j++)
	{
	if (upContour[j] == i || downContour[j] == i)
	Contr.at<uchar>(i, j) = 255;
	else Contr.at<uchar>(i, j) = 0;
	}
	}

	namedWindow("contourRow");
	imshow("contourRow", Contr);
	waitKey(0);
	#endif // DEBUG*/


	/* ����������δ���Կ��ܳ��ֵĲ�����������д��� */
	for (int i = 0; i < dstLength; i++)
	{
		/* ���������ͷ�в���������� */
		if (i == 0 && upContour[i] == 0)
		{
			int j = i + 1;				//j��¼��һ�����յ����ص�
			while (upContour[j] == 0)
				j++;

			/* ֱ�ӽ���ȱ�Ĳ�����ֱ�߲�ȫ */
			for (int k = 0; k < j; k++)
				upContour[k] = upContour[j];

			i = j - 1;
		}
		else if (i != 0 && upContour[i] == 0)	//�������
		{
			int j = i + 1;				//j��¼��һ�����յ����ص㣨ע�ⲻ�ܳ�����Χ��
			while (j < dstLength && upContour[j] == 0)
				j++;

			if (j == dstLength)			//�����ȱ�Ĳ����ǽ�β������
			{
				/* ֱ�ӽ���ȱ������ֱ�߲�ȫ */
				for (int k = i; k < dstLength; k++)
					upContour[k] = upContour[i - 1];
			}
			else						//�������Ҫ�ҵ���ʼ����ֹ�����ص�
			{
				int diffY = j - i;		//����y������ز��
				int diffX = upContour[j] - upContour[i - 1];	//����������������ص��x���
				int ifNeg;				//��¼�����������Ǹ�����
				if (diffX < 0)			//����Ǹ�����������ifNeg������diffXȡ��
				{
					diffX = -diffX;
					ifNeg = -1;
				}
				else ifNeg = 1;			//����ifNeg��Ϊ1
				diffX--;				//ͳһ��diffX����1����ʾ������x��ࣩ

				int diff = diffX / diffY;		//�����������ı�ֵ����������
				if (ifNeg == -1) diff = -diff;	//�����������ķ���
				if (diff != 0)			//�����ֵ��Ϊ0����˵��x������ص����y���
				{
					for (int k = i; k < j; k += 1)	//����ݼ����и�ֵ���ص����ھ�������ԭ����б�ʣ�
						upContour[k] = upContour[k - 1] + diff;
				}
				else					//������Ҫ����һ��ֱ��������xֵ
				{
					if (diffX == 0)		//��������һ�����ص㣬��ֱ���ú����
					{
						for (int k = i; k < j; k++)
							upContour[k] = upContour[k - 1];
						continue;
					}
					int r = diffY / diffX;			//r��ʾһ���߶εĳ���
					for (int k = i; k < j; k++)
					{
						if ((k - i) % r == 0)		//���������ʾ��Ҫ���߶�̧�ߣ��򽵵ͣ�1�����ص�
							upContour[k] = upContour[k - 1] + ifNeg;
						else			//���򱣳ֲ���
							upContour[k] = upContour[k - 1];
					}
				}
			}
		}
	}

	/* ����������������������������ȫ��ͬ */
	for (int i = 0; i < dstLength; i++)
	{
		/* ���������ͷ�в���������� */
		if (i == 0 && downContour[i] == 0)
		{
			int j = i + 1;				//j��¼��һ�����յ����ص�
			while (downContour[j] == 0)
				j++;

			/* ֱ�ӽ���ȱ�Ĳ�����ֱ�߲�ȫ */
			for (int k = 0; k < j; k++)
				downContour[k] = downContour[j];

			i = j - 1;
		}
		else if (i != 0 && downContour[i] == 0)	//�������
		{
			int j = i + 1;				//j��¼��һ�����յ����ص㣨ע�ⲻ�ܳ�����Χ��
			while (j < dstLength && downContour[j] == 0)
				j++;

			if (j == dstLength)			//�����ȱ�Ĳ����ǽ�β������
			{
				/* ֱ�ӽ���ȱ������ֱ�߲�ȫ */
				for (int k = i; k < dstLength; k++)
					downContour[k] = downContour[i - 1];
			}
			else						//�������Ҫ�ҵ���ʼ����ֹ�����ص�
			{
				int diffY = j - i;		//����y������ز��
				int diffX = downContour[j] - downContour[i - 1];	//����������������ص��x���
				int ifNeg;				//��¼�����������Ǹ�����
				if (diffX < 0)			//����Ǹ�����������ifNeg������diffXȡ��
				{
					diffX = -diffX;
					ifNeg = -1;
				}
				else ifNeg = 1;			//����ifNeg��Ϊ1
				diffX--;				//ͳһ��diffX����1����ʾ������x��ࣩ

				int diff = diffX / diffY;		//�����������ı�ֵ����������
				if (ifNeg == -1) diff = -diff;	//�����������ķ���
				if (diff != 0)			//�����ֵ��Ϊ0����˵��x������ص����y���
				{
					for (int k = i; k < j; k += 1)	//����ݼ����и�ֵ���ص����ھ�������ԭ����б�ʣ�
						downContour[k] = downContour[k - 1] + diff;
				}
				else					//������Ҫ����һ��ֱ��������xֵ
				{
					if (diffX == 0)		//��������һ�����ص㣬��ֱ���ú����
					{
						for (int k = i; k < j; k++)
							downContour[k] = downContour[k - 1];
						continue;
					}
					int r = diffY / diffX;			//r��ʾһ���߶εĳ���
					for (int k = i; k < j; k++)
					{
						if ((k - i) % r == 0)		//���������ʾ��Ҫ���߶�̧�ߣ��򽵵ͣ�1�����ص�
							downContour[k] = downContour[k - 1] + ifNeg;
						else			//���򱣳ֲ���
							downContour[k] = downContour[k - 1];
					}
				}
			}
		}
	}

#ifdef DEBUG
	Mat Contr = bwimg;
	for (int i = 0; i < dstWidth; i++)
	{
		for (int j = 0; j < dstLength; j++)
		{
			if (upContour[j] == i || downContour[j] == i)
				Contr.at<uchar>(i, j) = 255;
			else Contr.at<uchar>(i, j) = 0;
		}
	}

	namedWindow("contour");
	imshow("contour", Contr);
	waitKey(0);
#endif // DEBUG

}

/* �����ж�ֵ������������ת������Mat�� */
Mat trans(Mat & img)
{
	Mat show = img;
	for (int i = 0; i < dstWidth; i++)
	{
		for (int j = 0; j < dstLength; j++)
			show.at<uchar>(i, j) = transV[i][j];
	}

#ifdef DEBUG
	namedWindow("trans");
	imshow("trans", show);
	//imwrite("trans.bmp", show);
	waitKey(0);
#endif

	return show;
}

/* ��ֵ�˲�ģ����ƽ�����߽� */
/*Mat mSmooth(Mat & img)
{
	Mat src = img;
	Mat dst;

	//��ֵ�˲�
	medianBlur(src, dst, 3);

#ifdef DEBUG
	namedWindow("mSmoothed");
	imshow("mSmoothed", dst);
	//imwrite("smoothed.bmp", dst);
	waitKey(0);
#endif // DEBUG
	return dst;
}*/

//��˹�˲�
Mat gSmooth(Mat & img)
{
	Mat src = img;
	Mat dst;

	//�����ǰ�˳��д��
	//��˹�˲�
	//src:����ͼ��
	//dst:���ͼ��
	//Size(5,5)ģ���С��Ϊ����
	//x���򷽲�
	//Y���򷽲�
	//ģ��Խ��Խģ��
	GaussianBlur(src, dst, Size(3, 3), 0, 0);

#ifdef DEBUG
	namedWindow("gSmoothed");
	imshow("gSmoothed", dst);
	//imwrite("smoothed.bmp", dst);
	waitKey(0);
#endif // DEBUG
	return dst;
}

//��ת����
Mat rotate(Mat & img)
{
	Mat rot = img;
	//�������ϵĵ�ÿ�ĸ�ȡһ������㼯
	for (int i = 0; i < dstLength; i += 4)
	{
		int y = (upContour[i] + downContour[i]) / 2;
		midPoints.push_back(Point(i, y));
	}
	//����
	//midLine[0][1][2][3]�ֱ�Ϊsin cos ������ĳ������
	fitLine(Mat(midPoints), midLine, CV_DIST_L2, 0, 0.01, 0.01);
	double k1 = asin(midLine[1]);
	double k2 = acos(midLine[0]);
	double k3 = atan(midLine[1] / midLine[0]);
	double kkk = Rad2Deg(k3);//�Ƕ�
	midP[0] = midLine[3];
	midP[1] = midLine[4];

	Mat rot_mat(2, 3, CV_8UC1);
	Point center = Point(midP[1], midP[0]);
	double angle = kkk + 0.47;

	rot_mat = getRotationMatrix2D(center, angle, 1);
	warpAffine(img, rot, rot_mat, img.size());

#ifdef DEBUG
	namedWindow("rotate");
	imshow("rotate", rot);
	waitKey(0);
#endif // DEBUG

	return rot;
}

//enroll����
void Enroll(const char *src_path, const char *dst_path)
{
	Mat img;				//����ͼƬ
	img = imread(src_path, CV_8UC1);

#ifdef DEBUG
	namedWindow("input");
	imshow("input", img);
	waitKey(0);
#endif // DEBUG

	img = Compress(img);	//��ͼƬѹ��
	Contour(img);			//Ѱ������

	img = gSmooth(img);		//��˹ģ��

	/* ��ѹ��ͼƬ����color���� */
	for (int i = 0; i < dstWidth; i++)
	{
		for (int j = 0; j < dstLength; j++)
			//uchar->unsignd char
			color[i][j] = img.at<uchar>(i, j);
	}

	for (int y = 0; y<dstWidth; ++y)							//����ͼ��ÿ���Ƿ�Ϊͼ������(Բ����)
	for (int x = 0; x < dstLength; ++x)
	{
		if (upContour[x] >= y || downContour[x] <= y)
		{
			transV[y][x] = 0;
			continue;       //��ָ������
		}
		int counter = 0;
		for (int dy = 0; dy <= dis; ++dy)
		{
			for (int dx = 0; dx*dx <= dis*dis - dy*dy; ++dx)
			{
				if (color[y + dy][x + dx] - color[y][x] <= t) counter++;
				if (dx&&color[y + dy][x - dx] - color[y][x] <= t) counter++;
				if (dy&&color[y - dy][x + dx] - color[y][x] <= t) counter++;
				if (dy&&dx&&color[y - dy][x - dx] - color[y][x] <= t) counter++;
			}
		}
		if (counter <= g)
		{
			transV[y][x] = 255;
		}
		else
			transV[y][x] = 0;
	}

	/* ����ֵ�����ͼƬ���һ������ĵ�ȫ��Ϳ�ڣ���������lredunArea��rredunArea���壩 */
	for (int k = 0; k < lredunArea; k++)
	{
		for (int i = 0; i < dstWidth; i++)
			transV[i][k] = 0;
	}

	for (int k = 0; k < rredunArea; k++)
	{
		for (int i = 0; i < dstWidth; i++)
			transV[i][dstLength - 1 - k] = 0;
	}

	img = trans(img);		//����ֵ���������תΪMat��
	img = rotate(img);		//��תͼƬ
	//img = mSmooth(img);	//�ٴν����˲�

	/*for (int i = 0; i < dstWidth; i++)
	{
	for (int j = 0; j < dstLength; j++)
	{
	if (img.at<uchar>(i, j) != 255)
	img.at<uchar>(i, j) = 0;
	}
	}*/

	/* ������洢��Ŀ�ĵ�ַ */
	ofstream outFile(dst_path, ios::out | ios::binary);
	for (int y = 0; y < dstWidth; ++y)
	{
		for (int x = 0; x < dstLength; ++x)
		{
			unsigned char tmp = img.at<uchar>(y, x);
			outFile.write((char *)&tmp, sizeof(char));
		}
	}
	outFile.close();
}

int main(int argc, char *argv[])
{
	char *src_path = argv[1];
	char *dst_path = argv[2];
	Enroll(src_path, dst_path);
	return 0;
}