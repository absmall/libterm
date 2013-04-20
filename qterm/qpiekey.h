#ifndef __QPIEKEY_H__
#define __QPIEKEY_H__

#include <QWidget>
#include <QBitmap>
#include <string>
#include <vector>
#include <map>

class QPieKeyboard;

class QPieKey : public QWidget
{
    Q_OBJECT
public:
    QPieKey(QWidget *parent = 0);
    ~QPieKey( );

    void initialize(QPieKeyboard *keyboard, int sections, const std::vector<Qt::Key> &keylist);
    void activate(int x, int y);
    void select(int key, const std::vector<Qt::Key> *selection);
    void moveTouch(int x, int y);
 
protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void keypress(Qt::Key key);
    void selectionChanged(const std::vector<Qt::Key> *keys);
    void released();

private:
    QPieKeyboard *keyboard;
    void size_ring();
    QChar toChar(Qt::Key key);
    double angle;
    int sections;
    int letters_per_section;
    int highlighted_section;
    std::vector<std::vector<Qt::Key> > keylist;
    QBitmap *bitmap;
    std::map<int, const std::vector<Qt::Key> *> selections;
    int size;
    int keys;
};

#endif /* __QPIEKEY_H__ */
