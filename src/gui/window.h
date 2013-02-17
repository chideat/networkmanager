#ifndef WINDOW_H
#define WINDOW_H

#include <QWebView>

class Window: public QWebView {
    Q_OBJECT
public:
    Window(QWebView *parent = NULL);
    ~Window(){}
    
    
    
};




#endif // WINDOW_H