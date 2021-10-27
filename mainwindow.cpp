#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QString>
#include <QInputDialog>
#include <QtQuickWidgets/QQuickWidget>
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace  {
    const int scaleElement = 30 ;
    const int startX       = 300;
    const int startY       = 300;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QObject::connect(ui->open,        &QAction::triggered, this, [this](){open()                     ;});
    QObject::connect(ui->write,       &QAction::triggered, this, [this](){write()                    ;});

    QObject::connect(ui->add,       &QAction::triggered, this, [this](){add()                        ;});
    QObject::connect(ui->remove,    &QAction::triggered, this, [this](){remove()                     ;});
    QObject::connect(ui->removeAll, &QAction::triggered, this, [this](){ui->tableWidget->clear()     ;});
    QObject::connect(ui->calc,      &QAction::triggered, this, [this](){setDrawState(!getDrawState());});

    QObject::connect(ui->baseLeft,  &QCheckBox::stateChanged, this, [this](){setLeftBase(!getLeftBase());});
    QObject::connect(ui->baseRight,  &QCheckBox::stateChanged, this, [this](){setRightBase(!getRightBase());});



}

void MainWindow::fromJsonToTable(QJsonObject &obj)
{

    QJsonValue val = obj.value("elements");
    QJsonArray array = val.toArray();

    removeAll();


    foreach(const QJsonValue &v, array)
    {

        QTableWidgetItem *itemL = new QTableWidgetItem;
        itemL->setText(v.toObject().value("L").toString());

        QTableWidgetItem *itemA = new QTableWidgetItem;
        itemA->setText(v.toObject().value("A").toString());

        QTableWidgetItem *itemE = new QTableWidgetItem;
        itemE->setText(v.toObject().value("E").toString());

        QTableWidgetItem *itemSig = new QTableWidgetItem;
        itemSig->setText(v.toObject().value("sigma").toString());

        QTableWidgetItem *itemQ = new QTableWidgetItem;
        itemQ->setText(v.toObject().value("Q").toString());

        QTableWidgetItem *itemF = new QTableWidgetItem;
        itemF->setText(v.toObject().value("F").toString());

        add();
        qDebug(qPrintable(v.toObject().value("L").toString()));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0,     itemL  );
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, A,     itemA  );
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, E,     itemE  );
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, sigma, itemSig);
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, q,     itemQ  );
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, F,     itemF  );
    }
}

void MainWindow::open(){
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open project"), "", tr("TXT Document (*.txt)"));

    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

//    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    removeAll();
    QString str = file.readAll();
    qDebug(qPrintable(str));
    QStringList list = str.split(" ");
    int i = 0;

    for (auto &it:list)
    {
            if (i==0)
            {
                add();
                qDebug("add %i", i);
            }
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(it);
//            qDebug(qPrintable(item->text()));
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, i, item);
            i++;
            if (i == 6)
                i = 0;
    }
    file.close();
    ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);
//    QJsonObject obj = doc.object();
//    fromJsonToTable(obj);

}

void MainWindow::write()
{

    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Open project"), "", tr("TXT Document (*.txt)"));

QString data;
QFile file;
file.setFileName(filename);
file.open(QIODevice::WriteOnly | QIODevice::Text);
QTextStream stream(&file);
for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
     for (int j = 0; j < ui->tableWidget->columnCount(); j++)
     {

         stream << ui->tableWidget->item(i,j)->text().toUtf8();
         stream << " ";
     }
//     stream << "\n";
 }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setDrawState(bool state)
{
    wigetState.drawState = state;
}

void MainWindow::setLeftBase(bool state)
{
    wigetState.leftBase = state;
}

void MainWindow::setRightBase(bool state)
{
    wigetState.rightBase = state;
}

bool MainWindow::getDrawState()
{
    return wigetState.drawState;
}

bool MainWindow::getRightBase()
{
    return wigetState.rightBase;
}

bool MainWindow::getLeftBase()
{
    return wigetState.leftBase;
}

bool MainWindow::validation()
{
    for (int i = 0; i < ui->tableWidget->columnCount()-2; i++)
    {
        for (int j = 0; j < ui->tableWidget->rowCount(); j++)
        {
            if (i == 0 && ui->tableWidget->item(j,i)->text().toDouble() <= 0)
                return 0;
            if (i == 1 && ui->tableWidget->item(j,i)->text().toDouble() <= 0)
                return 0;
            if (i == 2 && ui->tableWidget->item(j,i)->text().toDouble() <= 0)
                return 0;
            if (i == 3 && ui->tableWidget->item(j,i)->text().toDouble() <= 0)
                return 0;
        }
    }
    return 1;
}


void MainWindow::initRow()
{
    for (int i = 0; i < ui->tableWidget->columnCount(); i++)
    {
        qDebug("i-%d, row-%d",i,ui->tableWidget->rowCount()-1);
        ui->tableWidget->item(ui->tableWidget->rowCount()-1, i)->setText("0");

    }
}

