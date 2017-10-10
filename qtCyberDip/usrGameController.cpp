#include "usrGameController.h"

#ifdef VIA_OPENCV

using namespace std;
using namespace cv;

//�������ʼ��
usrGameController::usrGameController(void* qtCD)
{
	qDebug() << "usrGameController online.";
	device = new deviceCyberDip(qtCD);//�豸������
	cv::namedWindow(WIN_NAME);
	cv::setMouseCallback(WIN_NAME, mouseCallback, (void*)&(argM));
}

//����
usrGameController::~usrGameController()
{
	cv::destroyAllWindows();
	if (device != nullptr)
	{
		delete device;
	}
	qDebug() << "usrGameController offline.";
}

//����ͼ�� 
int usrGameController::usrProcessImage(cv::Mat& img)
{
	cv::Size imgSize(img.cols, img.rows - UP_CUT);
	if (imgSize.height <= 0 || imgSize.width <= 0)
	{
		qDebug() << "Invalid image. Size:" << imgSize.width <<"x"<<imgSize.height;
		return -1;
	}

	//��ȡͼ���Ե
	cv::Mat pt = img(cv::Rect(0, UP_CUT, imgSize.width,imgSize.height));
	cv::imshow(WIN_NAME, pt);

	/************************************����Ϊ�����*******************************************/

	//cv::Mat Origin_Img = cv::imread("./game_pic/pic2.jpg");

	//��д�룬���ȡ��ptΪ��ͨ��
	//	cv::imwrite("./game_pic/origin.png",pt);
	//	cv::Mat Origin_Img = cv::imread("./game_pic/origin.png");

	cv::Mat Origin_Img = pt.clone();

	/*
	int a = Origin_Img.at<cv::Vec4b>(120, 300)[0];
	int b = Origin_Img.at<cv::Vec4b>(120, 300)[1];
	int c = Origin_Img.at<cv::Vec4b>(120, 300)[2];
	int d = Origin_Img.at<cv::Vec4b>(120, 300)[3];
	*/

	//status, 0:������δִ��; 1:������ִ��һ��
	if (status == 0)
	{
		std::vector<Block> block;
		get_block(Origin_Img, block, Up_height, Down_height);

		//��ʾ������ɫ���Ч��

		for (int i = 0; i < block.size(); i++)
		{
			if (block[i].flag == UPPER)
				cv::circle(Origin_Img, cv::Point(block[i].cp.x, block[i].cp.y), 5, cv::Scalar(0, 0, 255, 255), 2);
			else if (block[i].flag == PREFILLED)
				cv::circle(Origin_Img, cv::Point(block[i].cp.x, block[i].cp.y), 5, cv::Scalar(0, 255, 0, 255), 2);
			else if (block[i].flag == UNFILLED)
				cv::circle(Origin_Img, cv::Point(block[i].cp.x, block[i].cp.y), 5, cv::Scalar(255, 0, 0, 255), 2);
		}
		cv::imshow("pic", Origin_Img);

		// �㷨
		vector<int> results(block.size(), -1);
		if (Arrange(block, results)) {

			for (int i = 0; i < block.size(); i++)
			{
				if (block[i].flag == slot_status::FILLED)
				{
					int dst_index = results[i];
					device->comMoveToScale((double)block[dst_index].cp.x / pt.cols, (double)block[dst_index].cp.y / pt.rows);
					device->comHitOnce();
					device->comMoveToScale((double)block[i].cp.x / pt.cols, (double)block[i].cp.y / pt.rows);
					device->comHitOnce();
					cv::waitKey(3000);
				}
			}
		}
		status = 1;
		cv::waitKey(3000);
		return 0;
	}

	if (status == 1)
	{
		status = 0;
		long int tt = 0;
		//���success��־,iΪimg rows��Χ,jΪimg cols��Χ
		for (int j = 350; j < 620; j++)
			tt += Origin_Img.at<cv::Vec4b>(152, j)[0] + Origin_Img.at<cv::Vec4b>(152, j)[1] + Origin_Img.at<cv::Vec4b>(152, j)[2];

		//�ɹ�
		if (tt > 255 * 10)
		{
			//����һ��
			device->comMoveToScale(620.0 / pt.cols, 692.0 / pt.rows);
			device->comHitOnce();
			cv::waitKey(3000);
			return 0;
		}
		else
		{
			//ˢ�º���һ��
			device->comMoveToScale(497.0 / pt.cols, 692.0 / pt.rows);
			device->comHitOnce();
			cv::waitKey(500);
			device->comMoveToScale(620.0 / pt.cols, 692.0 / pt.rows);
			device->comHitOnce();
			cv::waitKey(3000);
			return 0;
		}
	}

	// ��ɫСȦ
	/*
	unsigned int start[50];		//��ſ�ʼ��
	unsigned int stop[50];		//��Ž�����
	int m = 0;
	int n = 0;

	for (int i = 0; i < block[0].num; i++)
	{
	if (block[i].flag == 0)
	{
	start[m] = i;
	m++;
	}
	if (block[i].flag == 2)
	{
	stop[n] = i;
	n++;
	}
	}

	if (m == n)
	{
	for (int i = 0; i < m; i++)
	{
	device->comMoveToScale((double)block[start[i]].cp.x / pt.cols, (double)block[start[i]].cp.y / pt.rows);
	device->comHitOnce();
	device->comMoveToScale((double)block[stop[i]].cp.x / pt.cols, (double)block[stop[i]].cp.y / pt.rows);
	device->comHitOnce();
	cv::waitKey(2000);
	}
	}
	*/


	/***************************************************/
	
	//�ж�������ߴ�
	if (argM.box.x >= 0 && argM.box.x < imgSize.width&&
		argM.box.y >= 0 && argM.box.y < imgSize.height
		)
	{
		qDebug() << "X:" << argM.box.x << " Y:" << argM.box.y;
		if (argM.Hit)
		{
			device->comHitDown();
		}
		device->comMoveToScale(((double)argM.box.x + argM.box.width) / pt.cols, ((double)argM.box.y + argM.box.height) / pt.rows);
		argM.box.x = -1; argM.box.y = -1;
		if (argM.Hit)
		{
			device->comHitUp();
		}
		else
		{
			device->comHitOnce();
		}
	}
	return 0; 
}

