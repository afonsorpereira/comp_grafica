/**
#include  <QApplication>
#include  <QSurfaceFormat>
#include "mainwindow.h"
int  main(int argc , char *argv [])
{
    QSurfaceFormat  format;
#ifdef  Q_OS_MAC
    format.setVersion (1, 2);
    format.setProfile(QSurfaceFormat :: CoreProfile);
#endif
    // defining  anti -aliasing  -> can be used  for  win/linux
    format.setDepthBufferSize (24);
    format.setSamples (4);
    QSurfaceFormat :: setDefaultFormat(format);
    QApplication a(argc , argv);
    MainWindow w;
    w.show();
    return a.exec();
}
**/

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
