#include "image_op.h"

#include <algorithm>
#include <array>
#include <numeric>
#include <type_traits>

#include "util.h"
template <typename CV_format_type>
QImage transform_img(const QImage& src_img, CV_format_type type)
{
	std::array<unsigned int, gray_scale> hist = {};
	std::array<unsigned int, gray_scale> hist_cdf = {};
	std::array<unsigned int, gray_scale> normalized = {};

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
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			uint8_t idx;
			idx = static_cast<uint8_t>(src.at<uchar>(i, j));
			hist[idx]++;
		}
	}
	/**calculate cdf corresponding to pixel */
	std::partial_sum(hist.begin(), hist.end(), hist_cdf.begin());

	// using general histogram equalization formula
	std::transform(hist_cdf.begin(), hist_cdf.end(), normalized.begin(),
	               [&](unsigned int hist_cdf_val) {
		               return static_cast<unsigned int>(
		                   (hist_cdf_val - hist_cdf[0]) * (gray_scale_level) / (img_size - hist_cdf[0]));
	               });

	cv::Mat_<uchar>::iterator it_out = dst.begin<uchar>();
	cv::Mat_<uchar>::iterator it_ori = src.begin<uchar>();
	cv::Mat_<uchar>::iterator itend_ori = src.end<uchar>();
	for (; it_ori != itend_ori; it_ori++) {
		unsigned int pixel_value = static_cast<unsigned int>(*it_ori);
		*it_out = normalized[pixel_value];
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

	return ConvertMatToQImage(histImage,true);
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
		dst_img = draw_image_histogram(src_img, CV_8UC4);
		break;
	}
	return dst_img;
}