//���ص�����
void mouseCallback(int event, int x, int y, int flags, void*param)
{
	usrGameController::MouseArgs* m_arg = (usrGameController::MouseArgs*)param;
	switch (event)
	{
	case CV_EVENT_MOUSEMOVE: // ����ƶ�ʱ
	{
		if (m_arg->Drawing)
		{
			m_arg->box.width = x - m_arg->box.x;
			m_arg->box.height = y - m_arg->box.y;
		}
	}
	break;
	case CV_EVENT_LBUTTONDOWN:case CV_EVENT_RBUTTONDOWN: // ��/�Ҽ�����
	{
		m_arg->Hit = event == CV_EVENT_RBUTTONDOWN;
		m_arg->Drawing = true;
		m_arg->box = cvRect(x, y, 0, 0);
	}
	break;
	case CV_EVENT_LBUTTONUP:case CV_EVENT_RBUTTONUP: // ��/�Ҽ�����
	{
		m_arg->Hit = false;
		m_arg->Drawing = false;
		if (m_arg->box.width < 0)
		{
			m_arg->box.x += m_arg->box.width;
			m_arg->box.width *= -1;
		}
		if (m_arg->box.height < 0)
		{
			m_arg->box.y += m_arg->box.height;
			m_arg->box.height *= -1;
		}
	}
	break;
	}
}

