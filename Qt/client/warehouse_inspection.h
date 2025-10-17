#ifndef WAREHOUSE_INSPECTION_H
#define WAREHOUSE_INSPECTION_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Warehouse_inspection;
}
QT_END_NAMESPACE

class Warehouse_inspection : public QWidget
{
    Q_OBJECT

public:
    Warehouse_inspection(QWidget *parent = nullptr);
    ~Warehouse_inspection();

private:
    Ui::Warehouse_inspection *ui;
};
#endif // WAREHOUSE_INSPECTION_H
