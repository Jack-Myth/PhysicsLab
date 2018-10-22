#include "elecappliancespanel.h"
#include "mainwindow.h"
#include "ui_elecappliancespanel.h"

#include <QSettings>

ElecappliancesPanel::ElecappliancesPanel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ElecappliancesPanel)
{
    ui->setupUi(this);
    QSettings ElecappliancesList("./Profiles/Elecappliances.ini",QSettings::IniFormat);
    QStringList sections = ElecappliancesList.childGroups();
    for(QString section:sections)
    {
        QString Name = ElecappliancesList.value(QString("/")+section+QString("/Name")).toString();
        QString ImgPath= ElecappliancesList.value(QString("/")+section+QString("/ImgPath")).toString();
        QString UClassPath= ElecappliancesList.value(QString("/")+section+QString("/UClassPath")).toString();
        QString Intro= ElecappliancesList.value(QString("/")+section+QString("/Intro")).toString();
        Intro.replace("\\n","\n");
        EleapplianceInfo tmpInfo;
        tmpInfo.Name=Name;
        tmpInfo.ImgPath=ImgPath;
        tmpInfo.UClassPath=UClassPath;
        tmpInfo.Intro=Intro;
        EleapplianceList.push_back(tmpInfo);
        //Item Index corresponding the item in EleapplianceList
        QListWidgetItem* tmpItem=new QListWidgetItem(Name);
        ui->listWidget->addItem(tmpItem);
    }
}

ElecappliancesPanel::~ElecappliancesPanel()
{
    delete ui;
}

void ElecappliancesPanel::on_listWidget_currentRowChanged(int currentRow)
{
    if(currentRow>=0)
    {
        ui->pushButton->setEnabled(true);
        QImage img;
        ui->labelName->setText(EleapplianceList[currentRow].Name);
        ui->labelIntro->setText(EleapplianceList[currentRow].Intro);
        if(img.load(EleapplianceList[currentRow].ImgPath))
        {
            ui->labelImg->setPixmap(QPixmap::fromImage(img));
        }

    }
    else
    {
        ui->pushButton->setEnabled(false);
    }
}

void ElecappliancesPanel::on_pushButton_clicked()
{
    if(ui->listWidget->currentRow()>=0)
    {
        QString Path =EleapplianceList[ui->listWidget->currentRow()].UClassPath;
        MainWindow::pInstance->RequestSpawnActor(Path);
    }
}
