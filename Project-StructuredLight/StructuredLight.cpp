#include "StructuredLight.h"

void GrayPhase::SetInput(const cv::Mat& img_bright, const cv::Mat& img_dark,
	const std::vector<cv::Mat>& imgs_gray_code,
	const std::vector<cv::Mat>& imgs_phase_shift)
{
	this->mask_map_ = img_bright - img_dark;
	this->imgs_gray_code_ = imgs_gray_code;
	this->imgs_phase_shift_ = imgs_phase_shift;

	cv::threshold(this->mask_map_, this->mask_map_, 0, 255,
		cv::THRESH_BINARY | cv::THRESH_OTSU);

	for (auto& iter : this->imgs_phase_shift_)
	{
		iter.convertTo(iter, CV_64FC1, 1.0);
	}
}

/// @brief ���������������Ƽ�����ֵͼ���Ը������ƽ��ж�ֵ��
/// @param threshold_map ��ֵͼ
/// @param imgs_phase_code ������������
void ThresholdMap(cv::Mat& threshold_map, const std::vector<cv::Mat>& imgs_phase_code)
{
	const int num = imgs_phase_code.size();
	threshold_map = cv::Mat::zeros(imgs_phase_code.front().size(), CV_64FC1);
	for (rsize_t index = 0; index < num; index++)
	{
		threshold_map += imgs_phase_code[index];
	}

	threshold_map /= num;
}

/// @brief �������ƵĶ�ֵ��
/// @param img_gray_code ����ֵ���ĸ�������
/// @param threshold_map ��ֵͼ
/// @param mask_map Maskͼ
void GrayCodesBinary(cv::Mat& img_gray_code, const cv::Mat& threshold_map,
	const cv::Mat& mask_map)
{
	const int w = mask_map.cols;
	const int h = mask_map.rows;

	for (size_t i = 0; i < h; i++)
	{
		for (size_t j = 0; j < w; j++)
		{
			if (mask_map.at<uchar>(i, j) != 0)
			{
				if (img_gray_code.at<uchar>(i, j) < std::round(threshold_map.at<double>(i, j)))
				{
					img_gray_code.at<uchar>(i, j) = 0;
				}
				else
				{
					img_gray_code.at<uchar>(i, j) = 1;
				}
			}
			else
			{
				img_gray_code.at<uchar>(i, j) = 0;
			}
		}
	}
}

/// @brief ���������λ
/// @param wrapped_phase ������λ
/// @param imgs_phase_shift ������������
/// @param mask_map Maskͼ
void WrappedPhase(cv::Mat& wrapped_phase, const std::vector<cv::Mat>& imgs_phase_shift,
	const cv::Mat& mask_map)
{
	const int w = mask_map.cols;
	const int h = mask_map.rows;

	wrapped_phase = cv::Mat::zeros(h, w, CV_64FC1);
	for (size_t i = 0; i < h; i++)
	{
		for (size_t j = 0; j < w; j++)
		{
			if (mask_map.at<uchar>(i, j) != 0)
			{
				// ����Ҫע������������������ͼ����λ˳��
				const double value0 = imgs_phase_shift[0].at<double>(i, j);
				const double value1 = imgs_phase_shift[1].at<double>(i, j);
				const double value2 = imgs_phase_shift[2].at<double>(i, j);
				const double value3 = imgs_phase_shift[3].at<double>(i, j);

				// ������λ�ļ��㷽ʽ���һ���������Ƶ���λ�й�
				/*if (value0 == value2 && value1 > value3)
				{
					wrapped_phase.at<double>(i, j) = 0;
				}
				else if (value1 == value3 && value0 > value2)
				{
					wrapped_phase.at<double>(i, j) = PI / 2;
				}
				else if (value0 == value2 && value1 < value3)
				{
					wrapped_phase.at<double>(i, j) = PI;
				}
				else if (value1 == value3 && value0 < value2)
				{
					wrapped_phase.at<double>(i, j) = 3 * PI / 2;
				}
				else if (value0 > value2 && value1 > value3)
				{
					wrapped_phase.at<double>(i, j) = std::atan((value0 - value2) / (value1 - value3));
				}
				else if (value0 > value2 && value1 < value3)
				{
					wrapped_phase.at<double>(i, j) = PI - std::atan((value0 - value2) / (value3 - value1));
				}
				else if (value0 < value2 && value1 < value3)
				{
					wrapped_phase.at<double>(i, j) = PI + std::atan((value0 - value2) / (value1 - value3));
				}
				else if (value0 < value2 && value1 > value3)
				{
					wrapped_phase.at<double>(i, j) = 2 * PI - std::atan((value2 - value0) / (value1 - value3));
				}*/

				if (value0 == value2 && value1 > value3)
				{
					wrapped_phase.at<double>(i, j) = 0;
				}
				else if (value1 == value3 && value0 > value2)
				{
					wrapped_phase.at<double>(i, j) = PI / 2;
				}
				else if (value0 == value2 && value1 < value3)
				{
					wrapped_phase.at<double>(i, j) = PI;
				}
				else if (value1 == value3 && value0 < value2)
				{
					wrapped_phase.at<double>(i, j) = 3 * PI / 2;
				}
				else if (value0 > value2 && value1 > value3)
				{
					wrapped_phase.at<double>(i, j) = std::atan((value0 - value2) / (value1 - value3));
				}
				else if (value0 > value2 && value1 < value3)
				{
					wrapped_phase.at<double>(i, j) = PI / 2 + std::atan((value3 - value1) / (value0 - value2));
				}
				else if (value0 < value2 && value1 < value3)
				{
					wrapped_phase.at<double>(i, j) = PI + std::atan((value2 - value0) / (value3 - value1));
				}
				else if (value0 < value2 && value1 > value3)
				{
					wrapped_phase.at<double>(i, j) = 3 * PI / 2 + std::atan((value1 - value3) / (value2 - value0));
				}
			}
		}
	}
}

