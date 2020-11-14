#ifndef UTIL_H
#define UTIL_H
#include <assert.h>

#include <QImage>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <type_traits>
#include <variant>
#include <vector>
/*   When the src QImage format is QImage::Format_RGB8888 and the enableDeepCopy == false,
 *  the ConvertQImageToMat() will change the pixel order of src QImage(RGB to BGR).
 *  ShadowCopy for show image(faster), DeepCopy for process image data(safer).
 */
using RGB_Mat_iter = cv::Mat_<cv::Vec3b>::iterator;
QImage ConvertMatToQImage(const cv::Mat& src, bool enableDeepCopy = false);
cv::Mat ConvertQImageToMat(const QImage& src, bool enableDeepCopy = false);
std::vector<uint8_t> surrounded_pixel(RGB_Mat_iter src_begin,
                                      RGB_Mat_iter src_end,
                                      RGB_Mat_iter src_left_bound,
                                      RGB_Mat_iter src_right_bound,
                                      RGB_Mat_iter src_iter,
                                      int width,
                                      int cur_channel);
#endif