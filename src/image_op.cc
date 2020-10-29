#include "image_op.h"

#include <algorithm>
#include <array>
#include <numeric>

#include "util.h"
QImage transform_gray_img(const QImage& src_img)
{
	std::array<uint8_t, MAX_PIX_SIZE> hist;
	std::array<uint8_t, MAX_PIX_SIZE> hist_cdf;
	std::array<uint8_t, MAX_PIX_SIZE> normalized;
    
	int h = src_img.height();
	int w = src_img.width();
	const int img_size = w * h;
	QImage dst_img = src_img;
	cv::Mat dst = cv::Mat(src_img.height(),
	                      src_img.width(),
	                      CV_8UC1,
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
	               [&](uint8_t hist_cdf_val) {
		               return Round(
		                   (hist_cdf_val - hist_cdf[0]) * (255) / (img_size - hist_cdf[0]));
	               });

	cv::Mat_<uchar>::iterator it_out = dst.begin<uchar>();
	cv::Mat_<uchar>::iterator it_ori = src.begin<uchar>();
	cv::Mat_<uchar>::iterator itend_ori = src.end<uchar>();
	for (; it_ori != itend_ori; it_ori++) {
		unsigned int pixel_value = static_cast<uint8_t>(*it_ori);
		*it_out = normalized[pixel_value];
		it_out++;
	}
	return ConvertMatToQImage(dst, true);
}
QImage apply_histogram_equalization(QImage&& src_img)
{
	int h = src_img.height();
	int w = src_img.width();
	const int img_size = w * h;
	QImage dst_img;
	switch (src_img.format()) {
	case QImage::Format_Grayscale8:
		dst_img = transform_gray_img(src_img);
	case QImage::Format_RGB32:
		break;
	case QImage::Format_RGB888:
		break;
	}
	return dst_img;
}
