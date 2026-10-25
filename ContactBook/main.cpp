#include "mywidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyWidget window;
    window.show();
    return app.exec();
}