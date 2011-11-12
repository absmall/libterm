#include <QPainter>
#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QPaintEvent>
#include <QFontMetrics>
#include <qterm.h>
#include <stdio.h>
#include <QKeyEvent>
#include <sys/select.h>

#define WIDTH	80
#define HEIGHT	25

QTerm::QTerm(QWidget *parent) : QWidget(parent)
{
	char_width = 0;
	char_height = 0;

	term_create( WIDTH, HEIGHT, 0, &terminal );
	term_set_user_data( terminal, this );
	term_register_update( terminal, term_update );
	notifier = new QSocketNotifier( term_get_file_descriptor(terminal), QSocketNotifier::Read );
	QObject::connect(notifier, SIGNAL(activated(int)), this, SLOT(terminal_data()));
}

QTerm::QTerm(QWidget *parent, term_t terminal) : QWidget(parent)
{
	char_width = 0;
	char_height = 0;

	this->terminal = terminal;
	term_set_user_data( terminal, this );
	term_register_update( terminal, term_update );
	notifier = new QSocketNotifier( term_get_file_descriptor(terminal), QSocketNotifier::Read );
	QObject::connect(notifier, SIGNAL(activated(int)), this, SLOT(terminal_data()));
}

QTerm::~QTerm()
{
	term_free( terminal );
	delete notifier;
}

void QTerm::term_update(term_t handle, int x, int y, int width, int height)
{
	QTerm *term = (QTerm *)term_get_user_data( handle );
	// Keep things simple, just redraw the whole display
	term->repaint(term->contentsRect());
}


void QTerm::terminal_data()
{
	if( !term_process_child( terminal ) ) {
		exit(0);
	}
}
 
void QTerm::paintEvent(QPaintEvent *event)
{
	int i, j;
	const uint32_t **grid;
	QPainter painter(this);

	painter.setBackgroundMode(Qt::TransparentMode);
	painter.setBrush(QColor(0, 0, 0));

	// First erase the grid with its current dimensions
	painter.drawRect(event->rect());
	
	char_width = painter.fontMetrics().maxWidth();
	char_height = painter.fontMetrics().lineSpacing();

	painter.setPen(QColor(255, 255, 255));
	grid = term_get_grid( terminal );
	for( i = 0; i < HEIGHT; i ++ ) {
		for( j = 0; j < WIDTH; j ++ ) {
			painter.drawText(j * char_width, (i + 1) * char_height, QString( QChar( grid[ i ][ j ] ) ) );
		}
	}
}

void QTerm::keyPressEvent(QKeyEvent *event)
{
	term_send_data( terminal, event->text().toUtf8().constData(), event->text().count() );
}
 
int main(int argc, char *argv[])
{
	term_t terminal;
    QApplication app(argc, argv);
 
	term_create( WIDTH, HEIGHT, 0, &terminal );
    QTerm term(NULL, terminal);
    term.show();
 
    return app.exec();
}
