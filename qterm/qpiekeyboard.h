#include <vector>
#include <QWidget>
#include <QTimer>
#include <qpiekey.h>

class QPieKeyboard : public QObject
{
    Q_OBJECT
public:
    QPieKeyboard(QWidget *parent = 0);
    ~QPieKeyboard( );
    void initialize(int keys, const std::vector<Qt::Key> &keylist);
    void activate(int touchId, int x1, int y1);
    void moveTouch(int touchId, int x, int y);
    void release();
 
signals:
    void keypress(Qt::Key key);

private:
    std::vector<Qt::Key> reorder(int sections, std::vector<Qt::Key> keylist);
    int keycount;
    int sections;
    QPieKey *keys;
    const std::vector<Qt::Key> **selections;
    QWidget *parent;

private slots:
    void piekeypressed(Qt::Key key);
    void selectionChanged(const std::vector<Qt::Key> *selection);
    void released();
};
