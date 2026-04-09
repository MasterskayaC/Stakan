
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <cstddef>
#include"graphicalui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphicalUI graph(nullptr);
    graph.show();
    return a.exec();
}
