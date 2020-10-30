#ifndef IMAGE_OP_H
#define IMAGE_OP_H
#include <QImage>
#include <QPixmap>
#include <functional>
#include <iostream>
#include <type_traits>
#define gray_scale 256
#define gray_scale_level ((gray_scale) - (1))
using img_op = std::function<QImage(QImage &&)>;
QImage apply_histogram_equalization(QImage &&src_img);
#endif