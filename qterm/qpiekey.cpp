#include <qpiekey.h>
#include <QSizePolicy>
#include <QPainter>
#include <QPaintEvent>
#include <math.h>
#include <stdio.h>
#include "term_logging.h"

using namespace std;

QPieKey::QPieKey(QWidget *parent) : QWidget(parent)
{
    sections = 0;
    size = 0;
    keys = 0;
    charlist = NULL;
    selections = NULL;

    hide();
}

QPieKey::~QPieKey()
{
}

void QPieKey::paintEvent(QPaintEvent *event)
{
    int inSelection = 0, selectedChar;
    int i, j,k;
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
            for( j = 0; j < letters_per_section; j ++ ) {
                // The angle of the section extends from angle*(i-0.5) to
                // angle*(i+0.5). We want to center 'sections' characters
                // with equal angular spacing in this area. Add a full width
                // on each side to separate a section from its neighbour
                character = QChar(charlist[i][j]);
                charangle = angle*((j+1.0)/(sections+1.0)+i-0.5);
                selectedChar = (highlighted_section == -1 || highlighted_section == i);
                if( selectedChar ) {
                    for( k = 0; k < keys; k ++ ) {
                        if( k == i ) continue;
                        if( strchr( selections[k].c_str(), character.toLatin1() ) == NULL ) {
                            selectedChar = false;
                            break;
                        }
                    }
                }
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
    moveTouch(event->x(), event->y());
}

void QPieKey::moveTouch(int x, int y)
{
    int section;
    double distance;
    double mouseangle;
    // Find whether we are in the wheel
    distance = sqrt((x-size)*(x-size)+(y-size)*(y-size));

    if( distance * 2 < size ) {
        section = -1;
    } else {
        mouseangle = atan2(((double)size - x), ((double)y - size)) + M_PI;
        section = ((int)((mouseangle + 0.5) / angle)) % sections;
    }

    if( section != highlighted_section ) {
        if( section == -1 ) {
            // We're losing our selection, send the signal now
            for(int i = 0; i < letters_per_section; i ++ ) {
                int j;
                size_t s;
                for( j = 0; j < keys; j ++ ) {
                    if( (s = selections[j].find( charlist[highlighted_section][i] )) == string::npos ) {
                        break;
                    }
                }
                if( j == keys ) {
                    emit(keypress(charlist[highlighted_section][i]));
                }
            }
            emit selectionChanged(NULL);
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
    releaseMouse();
    emit released();
}
 
void QPieKey::initialize(int keys, int sections, const char *charlist)
{
    int i;
    this->keys = keys;
    if( this->charlist != NULL ) {
        for(i = 0; i < this->sections; i ++ ) {
            delete this->charlist[i];
        }
        delete [] this->charlist;
    }

    if( this->selections != NULL ) {
        delete [] this->selections;
    }
    this->selections = new string[keys];
    this->letters_per_section = strlen(charlist) / sections;
    this->sections = sections;
    this->charlist = new char *[letters_per_section];
    for( i = 0; i < sections; i ++ ) {
        this->charlist[i] = new char[letters_per_section];
        memcpy(this->charlist[i], charlist + i*sections, letters_per_section);
    }

    angle = 2*M_PI / sections;
    highlighted_section = -1;
    size_ring();
}

void QPieKey::activate(int x, int y)
{
    // Center the widget on the mouse coordinates
    highlighted_section = -1;
    setGeometry(x - width() / 2, y - height() / 2, width(), height());
    show();
    grabMouse();
}

void QPieKey::select(int key, const char *selection)
{
    this->selections[key] = selection ? selection : "";
    update(0,0, size*2, size*2);
}

void QPieKey::size_ring()
{
    // TODO Figure out how big the largest section has to be so letters don't overlap
    size = 200;

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
