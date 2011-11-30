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
    void initialize(int sections, const char *keylist);
    void activate(int x1, int y1, int x2, int y2);
    void testMode(int delay);
    void moveTouch(int touchId, int x, int y);
    void release();
 
signals:
    void keypress(char key);

private:
    char *reorder(int sections, char *keylist);
    // Indicates the left and right piekeys are swapped in position
    int swapped;
    int sections;
    QPieKey left;
    QPieKey right;
    int testDelay;
    int testX, testY;
    QTimer testTimer;
    char *leftSelection;
    char *rightSelection;

private slots:
    void testTime();
    void piekeypressed(char key);
    void leftSelectionChanged(char *selection);
    void rightSelectionChanged(char *selection);
    void released();
};
