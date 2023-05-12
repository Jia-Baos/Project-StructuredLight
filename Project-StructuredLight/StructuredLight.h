#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <opencv2/opencv.hpp>

const double PI = std::acos(-1);

const std::vector<std::string> gray_code_4bits
{
	"0000","0001","0011","0010","0110","0111","0101","0100",
	"1100","1101","1111","1110","1010","1011","1001","1000"
};

const std::vector<std::string> gray_code_5bits
{
	"00000","00001","00011","00010","00110","00111","00101","00100",
	"01100","01101","01111","01110","01010","01011","01001","01000",
	"11000","11001","11011","11010","11110","11111","11101","11100",
	"10100","10101","10111","10110","10010","10011","10001","10000"
};

class StructuredLight
{
public:
	cv::Mat mask_map_;	// Maskͼ
	cv::Mat threshold_map;	// ��ֵͼ
	cv::Mat wrapped_phase_;	// ������λ
	cv::Mat unwrapped_phase_;	// ������λ
	cv::Mat unwrapped_phase_dis_;	// ������λ���ӻ�

	cv::Mat fringe_order1_;	// ����ͼ1
	cv::Mat fringe_order2_;	// ����ͼ2

	std::vector<cv::Mat> imgs_gray_code_;	// ��������
	std::vector<cv::Mat> imgs_phase_shift_;	// ������������

	/// @brief �ṹ��������
	void Compute();

	/// @brief ��ȡ�ṹ������ͼ��
	/// @param img_bright ȫ����ͼ
	/// @param img_dark ȫ�ְ�ͼ
	/// @param imgs_gray_code ��������
	/// @param imgs_phase_shift ������������
	void SetInput(const cv::Mat& img_bright, const cv::Mat& img_dark,
		const std::vector<cv::Mat>& imgs_gray_code,
		const std::vector<cv::Mat>& imgs_phase_shift);
};
