#include "charts.h"
#include "ui_charts.h"

Charts::Charts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Charts)
{
    ui->setupUi(this);
}

Charts::~Charts()
{
    delete ui;
}
