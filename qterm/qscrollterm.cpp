#include "qscrollterm.h"
#include "term_logging.h"
#include <QResizeEvent>

QScrollTerm::QScrollTerm(QWidget *parent, term_t terminal) : term(this, terminal)
{
    QObject::connect(this, SIGNAL(parentResize(QSize)), &term, SLOT(resizeRequest(QSize)));
    QObject::connect(&term, SIGNAL(gridUpdated(QRect)), this, SLOT(childUpdated(QRect)));
    setFocusProxy(&term);
    setFocusPolicy(Qt::StrongFocus);
    setWidget(&term);
}

void QScrollTerm::resizeEvent(QResizeEvent *event)
{
    emit parentResize(event->size());
}

void QScrollTerm::childUpdated(QRect region)
{
    slog("ensurevisible %d %d", region.x(), region.y());
    ensureVisible(region.x(), region.y(), 0, 0);
}
