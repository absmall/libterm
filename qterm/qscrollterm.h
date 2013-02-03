#ifndef __QSCROLLTERM_H__
#define __QSCROLLTERM_H__

#include <QWidget>
#include <QScrollArea>
#include "qterm.h"

class QScrollTerm : public QScrollArea
{
    Q_OBJECT;

public:
    QScrollTerm(QWidget *parent, term_t);

protected:
    void resizeEvent(QResizeEvent *);

signals:
    void parentResize(QSize size);

private:
    QTerm term;
};

#endif /* __QSCROLLTERM_H__ */
