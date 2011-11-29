#include <QWidget>
#include <QSocketNotifier>
#include <libterm.h>
#include <QTimer>
#include <QVBoxLayout>
#include <QPushButton>
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
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *event);
    bool event(QEvent *event);
 
private:
    void init( );
    int char_width;
    int char_height;
    int char_descent;
    int cursor_x;
    int cursor_y;
    int cursor_on;
    int piekey_active;
    term_t terminal;
    static void term_bell(term_t handle);
    static void term_update(term_t handle, int x, int y, int width, int height);
    static void term_update_cursor(term_t handle, int x, int y);
    QSocketNotifier *notifier;
    QSocketNotifier *exit_notifier;
    QTimer *cursor_timer;
    QPieKeyboard *piekeyboard;

private slots:
    void terminal_data();
    void terminate();
    void blink_cursor();
    void piekeypress(char key);
};
