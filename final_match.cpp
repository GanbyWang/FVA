//���С��project
//��������ʦ��
//match����
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
#define dstLength 128		//Ŀ��ͼ����
#define dstWidth 96

unsigned char match1[maxn][maxn];	//����ĵ�һ��ģ��
unsigned char match2[maxn][maxn];	//����ĵڶ���ģ��

void Match(const char *t1_path, const char *t2_path)
{
	ifstream inFile1(t1_path, ios::in | ios::binary);
	ifstream inFile2(t2_path, ios::in | ios::binary);

	int white1, white2;
	white1 = 0;
	white2 = 0;

	//����ģ��
	for (int i = 0; i < dstWidth; i++)
	{
		for (int j = 0; j < dstLength; j++)
		{
			//uchar->unsignd char
			inFile1.read((char *)&match1[i][j], sizeof(char));
			inFile2.read((char *)&match2[i][j], sizeof(char));

			if (match1[i][j] == 255)
				white1++;

			if (match2[i][j] == 255)
				white2++;
		}
	}
	//�ر��ļ�
	inFile1.close();
	inFile2.close();

	int count;			//count��¼ƥ��İ�ɫ���ص����
	int max = 0;		//max��¼����ƥ��ֵ

						//���������ƶ�ͼƬ����ƥ�䣬������ֵ
	for (int a = 0; a <= 15; a++)
	{
		for (int b = 0; b <= 15; b++)
		{
			count = 0;	//����
			for (int i = 0; i < dstWidth; i++)
			{
				for (int j = 0; j < dstLength; j++)
				{
					//���������Ч���Ҷ�Ӧ����ͼ�����ص��Ϊʶ������
					if (i - a >= 0 && j - b >= 0 &&
						match1[i - a][j - b] == match2[i][j] &&
						match2[i][j] == 255)
						count++;	//�����Ӽ�
				}
			}
			if (count > max) max = count;

			count = 0;	//����
			for (int i = 0; i < dstWidth; i++)
			{
				for (int j = 0; j < dstLength; j++)
				{
					//���������Ч���Ҷ�Ӧ����ͼ�����ص��Ϊʶ������
					if (i - a >= 0 && j + b < dstLength &&
						match1[i - a][j + b] == match2[i][j] &&
						match2[i][j] == 255)
						count++;	//�����Ӽ�
				}
			}
			if (count > max) max = count;

			count = 0;	//����
			for (int i = 0; i < dstWidth; i++)
			{
				for (int j = 0; j < dstLength; j++)
				{
					//���������Ч���Ҷ�Ӧ����ͼ�����ص��Ϊʶ������
					if (i + a < dstWidth && j - b >= 0 &&
						match1[i + a][j - b] == match2[i][j] &&
						match2[i][j] == 255)
						count++;	//�����Ӽ�
				}
			}
			if (count > max) max = count;

			count = 0;	//����
			for (int i = 0; i < dstWidth; i++)
			{
				for (int j = 0; j < dstLength; j++)
				{
					//���������Ч���Ҷ�Ӧ����ͼ�����ص��Ϊʶ������
					if (i + a < dstWidth && j + b < dstLength &&
						match1[i + a][j + b] == match2[i][j] &&
						match2[i][j] == 255)
						count++;	//�����Ӽ�
				}
			}
			if (count > max) max = count;
		}
	}

	//��������ֵƽ�������
	double result;
	result = (double)max / white1;
	result += (double)max / white2;
	result = result / 2;
	cout << result << endl;
}

int main(int argc, char *argv[])
{
	char *t1_path = argv[1];
	char *t2_path = argv[2];
	Match(t1_path, t2_path);
	return 0;
}