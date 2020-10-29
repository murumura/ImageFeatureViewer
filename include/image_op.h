#ifndef IMAGE_OP_H
#define IMAGE_OP_H
#include <QImage>
#include <QPixmap>
#include <functional>
#include <iostream>
#include <type_traits>
#define MAX_PIX_SIZE 256
using img_op = std::function<QImage(QImage &&)>;
QImage apply_histogram_equalization(QImage &&src_img);
#endif