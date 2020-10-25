#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QList>
#include <QMessageBox>
#include <QPixmap>
#include <QPluginLoader>
#include <QSplitter>

#include "chip.h"
#include "view.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      top_scene(new QGraphicsScene(0, 0, 1920, 1080, this)),
      top_histogram_scene(new QGraphicsScene(0, 0, 1920, 1080, this)),
      bottom_scene(new QGraphicsScene(0, 0, 1920, 1080, this)),
      bottom_histogram_scene(new QGraphicsScene(0, 0, 1920, 1080, this)),
      h1Splitter(new QSplitter(this)),
      h2Splitter(new QSplitter(this)),
      fileMenu(nullptr),
      exitMenu(nullptr),
      editMenu(nullptr),
      CurImage(nullptr)
{
	QSplitter *vSplitter = new QSplitter();
	vSplitter->setOrientation(Qt::Vertical);

	vSplitter->addWidget(h1Splitter);
	vSplitter->addWidget(h2Splitter);

	View *view = new View("Original");
	view->view()->setScene(top_scene);
	h1Splitter->addWidget(view);
	activeSceneList.push_back(top_scene);

	view = new View("Histogram view");
	view->view()->setScene(top_histogram_scene);
	h1Splitter->addWidget(view);
	activeSceneList.push_back(top_histogram_scene);

	view = new View("After processing");
	view->view()->setScene(bottom_scene);
	h2Splitter->addWidget(view);
	activeSceneList.push_back(bottom_scene);

	view = new View("Histogram view");
	view->view()->setScene(bottom_histogram_scene);
	h2Splitter->addWidget(view);
	activeSceneList.push_back(bottom_histogram_scene);

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
	editMenu = menuBar->addMenu("&Edit");
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
	edit_Histogram_Action = new QAction("&Histogram", this);
	editMenu->addAction(edit_Histogram_Action);
	// connect the signals and slots
	connect(exitAction, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
	connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));
	connect(edit_Histogram_Action, SIGNAL(triggered(bool)), this, SLOT(histogram_equalization()));
}
void MainWindow::histogram_equalization()
{
	if (CurImage == nullptr) {
		QMessageBox::information(this, "Information", "No image to edit.");
		return;
	}
	CVImage *histogramImage = new CVImage(*CurImage);
	histogramImage->setPos(QPointF(0, 0));
	top_histogram_scene->addItem(histogramImage);
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
void MainWindow::clearScene()
{
	if (CurImage != nullptr) {
		delete CurImage;
		CurImage = nullptr;
	}
	for (auto &scene : activeSceneList) {
		scene->clear();
	}
}
void MainWindow::updateScene()
{
}
void MainWindow::populateScene()
{
	/** In the QSplitter, add a custom QWidget which uses a QImage to draw in it's repaint methods. */
	clearScene();
	CurImage = new CVImage(currentImagePath);
	CurImage->setPos(QPointF(0, 0));
	top_scene->addItem(CurImage);
	updateScene();
}