#include "gui/window.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    
    //  mainWindow->setAttribute (Qt::WA_ShowModal,true);
     // mainWindow->setWindowOpacity (1);
  //    mainWindow->setWindowFlags (Qt::FramelessWindowHint);
  //    mainWindow->setAttribute (Qt::WA_TranslucentBackground);
    
    return app.exec();
}