//�����ҵ�������ĵ�
void get_block(cv::Mat &img, std::vector<Block>& block, int up_height, int down_height)
{
	cv::Point LU;							//��ɫ�����Ͻ�
	int block_width = 0;					//��ɫ����
	int num = 0;							//��ɫ�����
	int sum = 0;
	int left_flag = 0;
	int right_flag = 0;
	int heap = 1;							//��ʾ��ɫ���
	int heap2_exist = 0;					//�ڶ��Ѵ��ڱ�־

	//ɨ���ȡ���λ�ã����к���
	for (int i = up_height; i < (img.rows - down_height); i++)
	{
		long int all = 0;
		for (int j = 10; j < (img.cols - 2); j++)
		{
			sum = img.at<cv::Vec4b>(i, j)[0] + img.at<cv::Vec4b>(i, j)[1] + img.at<cv::Vec4b>(i, j)[2];
			all += (img.at<cv::Vec4b>(i, j)[0] + img.at<cv::Vec4b>(i, j)[1] + img.at<cv::Vec4b>(i, j)[2]);

			//��һ��
			if (heap == 1)
			{
				//�����ϱ߽�
				if ((sum > 10) && (left_flag == 0))
				{
					LU.y = i;
					i += 6;	//�ƹ�Բ��
					left_flag = 1;
					break;
				}
				if ((sum > 10) && (left_flag == 1) && (right_flag == 0))
				{
					//�����ұ��Ƿ�����ɫ��(����˵����⵽�˱߽�)���޳��������
					int temp = 0;
					temp = img.at<cv::Vec4b>(i, j + 2)[0] + img.at<cv::Vec4b>(i, j + 2)[1] + img.at<cv::Vec4b>(i, j + 2)[2];
					if (temp > 10)
					{
						LU.x = j;
						right_flag = 1;
					}
				}
				//������ɫ���ҽ�
				if ((sum < 10) && (right_flag == 1))
				{
					block_width = j - LU.x;

					//����С���
					if (block_width > 30)
					{
						Block temp;
						temp.cp.x = LU.x + block_width / 2;
						temp.cp.y = LU.y + block_width / 2;
						temp.flag = UPPER;

						//rgb��Ӧopencv˳�����̺�
						temp.rgb.x = img.at<cv::Vec4b>(temp.cp.y, temp.cp.x)[2];
						temp.rgb.y = img.at<cv::Vec4b>(temp.cp.y, temp.cp.x)[1];
						temp.rgb.z = img.at<cv::Vec4b>(temp.cp.y, temp.cp.x)[0];
						num++;
						right_flag = 0;
						block.push_back(temp);
					}
					else
					{
						i += 30;
						right_flag = 0;
					}
				}

				//�����ұ߽磬��ʼ��һ��ɨ��
				if ((j == (img.cols - 3)) && (left_flag == 1))
				{
					left_flag = 0;
					i = LU.y + block_width + 2;	//�����һ����ɫ�飬ȥ��һ��
				}
				if ((j == (img.cols - 3)) && (all < 255) && (num > 0))
				{
					all = 0;
					for (int j = 10; j < (img.cols - 2); j++)
						all += (img.at<cv::Vec4b>(i + block_width / 2, j)[0] + img.at<cv::Vec4b>(i + block_width / 2, j)[1] + img.at<cv::Vec4b>(i + block_width / 2, j)[2]);
					//��һ�ѽ���
					if (all == 0)
					{
						i = i + block_width - 10;
						heap = 2;	//�����һ����ڶ��ѿ�϶������һ�ѽ���
					}
				}

			}


			//�ڶ���
			if (heap == 2)
			{
				//�����ϱ߽�
				if ((sum > 30) && (left_flag == 0))
				{
					LU.y = i;
					i += 10;	//�ƹ�Բ��
					left_flag = 1;
					heap2_exist = 1;	//�ҵ��ڶ���
					break;
				}
				if ((sum > 30) && (left_flag == 1) && (right_flag == 0))
				{
					//�����ұ��Ƿ�����ɫ��(����˵����⵽�˱߽�)���޳��������
					int temp = 0;
					temp = img.at<cv::Vec4b>(i, j + 2)[0] + img.at<cv::Vec4b>(i, j + 2)[1] + img.at<cv::Vec4b>(i, j + 2)[2];
					if (temp > 10)
					{
						LU.x = j;
						right_flag = 1;
					}
				}
				//�����ұ߽�
				if ((sum < 30) && (right_flag == 1))
				{
					Block temp;
					temp.cp.x = LU.x + block_width / 2;
					temp.cp.y = LU.y + block_width / 2;
					//��⵽��ʾ��(���п�)
					if ((j - LU.x) > 25)
					{
						block_width = j - LU.x;
						temp.flag = PREFILLED;
						temp.rgb.x = img.at<cv::Vec4b>(temp.cp.y, temp.cp.x)[2];
						temp.rgb.y = img.at<cv::Vec4b>(temp.cp.y, temp.cp.x)[1];
						temp.rgb.z = img.at<cv::Vec4b>(temp.cp.y, temp.cp.x)[0];
					}
					//��⵽�հ׿�(�п�)
					else
					{
						temp.flag = UNFILLED;		//��ʾ��⵽�հ׿�
						j = LU.x + block_width + 2;	//�����һ����ɫ�飬ȥ��һ��
						if (j > (img.cols - 3))
							j = img.cols - 3;
					}
					num++;
					right_flag = 0;
					block.push_back(temp);
				}
				//�����ұ߽磬��ʼ��һ��ɨ��
				if ((j == (img.cols - 3)) && (left_flag == 1))
				{
					left_flag = 0;
					i = LU.y + block_width + 3;		//�����һ����ɫ�飬ȥ��һ��
				}
				if ((j == (img.cols - 3)) && (all < 255) && (num > 0) && (heap2_exist != 0))
				{
					all = 0;
					//����10����λ��ɨ��һ�У��ж��Ƿ�ڶ��ѽ���
					for (int j = 10; j < (img.cols - 2); j++)
					{
						all += (img.at<cv::Vec4b>(i + 10, j)[0] + img.at<cv::Vec4b>(i + 10, j)[1] + img.at<cv::Vec4b>(i + 10, j)[2]);
					}
					if (all<255)
						heap = 0;					//�ڶ��ѽ���
				}
			}

			if (heap == 0)
				break;
		}

		if (heap == 0)
		{
			//block[0].num = num;					//��¼��⵽�Ŀ������
			break;								//�������
		}
	}
}

