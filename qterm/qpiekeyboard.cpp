#include <math.h>
#include <stdio.h>
#include <QWidget>
#include <qpiekeyboard.h>
#include "term_logging.h"

using namespace std;

QPieKeyboard::QPieKeyboard(QWidget *parent) : QObject(NULL), parent(parent)
{
    sections = 0;
    selections = NULL;
}

QPieKeyboard::~QPieKeyboard()
{
}

void QPieKeyboard::initialize(int keycount, const std::vector<Qt::Key> &keylist)
{
    int i;
    vector<Qt::Key> baselist;
    vector<Qt::Key> reorderlist;
    if( keycount > 2 ) {
        throw "More than 2 piekeys are not supported yet";
    }

    // Load any pixmaps in the string to cache
    for( std::vector<Qt::Key>::const_iterator i = keylist.begin(); i != keylist.end(); i ++ ) {
        switch( *i ) {
            case Qt::Key_Tab:
                pixmapCache[*i] = QPixmap("app/native/tab.png");
                break;
            case Qt::Key_Escape:
                pixmapCache[*i] = QPixmap("app/native/escape.png");
                break;
            case Qt::Key_Up:
                pixmapCache[*i] = QPixmap("app/native/up.png");
                break;
            case Qt::Key_Down:
                pixmapCache[*i] = QPixmap("app/native/down.png");
                break;
            case Qt::Key_Right:
                pixmapCache[*i] = QPixmap("app/native/right.png");
                break;
            case Qt::Key_Left:
                pixmapCache[*i] = QPixmap("app/native/left.png");
                break;
        }
    }

    // sections is key'th root of number of characters to show, rounded up to the nearest integer
    sections = keylist.size();
    sections = ceil(pow(sections-0.001, 1.0/keycount));
    this->keycount = keycount;
    keys = new QPieKey[keycount];
    selections = new const vector<Qt::Key> *[keycount];
    for( i = 0; i < keycount; i ++ ) {
        QObject::connect(&keys[i], SIGNAL(selectionChanged(const std::vector<Qt::Key> *)), this, SLOT(selectionChanged(const std::vector<Qt::Key> *)));
        QObject::connect(&keys[i], SIGNAL(keypress(Qt::Key)), this, SLOT(piekeypressed(Qt::Key)));
        QObject::connect(&keys[i], SIGNAL(released()), this, SLOT(released()));
        keys[i].setParent(parent);
    }

    // This can be generalized to any number of piekeys, but haven't done it yet
    if( keycount == 1 ) {
        keys[0].initialize(this, sections, keylist);
    } else if( keycount == 2 ){
        baselist = vector<Qt::Key>(keylist.size());
        reorderlist = reorder(sections, baselist);

        keys[0].initialize(this, sections, baselist);
        keys[1].initialize(this, sections, reorderlist);
    }
}

void QPieKeyboard::activate(int touchId, int x, int y)
{
    int i, j;
    for( i = 0; i < keycount; i ++ ) {
        selections[i] = NULL;
    }
    keys[touchId].activate(x, y);
}

void QPieKeyboard::moveTouch(int touchId, int x, int y)
{
    keys[touchId].moveTouch(x-keys[touchId].x(), y-keys[touchId].y());
}

vector<Qt::Key> QPieKeyboard::reorder(int sections, vector<Qt::Key> keylist)
{
    int i, j;
    vector<Qt::Key> ret;

    ret = vector<Qt::Key>(keylist.size());

    for(i = 0; i < sections; i ++ ) {
        for( j = 0; j < sections; j ++ ) {
            ret[i+sections*j] = keylist[ j+sections*i];
        }
    }

    return ret;
}

void QPieKeyboard::piekeypressed(Qt::Key key)
{
    // Just pass this along to the parent
    emit keypress(key);
}

void QPieKeyboard::selectionChanged(const vector<Qt::Key> *selection)
{
    int i, index;
    QObject *s = sender();
    for(index = 0; index < keycount; index ++ ) {
        if( &keys[index] == s ) break;
    }
    selections[index] = selection;

    for( i = 0; i < keycount; i ++ ) {
        if( i != index ) {
            keys[i].select( index, selection );
        }
    }
}

void QPieKeyboard::release()
{
    int i;
    for( i = 0; i < keycount; i ++ ) {
        keys[i].hide();
    }
}

const QPixmap &QPieKeyboard::cacheImage(Qt::Key key)
{
    return pixmapCache[key];
}

void QPieKeyboard::released()
{
    release();
}