/// @brief ��⼶��ͼ
/// @param fringe_order1 ����ͼ1
/// @param fringe_order2 ����ͼ2
/// @param imgs_gray_code ��������
/// @param mask_map Maskͼ
void FringeOrder(cv::Mat& fringe_order1, cv::Mat& fringe_order2, const std::vector<cv::Mat>& imgs_gray_code,
	const cv::Mat& mask_map)
{
	const int w = mask_map.cols;
	const int h = mask_map.rows;
	fringe_order1 = cv::Mat::zeros(h, w, CV_8UC1);
	fringe_order2 = cv::Mat::zeros(h, w, CV_8UC1);

	for (size_t i = 0; i < h; i++)
	{
		for (size_t j = 0; j < w; j++)
		{
			if (mask_map.at<uchar>(i, j) != 0)
			{
				const int value0 = imgs_gray_code[0].at<uchar>(i, j);
				const int value1 = imgs_gray_code[1].at<uchar>(i, j);
				const int value2 = imgs_gray_code[2].at<uchar>(i, j);
				const int value3 = imgs_gray_code[3].at<uchar>(i, j);
				const int value4 = imgs_gray_code[4].at<uchar>(i, j);

				const std::string code1 = std::to_string(value0) + std::to_string(value1)
					+ std::to_string(value2) + std::to_string(value3);
				auto itr1 = std::find(gray_code_4bits.begin(), gray_code_4bits.end(), code1);
				fringe_order1.at<uchar>(i, j) = std::distance(gray_code_4bits.begin(), itr1);

				const std::string code2 = std::to_string(value0) + std::to_string(value1)
					+ std::to_string(value2) + std::to_string(value3) + std::to_string(value4);
				auto itr2 = std::find(gray_code_5bits.begin(), gray_code_5bits.end(), code2);
				fringe_order2.at<uchar>(i, j) = std::floor((std::distance(gray_code_5bits.begin(), itr2) + 1) / 2);
			}
		}
	}
}

/// @brief ���������λ
/// @param unwrapped_phase ������λ
/// @param wrapped_phase ������λ
/// @param fringe_order1 ����ͼ1
/// @param fringe_order2 ����ͼ2
/// @param mask_map Maskͼ
void UnWrappedPhase(cv::Mat& unwrapped_phase, const cv::Mat& wrapped_phase,
	const cv::Mat& fringe_order1, const cv::Mat& fringe_order2, const cv::Mat& mask_map)
{
	const int w = mask_map.cols;
	const int h = mask_map.rows;

	unwrapped_phase = cv::Mat::zeros(h, w, CV_64FC1);
	for (size_t i = 0; i < h; i++)
	{
		for (size_t j = 0; j < w; j++)
		{
			if (mask_map.at<uchar>(i, j) != 0)
			{
				const double value0 = wrapped_phase.at<double>(i, j);
				const float value1 = fringe_order1.at<uchar>(i, j);
				const float value2 = fringe_order2.at<uchar>(i, j);
				if (value0 <= PI / 2)
				{
					unwrapped_phase.at<double>(i, j) = value0 + 2 * PI * value2;
				}
				else if (value0 > PI / 2 && value0 < 3 * PI / 2)
				{
					unwrapped_phase.at<double>(i, j) = value0 + 2 * PI * value1;
				}
				else if (value0 >= 3 * PI / 2)
				{
					unwrapped_phase.at<double>(i, j) = value0 + 2 * PI * (value2 - 1);
				}
			}
		}
	}
}

void GrayPhase::Compute()
{
	// �����ֵͼ
	ThresholdMap(this->threshold_map, this->imgs_phase_shift_);

	// �������ƶ�ֵ��
	for (auto& iter : this->imgs_gray_code_)
	{
		GrayCodesBinary(iter, this->threshold_map, this->mask_map_);
	}

	// ��������λ
	WrappedPhase(this->wrapped_phase_, this->imgs_phase_shift_, this->mask_map_);
	//this->wrapped_phase_ /= (2 * PI);

	// ��⼶��ͼ
	FringeOrder(this->fringe_order1_, this->fringe_order2_, this->imgs_gray_code_, this->mask_map_);

	// ��������λ
	UnWrappedPhase(this->unwrapped_phase_, this->wrapped_phase_,
		this->fringe_order1_, this->fringe_order2_, this->mask_map_);
	this->unwrapped_phase_.convertTo(this->unwrapped_phase_dis_, CV_8UC1, 255 / (32 * PI));
}
