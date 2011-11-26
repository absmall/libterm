#include <QWidget>
#include <QSocketNotifier>
#include <libterm.h>
#include <QVBoxLayout>
#include <QPushButton>
#include <qpiekey.h>

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
    void mousePressEvent(QMouseEvent *event);
 
private:
    void init( );
    int char_width;
    int char_height;
    int char_descent;
    int cursor_x;
    int cursor_y;
    int cursor_on;
    term_t terminal;
    static void term_update(term_t handle, int x, int y, int width, int height);
    static void term_update_cursor(term_t handle, int x, int y);
    QSocketNotifier *notifier;
    QPieKey *piekey;

private slots:
    void terminal_data();
};
