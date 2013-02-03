#include "qscrollterm.h"
#include "term_logging.h"
#include <QResizeEvent>

QScrollTerm::QScrollTerm(QWidget *parent, term_t terminal) : term(this, terminal)
{
    QObject::connect(this, SIGNAL(parentResize(QSize)), &term, SLOT(resizeRequest(QSize)));
    setFocusProxy(&term);
    setFocusPolicy(Qt::StrongFocus);
    setWidget(&term);
}

void QScrollTerm::resizeEvent(QResizeEvent *event)
{
    emit parentResize(event->size());
}
