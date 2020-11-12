#ifndef IMAGE_OP_H
#define IMAGE_OP_H
#include <QImage>
#include <QPixmap>
#include <functional>
#include <iostream>
#include <numeric>  // std::inner_product
#include <type_traits>

#include "util.h"
#define gray_scale 256
#define gray_scale_level ((gray_scale) - (1))

/** function prototypes */
using img_op = std::function<QImage(QImage &)>;

/** function prototypes with only one parameter*/
template <typename Type>
using img_op_with_param = std::function<QImage(QImage &, Type)>;


template <typename Type = std::vector<int>>
QImage apply_sobel_combine_filter(QImage &src_img, Type sobel_kernels) {
    QImage dst_img;
	cv::Mat src = ConvertQImageToMat(src_img);
	cv::Mat dst = cv::Mat(src_img.height(),
	                      src_img.width(),
	                      src.type());
	//sanity check
	if constexpr (std::is_same_v<Type, std::vector<int>>) {
		int color_space_n = src.type() == CV_8UC1 ? 1 : 3;
		RGB_Mat_iter it_out = dst.begin<cv::Vec3b>();
		RGB_Mat_iter it_ori = src.begin<cv::Vec3b>();
		RGB_Mat_iter it_right_bound = src.begin<cv::Vec3b>() + src.cols - 1;
		RGB_Mat_iter it_left_bound = src.begin<cv::Vec3b>();
		RGB_Mat_iter itend_ori = src.end<cv::Vec3b>();
		for (; it_ori != itend_ori; it_ori++) {
			for (int k = 0; k < color_space_n; k++) {
				std::vector<uchar> near_pixel = surrounded_pixel(src.begin<cv::Vec3b>(),
				                                                 src.end<cv::Vec3b>(),
				                                                 it_left_bound,
				                                                 it_right_bound,
				                                                 it_ori,
				                                                 src.cols,
				                                                 k);
				std::vector<int> Int_near_pixel(near_pixel.size());
				std::transform(
				    std::begin(near_pixel),
				    std::end(near_pixel),
				    std::begin(Int_near_pixel), [](uchar i) { return static_cast<int>(i); });
				int inner_product = std::inner_product(std::begin(Int_near_pixel), std::end(Int_near_pixel), std::begin(sobel_kernels), 0.0);
				(*it_out)[k] = (inner_product > 150) ? 255 : (inner_product < 0 ? 0 : inner_product);
			}
			if (it_ori == it_right_bound) {
				it_right_bound = it_right_bound + src.cols;
				it_left_bound = it_left_bound + src.cols;
			}
			it_out++;
		}
	}
	return ConvertMatToQImage(dst, true);
}

QImage apply_histogram_equalization(QImage &src_img);
QImage apply_create_histogram(QImage &src_img);
QImage apply_transform_to_gray_scale(QImage &src_img);
QImage apply_median_filter(QImage &src_img);
QImage apply_mean_filter(QImage &src_img);

template <typename Type = std::vector<int>>
QImage apply_sobel_filter(QImage &src_img, Type sobel_kernels)
{
	QImage dst_img;
	cv::Mat src = ConvertQImageToMat(src_img);
	cv::Mat dst = cv::Mat(src_img.height(),
	                      src_img.width(),
	                      src.type());
	//sanity check
	if constexpr (std::is_same_v<Type, std::vector<int>>) {
		int color_space_n = src.type() == CV_8UC1 ? 1 : 3;
		RGB_Mat_iter it_out = dst.begin<cv::Vec3b>();
		RGB_Mat_iter it_ori = src.begin<cv::Vec3b>();
		RGB_Mat_iter it_right_bound = src.begin<cv::Vec3b>() + src.cols - 1;
		RGB_Mat_iter it_left_bound = src.begin<cv::Vec3b>();
		RGB_Mat_iter itend_ori = src.end<cv::Vec3b>();
		for (; it_ori != itend_ori; it_ori++) {
			for (int k = 0; k < color_space_n; k++) {
				std::vector<uchar> near_pixel = surrounded_pixel(src.begin<cv::Vec3b>(),
				                                                 src.end<cv::Vec3b>(),
				                                                 it_left_bound,
				                                                 it_right_bound,
				                                                 it_ori,
				                                                 src.cols,
				                                                 k);
				std::vector<int> Int_near_pixel(near_pixel.size());
				std::transform(
				    std::begin(near_pixel),
				    std::end(near_pixel),
				    std::begin(Int_near_pixel), [](uchar i) { return static_cast<int>(i); });
				int inner_product = std::inner_product(std::begin(Int_near_pixel), std::end(Int_near_pixel), std::begin(sobel_kernels), 0.0);
				(*it_out)[k] = (inner_product > 150) ? 255 : (inner_product < 0 ? 0 : inner_product);
			}
			if (it_ori == it_right_bound) {
				it_right_bound = it_right_bound + src.cols;
				it_left_bound = it_left_bound + src.cols;
			}
			it_out++;
		}
	}
	return ConvertMatToQImage(dst, true);
}
template <typename Type>
QImage apply_threshold(QImage &src_img, Type threshold)
{
	QImage dst_img;
	cv::Mat src = ConvertQImageToMat(src_img);
	cv::Mat dst = cv::Mat(src_img.height(),
	                      src_img.width(),
	                      src.type());
	int color_space_n = src.type() == CV_8UC1 ? 1 : 3;
	RGB_Mat_iter it_out = dst.begin<cv::Vec3b>();
	RGB_Mat_iter it_ori = src.begin<cv::Vec3b>();
	RGB_Mat_iter itend_ori = src.end<cv::Vec3b>();
	for (; it_ori != itend_ori; it_ori++) {
		for (int k = 0; k < color_space_n; k++) {
			if ((*it_ori)[k] >= threshold)
				(*it_out)[k] = 255;
			else
				(*it_out)[k] = 0;
		}
		it_out++;
	}
	return ConvertMatToQImage(dst, true);
}
template <typename Type>
QImage apply_extract_channel(const QImage &src_img, Type channel)
{
	QImage dst_img;
	cv::Mat src = ConvertQImageToMat(src_img);
	cv::Mat dst = cv::Mat(src_img.height(),
	                      src_img.width(),
	                      CV_8UC1);
	int color_space_n = src.type() == CV_8UC1 ? 1 : 3;
	if (color_space_n == 1) {
		return src_img.copy();
	}

	RGB_Mat_iter it_out = dst.begin<cv::Vec3b>();
	RGB_Mat_iter it_ori = src.begin<cv::Vec3b>();
	RGB_Mat_iter itend_ori = src.end<cv::Vec3b>();
	for (; it_ori != itend_ori; it_ori++) {
		for (int k = 0; k < color_space_n; k++) {
			if (k == channel)
				(*it_out)[k] = (*it_ori)[k];
		}
		it_out++;
	}

	return ConvertMatToQImage(dst, true);
}
#endif