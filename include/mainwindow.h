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
#include <QWidget>

#include "image.h"
class MainWindow : public QWidget {
	Q_OBJECT
   public:
	MainWindow(QWidget *parent = nullptr);
   private slots:
	void openImage();

   private:
	void initUI();
	void createActions();
	void setupMatrix();
	void populateScene();
	void showImage(QString);
	QMenuBar *menuBar;

	QMenu *fileMenu;
	QMenu *exitMenu;

	QAction *openAction;
	QAction *exitAction;
	QGraphicsScene *top_scene;
	QGraphicsScene *bottom_scene;
	QSplitter *h1Splitter;
	QSplitter *h2Splitter;

	QString currentImagePath;
	CVImage *CurImage;
};

#endif  // MAINWINDOW_H