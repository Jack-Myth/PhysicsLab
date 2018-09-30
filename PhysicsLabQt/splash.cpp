#include "mainwindow.h"
#include "splash.h"
#include "ui_splash.h"

Splash::Splash(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Splash)
{
    ui->setupUi(this);
    pMainWindow=new MainWindow();
}

Splash::~Splash()
{
    delete ui;
}
