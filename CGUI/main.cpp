
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include"graphicalui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphicalUI graph;
    graph.show();
   // MainWindow w;
   // w.show();
    return a.exec();
}
