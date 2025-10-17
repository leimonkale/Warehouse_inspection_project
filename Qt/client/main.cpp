#include "warehouse_inspection.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Warehouse_inspection w;
    w.show();
    return a.exec();
}
