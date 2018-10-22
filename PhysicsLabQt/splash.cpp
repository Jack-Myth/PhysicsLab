#include "mainwindow.h"
#include "splash.h"
#include "ui_splash.h"

Splash* Splash::self;
Splash::Splash(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Splash)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    QPixmap bkgnd("Splash.jpg");
    this->resize(bkgnd.size());
    //bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);
    ui->label->move(10,this->height()-ui->label->height()-10);
}

Splash::~Splash()
{
    delete ui;
}
