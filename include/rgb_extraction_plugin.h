#ifndef RGB_EXTRACTION_PLUGIN_H
#define RGB_EXTRACTION_PLUGIN_H
#include <QObject>
#include <QtPlugin>

#include "editor_plugin_interface.h"
class rgbExtractionPlugin : public QObject, public EditorPluginInterface {
	Q_OBJECT
   public:
	QString name();
	void edit(const cv::Mat &input, cv::Mat &output);
};
#endif