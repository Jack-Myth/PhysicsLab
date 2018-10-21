#include "elecappliancespanel.h"
#include "ui_elecappliancespanel.h"

ElecappliancesPanel::ElecappliancesPanel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ElecappliancesPanel)
{
    ui->setupUi(this);
}

ElecappliancesPanel::~ElecappliancesPanel()
{
    delete ui;
}
