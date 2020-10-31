#ifndef IMAGE_OP_H
#define IMAGE_OP_H
#include <QImage>
#include <QPixmap>
#include <functional>
#include <iostream>
#include <type_traits>
#define gray_scale 256
#define gray_scale_level ((gray_scale) - (1))
using img_op = std::function<QImage(QImage &)>;
QImage apply_histogram_equalization(QImage &src_img);
QImage apply_create_histogram(QImage &src_img);
QImage apply_extract_channel(QImage &src_img, int channel);
QImage apply_extract_r_channel(QImage &src_img);
QImage apply_extract_g_channel(QImage &src_img);
QImage apply_extract_b_channel(QImage &src_img);
#endif