void MainWindow::add()
{

    ui->tableWidget->insertRow(ui->tableWidget->rowCount());

    QString data = "0";

    for (int i = 0; i < ui->tableWidget->columnCount(); i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText("0");
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,i, item);
    }

}

void MainWindow::remove()
{
    int num = QInputDialog::getInt(this, "Введите номер стержня", "Номер стержня", 1, 1, ui->tableWidget->rowCount());
    qDebug("%d", num);
    ui->tableWidget->removeRow(num - 1);
}

inline void paintElement(QPainter &painter, int x, int centerY, qreal width, qreal height)
{
    painter.drawRect(x, centerY - height/2, width, height);
}

inline void MainWindow::paintQ(QPainter &painter, int x, int centerY, qreal width, QString q)
{
    painter.setPen(QPen(Qt::blue, 3, Qt::SolidLine, Qt::FlatCap));

    painter.drawLine(x, centerY , x + width, centerY);
    painter.drawText(x + width/2, centerY + 40, q + "q");

    if (q.toInt() > 0)
        for(int i = x + 15; i < x + width; i += 30)
        {
            painter.drawLine(i, centerY, i - scaleElement / 3 ,centerY + scaleElement/3);
            painter.drawLine(i, centerY, i - scaleElement / 3 ,centerY - scaleElement/3);
            qDebug("x %d", i);
        }
    else
        for(int i = x + 15; i < x + width; i += 30)
        {
            painter.drawLine(i, centerY, i + scaleElement / 3 ,centerY + scaleElement/3);
            painter.drawLine(i, centerY, i + scaleElement / 3 ,centerY - scaleElement/3);
        }
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
}

inline void paintF(QPainter &painter, int x, int centerY, QString f)
{
    painter.setPen  (QPen  (Qt::red, 3, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));

    QPainterPath force;

    if (f.toInt() > 0)
    {
        force.addRect(x, centerY - scaleElement/4, scaleElement/2, scaleElement/2);
        force.moveTo(x + scaleElement/2, centerY);
        force.lineTo(x + scaleElement/2, centerY - scaleElement/2);
        force.lineTo(x + scaleElement, centerY);
        force.lineTo(x + scaleElement/2, centerY + scaleElement/2);
        force.lineTo(x + scaleElement/2, centerY);

        painter.drawPath(force);
    }
    else
    {
        force.addRect(x - scaleElement/2, centerY - scaleElement/4, scaleElement/2, scaleElement/2);
        force.moveTo (x - scaleElement/2, centerY);
        force.lineTo (x - scaleElement/2, centerY - scaleElement/2);
        force.lineTo (x - scaleElement, centerY);
        force.lineTo (x - scaleElement/2, centerY + scaleElement/2);
        force.lineTo (x - scaleElement/2, centerY);

        painter.drawPath(force);
    }
    painter.drawText(x+15, centerY+25, f + "f");

    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
}

inline void paintBase(QPainter &painter, int x, int centerY, bool base) //base, 0 - left, 1 - right
{
    QPainterPath path;

    if (!base)
    {
        path.moveTo(x, centerY);
        path.lineTo(x, centerY - scaleElement * 2);
        path.lineTo(x, centerY + scaleElement * 2);
        painter.drawPath(path);

    }
    else
    {
        path.moveTo(x, centerY);
        path.lineTo(x, centerY - scaleElement * 2);
        path.lineTo(x, centerY + scaleElement * 2);
        painter.drawPath(path);

    }
}

void MainWindow::paintEvent(QPaintEvent* )
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));


    if (getDrawState() && validation())
    {
        qreal lastHeight = 0;

        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
        {
            qreal width  = ui->tableWidget->item(i, L)->text().toDouble() * scaleElement;
            qreal height = ui->tableWidget->item(i, A)->text().toDouble() * scaleElement;

            paintElement(painter, startX + lastHeight, startY, width, height);

            if (ui->tableWidget->item(i, q)->text().toInt() != 0)
                paintQ(painter, startX + lastHeight, startY, width, ui->tableWidget->item(i, q)->text());

            if (ui->tableWidget->item(i, F)->text().toInt() != 0)
                paintF(painter, startX + lastHeight, startY, ui->tableWidget->item(i, F)->text());

            lastHeight += ui->tableWidget->item(i, L)->text().toDouble() * scaleElement;

            if (getLeftBase())
            {
                qDebug("left");
                paintBase(painter, startX, startY, 0);
            }
//            qDebug("i - %d", i);
            if (getRightBase() && i == ui->tableWidget->rowCount() - 1)
            {
                paintBase(painter, startX + lastHeight, startY, 1);
                qDebug("riGHT");
            }
            update();
        };

        lastHeight = 0;
    }
}

void MainWindow::removeAll()
{
    for(int i = ui->tableWidget->rowCount()-1; i >= 0; i--){
        ui->tableWidget->removeRow(i);
    }
}

