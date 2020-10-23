#ifndef EDITOTR_INTERFACE
#define EDITOTR_INTERFACE
#include <QObject>
#include <QtPlugin>

#include "opencv2/opencv.hpp"
class EditorPluginInterface {
   public:
	virtual ~EditorPluginInterface() {}
	virtual QString name() = 0;
	virtual void edit(const cv::Mat &input, cv::Mat &output) = 0;
};
#endif