#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QList>
#include <QMessageBox>
#include <QPixmap>
#include <QPluginLoader>
#include <QSplitter>
#include <typeinfo>

#include "chip.h"
#include "image_op.h"
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

	view = new View("Histogram view");
	view->view()->setScene(top_histogram_scene);
	h1Splitter->addWidget(view);
	activeSceneList.push_back(top_histogram_scene);

	view = new View("After processing");
	view->view()->setScene(bottom_scene);
	h2Splitter->addWidget(view);
	activeSceneList.push_back(bottom_scene);

	view = new View("Histogram view(Process)");
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

	edit_extract_R_Action = new QAction("Extract&R", this);
	editMenu->addAction(edit_extract_R_Action);

	edit_extract_G_Action = new QAction("Extract&G", this);
	editMenu->addAction(edit_extract_G_Action);

	edit_extract_B_Action = new QAction("Extract&B", this);
	editMenu->addAction(edit_extract_B_Action);

	edit_transform_to_Gray_Action = new QAction("&TransformToGray", this);
	editMenu->addAction(edit_transform_to_Gray_Action);

	edit_threshold_Action = new QAction("&Threshold", this);
	editMenu->addAction(edit_threshold_Action);

	edit_median_filter_Action = new QAction("MedianFilter", this);
	editMenu->addAction(edit_median_filter_Action);

	edit_mean_filter_Action = new QAction("MeanFilter", this);
	editMenu->addAction(edit_mean_filter_Action);

	edit_sobel_filter_Action = new QAction("SobelFilter", this);
	editMenu->addAction(edit_sobel_filter_Action);

	undoAction = new QAction("&Undo", this);
	editMenu->addAction(undoAction);
	// connect the signals and slots
	connect(exitAction, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
	connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));
	connect(edit_Histogram_Action, SIGNAL(triggered(bool)), this, SLOT(histogram_equalization()));
	connect(edit_extract_R_Action, SIGNAL(triggered(bool)), this, SLOT(extract_R_channel()));
	connect(edit_extract_G_Action, SIGNAL(triggered(bool)), this, SLOT(extract_G_channel()));
	connect(edit_extract_B_Action, SIGNAL(triggered(bool)), this, SLOT(extract_B_channel()));
	connect(edit_transform_to_Gray_Action, SIGNAL(triggered(bool)), this, SLOT(transform_to_gray()));
	connect(edit_threshold_Action, SIGNAL(triggered(bool)), this, SLOT(image_threshold()));
	connect(edit_median_filter_Action, SIGNAL(triggered(bool)), this, SLOT(median_filter()));
	connect(edit_mean_filter_Action, SIGNAL(triggered(bool)), this, SLOT(mean_filter()));
	connect(edit_sobel_filter_Action, SIGNAL(triggered(bool)), this, SLOT(soble_filter()));
	connect(undoAction, SIGNAL(triggered(bool)), this, SLOT(undo_image_transform()));
}
void MainWindow::soble_filter()
{
	if (CurImage == nullptr) {
		QMessageBox::information(this, "Information", "No image to edit.");
		return;
	}
	QStringList items;
	items << tr("Vertical") << tr("Horizontal") << tr("Combined");
	std::vector<int8_t> xsobel_kernel{-1, -2, -1, 0, 0, 0, 1, 2, 1};
	std::vector<int8_t> ysobel_kernel{1, 0, -1, 2, 0, 2, 1, 0, -1};
	bool ok;
	QString item = QInputDialog::getItem(this, tr("QInputDialog::getItem()"),
	                                     tr("Sobel-Type:"), items, 0, false, &ok);
	std::string choice = item.toStdString();
	if (choice == "Horizontal") {
		ProcImage = new CVImage(CurImage->get_qimage(), apply_sobel_filter<std::vector<int8_t>>, xsobel_kernel);
	}
	else if (choice == "Vertical") {
		ProcImage = new CVImage(CurImage->get_qimage(), apply_sobel_filter<std::vector<int8_t>>, ysobel_kernel);
	}
	else {
		ProcImage = new CVImage(CurImage->get_qimage());
		ProcImage->process_image<std::vector<int8_t>>(apply_sobel_filter<std::vector<int8_t>>, ysobel_kernel);
	}
	updateScene();

	ProcImage->setPos(QPointF(0, 0));
	bottom_scene->addItem(ProcImage);
}
void MainWindow::mean_filter()
{
	if (CurImage == nullptr) {
		QMessageBox::information(this, "Information", "No image to edit.");
		return;
	}
	updateScene();
	CVImage *original_img_hist = new CVImage(CurImage->get_qimage(), apply_create_histogram);
	original_img_hist->setPos(QPointF(150, 150));
	top_histogram_scene->addItem(original_img_hist);

	ProcImage = new CVImage(CurImage->get_qimage(), apply_mean_filter);
	ProcImage->setPos(QPointF(0, 0));
	bottom_scene->addItem(ProcImage);

	/**save prev image for undone action*/
	BackUpImg.emplace_back(ProcImage->get_qimage());

	CVImage *hist_of_processed_img = new CVImage(ProcImage->get_qimage(), apply_create_histogram);
	hist_of_processed_img->setPos(QPointF(150, 150));
	bottom_histogram_scene->addItem(hist_of_processed_img);
}
void MainWindow::median_filter()
{
	if (CurImage == nullptr) {
		QMessageBox::information(this, "Information", "No image to edit.");
		return;
	}
	updateScene();
	CVImage *original_img_hist = new CVImage(CurImage->get_qimage(), apply_create_histogram);
	original_img_hist->setPos(QPointF(150, 150));
	top_histogram_scene->addItem(original_img_hist);

	ProcImage = new CVImage(CurImage->get_qimage(), apply_median_filter);
	ProcImage->setPos(QPointF(0, 0));
	bottom_scene->addItem(ProcImage);

	/**save prev image for undone action*/
	BackUpImg.emplace_back(ProcImage->get_qimage());

	CVImage *hist_of_processed_img = new CVImage(ProcImage->get_qimage(), apply_create_histogram);
	hist_of_processed_img->setPos(QPointF(150, 150));
	bottom_histogram_scene->addItem(hist_of_processed_img);
}
void MainWindow::image_threshold()
{
	if (CurImage == nullptr) {
		QMessageBox::information(this, "Information", "No image to edit.");
		return;
	}
	bool ok;
	int threshold = QInputDialog::getInt(this, tr("QInputDialog::getInt()"),
	                                     tr("Threshold:"), 0, 1, 255, 1, &ok);
	updateScene();
	CVImage *original_img_hist = new CVImage(CurImage->get_qimage(), apply_create_histogram);
	original_img_hist->setPos(QPointF(150, 150));
	top_histogram_scene->addItem(original_img_hist);

	ProcImage = new CVImage(CurImage->get_qimage(), apply_threshold<int>, threshold);
	ProcImage->setPos(QPointF(0, 0));
	bottom_scene->addItem(ProcImage);

	/**save prev image for undone action*/
	BackUpImg.emplace_back(ProcImage->get_qimage());

	CVImage *hist_of_processed_img = new CVImage(ProcImage->get_qimage(), apply_create_histogram);
	hist_of_processed_img->setPos(QPointF(150, 150));
	bottom_histogram_scene->addItem(hist_of_processed_img);
}
void MainWindow::undo_image_transform()
{
	if (BackUpImg.size() < 2) {
		QMessageBox::information(this, "Information", "Not edit >=2 image, can't be undone");
		return;
	}
	updateScene();

	CVImage *original_img_hist = new CVImage(CurImage->get_qimage(), apply_create_histogram);
	original_img_hist->setPos(QPointF(150, 150));
	top_histogram_scene->addItem(original_img_hist);
	/**pop last image */
	BackUpImg.pop_back();

	CVImage *PrevImg = new CVImage(BackUpImg.back());
	PrevImg->setPos(QPointF(150, 150));
	bottom_scene->addItem(PrevImg);

	CVImage *hist_of_processed_img = new CVImage(PrevImg->get_qimage(), apply_create_histogram);
	hist_of_processed_img->setPos(QPointF(150, 150));
	bottom_histogram_scene->addItem(hist_of_processed_img);
}
void MainWindow::extract_R_channel()
{
	if (CurImage == nullptr) {
		QMessageBox::information(this, "Information", "No image to edit.");
		return;
	}
	updateScene();
	CVImage *original_img_hist = new CVImage(CurImage->get_qimage(), apply_create_histogram);
	original_img_hist->setPos(QPointF(150, 150));
	top_histogram_scene->addItem(original_img_hist);

	ProcImage = new CVImage(CurImage->get_qimage(), apply_extract_r_channel);
	ProcImage->setPos(QPointF(0, 0));
	bottom_scene->addItem(ProcImage);

	/**save prev image for undone action*/
	BackUpImg.emplace_back(ProcImage->get_qimage());

	CVImage *hist_of_processed_img = new CVImage(ProcImage->get_qimage(), apply_create_histogram);
	hist_of_processed_img->setPos(QPointF(150, 150));
	bottom_histogram_scene->addItem(hist_of_processed_img);
}
void MainWindow::extract_G_channel()
{
	if (CurImage == nullptr) {
		QMessageBox::information(this, "Information", "No image to edit.");
		return;
	}
	updateScene();
	CVImage *original_img_hist = new CVImage(CurImage->get_qimage(), apply_create_histogram);
	original_img_hist->setPos(QPointF(150, 150));
	top_histogram_scene->addItem(original_img_hist);

	ProcImage = new CVImage(CurImage->get_qimage(), apply_extract_g_channel);
	ProcImage->setPos(QPointF(0, 0));
	bottom_scene->addItem(ProcImage);

	/**save prev image for undone action*/
	BackUpImg.emplace_back(ProcImage->get_qimage());

	CVImage *hist_of_processed_img = new CVImage(ProcImage->get_qimage(), apply_create_histogram);
	hist_of_processed_img->setPos(QPointF(150, 150));
	bottom_histogram_scene->addItem(hist_of_processed_img);
}
void MainWindow::extract_B_channel()
{
	if (CurImage == nullptr) {
		QMessageBox::information(this, "Information", "No image to edit.");
		return;
	}
	updateScene();
	CVImage *original_img_hist = new CVImage(CurImage->get_qimage(), apply_create_histogram);
	original_img_hist->setPos(QPointF(150, 150));
	top_histogram_scene->addItem(original_img_hist);

	ProcImage = new CVImage(CurImage->get_qimage(), apply_extract_b_channel);
	ProcImage->setPos(QPointF(0, 0));
	bottom_scene->addItem(ProcImage);

	/**save prev image for undone action*/
	BackUpImg.emplace_back(ProcImage->get_qimage());

	CVImage *hist_of_processed_img = new CVImage(ProcImage->get_qimage(), apply_create_histogram);
	hist_of_processed_img->setPos(QPointF(150, 150));
	bottom_histogram_scene->addItem(hist_of_processed_img);
}
void MainWindow::transform_to_gray()
{
	if (CurImage == nullptr) {
		QMessageBox::information(this, "Information", "No image to edit.");
		return;
	}
	updateScene();
	CVImage *original_img_hist = new CVImage(CurImage->get_qimage(), apply_create_histogram);
	original_img_hist->setPos(QPointF(150, 150));
	top_histogram_scene->addItem(original_img_hist);

	ProcImage = new CVImage(CurImage->get_qimage(), apply_transform_to_gray_scale);
	ProcImage->setPos(QPointF(0, 0));
	bottom_scene->addItem(ProcImage);

	/**save prev image for undone action*/
	BackUpImg.emplace_back(ProcImage->get_qimage());

	CVImage *hist_of_processed_img = new CVImage(ProcImage->get_qimage(), apply_create_histogram);
	hist_of_processed_img->setPos(QPointF(150, 150));
	bottom_histogram_scene->addItem(hist_of_processed_img);
}
void MainWindow::histogram_equalization()
{
	if (CurImage == nullptr) {
		QMessageBox::information(this, "Information", "No image to edit.");
		return;
	}
	updateScene();
	CVImage *original_img_hist = new CVImage(CurImage->get_qimage(), apply_create_histogram);
	original_img_hist->setPos(QPointF(150, 150));
	top_histogram_scene->addItem(original_img_hist);

	ProcImage = new CVImage(CurImage->get_qimage(), apply_histogram_equalization);
	ProcImage->setPos(QPointF(0, 0));
	bottom_scene->addItem(ProcImage);

	/**save prev image for undone action*/
	BackUpImg.emplace_back(ProcImage->get_qimage());

	CVImage *hist_of_processed_img = new CVImage(ProcImage->get_qimage(), apply_create_histogram);
	hist_of_processed_img->setPos(QPointF(150, 150));
	bottom_histogram_scene->addItem(hist_of_processed_img);
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
void MainWindow::updateHistogramScene()
{
	top_histogram_scene->clear();
	bottom_histogram_scene->clear();
}
void MainWindow::updateScene()
{
	for (auto &scene : activeSceneList) {
		scene->clear();
	}
}
void MainWindow::populateScene()
{
	/** In the QSplitter, add a custom QWidget which uses a QImage to draw in it's repaint methods. */
	clearScene();
	CurImage = new CVImage(currentImagePath);
	CurImage->setPos(QPointF(0, 0));
	top_scene->addItem(CurImage);
}