bool Adjacent(const Block& color_block1, const Block& color_block2, int dist_step) {
	return (std::abs(std::abs(color_block1.cp.x - color_block2.cp.x) - dist_step) < 10 && std::abs(color_block1.cp.y - color_block2.cp.y) < 10) ||
		(std::abs(std::abs(color_block1.cp.y - color_block2.cp.y) - dist_step) < 10 && std::abs(color_block1.cp.x - color_block2.cp.x) < 10) ?
		true : false;
}

int OnlyHaveBlankBlock(const std::vector<Block>& color_blocks, int i, int j, int dist_step, std::vector<int>& records) {
	int num = 0;
	if (std::abs(color_blocks[i].cp.x - color_blocks[j].cp.x) < 10) {
		num = std::round((color_blocks[i].cp.y - color_blocks[j].cp.y) / float(dist_step));
		if (num == 0)
			return 0;
		num = std::abs(num);
		num -= 1;
		for (int i = 0, x = color_blocks[j].cp.x, y = color_blocks[j].cp.y; i < num; ++i) {
			y += color_blocks[i].cp.y > color_blocks[j].cp.y ? dist_step : -dist_step;
			bool temp = false;
			for (int j = 0; j < color_blocks.size(); ++j) {
				if (std::abs(color_blocks[j].cp.x - x) < 10 &&
					std::abs(color_blocks[j].cp.y - y) < 10 &&
					color_blocks[j].flag == UNFILLED) {
					temp = true;
					records.push_back(j);
					break;
				}
			}
			if (!temp)
				return 0;
		}
		return num;
	}
	else if (std::abs(color_blocks[i].cp.y - color_blocks[j].cp.y) < 10) {
		num = std::round((color_blocks[i].cp.x - color_blocks[j].cp.x) / float(dist_step));
		if (num == 0)
			return 0;
		num = std::abs(num);
		num -= 1;
		for (int i = 0, x = color_blocks[j].cp.x, y = color_blocks[j].cp.y; i < num; ++i) {
			x += color_blocks[i].cp.x > color_blocks[j].cp.x ? dist_step : -dist_step;
			bool temp = false;
			for (int j = 0; j < color_blocks.size(); ++j) {
				if (std::abs(color_blocks[j].cp.x - x) < 10 &&
					std::abs(color_blocks[j].cp.y - y) < 10 &&
					color_blocks[j].flag == UNFILLED) {
					temp = true;
					records.push_back(j);
					break;
				}
			}
			if (!temp)
				return 0;
		}
		return num;
	}
	else {
		return 0;
	}
}

