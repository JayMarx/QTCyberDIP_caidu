#ifdef VIA_OPENCV

#ifndef USRGAMECONTROLLER_H
#define USRGAMECONTROLLER_H

#include "qtcyberdip.h"

#define WIN_NAME "Frame"

//��ɨ��������±߽���޶�
#define Up_height 160
#define Down_height 105

#include <vector>
#include <cmath>
#include <algorithm>

//��Ϸ������
class usrGameController
{
private:
	deviceCyberDip* device;
//������Ϊ��ʵ����ʾЧ�������ӵ�����
	//���ص��ṹ��
	struct MouseArgs{
		cv::Rect box;
		bool Drawing, Hit;
		// init
		MouseArgs() :Drawing(false), Hit(false)
		{
			box = cv::Rect(0, 0, -1, -1);
		}
	};
	//���ص�����
	friend void  mouseCallback(int event, int x, int y, int flags, void*param);
	MouseArgs argM;
//������Ϊ��ʵ�ֿ�����ʾЧ�������ӵ�����

	int status = 0;		//ִ��״̬

public:
	//���캯�������б����ĳ�ʼ����Ӧ�ڴ����
	usrGameController(void* qtCD);
	//�������������ձ���������Դ
	~usrGameController();
	//����ͼ������ÿ���յ�ͼ��ʱ�������
	int usrProcessImage(cv::Mat& img);
};

//������Ϊ��ʵ����ʾЧ�������ӵ�����
//���ص�����
void  mouseCallback(int event, int x, int y, int flags, void*param);
//������Ϊ��ʵ�ֿ�����ʾЧ�������ӵ�����

enum slot_status {
	UPPER, UPPERUSED, PREFILLED, UNFILLED, FILLED
};

struct Block{
	cv::Point cp;			//���ĵ�
	int flag = -1;			//0:δ������ɫ��; 1:��ʾ��ɫ��; 2:�հ׿�
	int num = 0;			//����Ŀ
	cv::Point3i rgb;
};

void get_block(cv::Mat &img, std::vector<Block>& block, int up_height, int down_height);

bool Adjacent(const Block& color_block1, const Block& color_block2, int dist_step);
int OnlyHaveBlankBlock(const std::vector<Block>& color_blocks, int i, int j, int dist_step, std::vector<int>& records);
int ClosestColor(const std::vector<Block>& color_blocks, int i);
int GetDistStep(const std::vector<Block>& color_blocks);
int MyNorm(const cv::Point3i& a, const cv::Point3i& b);
bool Arrange(std::vector<Block>& color_blocks, std::vector<int>& results);

#endif
#endif