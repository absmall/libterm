#include <QWidget>
#include <QAbstractEventDispatcher>

class QVkbHandler : public QWidget
{
    QVkbHandler();
    ~QVkbHandler();
    void resize();

private:
    static QVkbHandler *instance;
    bool keyboardVisible;
    static bool eventFilter(void *message);
    static QAbstractEventDispatcher::EventFilter prevFilter;
};