int ClosestColor(const std::vector<Block>& color_blocks, int i) {
	int min_diff = 1000;
	int result = -1;
	for (int j = 0; j < color_blocks.size(); ++j) {
		if (i != j && color_blocks[j].flag == UPPER) {
			Vec3i color_diff = color_blocks[i].rgb - color_blocks[j].rgb;
			int abs_diff = std::abs(color_diff[0]) + std::abs(color_diff[1]) + std::abs(color_diff[2]);
			if (abs_diff < min_diff) {
				min_diff = abs_diff;
				result = j;
			}
		}
	}
	return result;
}

int GetDistStep(const std::vector<Block>& color_blocks) {
	// for x and y, the distance steps are the same.
	// x
	vector<int> x;
	for (const auto& e : color_blocks) {
		x.push_back(e.cp.x);
	}
	std::sort(x.begin(), x.end());
	auto last = std::unique(x.begin(), x.end());
	x.erase(last, x.end());
	if (x.size() > 1) {
		std::sort(x.begin(), x.end());
		int tmp = x[1] - x[0];
		return tmp < 10 ? x[2] - x[1] : tmp;
	}
	// y
	x.erase(x.begin(), x.end());
	for (const auto& e : color_blocks) {
		x.push_back(e.cp.y);
	}
	std::sort(x.begin(), x.end());
	last = std::unique(x.begin(), x.end());
	x.erase(last, x.end());
	std::sort(x.begin(), x.end());
	int tmp = x[1] - x[0];
	return tmp < 10 ? x[2] - x[1] : tmp;
}

int MyNorm(const cv::Point3i& a, const cv::Point3i& b) {
	return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z);
}

