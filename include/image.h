#ifndef IMAGE_H
#define IMAGE_H

#include <QColor>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <functional>
#include <iostream>

#include "image_op.h"
class mainwindow;
class CVImage : public QGraphicsItem {
   public:
	CVImage(const QString &img_path);
	/**ctor */
	CVImage(const CVImage &src_img);
	CVImage(const QImage &src_img);
	CVImage(QImage src_img, img_op op);
	CVImage(QImage src_img, img_op_with_param<int> op, int threshold)
	{
		assert(!src_img.isNull());
		qimage = op(src_img, threshold);
		assert(!qimage.isNull());
		m_pixmap = QPixmap::fromImage(qimage);
		m_height = src_img.height();
		m_width = src_img.width();
		setup();
	}
	CVImage(QImage src_img, img_op_with_param<std::vector<int>> op, std::vector<int> sobel_kernel)
	{
		assert(!src_img.isNull());
		qimage = op(src_img, sobel_kernel);
		assert(!qimage.isNull());
		m_pixmap = QPixmap::fromImage(qimage);
		m_height = src_img.height();
		m_width = src_img.width();
		setup();
	}
	CVImage &operator=(const CVImage &other);
	~CVImage();
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

	template <typename Args>
	void process_image(img_op_with_param<Args> img_op, Args args)
	{
		qimage = img_op(qimage, args);
		m_pixmap = QPixmap::fromImage(qimage);
		setup();
	}
	QPixmap get_pixmap() const
	{
		return m_pixmap;
	}
	QImage get_qimage() const
	{
		return qimage.copy();
	}
	int height() const
	{
		return m_height;
	}
	int width() const
	{
		return m_width;
	}
	void setup()
	{
		setZValue((m_width + m_height) % 2);
		setFlags(ItemIsSelectable | ItemIsMovable);
		setAcceptHoverEvents(true);
	}
	friend mainwindow;

   protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

   private:
	int m_height;
	int m_width;
	QColor color;
	QVector<QPointF> stuff;
	QImage qimage;
	QPixmap m_pixmap;
};

#endif