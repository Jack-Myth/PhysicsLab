#ifndef ELECAPPLIANCESPANEL_H
#define ELECAPPLIANCESPANEL_H

#include <QDialog>

namespace Ui {
class ElecappliancesPanel;
}

class ElecappliancesPanel : public QDialog
{
    Q_OBJECT

    struct EleapplianceInfo
    {
        QString Name;
        QString ImgPath;
        QString UClassPath;
        QString Intro;
    };

    QVector<EleapplianceInfo> EleapplianceList;
public:
    explicit ElecappliancesPanel(QWidget *parent = 0);
    ~ElecappliancesPanel();

private slots:
    void on_listWidget_currentRowChanged(int currentRow);

    void on_pushButton_clicked();

private:
    Ui::ElecappliancesPanel *ui;
};

#endif // ELECAPPLIANCESPANEL_H
