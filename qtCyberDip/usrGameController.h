#ifdef VIA_OPENCV

#ifndef USRGAMECONTROLLER_H
#define USRGAMECONTROLLER_H

#include "qtcyberdip.h"

#define WIN_NAME "Frame"

//行扫描距离上下边界的限定
#define Up_height 160
#define Down_height 105

#include <vector>
#include <cmath>
#include <algorithm>

//游戏控制类
class usrGameController
{
private:
	deviceCyberDip* device;
//以下是为了实现演示效果，增加的内容
	//鼠标回调结构体
	struct MouseArgs{
		cv::Rect box;
		bool Drawing, Hit;
		// init
		MouseArgs() :Drawing(false), Hit(false)
		{
			box = cv::Rect(0, 0, -1, -1);
		}
	};
	//鼠标回调函数
	friend void  mouseCallback(int event, int x, int y, int flags, void*param);
	MouseArgs argM;
//以上是为了实现课堂演示效果，增加的内容

	int status = 0;		//执行状态

public:
	//构造函数，所有变量的初始化都应在此完成
	usrGameController(void* qtCD);
	//析构函数，回收本类所有资源
	~usrGameController();
	//处理图像函数，每次收到图像时都会调用
	int usrProcessImage(cv::Mat& img);
};

//以下是为了实现演示效果，增加的内容
//鼠标回调函数
void  mouseCallback(int event, int x, int y, int flags, void*param);
//以上是为了实现课堂演示效果，增加的内容

enum slot_status {
	UPPER, UPPERUSED, PREFILLED, UNFILLED, FILLED
};

struct Block{
	cv::Point cp;			//中心点
	int flag = -1;			//0:未处理颜色块; 1:提示颜色块; 2:空白框
	int num = 0;			//块数目
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