// results[j] = k �� color_blocks �е�j��Ԫ�أ��տ�Ӧ�����k��Ԫ�أ�ɫ�飩
bool Arrange(std::vector<Block>& color_blocks, std::vector<int>& results) {
	int unfilled_num = 0, upper_num = 0;
	for (const auto& e : color_blocks) {
		if (e.flag == UNFILLED)
			++unfilled_num;
		else if (e.flag == UPPER)
			++upper_num;
	}
	if (upper_num != unfilled_num)
		return false;

	bool changed = false;
	int dist_step = GetDistStep(color_blocks);

	// for pre-filled blocks
	//   if have only one un-filled slot
	//     fill it with closest block
	/*
	for (int i = 0; i < color_blocks.size(); ++i) {
	if (color_blocks[i].flag == PREFILLED) {
	vector<int> j_record;
	for (int j = 0; j < color_blocks.size(); ++j) {
	if (i == j)
	continue;
	if (Adjacent(color_blocks[i], color_blocks[j], dist_step) && color_blocks[j].flag == UNFILLED) {
	j_record.push_back(j);
	}
	}
	if (j_record.size() == 1) {
	// find the closest
	int tmp = ClosestColor(color_blocks, i);
	results[j_record[0]] = tmp;
	color_blocks[j_record[0]].flag = slot_status::FILLED;
	color_blocks[j_record[0]].color = color_blocks[tmp].color;
	}
	}
	}
	*/

	while (true) {
		changed = false;
		// for filled blocks
		//   do the same until no more changes

		while (true) {
			vector<bool> flag(color_blocks.size(), false);
			for (int i = 0; i < color_blocks.size(); ++i) {
				if (color_blocks[i].flag == PREFILLED || color_blocks[i].flag == slot_status::FILLED) {
					flag[i] = true;
					vector<int> j_record;
					for (int j = 0; j < color_blocks.size(); ++j) {
						if (i == j)
							continue;
						if (Adjacent(color_blocks[i], color_blocks[j], dist_step) && color_blocks[j].flag == UNFILLED) {
							j_record.push_back(j);
						}
					}
					if (j_record.size() == 1) {
						// find the closest
						int tmp = ClosestColor(color_blocks, i);
						changed = true;
						results[j_record[0]] = tmp;
						color_blocks[j_record[0]].flag = slot_status::FILLED;
						color_blocks[j_record[0]].rgb = color_blocks[tmp].rgb;
						color_blocks[tmp].flag = UPPERUSED;
					}
					else {
						flag[i] = false;
					}
				}
				else {
					flag[i] = false;
				}
			}
			bool break_flag = flag[0];
			for (int i = 1; i < color_blocks.size(); ++i) {
				break_flag |= flag[i];
			}
			if (break_flag) {
				continue;
			}
			else {
				break;
			}
		}


		// for every two filled blocks
		//   if have continuous un-filled slots between them
		//     fill them with the same slope
		for (int i = 0; i < color_blocks.size(); ++i) {
			if (color_blocks[i].flag == PREFILLED || color_blocks[i].flag == slot_status::FILLED) {
				for (int j = i + 1; j < color_blocks.size(); ++j) {
					if (color_blocks[j].flag == PREFILLED || color_blocks[j].flag == slot_status::FILLED) {
						int dir = -1, num = 0;
						vector<int> records;
						if (num = OnlyHaveBlankBlock(color_blocks, i, j, dist_step, records)) {
							Point3i color_diff = (color_blocks[i].rgb - color_blocks[j].rgb) / (num + 1);
							Point3i color = color_blocks[j].rgb;
							for (int k = 0; k < num; ++k) {
								color += color_diff;
								// match color
								int min_color_diff = 1000, min_l = -1;
								for (int l = 0; l < color_blocks.size(); ++l) {
									if (color_blocks[l].flag != UPPER)
										continue;
									//                if (MyNorm(color_blocks[l].color, color) < 25) {
									//                 results[records[k]] = l;
									//                  break;
									//                }
									int temp = MyNorm(color_blocks[l].rgb, color);
									if (temp < min_color_diff) {
										changed = true;
										min_color_diff = temp;
										min_l = l;
										results[records[k]] = l;
										color_blocks[records[k]].flag = slot_status::FILLED;
										color_blocks[records[k]].rgb = color_blocks[l].rgb;
									}
								}
								if (min_l > -1 && min_l < color_blocks.size())
									color_blocks[min_l].flag = UPPERUSED;
							}
						}

					}
				}
			}
		}

		bool det = false;
		for (int i = 0; i < color_blocks.size(); ++i) {
			if (color_blocks[i].flag == UNFILLED) {
				det = true;
				break;
			}
		}
		if (det && changed)
			continue;
		else if (det)
			return false;
		else
			return true;
	}
	// finished
}

#endif