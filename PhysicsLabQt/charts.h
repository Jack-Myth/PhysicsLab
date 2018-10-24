#ifndef CHARTS_H
#define CHARTS_H

#include <QDialog>
#include <QtCharts>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class Charts;
}

class Charts : public QDialog
{
    Q_OBJECT
    static Charts* pInstance;
    QLineSeries* tmpLine;
public:
    explicit Charts(QWidget *parent = 0);
    ~Charts();
    static void UpdateRecordPoint(float Value);
    static void ClearRecordPoints();
    static void Show_SingleInstance();
    float MaxV=0,MinV=999;
private:
    Ui::Charts *ui;
};

#endif // CHARTS_H
