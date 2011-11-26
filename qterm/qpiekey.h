#include <QWidget>
#include <QBitmap>

class QPieKey : public QWidget
{
    Q_OBJECT
public:
    QPieKey(QWidget *parent = 0);
    ~QPieKey( );
 
protected:
    void paintEvent(QPaintEvent *event);
private:
    QBitmap *bitmap;
};
