#ifndef UTIL_H
#define UTIL_H
#include <assert.h>

#include <QImage>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
/*   When the src QImage format is QImage::Format_RGB8888 and the enableDeepCopy == false,
 *  the ConvertQImageToMat() will change the pixel order of src QImage(RGB to BGR).
 *  ShadowCopy for show image(faster), DeepCopy for process image data(safer).
 */
QImage ConvertMatToQImage(const cv::Mat& src, bool enableDeepCopy = false);
cv::Mat ConvertQImageToMat(const QImage& src, bool enableDeepCopy = false);
#endif