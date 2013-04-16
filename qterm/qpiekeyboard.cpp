#include <math.h>
#include <stdio.h>
#include <QWidget>
#include <qpiekeyboard.h>
#include "term_logging.h"

QPieKeyboard::QPieKeyboard(QWidget *parent) : QObject(NULL), parent(parent)
{
    sections = 0;
    selections = NULL;
}

QPieKeyboard::~QPieKeyboard()
{
}

void QPieKeyboard::initialize(int keycount, const char *keylist)
{
    int i;
    char *baselist;
    char *reorderlist;
    if( keycount > 2 ) {
        throw "More than 2 piekeys are not supported yet";
    }

    // sections is key'th root of number of characters to show, rounded up to the nearest integer
    sections = strlen(keylist);
    sections = ceil(pow(sections-0.001, 1.0/keycount));
    slog("Sections: %d for %s", sections, keylist);
    this->keycount = keycount;
    keys = new QPieKey[keycount];
    selections = new char *[keycount];
    for( i = 0; i < keycount; i ++ ) {
        QObject::connect(&keys[i], SIGNAL(selectionChanged(int i, char *)), this, SLOT(selectionChanged(int i, char *)));
        QObject::connect(&keys[i], SIGNAL(keypress(char)), this, SLOT(piekeypressed(char)));
        QObject::connect(&keys[i], SIGNAL(released()), this, SLOT(released()));
        keys[i].setParent(parent);
    }

    // This can be generalized to any number of piekeys, but haven't done it yet
    if( keycount == 1 ) {
        keys[0].initialize(keycount, sections, keylist);
    } else if( keycount == 2 ){
        baselist = new char[sections*sections];
        memset(baselist, 0, sections*sections);
        memcpy(baselist, keylist, strlen(keylist));
        reorderlist = reorder(sections, baselist);

        keys[0].initialize(keycount, sections, baselist);
        keys[1].initialize(keycount, sections, reorderlist);

        delete baselist;
        delete reorderlist;
    }
}

void QPieKeyboard::activate(int touchId, int x, int y)
{
    int i;
    for( i = 0; i < keycount; i ++ ) {
        selections[i] = NULL;
    }
    keys[touchId].activate(x, y);
    for( i = 0; i < keycount; i ++ ) {
        // TODO - This needs to change to support > 2 piekeys
        if( i != touchId ) {
            keys[i].select( touchId, selections[!touchId] );
        }
    }
}

void QPieKeyboard::moveTouch(int touchId, int x, int y)
{
    keys[touchId].moveTouch(x-keys[touchId].x(), y-keys[touchId].y());
}

char *QPieKeyboard::reorder(int sections, char *keylist)
{
    int i, j;
    char *ret;

    ret = new char[sections*sections];

    for(i = 0; i < sections; i ++ ) {
        for( j = 0; j < sections; j ++ ) {
            ret[i+sections*j] = keylist[ j+sections*i];
        }
    }

    return ret;
}

void QPieKeyboard::piekeypressed(char key)
{
    // Just pass this along to the parent
    emit keypress(key);
}

void QPieKeyboard::selectionChanged(int index, char *selection)
{
    int i;
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

void QPieKeyboard::released()
{
    release();
}
