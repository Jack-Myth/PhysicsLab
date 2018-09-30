#include "mainwindow.h"
#include "splash.h"
#include "ui_splash.h"

Splash* Splash::self;
Splash::Splash(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Splash)
{
    ui->setupUi(this);
}

Splash::~Splash()
{
    delete ui;
}
