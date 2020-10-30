#include "image.h"

#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QStyleOptionGraphicsItem>
#include <cassert>
CVImage::CVImage(const QString &img_path)
{
	qimage = QImage(img_path);
	m_pixmap = QPixmap(img_path);
	this->m_width = qimage.width();
	this->m_height = qimage.height();
	setZValue((m_width + m_height) % 2);
	setFlags(ItemIsSelectable | ItemIsMovable);
	setAcceptHoverEvents(true);
}
CVImage::CVImage(const CVImage &src_img)
{
	qimage = src_img.get_qimage().copy();
	m_pixmap = src_img.get_pixmap();
	m_height = src_img.height();
	m_width = src_img.width();
	setZValue((m_width + m_height) % 2);
	setFlags(ItemIsSelectable | ItemIsMovable);
	setAcceptHoverEvents(true);
}
CVImage::CVImage(QImage src_qimg, img_op op)
{
	assert(!src_qimg.isNull());
	qimage = op(src_qimg);
	assert(!qimage.isNull());
	m_pixmap = QPixmap::fromImage(qimage);
	m_height = src_qimg.height();
	m_width = src_qimg.width();
	setZValue((m_width + m_height) % 2);
	setFlags(ItemIsSelectable | ItemIsMovable);
	setAcceptHoverEvents(true);
}
CVImage::~CVImage()
{
	qDebug() << "CVImage::~CVImage()";
}
CVImage &CVImage::operator=(const CVImage &other)
{
	// check for self-assignment
	if (this == &other)
		return *this;
	qimage = other.get_qimage();
	m_pixmap = other.get_pixmap();
	m_height = other.height();
	m_width = other.width();
	return *this;
}
QRectF CVImage::boundingRect() const
{
	return QRectF(m_pixmap.rect());
}
void CVImage::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
	update();
}
void CVImage::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->modifiers() & Qt::ShiftModifier) {
		stuff << event->pos();
		update();
		return;
	}
	QGraphicsItem::mouseMoveEvent(event);
}
void CVImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(widget);
	QPointF center(m_pixmap.width() / qreal(2), m_pixmap.height() / qreal(2));
	painter->translate(QPolygonF().at(0) - center);
	painter->translate(center);
	painter->drawPixmap(QPointF(0, 0), m_pixmap);
	painter->setPen(QPen(QColor(255, 0, 0, 155), 0.25, Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin));
	painter->setBrush(Qt::NoBrush);
	painter->drawRect(QRectF(0, 0, m_pixmap.width(), m_pixmap.height()).adjusted(-2, -2, 2, 2));
}
void CVImage::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseReleaseEvent(event);
	update();
}