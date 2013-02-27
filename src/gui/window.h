#ifndef WINDOW_H
#define WINDOW_H

#include <QWebView>
class Window: public QWebView {
    Q_OBJECT
public:
    Window(QWebView *parent = NULL);
    ~Window(){}
protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent* event);
signals:
    void status(QString s);
};




#endif // WINDOW_H