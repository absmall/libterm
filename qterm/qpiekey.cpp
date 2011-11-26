#include <qpiekey.h>
#include <QSizePolicy>
#include <QPainter>
#include <QPaintEvent>

#define SIZE 60

QPieKey::QPieKey(QWidget *parent) : QWidget(parent)
{
    bitmap = new QBitmap(SIZE*2,SIZE*2);
    QPainter painter(bitmap);

    setFixedSize(SIZE*2, SIZE*2);

    bitmap->clear();
    painter.setBrush(Qt::color1);
    painter.drawEllipse(1, 1, SIZE*2-2, SIZE*2-2);
    painter.setBrush(Qt::color0);
    painter.drawEllipse(SIZE/2, SIZE/2, SIZE, SIZE);
    setMask(*bitmap);
    hide();
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
