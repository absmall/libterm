#include <QWidget>
#include <QAbstractEventDispatcher>

class QVkbHandler : public QWidget
{
    Q_OBJECT

public:
    QVkbHandler();
    ~QVkbHandler();
    void resize();
protected:
    void resizeEvent(QResizeEvent *);


private:
    static QVkbHandler *instance;
    bool keyboardVisible;
    static bool eventFilter(void *message);
    static QAbstractEventDispatcher::EventFilter prevFilter;
};
