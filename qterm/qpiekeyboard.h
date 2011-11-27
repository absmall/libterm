#include <vector>
#include <QWidget>
#include <qpiekey.h>

class QPieKeyboard : public QWidget
{
    Q_OBJECT
public:
    QPieKeyboard(QWidget *parent = 0);
    ~QPieKeyboard( );
    void initialize(int sections, const char *keylist);
    void activate(int x1, int y1, int x2, int y2);
 
signals:
    void keypress(char key);

private:
    char *reorder(int sections, char *keylist);
    int sections;
    QPieKey left;
    QPieKey right;
};
