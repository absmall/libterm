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
    void initialize(int keys, const char *keylist);
    void activate(int touchId, int x1, int y1);
    void moveTouch(int touchId, int x, int y);
    void release();
 
signals:
    void keypress(char key);

private:
    char *reorder(int sections, char *keylist);
    int keycount;
    int sections;
    QPieKey *keys;
    char **selections;
    QWidget *parent;

private slots:
    void piekeypressed(char key);
    void selectionChanged(int index, char *selection);
    void released();
};
