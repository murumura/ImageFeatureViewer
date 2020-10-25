#ifndef IMAGE_H
#define IMAGE_H

#include <QColor>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QImage>
#include "image_op.h"
class mainwindow;
class CVImage : public QGraphicsItem {
   public:
	CVImage(const QString &img_path);
	CVImage(const CVImage &src_img);
	CVImage &operator=(const CVImage &other);
	~CVImage();
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
	QPixmap get_pixmap() const
	{
		return m_pixmap;
	}
	QImage get_qimage() const
	{
		return qimage;
	}
	int height() const
	{
		return m_height;
	}
	int width() const
	{
		return m_width;
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