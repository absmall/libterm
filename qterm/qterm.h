#ifndef __QTERM_H__
#define __QTERM_H__

#include <QWidget>
#include <QSocketNotifier>
#include <libterm.h>
#include <QTimer>
#include <qpiekeyboard.h>

class QTerm : public QWidget
{
    Q_OBJECT
public:
    QTerm(QWidget *parent = 0);
    QTerm(QWidget *parent, term_t terminal);
    ~QTerm( );
 
protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *);
    void parentResizeEvent(QResizeEvent *);
    void update(int, int, int, int);
    void update_grid(int, int, int, int);
    void mousePressEvent(QMouseEvent *event);
    bool event(QEvent *event);
 
private:
    void init( );
    int char_width;
    int char_height;
    int char_descent;
    int scrollback_height;
    int cursor_on;
    int piekey_active;
    term_t terminal;

    QFont *font;
    bool  fontWorkAround;
    void resize_term();
    static void term_bell(term_t handle);
    static void term_update(term_t handle, int x, int y, int width, int height);
    void term_update(int x, int y, int width, int height);
    static void term_update_cursor(term_t handle, int old_x, int old_y, int new_x, int new_y);
    void getRenderedStringRect( const QString string, int attrib, 
                                QFont *pFont, QRect *pUpdateRect );
    QSocketNotifier *notifier;
    QSocketNotifier *exit_notifier;
    QTimer *cursor_timer;
    QPieKeyboard *piekeyboard;

signals:
    void gridUpdated();

private slots:
    void terminal_data();
    void terminate();
    void blink_cursor();
    void piekeypress(char key);
    void resizeRequest(QSize size);
};

#endif /* __QTERM_H__ */
