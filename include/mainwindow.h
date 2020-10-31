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

#include "image.h"
class MainWindow : public QWidget {
	Q_OBJECT
   public:
	MainWindow(QWidget *parent = nullptr);
   private slots:
	void openImage();
	void histogram_equalization();
	void extract_channel();
	void transform_to_gray();

   private:
	void initUI();
	void createActions();
	void setupMatrix();
	void populateScene();
	void showImage(QString);
	void clearScene();
	void updateScene();
	QMenuBar *menuBar;

	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *exitMenu;

	QAction *openAction;
	QAction *edit_Histogram_Action;
	QAction *edit_extract_RGB_Action;
	QAction *edit_transform_to_Gray_Action;
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
};

#endif  // MAINWINDOW_H