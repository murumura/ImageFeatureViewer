#include "image_op.h"

#include <algorithm>
#include <array>
#include <numeric>
#include <type_traits>
#include <variant>

#include "util.h"
using RGB_Mat_iter = cv::Mat_<cv::Vec3b>::iterator;
template <typename CV_format_type>
QImage transform_img(const QImage& src_img, CV_format_type type)
{
	int color_space_n = (type == CV_8UC1) ? 1 : 3;
	std::vector<unsigned int> hist(gray_scale * color_space_n, 0);
	std::vector<unsigned int> hist_cdf(gray_scale * color_space_n, 0);
	std::vector<unsigned int> normalized(gray_scale * color_space_n, 0);
	int h = src_img.height();
	int w = src_img.width();
	const int img_size = w * h;
	cv::Mat dst = cv::Mat(src_img.height(),
	                      src_img.width(),
	                      type,
	                      const_cast<unsigned char*>(src_img.constBits()),
	                      static_cast<size_t>(src_img.bytesPerLine()));

	cv::Mat src = ConvertQImageToMat(src_img);
	/** occurrence number of a pixel of level i=0~256 */
	RGB_Mat_iter it_src = src.begin<cv::Vec3b>();
	RGB_Mat_iter itend_src = src.end<cv::Vec3b>();
	for (; it_src != itend_src; it_src++) {
		for (int k = 0; k < color_space_n; k++) {
			uint8_t idx = static_cast<uint8_t>((*it_src)[k]);
			hist[idx + k * gray_scale]++;
		}
	}
	/**calculate cdf corresponding to pixel */
	for (int k = 0; k < color_space_n; k++)
		std::partial_sum(hist.begin() + k * gray_scale,
		                 hist.begin() + (k + 1) * gray_scale,
		                 hist_cdf.begin() + k * gray_scale);

	// using general histogram equalization formula
	for (int k = 0; k < color_space_n; k++)
		std::transform(hist_cdf.begin() + k * gray_scale,
		               hist_cdf.begin() + (k + 1) * gray_scale,
		               normalized.begin() + k * gray_scale,
		               [&](unsigned int hist_cdf_val) {
			               return static_cast<unsigned int>(
			                   (hist_cdf_val - hist_cdf[k * gray_scale]) * (gray_scale_level) / (img_size - hist_cdf[k * gray_scale]));
		               });

	RGB_Mat_iter it_out = dst.begin<cv::Vec3b>();
	RGB_Mat_iter it_ori = src.begin<cv::Vec3b>();
	RGB_Mat_iter itend_ori = src.end<cv::Vec3b>();
	for (; it_ori != itend_ori; it_ori++) {
		for (int k = 0; k < color_space_n; k++) {
			uint8_t pixel_value = static_cast<uint8_t>((*it_ori)[k]);
			(*it_out)[k] = normalized[pixel_value + k * gray_scale];
		}
		it_out++;
	}

	return ConvertMatToQImage(dst, true);
}
QImage apply_histogram_equalization(QImage& src_img)
{
	int h = src_img.height();
	int w = src_img.width();
	const int img_size = w * h;
	QImage dst_img;
	switch (src_img.format()) {
	case QImage::Format_Grayscale8:
		dst_img = transform_img(src_img, CV_8UC1);
		break;
	case QImage::Format_RGB32:
	case QImage::Format_RGB888:
		dst_img = transform_img(src_img, CV_8UC4);
		break;
	}
	return dst_img;
}
template <typename CV_format_type>
QImage draw_image_histogram(const QImage& src_img, CV_format_type type)
{
	using namespace cv;
	using namespace std;
	cv::Mat src = ConvertQImageToMat(src_img);

	vector<Mat> bgr_planes;
	split(src, bgr_planes);
	int histSize = 256;
	float range[] = {0, 256};  //the upper boundary is exclusive
	const float* histRange = {range};
	bool uniform = true, accumulate = false;
	int color_space_n = type == CV_8UC1 ? 1 : 3;
	vector<Mat> hists(color_space_n);
	Mat b_hist, g_hist, r_hist;
	for (int i = 0; i < color_space_n; i++) {
		calcHist(&bgr_planes[i], 1, 0, Mat(), hists[i], 1, &histSize, &histRange, uniform, accumulate);
	}

	int hist_w = 512, hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);
	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
	for (int i = 0; i < color_space_n; i++) {
		normalize(hists[i], hists[i], 0, histImage.rows, NORM_MINMAX, -1, Mat());
	}

	for (int i = 1; i < histSize; i++) {
		for (int j = 0; j < color_space_n; j++) {
			line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hists[j].at<float>(i - 1))),
			     Point(bin_w * (i), hist_h - cvRound(hists[j].at<float>(i))),
			     Scalar(j == 0 ? 255 : 0, j == 1 ? 255 : 0, j == 2 ? 255 : 0), 2, 8, 0);
		}
	}

	return ConvertMatToQImage(histImage, true);
}
QImage apply_create_histogram(QImage& src_img)
{
	QImage dst_img;
	switch (src_img.format()) {
	case QImage::Format_Grayscale8:
		dst_img = draw_image_histogram(src_img, CV_8UC1);
		break;
	case QImage::Format_RGB32:
	case QImage::Format_RGB888:
		dst_img = draw_image_histogram(src_img, CV_8UC3);
		break;
	}
	return dst_img;
}
template <typename CV_format_type>
QImage extract_channel(const QImage& src_img, int channel, CV_format_type type)
{
	if (type == CV_8UC1)
		return src_img.copy();
	int h = src_img.height();
	int w = src_img.width();
	const int img_size = w * h;
	const int color_space_n = 3;
	cv::Mat dst = cv::Mat(src_img.height(),
	                      src_img.width(),
	                      CV_8UC1);

	cv::Mat src = ConvertQImageToMat(src_img);

	cv::Mat_<cv::Vec3b>::iterator it_out = dst.begin<cv::Vec3b>();
	cv::Mat_<cv::Vec3b>::iterator it_ori = src.begin<cv::Vec3b>();
	cv::Mat_<cv::Vec3b>::iterator itend_ori = src.end<cv::Vec3b>();
	for (; it_ori != itend_ori; it_ori++) {
		for (int k = 0; k < color_space_n; k++) {
			if (k == channel)
				(*it_out)[k] = (*it_ori)[k];
		}
		it_out++;
	}
	return ConvertMatToQImage(dst, true);
}
QImage apply_extract_channel(QImage& src_img, int channel)
{
	QImage dst_img;
	switch (src_img.format()) {
	case QImage::Format_Grayscale8:
		dst_img = extract_channel(src_img, channel, CV_8UC1);
		break;
	case QImage::Format_RGB32:
	case QImage::Format_RGB888:
		dst_img = extract_channel(src_img, channel, CV_8UC4);
		break;
	}
	return dst_img;
}
QImage apply_extract_r_channel(QImage& src_img)
{
	return apply_extract_channel(src_img, 2);
}
QImage apply_extract_g_channel(QImage& src_img)
{
	return apply_extract_channel(src_img, 1);
}
QImage apply_extract_b_channel(QImage& src_img)
{
	return apply_extract_channel(src_img, 0);
}
QImage apply_transform_to_gray_scale(QImage& src_img)
{
	switch (src_img.format()) {
	case QImage::Format_Grayscale8:
		return src_img.copy();
		break;
	case QImage::Format_RGB32:
	case QImage::Format_RGB888:
	default:
		cv::Mat dst = cv::Mat(src_img.height(),
		                      src_img.width(),
		                      CV_8UC1);
		cv::Mat src = ConvertQImageToMat(src_img);
		cv::Mat_<uchar>::iterator it_out = dst.begin<uchar>();
		cv::Mat_<cv::Vec3b>::iterator it_ori = src.begin<cv::Vec3b>();
		cv::Mat_<cv::Vec3b>::iterator itend_ori = src.end<cv::Vec3b>();
		for (; it_ori != itend_ori; it_ori++) {
			/**Gray = R*0.299 + G*0.587 + B*0.114 */
			(*it_out) = (*it_ori)[0] * 0.114 + (*it_ori)[1] * 0.587 + (*it_ori)[2] * 0.299;
			it_out++;
		}
		return ConvertMatToQImage(dst, true);
		break;
	}
}
