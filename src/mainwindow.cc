#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPixmap>
#include <QPluginLoader>
#include <QSplitter>
#include <QList>

#include "chip.h"
#include "view.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      top_scene(new QGraphicsScene(0,0,1700,1000,this)),
	  bottom_scene(new QGraphicsScene(0,0,1700,1000,this)),
      h1Splitter(new QSplitter(this)),
      h2Splitter(new QSplitter(this)),
      fileMenu(nullptr),
      exitMenu(nullptr)
{
	QSplitter *vSplitter = new QSplitter();
	vSplitter->setOrientation(Qt::Vertical);
	
	vSplitter->addWidget(h1Splitter);
	vSplitter->addWidget(h2Splitter);


	View *view = new View("Top view");
	view->view()->setScene(top_scene);
	h1Splitter->addWidget(view);
	
	view = new View("Bottom view");
	view->view()->setScene(bottom_scene);
	h2Splitter->addWidget(view);
	
	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(vSplitter);
	initUI();
	layout->addWidget(menuBar);
	setLayout(layout);
	setWindowTitle(tr("Image Viewer"));
}
void MainWindow::initUI()
{
	menuBar = new QMenuBar();
	fileMenu = menuBar->addMenu("&File");
	exitMenu = menuBar->addMenu("&Exit");
	createActions();
}
void MainWindow::createActions()
{
	// create actions, add them to menus
	openAction = new QAction("&Open", this);
	fileMenu->addAction(openAction);
	exitAction = new QAction("&Exit", this);
	exitMenu->addAction(exitAction);
	// connect the signals and slots
	connect(exitAction, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
	connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));
}
void MainWindow::openImage()
{
	QFileDialog dialog(this);
	dialog.setWindowTitle("Open Image");
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
	QStringList filePaths;
	if (dialog.exec()) {
		filePaths = dialog.selectedFiles();
		showImage(filePaths.at(0));
	}
}
void MainWindow::showImage(QString path)
{
	QPixmap image(path);
	currentImagePath = path;
	populateScene();
}
void MainWindow::populateScene()
{
	/** In the QSplitter, add a custom QWidget which uses a QImage to draw in it's repaint methods. */
	
	CVImage *item = new CVImage(currentImagePath);
	item->setPos(QPointF(0, 0));
	top_scene->addItem(item);
	
	QImage image("../picture/colorful.jpg");

	/*
	int xx = 0;
	int nitems = 0;

	for (int i = -11000; i < 11000; i += 110) {
		++xx;
		int yy = 0;
		for (int j = -7000; j < 7000; j += 70) {
			++yy;
			qreal x = (i + 11000) / 22000.0;
			qreal y = (j + 7000) / 14000.0;

			QColor color(image.pixel(int(image.width() * x), int(image.height() * y)));
			QGraphicsItem *item = new Chip(color, xx, yy);
			item->setPos(QPointF(i, j));
			scene->addItem(item);

			++nitems;
		}
	}
	*/
}