#include "warehouse_inspection.h"
#include "ui_warehouse_inspection.h"

Warehouse_inspection::Warehouse_inspection(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Warehouse_inspection)
{
    ui->setupUi(this);
}

Warehouse_inspection::~Warehouse_inspection()
{
    delete ui;
}
