#include <qpiekey.h>
#include <QSizePolicy>
#include <QPainter>
#include <QPaintEvent>
#include <math.h>
#include <stdio.h>

QPieKey::QPieKey(QWidget *parent) : QWidget(parent)
{
    sections = 0;
    size = 0;

    hide();
}

QPieKey::~QPieKey()
{
}

void QPieKey::paintEvent(QPaintEvent *event)
{
    int i, j;
    double charangle;
    QPainter painter(this);
    QChar character;

    painter.setBrush(QColor(128, 128, 128));
    painter.drawRect(event->rect());
    painter.setBrush(QColor(0, 0, 0));

    if( sections ) {
        for( i = 0; i < sections; i ++ ) {
            for( j = 0; j < sections; j ++ ) {
                // The angle of the section extends from angle*(i-0.5) to
                // angle*(i+0.5). We want to center 'sections' characters
                // with equal angular spacing in this area. Add a full width
                // on each side to separate a section from its neighbour
                character = QChar(charlist[i*sections+j]);
                charangle = angle*((j+1.0)/(sections+1.0)+i-0.5);

                painter.drawText(size+size*sin(charangle)*3/4-painter.fontMetrics().width(character)/2, size-size*cos(charangle)*3/4+painter.fontMetrics().height()/2, character);
            }
        }
        for( i = 0; i < sections; i ++ ) {
            painter.drawLine(size, size, size+size*sin(angle*(i+0.5)), size-size*cos(angle*(i+0.5)));
        }
    }
}

void QPieKey::mouseReleaseEvent(QMouseEvent *event)
{
    hide();
    releaseMouse();
}
 

void QPieKey::initialize(int sections, const char *charlist)
{
    this->sections = sections;
    this->charlist = new char[sections*sections];
    memcpy(this->charlist, charlist, sections*sections);

    angle = 2*M_PI / sections;
    highlighted_section = -1;
    size_ring();
}

void QPieKey::activate(int x, int y)
{
    // Center the widget on the mouse coordinates
    setGeometry(x - width() / 2, y - height() / 2, width(), height());
    show();
    grabMouse();
}

void QPieKey::size_ring()
{
    // TODO Figure out how big the largest section has to be so letters don't overlap
    size = 100;

    // Update the mask of the widget appropriately
    bitmap = new QBitmap(size*2,size*2);
    QPainter painter(bitmap);

    setFixedSize(size*2, size*2);

    bitmap->clear();
    painter.setBrush(Qt::color1);
    painter.drawEllipse(1, 1, size*2-2, size*2-2);
    painter.setBrush(Qt::color0);
    painter.drawEllipse(size/2, size/2, size, size);
    setMask(*bitmap);
}
