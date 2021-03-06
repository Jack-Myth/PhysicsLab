#include "charts.h"
#include "elecappliancespanel.h"
#include "mainwindow.h"
#include "splash.h"
#include "ui_mainwindow.h"
#include "unrealcommunicatorhelper.h"
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QProcess>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QtNetwork>

#define INIT_JSON_MSG(OBJECT) \
    QJsonDocument __JsonO;\
    QJsonObject OBJECT;
#define SEND_JSON_MSG(OBJECT) \
    __JsonO.setObject(OBJECT);\
    UnrealCommunicatorHelper::SendJson(UnrealCommunicator,__JsonO.toJson().data(),__JsonO.toBinaryData().length());

MainWindow* MainWindow::pInstance;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setAttribute(Qt::WA_DeleteOnClose);
    pInstance=this;
    Splash::self=new Splash();
    Splash::self->show();
    CachedMessage.CachedMessageTargetSize=0;
    UnrealCommunicatorServer.listen(QHostAddress::Any,10703);
    ui->UnrealFrame->setAttribute(Qt::WA_NativeWindow);
    connect(&UnrealCommunicatorServer,&QTcpServer::newConnection,this,&MainWindow::OnUnrealConnected,Qt::UniqueConnection);

    //Build Menu
    connect(ui->action_ElecAppliances,&QAction::triggered,this,[=](bool checked)
    {
        ElecappliancesPanel* EP=new ElecappliancesPanel(nullptr);
        EP->show();
    });
    connect(ui->action_Charts,&QAction::triggered,this,[=]()
    {
        Charts::Show_SingleInstance();
    });
#ifdef QT_NO_DEBUG
    QStringList ArgList;
    ArgList.push_back("-ResX=1");
    ArgList.push_back("-ResY=1");
    ArgList.push_back("-WINDOWED");
    QProcess::startDetached("./PhysicsLabUnreal/Binaries/Win64/PhysicsLabUnreal-Win64-Shipping.exe",ArgList);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::RequestSpawnActor(QString UClassPath)
{
    INIT_JSON_MSG(JsonO);
    JsonO.insert("Action","SpawnBPClass");
    JsonO.insert("UClassPath",UClassPath);
    SEND_JSON_MSG(JsonO);
}

void MainWindow::OnUnrealConnected()
{
    if (!UnrealCommunicator)
    {
        UnrealCommunicator=UnrealCommunicatorServer.nextPendingConnection();
        connect(UnrealCommunicator,&QTcpSocket::readyRead,this,&MainWindow::OnCommunicationMessageArrived,Qt::UniqueConnection);
        UnrealCommunicatorServer.close();
        QJsonDocument JsonD;
        QJsonObject JsonO;
        JsonO.insert("Action","RequestHwnd");
        JsonD.setObject(JsonO);
        UnrealCommunicatorHelper::SendJson(UnrealCommunicator,JsonD.toJson().data(),JsonD.toBinaryData().length());
    }
}

void MainWindow::OnCommunicationMessageArrived()
{
    if(!CachedMessage.CachedMessageTargetSize)
    {
        int PackageSize;
        UnrealCommunicator->read((char*)&PackageSize,sizeof(int));
        CachedMessage.CachedMessageTargetSize=PackageSize;
        CachedMessage.CachedMessageData.resize(CachedMessage.CachedMessageTargetSize);
        CachedMessage.CachedMessageCurrentSize = UnrealCommunicator->read(CachedMessage.CachedMessageData.data(),
                                                                          CachedMessage.CachedMessageTargetSize);
        if(CachedMessage.CachedMessageCurrentSize>=CachedMessage.CachedMessageTargetSize)
        {
            InvokeAction();
            CachedMessage.CachedMessageData.clear();
            CachedMessage.CachedMessageCurrentSize=0;
            CachedMessage.CachedMessageTargetSize=0;
        }
    }
    else
    {
        CachedMessage.CachedMessageCurrentSize += UnrealCommunicator->read(
                    CachedMessage.CachedMessageData.data()+CachedMessage.CachedMessageCurrentSize,
                    CachedMessage.CachedMessageTargetSize-CachedMessage.CachedMessageCurrentSize);
        if (CachedMessage.CachedMessageCurrentSize>=CachedMessage.CachedMessageTargetSize)
        {
            InvokeAction();
            CachedMessage.CachedMessageData.clear();
            CachedMessage.CachedMessageCurrentSize=0;
            CachedMessage.CachedMessageTargetSize=0;
        }
    }
}

void MainWindow::InvokeAction()
{
    QJsonParseError ErrorMsg;
    QString JsonStr=QString(CachedMessage.CachedMessageData.data());
    QByteArray JsonData=JsonStr.toUtf8();
    QJsonDocument MessageJson = QJsonDocument::fromJson(JsonData,&ErrorMsg);
    if(ErrorMsg.error!=QJsonParseError::NoError)
    {
        qDebug()<<ErrorMsg.errorString();
        qDebug()<<"Json:";
        qDebug(CachedMessage.CachedMessageData.data());
    }
    if (MessageJson.isNull())
        return;
    TargetMsg=&MessageJson;
    QJsonObject MessageObj = MessageJson.object();
    this->metaObject()->invokeMethod(this,MessageObj.find("Action").value().toString().toStdString().c_str());
}

