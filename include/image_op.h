#ifndef IMAGE_OP_H
#define IMAGE_OP_H
#include <iostream>
#include <type_traits>
#include <functional>
#include <QImage>
template <typename ResultType=QImage, typename ... ArgumentTypes>
class Image_operator{
    public:
        std::function<ResultType(ArgumentTypes&&...)> call;
        std::function<ResultType(ArgumentTypes&&...)> destroy;
};
#endif