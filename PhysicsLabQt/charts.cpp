#include "charts.h"
#include "mainwindow.h"
#include "ui_charts.h"
#include <QLineSeries>

QT_CHARTS_USE_NAMESPACE

Charts* Charts::pInstance;
Charts::Charts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Charts)
{
    ui->setupUi(this);
    tmpLine=new QLineSeries();
    ui->graphicsView->chart()->addSeries(tmpLine);
    ui->graphicsView->chart()->createDefaultAxes();
    ui->graphicsView->chart()->axisX()->setTitleText("时间/Second");
    ui->graphicsView->chart()->axisY()->setTitleText("电流/Ampere");
}

Charts::~Charts()
{
    delete ui;
    pInstance=nullptr;
}

void Charts::UpdateRecordPoint(float Value)
{
    qDebug()<<Value;
    if (pInstance)
    {
        QVector<QPointF> mxlist = pInstance->tmpLine->pointsVector();
        if(mxlist.count()>100)
            mxlist.pop_front();
        for(QPointF& point : mxlist)
            point.setX(point.x()-0.1f);
        pInstance->tmpLine->replace(mxlist);
        pInstance->tmpLine->append(10,Value);
        pInstance->MaxV=pInstance->MaxV<Value?Value:pInstance->MaxV;
        pInstance->MinV=pInstance->MinV>Value?Value:pInstance->MinV;
        pInstance->ui->graphicsView->chart()->axisX()->setRange(0,10);
        pInstance->ui->graphicsView->chart()->axisY()->setRange(pInstance->MinV,pInstance->MaxV);
    }
}

void Charts::ClearRecordPoints()
{
    if(pInstance)
    {
        pInstance->tmpLine->clear();
    }
}

void Charts::Show_SingleInstance()
{
    if(!pInstance)
        pInstance= new Charts();
    pInstance->show();
}