void MainWindow::SendActorDetail(QString PropertyName, QString PropertyType, const QJsonValue& PropertyValue)
{
    INIT_JSON_MSG(JsonO);
    JsonO.insert("Action","SendActorDetail");
    JsonO.insert("Name",PropertyName);
    JsonO.insert("Type",PropertyType);
    JsonO.insert("Value",PropertyValue);
    SEND_JSON_MSG(JsonO);
}

void MainWindow::SendHwnd()
{
    if(Splash::self)
        Splash::self->close();
    UnrealHwnd = (HWND)TargetMsg->object().find("Hwnd").value().toString().toLongLong();
    SetParent(UnrealHwnd,(HWND)ui->UnrealFrame->winId());
    qDebug()<<ui->UnrealFrame->size().height();
    this->show();
    //Request Refresh
}

void MainWindow::SyncScene()
{
    ui->SceneTree->clear();
    QJsonArray ActorList = TargetMsg->object().find("ActorList").value().toArray();
    QList<QTreeWidgetItem*> WidgetItemList;
    for(QJsonValueRef JsV:ActorList)
    {
        if(JsV.isObject())
            WidgetItemList.append(Internal_SyncScene(JsV.toObject()));
        else
        {
            QTreeWidgetItem* tmpT = new QTreeWidgetItem();
            tmpT->setText(0,JsV.toString());
            WidgetItemList.append(tmpT);
        }
    }
    ui->SceneTree->addTopLevelItems(WidgetItemList);
}

void MainWindow::SyncActorDetails()
{
    ui->Details->clear();
    QJsonObject Properties = TargetMsg->object().find("Properties").value().toObject();
    for(auto it = Properties.begin();it!=Properties.end();++it)
    {
        QHBoxLayout* MainHorizontalBox=new QHBoxLayout();
        QListWidgetItem* PropertyListItem=new QListWidgetItem();
        QSize ItemSizeHint=PropertyListItem->sizeHint();
        ItemSizeHint.setHeight(50);
        PropertyListItem->setSizeHint(ItemSizeHint);
        ui->Details->addItem(PropertyListItem);
        QJsonObject PropertyObject = it.value().toObject();
        QString PropertyName=it.key();
        QLabel* DisplayName= new QLabel();
        DisplayName->setText(PropertyObject.find("DisplayName").value().toString());
        MainHorizontalBox->addWidget(DisplayName);
        QString PropertyType=PropertyObject.find("Type").value().toString();
        if(PropertyType=="Float")
        {
            QDoubleSpinBox* PropertyValue=new QDoubleSpinBox();
            PropertyValue->setMaximum(9999999999);
            PropertyValue->setValue(PropertyObject.find("Value").value().toDouble());
            void (QDoubleSpinBox::*pf)(double)=&QDoubleSpinBox::valueChanged;
            connect(PropertyValue,pf,this,[=](double Value)
            {
                SendActorDetail(PropertyName,"Float",QJsonValue(Value));
            },Qt::UniqueConnection);
            MainHorizontalBox->addWidget(PropertyValue);
        }
        else
        {
            QLineEdit* PropertyValue=new QLineEdit();
            PropertyValue->setText(PropertyObject.find("Value").value().toString());
            connect(PropertyValue,&QLineEdit::editingFinished,this,[=]()
            {
                SendActorDetail(PropertyName,PropertyType,QJsonValue(PropertyValue->text()));
            },Qt::UniqueConnection);
            MainHorizontalBox->addWidget(PropertyValue);
        }
        QWidget* x=new QWidget();
        x->setLayout(MainHorizontalBox);
        ui->Details->setItemWidget(PropertyListItem,x);
    }
}

void MainWindow::SendDataPoint()
{
    Charts::UpdateRecordPoint(TargetMsg->object().find("Value").value().toDouble());
}

void MainWindow::ClearDataPoints()
{
    Charts::ClearRecordPoints();
}

QTreeWidgetItem *MainWindow::Internal_SyncScene(QJsonObject ChildActor)
{
    QString ParentName=ChildActor.keys()[0];
    QTreeWidgetItem* TreeItem=new QTreeWidgetItem();
    QList<QTreeWidgetItem*> ChildList;
    QJsonArray ChildArray = ChildActor["ParentName"].toArray();
    for(QJsonValueRef Child2Child:ChildArray)
    {
        if(Child2Child.isObject())
            ChildList.append(Internal_SyncScene(Child2Child.toObject()));
        else
        {
            QTreeWidgetItem* tmpT=new QTreeWidgetItem();
            tmpT->setText(0,Child2Child.toString());
            ChildList.append(tmpT);
        }
    }
    TreeItem->addChildren(ChildList);
    return TreeItem;
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    QMainWindow::moveEvent(event);
    SendMessage(UnrealHwnd,WM_MOVE,0,0);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    INIT_JSON_MSG(JsonO);
    JsonO.insert("Action","Quit");
    SEND_JSON_MSG(JsonO);
    QMainWindow::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    SetWindowPos(UnrealHwnd,NULL,0,0,ui->UnrealFrame->size().width(),ui->UnrealFrame->size().height(),NULL);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    SetWindowPos(UnrealHwnd,NULL,0,0,ui->UnrealFrame->size().width(),ui->UnrealFrame->size().height(),NULL);
}

void MainWindow::on_SceneTree_itemClicked(QTreeWidgetItem *item, int column)
{
    (void)column;
    INIT_JSON_MSG(JsonO);
    JsonO.insert("Action","SelectActor");
    JsonO.insert("ActorName",item->text(0));
    SEND_JSON_MSG(JsonO);
}
