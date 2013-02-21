#include "qscrollterm.h"
#include "term_logging.h"
#include <QResizeEvent>

QScrollTerm::QScrollTerm(QWidget *parent, term_t terminal) : QScrollArea(parent), term(this, terminal)
{
    QObject::connect(this, SIGNAL(parentResize(QSize)), &term, SLOT(resizeRequest(QSize)));
    QObject::connect(&term, SIGNAL(gridUpdated()), this, SLOT(childUpdated()));
    setFocusProxy(&term);
    setFocusPolicy(Qt::StrongFocus);
    setWidget(&term);
}

void QScrollTerm::resizeEvent(QResizeEvent *event)
{
    emit parentResize(event->size());
}

void QScrollTerm::childUpdated()
{
    QWidget *child = widget();
    ensureVisible(child->width(), child->height(), 0, 0);
}
