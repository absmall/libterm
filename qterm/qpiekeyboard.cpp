#include <stdio.h>
#include <qpiekeyboard.h>

QPieKeyboard::QPieKeyboard(QWidget *parent) : QObject(NULL), left(parent), right(parent), testTimer(this)
{
    testDelay = 0;
    sections = 0;
    QObject::connect(&testTimer, SIGNAL(timeout()), this, SLOT(testTime()));
    QObject::connect(&left, SIGNAL(selectionChanged(char *)), this, SLOT(leftSelectionChanged(char *)));
    QObject::connect(&right, SIGNAL(selectionChanged(char *)), this, SLOT(rightSelectionChanged(char *)));
    QObject::connect(&left, SIGNAL(keypress(char)), this, SLOT(piekeypressed(char)));
    QObject::connect(&right, SIGNAL(keypress(char)), this, SLOT(piekeypressed(char)));
    QObject::connect(&left, SIGNAL(released()), this, SLOT(released()));
    QObject::connect(&right, SIGNAL(released()), this, SLOT(released()));

    leftSelection = NULL;
    rightSelection = NULL;
}

QPieKeyboard::~QPieKeyboard()
{
}

void QPieKeyboard::testMode(int delay)
{
    testDelay = delay;
}

void QPieKeyboard::initialize(int sections, const char *keylist)
{
    char *baselist;
    char *reorderlist;

    this->sections = sections;

    baselist = new char[sections*sections];
    memset(baselist, 0, sections*sections);
    memcpy(baselist, keylist, strlen(keylist));
    reorderlist = reorder(sections, baselist);

    left.initialize(sections, baselist);
    right.initialize(sections, reorderlist);

    delete baselist;
    delete reorderlist;
}

void QPieKeyboard::activate(int x1, int y1, int x2, int y2)
{
    if( x2 < x1 ) {
        x2 ^= x1 ^= x2 ^= x1;
        y2 ^= y1 ^= y2 ^= y1;
        swapped = 1;
    } else {
        swapped = 0;
    }
    leftSelection = NULL;
    rightSelection = NULL;
    left.activate(x1, y1);
    left.select( rightSelection );
    if( !testDelay ) {
        right.activate(x2, y2);
        right.select( leftSelection );
    } else {
        testX = x2;
        testY = y2;
        testTimer.start(testDelay*1000);
    }
}

void QPieKeyboard::moveTouch(int touchId, int x, int y)
{
    if( touchId ^ swapped == 0 ) {
        left.moveTouch(x - left.x(), y - left.y());
    } else if( touchId == 1 ) {
        right.moveTouch(x - right.x(), y - right.y());
    }
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

void QPieKeyboard::testTime()
{
    testTimer.stop();
    right.activate(testX, testY);
    right.select( leftSelection );
}

void QPieKeyboard::piekeypressed(char key)
{
    // Just pass this along to the parent
    emit keypress(key);
}

void QPieKeyboard::leftSelectionChanged(char *selection)
{
    leftSelection = selection;
    right.select( leftSelection );
}

void QPieKeyboard::rightSelectionChanged(char *selection)
{
    rightSelection = selection;
    left.select( rightSelection );
}

void QPieKeyboard::release()
{
    left.hide();
    right.hide();
}

void QPieKeyboard::released()
{
    release();
}
