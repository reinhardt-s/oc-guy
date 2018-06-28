#include "guy.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Guy w;
    w.show();

    return a.exec();
}
