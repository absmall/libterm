#include <QWidget>
#include <QSocketNotifier>
#include <libterm.h>
#include <QVBoxLayout>

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
 
private:
	void init( );
	int char_width;
	int char_height;
	int cursor_x;
	int cursor_y;
	int cursor_on;
	term_t terminal;
	static void term_update(term_t handle, int x, int y, int width, int height);
	static void term_update_cursor(term_t handle, int x, int y);
	QSocketNotifier *notifier;

private slots:
	void terminal_data();
};
