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

#include "image.h"
class MainWindow : public QWidget {
	Q_OBJECT
   public:
	MainWindow(QWidget *parent = nullptr);
   private slots:
	void openImage();
	void histogram_equalization();

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
};

#endif  // MAINWINDOW_H