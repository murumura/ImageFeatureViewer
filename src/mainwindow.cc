#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QKeyEvent>
#include <QList>
#include <QMessageBox>
#include <QPixmap>
#include <QPluginLoader>
#include <QSplitter>
#include <typeinfo>

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

	layout = new QHBoxLayout();
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

	edit_transform_Action = new QAction("Transform", this);
	editMenu->addAction(edit_transform_Action);

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
	connect(edit_transform_Action, SIGNAL(triggered(bool)), this, SLOT(transform()));
	connect(undoAction, SIGNAL(triggered(bool)), this, SLOT(undo_image_transform()));
}
void MainWindow::transform()
{
	if (CurImage == nullptr) {
		QMessageBox::information(this, "Information", "No image to edit.");
		return;
	}
	updateScene();
	openImage(&ProcImage);
	TranformModeButton = new QToolButton();
	TranformModeButton->setText(tr("Transform"));
	TranformModeButton->setCheckable(true);
	TranformModeButton->setChecked(true);
	layout->addWidget(TranformModeButton);
	connect(TranformModeButton, &QAbstractButton::clicked, this, &MainWindow::register_back_to_original_image);
}
void MainWindow::register_back_to_original_image()
{
	updateScene();
	CVImage *original_img_hist = new CVImage(CurImage->get_qimage(), apply_create_histogram);
	original_img_hist->setPos(QPointF(150, 150));
	top_histogram_scene->addItem(original_img_hist);

	ProcImage->image_operation<void>(apply_transform_image_postion, CurImage->get_qimage(), ProcImage->get_qimage());
	ProcImage->setPos(QPointF(0, 0));
	bottom_scene->addItem(ProcImage);

	/**save prev image for undone action*/
	BackUpImg.emplace_back(ProcImage->get_qimage());

	CVImage *hist_of_processed_img = new CVImage(ProcImage->get_qimage(), apply_create_histogram);
	hist_of_processed_img->setPos(QPointF(150, 150));
	bottom_histogram_scene->addItem(hist_of_processed_img);
	delete TranformModeButton;
}
void MainWindow::soble_filter()
{
	if (CurImage == nullptr) {
		QMessageBox::information(this, "Information", "No image to edit.");
		return;
	}
	bool overlapped = false;
	int threshold;
	int result = QMessageBox::information(this,
	                                      "Image Overlapped",
	                                      "Do you wish to do image overlapped after apply sobel?",
	                                      QMessageBox::Yes | QMessageBox::Default,
	                                      QMessageBox::No | QMessageBox::Escape);
	switch (result) {
	case QMessageBox::Yes:
		overlapped = true;
		bool ok;
		threshold = QInputDialog::getInt(this, tr("QInputDialog::getInt()"),
		                                 tr("Threshold:"), 0, 1, 255, 1, &ok);
		break;
	case QMessageBox::No:
		break;
	}

	QStringList items;
	items << tr("Vertical") << tr("Horizontal") << tr("Combined");
	std::vector<int> xsobel_kernel{1, 0, -1, 2, 0, -2, 1, 0, -1};
	std::vector<int> ysobel_kernel{1, 2, 1, 0, 0, 0, -1, -2, -1};
	std::vector<int> combined(xsobel_kernel);
	combined.insert(combined.end(), ysobel_kernel.begin(), ysobel_kernel.end());
	bool ok;
	QString item = QInputDialog::getItem(this, tr("QInputDialog::getItem()"),
	                                     tr("Sobel-Type:"), items, 0, false, &ok);
	std::string choice = item.toStdString();
	if (choice == "Horizontal") {
		ProcImage = new CVImage(CurImage->get_qimage(), apply_sobel_filter<std::vector<int>>, xsobel_kernel);
	}
	else if (choice == "Vertical") {
		ProcImage = new CVImage(CurImage->get_qimage(), apply_sobel_filter<std::vector<int>>, ysobel_kernel);
	}
	else {
		ProcImage = new CVImage(CurImage->get_qimage());
		ProcImage->process_image<std::vector<int>>(apply_sobel_combine_filter<std::vector<int>>, combined);
	}
	if (overlapped) {
		ProcImage->process_image<int>(apply_threshold<int>, threshold);
		ProcImage->process_image<int>(apply_reserve_color<int>, 1);
		ProcImage->image_operation<void>(apply_merge_image, CurImage->get_qimage(), ProcImage->get_qimage(), 1);
	}
	updateScene();
	ProcImage->setPos(QPointF(0, 0));
	bottom_scene->addItem(ProcImage);

	CVImage *original_img_hist = new CVImage(CurImage->get_qimage(), apply_create_histogram);
	original_img_hist->setPos(QPointF(150, 150));
	top_histogram_scene->addItem(original_img_hist);

	CVImage *hist_of_processed_img = new CVImage(ProcImage->get_qimage(), apply_create_histogram);
	hist_of_processed_img->setPos(QPointF(150, 150));
	bottom_histogram_scene->addItem(hist_of_processed_img);
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

	ProcImage = new CVImage(CurImage->get_qimage(), apply_extract_channel<int>, (int)2);
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

	ProcImage = new CVImage(CurImage->get_qimage(), apply_extract_channel<int>, (int)1);
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

	ProcImage = new CVImage(CurImage->get_qimage(), apply_extract_channel<int>, (int)0);
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
void MainWindow::openImage(CVImage **Proc)
{
	QFileDialog dialog(this);
	dialog.setWindowTitle("Open Image");
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
	QStringList filePaths;
	if (dialog.exec()) {
		filePaths = dialog.selectedFiles();
		(*Proc) = new CVImage(filePaths.at(0));
		(*Proc)->setPos(QPointF(0, 0));
		bottom_scene->addItem((*Proc));
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