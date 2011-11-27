#include <qpiekeyboard.h>

QPieKeyboard::QPieKeyboard(QWidget *parent) : QWidget(parent), left(parent), right(parent), testTimer(this)
{
    testDelay = 0;
    sections = 0;
    QObject::connect(&testTimer, SIGNAL(timeout()), this, SLOT(testTime()));
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
    left.activate(x1, y1);
    if( !testDelay ) {
        right.activate(x2, y2);
    } else {
        testX = x2;
        testY = y2;
        testTimer.start(testDelay*1000);
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
    right.activate(testX, testY);
}
