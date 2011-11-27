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
    charlist = NULL;

    hide();
}

QPieKey::~QPieKey()
{
}

void QPieKey::paintEvent(QPaintEvent *event)
{
    int inSelection = 0, selectedChar;
    int i, j;
    double charangle;
    QPainter painter(this);
    QChar character;

    painter.setBrush(QColor(128, 128, 128));
    painter.drawRect(event->rect());

    if( sections ) {
        painter.setBrush(QColor(180, 180, 180));
        if( highlighted_section != -1 ) {
            painter.drawPie(0, 0, size*2, size*2, (90-(angle*(highlighted_section+0.5))*180/M_PI)*16, angle*180/M_PI*16);
        }
        painter.setBrush(QColor(0, 0, 0));
        for( i = 0; i < sections; i ++ ) {
            for( j = 0; j < sections; j ++ ) {
                // The angle of the section extends from angle*(i-0.5) to
                // angle*(i+0.5). We want to center 'sections' characters
                // with equal angular spacing in this area. Add a full width
                // on each side to separate a section from its neighbour
                character = QChar(charlist[i][j]);
                charangle = angle*((j+1.0)/(sections+1.0)+i-0.5);
                selectedChar = (highlighted_section == -1 || highlighted_section == i)
                    && (strchr( selection.c_str(), character.toLatin1() ) != NULL);
                if( selectedChar != inSelection ) {
                    QFont font;
                    inSelection = selectedChar;
                    font.setBold(inSelection);
                    if( inSelection ) {
                        if( font.pixelSize() != -1 ) {
                            font.setPixelSize(font.pixelSize() + 3);
                        }
                        if( font.pointSize() != -1 ) {
                            font.setPointSize(font.pointSize() + 3);
                        }
                    }
                    painter.setFont(font);
                }

                painter.drawText(size+size*sin(charangle)*3/4-painter.fontMetrics().width(character)/2, size-size*cos(charangle)*3/4+painter.fontMetrics().height()/2, character);
            }
        }
        for( i = 0; i < sections; i ++ ) {
            if( highlighted_section == -1 || (i != highlighted_section && i != (highlighted_section + 1) % sections)) {
                painter.drawLine(size, size, size+size*sin(angle*(i-0.5)), size-size*cos(angle*(i-0.5)));
            }
        }
    }
}

void QPieKey::mouseMoveEvent(QMouseEvent *event)
{
    int section;
    double distance;
    double mouseangle;
    // Find whether we are in the wheel
    distance = sqrt((event->x()-size)*(event->x()-size)+(event->y()-size)*(event->y()-size));

    if( distance * 2 < size ) {
        section = -1;
    } else {
        mouseangle = atan2(((double)size - event->x()), ((double)event->y() - size)) + M_PI;
        section = ((int)((mouseangle + 0.5) / angle)) % sections;
    }

    if( section != highlighted_section ) {
        if( section == -1 ) {
            // We're losing our selection, send the signal now
            for(int i = 0; i < sections; i ++ ) {
                size_t s;
                if( (s = selection.find( charlist[highlighted_section][i] )) != -1 ) {
                    emit(keypress(selection[s]));
                }
            }
        } else {
            // We're gaining a selection, notify our parent
            emit selectionChanged(charlist[ section ]);
        }
        highlighted_section = section;
        update(0,0, size*2, size*2);
    }
}

void QPieKey::mouseReleaseEvent(QMouseEvent *event)
{
    hide();
    releaseMouse();
}
 

void QPieKey::initialize(int sections, const char *charlist)
{
    int i;
    if( this->charlist != NULL ) {
        for(i = 0; i < this->sections; i ++ ) {
            delete this->charlist[i];
        }
        delete [] this->charlist;
    }

    this->sections = sections;
    this->charlist = new char *[sections];
    for( i = 0; i < sections; i ++ ) {
        this->charlist[i] = new char[sections];
        memcpy(this->charlist[i], charlist + i*sections, sections);
    }

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

void QPieKey::select(const char *selection)
{
    this->selection = selection;
    update(0,0, size*2, size*2);
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
