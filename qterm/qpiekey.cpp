#include <qpiekey.h>
#include <QSizePolicy>
#include <QPainter>
#include <QPaintEvent>

QPieKey::QPieKey(QWidget *parent) : QWidget(parent)
{
    bitmap = new QBitmap(40,40);
    QPainter painter(bitmap);

    bitmap->clear();
    painter.drawEllipse(0, 0, 40, 40);
    setFixedSize(40, 40);
    setMask(*bitmap);
}

QPieKey::~QPieKey()
{
}

void QPieKey::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setBrush(QColor(128, 128, 128));
    painter.drawRect(event->rect());
}
