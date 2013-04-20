#include <qpiekey.h>
#include <QSizePolicy>
#include <QPainter>
#include <QPaintEvent>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include "term_logging.h"

using namespace std;

QPieKey::QPieKey(QWidget *parent) : QWidget(parent)
{
    sections = 0;
    size = 0;

    hide();
}

QPieKey::~QPieKey()
{
}

QChar QPieKey::toChar(Qt::Key key)
{
    if( key >= Qt::Key_A && key <= Qt::Key_Z ) {
        return key - Qt::Key_A + 'a';
    }
}

void QPieKey::paintEvent(QPaintEvent *event)
{
    int inSelection = 0, selectedChar;
    int i, j,k;
    double charangle;
    QPainter painter(this);
    Qt::Key character;

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
                character = keylist[i][j];
                charangle = angle*(j/letters_per_section + i);
                selectedChar = (highlighted_section == -1 || highlighted_section == i);
                if( selectedChar ) {
                    for(map<int, const vector<Qt::Key> *>::const_iterator k = selections.begin(); k != selections.end(); k ++ ) {
                        if( k->second == NULL ) continue;
                        if( std::find(k->second->begin(), k->second->end(), selectedChar) == k->second->end() ) {
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

                switch( character ) {
                    case Qt::Key_Tab:
                        {
                            QPixmap img("app/native/tab.png");
                            painter.drawPixmap(size+size*sin(charangle)*3/4-img.width()/2, size-size*cos(charangle)*3/4-img.height()/2, img);
                        }
                        break;
                    case Qt::Key_Escape:
                        {
                            QPixmap img("app/native/escape.png");
                            painter.drawPixmap(size+size*sin(charangle)*3/4-img.width()/2, size-size*cos(charangle)*3/4-img.height()/2, img);
                        }
                        break;
                    case Qt::Key_Up:
                        {
                            QPixmap img("app/native/up.png");
                            painter.drawPixmap(size+size*sin(charangle)*3/4-img.width()/2, size-size*cos(charangle)*3/4-img.height()/2, img);
                        }
                        break;
                    case Qt::Key_Down:
                        {
                            QPixmap img("app/native/down.png");
                            painter.drawPixmap(size+size*sin(charangle)*3/4-img.width()/2, size-size*cos(charangle)*3/4-img.height()/2, img);
                        }
                        break;
                    case Qt::Key_Right:
                        {
                            QPixmap img("app/native/right.png");
                            painter.drawPixmap(size+size*sin(charangle)*3/4-img.width()/2, size-size*cos(charangle)*3/4-img.height()/2, img);
                        }
                        break;
                    case Qt::Key_Left:
                        {
                            QPixmap img("app/native/left.png");
                            painter.drawPixmap(size+size*sin(charangle)*3/4-img.width()/2, size-size*cos(charangle)*3/4-img.height()/2, img);
                        }
                        break;
                    default:
                        painter.drawText(size+size*sin(charangle)*3/4-painter.fontMetrics().width(character)/2, size-size*cos(charangle)*3/4+painter.fontMetrics().height()/2, toChar(character));
                        break;
                }
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
                std::map<int, const std::vector<Qt::Key> *>::const_iterator j;
                for(j = selections.begin(); j != selections.end(); j ++ ) {
                    if( std::find(j->second->begin(), j->second->end(), keylist[highlighted_section][i]) == j->second->end() ) {
                        break;
                    }
                }
                if( j == selections.end() ) {
                    emit(keypress(keylist[highlighted_section][i]));
                }
            }
            emit selectionChanged(NULL);
        } else {
            // We're gaining a selection, notify our parent
            emit selectionChanged(&keylist[ section ]);
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
 
void QPieKey::initialize(int sections, const vector<Qt::Key> &keylist)
{
    int i, j;

    this->selections.clear();
    letters_per_section = keylist.size() / sections;
    this->sections = sections;
    this->keylist.clear();
    for( i = 0; i < sections; i ++ ) {
        this->keylist.push_back(vector<Qt::Key>());
        for( j = 0; j < letters_per_section; j ++ ) {
            this->keylist.back().push_back(keylist[i*letters_per_section+j]);
        }
    }

    angle = 2*M_PI / sections;
    highlighted_section = -1;
    size_ring();
}

void QPieKey::activate(int x, int y)
{
    // Center the widget on the mouse coordinates
    highlighted_section = -1;
    selections.clear();
    setGeometry(x - width() / 2, y - height() / 2, width(), height());
    show();
    grabMouse();
}

void QPieKey::select(int key, const vector<Qt::Key> *selection)
{
    this->selections[key] = selection;
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
