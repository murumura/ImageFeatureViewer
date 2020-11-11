#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QAction>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QMainWindow>
#include <QMap>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QStatusBar>
#include <QToolBar>
#include <QVector>
#include <QWidget>
#include <memory>
#include <vector>

#include "image.h"
class MainWindow : public QWidget {
	Q_OBJECT
   public:
	MainWindow(QWidget *parent = nullptr);
   private slots:
	void openImage();
	void histogram_equalization();
	void extract_R_channel();
	void extract_G_channel();
	void extract_B_channel();
	void transform_to_gray();
	void image_threshold();
	void median_filter();
	void mean_filter();
	void soble_filter();
	void undo_image_transform();

   private:
	void initUI();
	void createActions();
	void setupMatrix();
	void populateScene();
	void showImage(QString);
	void clearScene();
	void updateScene();
	void updateHistogramScene();
	QMenuBar *menuBar;

	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *exitMenu;

	QAction *openAction;
	QAction *edit_Histogram_Action;
	QAction *edit_extract_R_Action;
	QAction *edit_extract_G_Action;
	QAction *edit_extract_B_Action;
	QAction *edit_transform_to_Gray_Action;
	QAction *edit_threshold_Action;
	QAction *edit_median_filter_Action;
	QAction *edit_mean_filter_Action;
	QAction *edit_sobel_filter_Action;
	QAction *undoAction;
	QAction *exitAction;
	QGraphicsScene *top_scene;
	QGraphicsScene *top_histogram_scene;
	QGraphicsScene *bottom_scene;
	QGraphicsScene *bottom_histogram_scene;
	QVector<QGraphicsScene *> activeSceneList;
	QSplitter *h1Splitter;
	QSplitter *h2Splitter;

	QString currentImagePath;
	CVImage *CurImage;
	CVImage *ProcImage;
	std::vector<QImage> BackUpImg;
};

#endif  // MAINWINDOW_H