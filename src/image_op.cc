#include "image_op.h"

#include <algorithm>
#include <array>
#include <numeric>
#include <type_traits>
#include <variant>

#include "util.h"

QImage apply_histogram_equalization(QImage& src_img)
{
	cv::Mat src = ConvertQImageToMat(src_img);
	cv::Mat dst = cv::Mat(src_img.height(),
	                      src_img.width(),
	                      src.type());
	const int img_size = src_img.height() * src_img.width();
	int color_space_n = src.type() == CV_8UC1 ? 1 : 3;
	std::vector<unsigned int> hist(gray_scale * color_space_n, 0);
	std::vector<unsigned int> hist_cdf(gray_scale * color_space_n, 0);
	std::vector<unsigned int> normalized(gray_scale * color_space_n, 0);

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
QImage apply_create_histogram(QImage& src_img)
{
	using namespace cv;
	using namespace std;
	int hist_w = 512, hist_h = 400;
	cv::Mat src = ConvertQImageToMat(src_img);
	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	int color_space_n = src.type() == CV_8UC1 ? 1 : 3;
	vector<Mat> bgr_planes;
	split(src, bgr_planes);
	int histSize = 256;
	float range[] = {0, 256};  //the upper boundary is exclusive
	const float* histRange = {range};
	bool uniform = true, accumulate = false;
	vector<Mat> hists(color_space_n);
	Mat b_hist, g_hist, r_hist;
	for (int i = 0; i < color_space_n; i++) {
		calcHist(&bgr_planes[i], 1, 0, Mat(), hists[i], 1, &histSize, &histRange, uniform, accumulate);
	}
	int bin_w = cvRound((double)hist_w / histSize);
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
		RGB_Mat_iter it_ori = src.begin<cv::Vec3b>();
		RGB_Mat_iter itend_ori = src.end<cv::Vec3b>();
		for (; it_ori != itend_ori; it_ori++) {
			/**Gray = R*0.299 + G*0.587 + B*0.114 */
			(*it_out) = (*it_ori)[0] * 0.114 + (*it_ori)[1] * 0.587 + (*it_ori)[2] * 0.299;
			it_out++;
		}
		return ConvertMatToQImage(dst, true);
		break;
	}
}
QImage apply_median_filter(QImage& src_img)
{
	cv::Mat src = ConvertQImageToMat(src_img);
	cv::Mat dst = cv::Mat(src_img.height(),
	                      src_img.width(),
	                      src.type());
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
			std::nth_element(near_pixel.begin(), near_pixel.begin() + 4, near_pixel.end());
			(*it_out)[k] = near_pixel[4];
		}
		if (it_ori == it_right_bound) {
			it_right_bound = it_right_bound + src.cols;
			it_left_bound = it_left_bound + src.cols;
		}
		it_out++;
	}
	return ConvertMatToQImage(dst, true);
}
QImage apply_mean_filter(QImage& src_img)
{
	cv::Mat src = ConvertQImageToMat(src_img);
	cv::Mat dst = cv::Mat(src_img.height(),
	                      src_img.width(),
	                      src.type());
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
			(*it_out)[k] = std::accumulate(near_pixel.begin(), near_pixel.end(), 0) / 9;
		}
		if (it_ori == it_right_bound) {
			it_right_bound = it_right_bound + src.cols;
			it_left_bound = it_left_bound + src.cols;
		}
		it_out++;
	}
	return ConvertMatToQImage(dst, true);
}
QImage apply_merge_image(const QImage& src_img, const QImage& src_img2, int channel)
{
	cv::Mat src1 = ConvertQImageToMat(src_img);
	cv::Mat src2 = ConvertQImageToMat(src_img2);

	cv::Mat dst = cv::Mat(src_img.height(),
	                      src_img.width(),
	                      CV_8UC3, cv::Scalar(255, 255, 255));
	int color_space_n = 3;

	RGB_Mat_iter it_out = dst.begin<cv::Vec3b>();
	/**iterator for source image 1 */
	RGB_Mat_iter it_ori_1 = src1.begin<cv::Vec3b>();
	RGB_Mat_iter itend_ori_1 = src1.end<cv::Vec3b>();
	/**iterator for source image 2 */
	RGB_Mat_iter it_ori_2 = src2.begin<cv::Vec3b>();
	RGB_Mat_iter itend_ori_2 = src2.end<cv::Vec3b>();

	for (; it_ori_1 != itend_ori_1; it_ori_1++, it_ori_2++) {
		for (int k = 0; k < color_space_n; k++) {
			if (k == channel)
				(*it_out)[k] = (*it_ori_2)[channel];
			else
				(*it_out)[k] = (*it_ori_1)[k] * 0.5;
		}
		it_out++;
	}

	return ConvertMatToQImage(dst, true);
}
QImage apply_transform_image_postion(const QImage& src_img, const QImage& dst_img)
{
	cv::Mat src = ConvertQImageToMat(src_img);
	cv::Mat dst = ConvertQImageToMat(dst_img);
	std::vector<cv::Point2f> pts_src, pts_dst;
	pts_src.push_back(cv::Point2f(0, 0));
	pts_src.push_back(cv::Point2f(0, src.cols - 1));
	pts_src.push_back(cv::Point2f(src.rows - 1, 0));
	pts_src.push_back(cv::Point2f(src.rows - 1, src.cols - 1));

	pts_dst.push_back(cv::Point2f(0, 0));
	pts_dst.push_back(cv::Point2f(0, dst.cols - 1));
	pts_dst.push_back(cv::Point2f(dst.rows - 1, 0));
	pts_dst.push_back(cv::Point2f(dst.rows - 1, dst.cols - 1));
	cv::Mat h = findHomography(pts_src, pts_dst);
	cv::warpPerspective(src, dst, h, dst.size());
	return ConvertMatToQImage(dst, true);
}