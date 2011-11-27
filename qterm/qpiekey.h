#include <QWidget>
#include <QBitmap>
#include <string>

class QPieKey : public QWidget
{
    Q_OBJECT
public:
    QPieKey(QWidget *parent = 0);
    ~QPieKey( );

    void initialize(int sections, const char *charlist);
    void activate(int x, int y);
    void select(const char *selection);
 
protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void keypress(char key);
    void selectionChanged(char *keys);

private:
    void size_ring();
    double angle;
    int sections;
    int highlighted_section;
    char **charlist;
    char *highlighted_chars;
    QBitmap *bitmap;
    std::string selection;
    int size;
};
