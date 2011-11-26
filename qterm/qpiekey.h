#include <QWidget>
#include <QBitmap>

class QPieKey : public QWidget
{
    Q_OBJECT
public:
    QPieKey(QWidget *parent = 0);
    ~QPieKey( );

    void initialize(int sections, const char *charlist);
    void activate(int x, int y);
 
protected:
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    void size_ring();
    double angle;
    int sections;
    int highlighted_section;
    char *charlist;
    QBitmap *bitmap;
    int size;
};
