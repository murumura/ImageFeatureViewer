#ifndef IMAGE_OP_H
#define IMAGE_OP_H
#include <QImage>
#include <QPixmap>
#include <functional>
#include <iostream>
#include <type_traits>

#include "util.h"
#define gray_scale 256
#define gray_scale_level ((gray_scale) - (1))

/** function prototypes */
using img_op = std::function<QImage(QImage &)>;

template <typename Type>
using img_op_with_param = std::function<QImage(QImage &, Type)>;

QImage apply_histogram_equalization(QImage &src_img);
QImage apply_create_histogram(QImage &src_img);
QImage apply_extract_channel(QImage &src_img, int channel);
QImage apply_extract_r_channel(QImage &src_img);
QImage apply_extract_g_channel(QImage &src_img);
QImage apply_extract_b_channel(QImage &src_img);
QImage apply_transform_to_gray_scale(QImage &src_img);
QImage apply_median_filter(QImage &src_img);
template <typename Type>
QImage apply_threshold(QImage &src_img, Type threshold)
{
	QImage dst_img;
	cv::Mat src = ConvertQImageToMat(src_img);
	cv::Mat dst = cv::Mat(src_img.height(),
	                      src_img.width(),
	                      src.type());
	int color_space_n = src.type() == CV_8UC1 ? 1 : 3;
	cv::Mat_<cv::Vec3b>::iterator it_out = dst.begin<cv::Vec3b>();
	cv::Mat_<cv::Vec3b>::iterator it_ori = src.begin<cv::Vec3b>();
	cv::Mat_<cv::Vec3b>::iterator itend_ori = src.end<cv::Vec3b>();
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
#endif