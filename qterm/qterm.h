#include <QWidget>
#include <QSocketNotifier>
#include <libterm.h>
#include <QVBoxLayout>

#if 0
class QTerm : public QWidget
{
};
#endif

class QTerm : public QWidget
{
    Q_OBJECT
public:
    QTerm(QWidget *parent = 0);
	~QTerm( );
 
protected:
	void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *);
 
private:
	int char_width;
	int char_height;
	term_t terminal;
	static void term_update(term_t handle, int x, int y, int width, int height);
	QSocketNotifier *notifier;

private slots:
	void terminal_data();
};
