#include "util.h"

#include <iostream>

QImage ConvertMatToQImage(const cv::Mat& src, bool enableDeepCopy)
{
	QImage dst;
	switch (src.type()) {
		// Gray image
	case CV_8UC1:
		dst = QImage(src.data, src.cols, src.rows, static_cast<int>(src.step), QImage::Format_Grayscale8);
		dst.setColorCount(256);
		for (int i = 0; i < 256; i++)
			dst.setColor(i, qRgb(i, i, i));
		break;
		// BRG image
	case CV_8UC3:
		dst = QImage(src.data, src.cols, src.rows, static_cast<int>(src.step), QImage::Format_RGB888);
		break;
		// BGRA image
	case CV_8UC4:
		dst = QImage(src.data, src.cols, src.rows, static_cast<int>(src.step), QImage::Format_RGB32);
		break;
	default:
		bool Nonsupport_Mat_Format = false;
		assert(Nonsupport_Mat_Format == true);
	}
	if (enableDeepCopy) {
		return dst.copy();
	}
	return dst;
}
cv::Mat ConvertQImageToMat(const QImage& src, bool enableDeepCopy)
{
	cv::Mat dst;
	switch (src.format()) {
	// Gray image
	case QImage::Format_Grayscale8:
	case QImage::Format_Indexed8:
		dst = cv::Mat(src.height(), src.width(), CV_8UC1, const_cast<unsigned char*>(src.constBits()), static_cast<size_t>(src.bytesPerLine()));
		break;
	// BGR or RGB image
	case QImage::Format_RGB888:
		dst = cv::Mat(src.height(), src.width(), CV_8UC3, const_cast<unsigned char*>(src.constBits()), static_cast<size_t>(src.bytesPerLine()));
		if (enableDeepCopy) {
			dst = dst.clone();
		}
		break;
		// BGRA
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32:
	case QImage::Format_ARGB32_Premultiplied:
		dst = cv::Mat(src.height(), src.width(), CV_8UC4, const_cast<unsigned char*>(src.constBits()), static_cast<size_t>(src.bytesPerLine()));
		break;
	default:
		bool Nonsupport_QImage_Format = false;
		assert(Nonsupport_QImage_Format == true);
	}
	if (enableDeepCopy) {
		return dst.clone();
	}
	return dst;
}