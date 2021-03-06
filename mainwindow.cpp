#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QString>
#include <QVector>
#include <unordered_map>

#include <QInputDialog>
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QDesktopServices>
#include <QProcess>

namespace  {
    const  int scaleElement = 30 ;
    static int startX       = 300;
    const  int startY       = 250;

    const  int graphOffset  =  50 ;
    const  int graphYLenght = 350 ;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    ui->tableWidget   ->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->resultWidget  ->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->UXresultWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->GxresultWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->fTable        ->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QObject::connect(ui->open,         &QAction::triggered, this, [this](){open()                     ;});
    QObject::connect(ui->write,        &QAction::triggered, this, [this](){write()                    ;});

    QObject::connect(ui->add,          &QAction::triggered, this, [this](){add()                        ;});
    QObject::connect(ui->remove,       &QAction::triggered, this, [this](){remove()                     ;});
    QObject::connect(ui->removeAll,    &QAction::triggered, this, [this](){ui->tableWidget->clear()     ;});
    QObject::connect(ui->about,        &QAction::triggered, this, [this](){about()                      ;});
    QObject::connect(ui->newProject,   &QAction::triggered, this, [this](){newProject()                 ;});
    QObject::connect(ui->exit,         &QAction::triggered, this, [this](){qApp->quit()                 ;});


    QObject::connect(ui->showElement,  &QCheckBox::stateChanged, this, [this](){setDrawState(!getDrawState());});
    QObject::connect(ui->baseLeft,     &QCheckBox::stateChanged, this, [this](){setLeftBase (!getLeftBase ());});
    QObject::connect(ui->baseRight,    &QCheckBox::stateChanged, this, [this](){setRightBase(!getRightBase());});
    QObject::connect(ui->Nx,           &QCheckBox::stateChanged, this, [this](){setNx       (!getNx       ());});
    QObject::connect(ui->Ux,           &QCheckBox::stateChanged, this, [this](){setUx       (!getUx       ());});
    QObject::connect(ui->Gx,           &QCheckBox::stateChanged, this, [this](){setGx       (!getGx       ());});

    QObject::connect(ui->Calc,         &QPushButton::clicked,    this, [this](){calc();});

    QObject::connect(ui->horizontalScrollBar,         &QAbstractSlider::valueChanged,    this, [this](){offsetX = -ui->horizontalScrollBar->sliderPosition() * 10;});


}

void MainWindow::about()
{

    QDesktopServices::openUrl(QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/about/about.html"));
    qDebug("%s", qPrintable(QDir::currentPath()));
}

void MainWindow::newProject()
{
    QProcess process;
    process.startDetached("SAPR",QStringList());

    qApp->quit();
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
    QStringList list = str.split(" ");
    int i = 0;

    for (auto &it:list)
    {
            if (i==0)
                add();


            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(it);

            if (i != 5)
                ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, i, item);
            else     ui->fTable->setItem(ui->tableWidget->rowCount()-1,0,item);
            i++;

            if (i == 6)
            {
                i = 0;
            }
    }
    file.close();
    ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);
    ui->fTable->removeRow(ui->fTable->rowCount()-1);
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
     stream << ui->fTable->item(i,0)->text().toUtf8();
 }
stream << " ";
stream << ui->fTable->item(ui->fTable->rowCount() , 0)->text().toUtf8();
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

void MainWindow::setNx(bool state)
{
    wigetState.Nx = state;
}

void MainWindow::setUx(bool state)
{
    wigetState.Ux = state;
}

void MainWindow::setGx(bool state)
{
    wigetState.Gx = state;
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

bool MainWindow::getNx()
{
    return wigetState.Nx;
}

bool MainWindow::getUx()
{
    return wigetState.Ux;
}

bool MainWindow::getGx()
{
    return wigetState.Gx;
}

double MainWindow::getValue (int element, int type)
{

    if (type != F)
    {
    double data = ui->tableWidget->item(element,type)->text().toDouble();
    return data;
    }
    else
    {
        double data = ui->fTable->item(element,0)->text().toDouble();
        return data;
    }
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
        ui->tableWidget->item(ui->tableWidget->rowCount()-1, i)->setText("0");
}

void MainWindow::add()
{

    ui->tableWidget->insertRow(ui->tableWidget->rowCount());


    QString data = "0";

    if (ui->tableWidget->rowCount() == 1)
    {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText("0");
        ui->fTable->insertRow(ui->fTable->rowCount());
        ui->fTable->setItem(ui->tableWidget->rowCount()-1,0, item);
    }

    ui->fTable->insertRow(ui->fTable->rowCount());
    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setText("0");
    ui->fTable->setItem(ui->fTable->rowCount()-1,0, item1);

    for (int i = 0; i < ui->tableWidget->columnCount(); i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText("0");
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,i, item);
    }




}

void MainWindow::remove()
{
    int num = QInputDialog::getInt(this, "?????????????? ?????????? ??????????????", "?????????? ??????????????", 1, 1, ui->tableWidget->rowCount());
    ui->tableWidget->removeRow(num - 1);
    ui->fTable->removeRow(num-1);
}

inline void paintElement(QPainter &painter, int x, int centerY, qreal width, qreal height)
{
    painter.drawRect(x, centerY - height/2, width, height);
}

inline void MainWindow::paintQ(QPainter &painter, int x, int centerY, qreal width, QString q)
{
    painter.setPen(QPen(Qt::blue, 3, Qt::SolidLine, Qt::FlatCap));

    painter.drawLine(x, centerY , x + width, centerY);
    painter.drawText(x + width/2, centerY + 10, q + "q");

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
        force.addRect(x, centerY - scaleElement/8, scaleElement/4, scaleElement/4);
        force.moveTo(x + scaleElement/4, centerY);
        force.lineTo(x + scaleElement/4, centerY - scaleElement/4);
        force.lineTo(x + scaleElement/2, centerY);
        force.lineTo(x + scaleElement/4, centerY + scaleElement/4);
        force.lineTo(x + scaleElement/4, centerY);

        painter.drawPath(force);
    }
    else
    {
        force.addRect(x - scaleElement/4, centerY - scaleElement/8, scaleElement/8, scaleElement/8);
        force.moveTo (x - scaleElement/4, centerY);
        force.lineTo (x - scaleElement/4, centerY - scaleElement/4);
        force.lineTo (x - scaleElement/2, centerY);
        force.lineTo (x - scaleElement/4, centerY + scaleElement/4);
        force.lineTo (x - scaleElement/4, centerY);

        painter.drawPath(force);
    }
    painter.drawText(x + 5, centerY + 20, f + "F");

    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
}

inline void paintBase(QPainter &painter, int x, int centerY, bool base) //base, 0 - left, 1 - right
{
    QPainterPath path;
    painter.setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
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
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
}

inline void MainWindow::showGraph(QPainter &painter, QVector <QPair<int, double>>  &Map, qreal startX, qreal startY)
{
    short max = -4001;
    short min = 4001;

    float scaleK = 1;

    for (auto it = Map.begin(); it != Map.end(); it++)
    {
        if (it->second > max) max = it->second;
        if (it->second < min) min = it->second;
    }

    if (max < 2 || min > -2)
        scaleK = 2;
    if (max > 10 || min < -10)
        scaleK = 0.5;
    if (max > 30 || min < -30)
        scaleK = 0.25;

    for (auto it = Map.begin(); it != Map.end(); it++)
    {
        painter.drawLine(startX + offsetX + it->first * scaleElement, startY + graphOffset, startX + offsetX + it->first * scaleElement, startY + graphYLenght);
        painter.drawLine(startX + offsetX, startY + (graphYLenght+graphOffset)/2, startX + offsetX + it->first * scaleElement, startY + (graphYLenght+graphOffset)/2);
    }

    painter.setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::FlatCap));
    for (auto it = Map.begin(); it != Map.end()-1; it++)
        painter.drawLine(startX + offsetX + it->first * scaleElement, startY + (graphYLenght+graphOffset)/2 -  it->second*10*scaleK, startX + offsetX + std::next(it)->first * scaleElement, startY + (graphYLenght+graphOffset)/2 - std::next(it)->second * 10 * scaleK);

    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
}

inline void MainWindow::showGraphUx(QPainter &painter, QVector <QPair<double, double>>  &Map, qreal startX, qreal startY)
{

    short max = -4001;
    short min = 4001;

    float scaleK = 1;

    for (auto it = Map.begin(); it != Map.end(); it++)
    {
        if (it->second > max) max = it->second;
        if (it->second < min) min = it->second;
    }

    if (max < 2 || min > -2)
        scaleK = 2;
    if (max > 10 || min < -10)
        scaleK = 0.5;
    if (max > 30 || min < -30)
        scaleK = 0.25;

    for (auto it = NxMap.begin(); it != NxMap.end(); it++)
    {
        painter.drawLine(startX + offsetX + it->first * scaleElement, startY + graphOffset, startX + offsetX + it->first * scaleElement, startY + graphYLenght);
        painter.drawLine(startX + offsetX, startY + (graphYLenght+graphOffset)/2, startX + offsetX + it->first * scaleElement, startY + (graphYLenght+graphOffset)/2);
    }

    painter.setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::FlatCap));

    for (int i = 0; i != Map.size(); i++)
        painter.drawPoint(startX + offsetX + Map[i].first * scaleElement,(graphYLenght+graphOffset)/2 + startY - Map[i].second * scaleElement * scaleK);

    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
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

            paintElement(painter, startX + offsetX + lastHeight, startY, width, height);

            if (ui->tableWidget->item(i, q)->text().toInt() != 0)
                paintQ(painter, startX + offsetX + lastHeight, startY, width, ui->tableWidget->item(i, q)->text());

            if (ui->tableWidget->rowCount() == 1)
            {

                lastHeight += ui->tableWidget->item(i, L)->text().toDouble() * scaleElement;

                if (getLeftBase() && !getRightBase() && getValue(i + 1, F) != 0) //???????????? ?????????? ??????????????
                    paintF(painter, startX + offsetX + lastHeight, startY, QString::number(getValue(i+1, F)));

                if (!getLeftBase() && getRightBase() && getValue(i, F) != 0) //???????????? ???????????? ??????????????
                    paintF(painter, startX + offsetX , startY, QString::number(getValue(i, F)));
            } else
                {
                    if (getValue(i, F) != 0)
                    {
                    if (!getLeftBase() && i == 0)
                        paintF(painter, startX + offsetX + lastHeight , startY, QString::number(getValue(i, F)));

                    if (!getRightBase() && i+1 == ui->fTable->rowCount()-1)
                        paintF(painter, startX + offsetX + lastHeight , startY, QString::number(getValue(i, F)));

                    if (i != 0 && i!= ui->fTable->rowCount()-1)
                        paintF(painter, startX + offsetX + lastHeight, startY, QString::number(getValue(i, F)));
                    }

                    lastHeight += ui->tableWidget->item(i, L)->text().toDouble() * scaleElement;
                }





            if (getLeftBase())
                paintBase(painter, startX + offsetX, startY, 0);

            if (getRightBase() && i == ui->tableWidget->rowCount() - 1)
                paintBase(painter, startX + offsetX + lastHeight, startY, 1);


            update();
        };



        lastHeight = 0;
    }

    if(getNx() && validation()) showGraph(painter, NxMap, startX, startY);
    if(getUx() && validation()) showGraphUx(painter, UxMap, startX, startY);
    if(getGx() && validation()) showGraph(painter, GxMap, startX, startY);
}

void MainWindow::removeAll()
{
    for(int i = ui->tableWidget->rowCount()-1; i >= 0; i--){
        ui->tableWidget->removeRow(i);
    }
}


// ???????????????????????????? ?? ???????????????????????? ?? https://prog-cpp.ru/gauss/
QVector<double> MainWindow::gauss(QVector<QVector<double>> matrixA, QVector<double> matrixB){

    QVector<double> result;
       int k = 0;
       int index;
       double maxValue;
       result.resize(matrixA.size());

       for(int i = 0; i < result.size(); i++)
           result[i] = 0;

       // ???????????? ??????
       while(k < matrixA.size()){
           maxValue = abs(matrixA[k][k]);
           index = k;

           for(int i = k + 1; i < matrixA.size();i++)
               if(abs(matrixA[i][k]) > maxValue){
                   maxValue = abs(matrixA[i][k]);
                   index = i;
               }
           if(maxValue < 0.00001){
               QMessageBox::warning(this,"????????????!","???????????????????? ???????????????? ?????????????? ????-???? ???????????????? ?????????????? " + QString::number(index) + " ?????????????? A");
               result.clear();
               return result;
           }
           // ?????????? X[i]
           for(int j = 0; j < matrixA.size(); j++){
               double temp = matrixA[k][j];
               matrixA[k][j] = matrixA[index][j];
               matrixA[index][j] = temp;
           }
           // ?????????? Y
           double temp = matrixB[k];
           matrixB[k] = matrixB[index];
           matrixB[index] = temp;

           for(int i = k; i < matrixA.size(); i++){
               double temp = matrixA[i][k];
               if(abs(temp) < 0.00001) continue;
               for(int j = 0; j < matrixA.size(); j++)
                   matrixA[i][j] /= temp;
               matrixB[i] /= temp;
               if(i == k) continue;
               for(int j = 0; j < matrixA.size();j++)
                   matrixA[i][j] -= matrixA[k][j];
               matrixB[i] -= matrixB[k];
           }
           k++;
       }

       // ???????????????? ??????
       for(k = matrixA.size()-1; k >= 0; k--){
           result[k] = matrixB[k];
           for(int i = 0; i < k; i++)
               matrixB[i] -= matrixA[i][k]*result[k];
       }

       return result;
}

void MainWindow::mapToTable(QVector <QPair<int, double>>  &Map, char tableName)
{
    for (auto it = Map.begin(); it != Map.end(); it++)
    {
    QTableWidgetItem *item = new QTableWidgetItem;
    QTableWidgetItem *item1 = new QTableWidgetItem;

    item->setText(QString::number(it->first));
    item1->setText(QString::number(it->second));

    switch (tableName) {
    case 0: {
            ui->resultWidget->insertRow(ui->resultWidget->rowCount());
            ui->resultWidget->setItem(ui->resultWidget->rowCount()-1,1, item1);
            ui->resultWidget->setItem(ui->resultWidget->rowCount()-1,0, item);
            }

    case 1: {
            ui->UXresultWidget->insertRow(ui->UXresultWidget->rowCount());
            ui->UXresultWidget->setItem(ui->UXresultWidget->rowCount()-1,1, item1);
            ui->UXresultWidget->setItem(ui->UXresultWidget->rowCount()-1,0, item);
            }

    case 2: {
            ui->GxresultWidget->insertRow(ui->GxresultWidget->rowCount());
            ui->GxresultWidget->setItem(ui->GxresultWidget->rowCount()-1,1, item1);
            ui->GxresultWidget->setItem(ui->GxresultWidget->rowCount()-1,0, item);
            }


    }
    }
//    ui->GxresultWidget->removeRow(0);
//    ui->GxresultWidget->removeRow(0);
}

void MainWindow::mapToTable(QVector <QPair<double, double>>  &Map, char tableName)
{
    for (auto it = Map.begin(); it != Map.end(); it++)
    {
    QTableWidgetItem *item = new QTableWidgetItem;
    QTableWidgetItem *item1 = new QTableWidgetItem;

    item->setText(QString::number(it->first));
    item1->setText(QString::number(it->second));

    switch (tableName) {
    case 0: {
            ui->resultWidget->insertRow(ui->resultWidget->rowCount());
            ui->resultWidget->setItem(ui->resultWidget->rowCount()-1,1, item1);
            ui->resultWidget->setItem(ui->resultWidget->rowCount()-1,0, item);
            }

    case 1: {
            ui->UXresultWidget->insertRow(ui->UXresultWidget->rowCount());
            ui->UXresultWidget->setItem(ui->UXresultWidget->rowCount()-1,1, item1);
            ui->UXresultWidget->setItem(ui->UXresultWidget->rowCount()-1,0, item);
            }

    case 2: {
            ui->GxresultWidget->insertRow(ui->GxresultWidget->rowCount());
            ui->GxresultWidget->setItem(ui->GxresultWidget->rowCount()-1,1, item1);
            ui->GxresultWidget->setItem(ui->GxresultWidget->rowCount()-1,0, item);
            }


    }
    }

    ui->UXresultWidget->removeRow(0);
    ui->UXresultWidget->removeRow(0);
}

void MainWindow::calc()
{
    if(validation() && ui->tableWidget->rowCount() != 0)
    {

    short size = ui->tableWidget->rowCount() + 1;

    QVector <QVector <double>> matrixA(size);
    QVector <double>           matrixB(size);

    QVector <float> EAM;

    for (int i = 0; i < size - 1; i++)
        EAM.push_back((ui->tableWidget->item(i,E)->text().toDouble() * ui->tableWidget->item(i,A)->text().toDouble())/ui->tableWidget->item(i,L)->text().toDouble());

    short left  = -1       ; // ?????????? ???? ?????????? ????????

    for (int i = 0; i < size; i++, left++) //?????????????????? ?????????????? ??
    {
          if (i == 0 )  matrixA[i].push_back(EAM[i]);
          if (i == 0 )  matrixA[i].push_back(-EAM[i]);
          if (i == 0 ) for (short j = 2; j < size; j++) matrixA[i].push_back(0);

          if (i > 0 && i < size-1)
          {
              for (short j = 0; j < left; j++)
                  matrixA[i].push_back(0);

              matrixA[i].push_back(-EAM[i-1]         );
              matrixA[i].push_back( EAM[i-1] + EAM[i]);
              matrixA[i].push_back(-EAM[i  ]         );

              for (short j = left + 3; j < size; j++)
                  matrixA[i].push_back(0);
         }

          if (i == size-1 ) for (short j = 0; j < size - 2; j++) matrixA[i].push_back(0);

          if (i == size-1 )  matrixA[i].push_back(-EAM[i - 1]);
          if (i == size-1 )  matrixA[i].push_back( EAM[i - 1]);

     }

    for (short i = 0; i < size; i++) //?????????????? B
    {
        if (i == 0)
        {
            if (getLeftBase())
                matrixB[i] = 0;
            else
                matrixB[i] = getValue(i, L) * getValue(i, q) / 2 + getValue(i, F);

        }
        if (i != 0 && i != size - 1) matrixB[i] = getValue(i,L) * getValue(i,q) / 2
                + getValue(i, F) + getValue(i-1, q) * getValue(i-1, L) / 2;

        if (i == size - 1 && getRightBase())
            matrixB[i] = 0;
         else
               if (!getRightBase() && size == 2 && i != 0)
                   matrixB[i] = getValue(i - 1, L) * getValue(i - 1, q) / 2 + getValue(i, F);




        printf(":%f ", matrixB[i]);
    } //cde0f1f2ff20ff20f2e5e1ff20ebfee1ebfe

    for (short i = 0; i < size; i++)
    {
        if (i == 0 && getLeftBase()) matrixA[i][0] = 1;
        if (i == 0 && getLeftBase()) matrixA[i][1] = 0;

        if (getLeftBase()  && i!= size-1 && i!= 0) matrixA[i][0] = 0;
        if (getRightBase() && i!= size-1 && i!= 0) matrixA[i][size-1] = 0;

        if (i == size-1 && getRightBase()) matrixA[i][size-1] = 1;
        if (i == size-1 && getRightBase()) matrixA[i][size-2] = 0;
    }
    QVector <double> resultVector = gauss(matrixA, matrixB);

    qDebug("matrixA:");
    for (int i = 0; i < matrixA.size(); i++)
        qDebug() << matrixA[i];

    qDebug("matrixB:");
    for (int i = 0; i < matrixB.size(); i++)
        qDebug() << matrixB[i];

    QVector <QPair<int, double>> resultMap;

    qDebug("gaussMAtrix:");
    for (int i = 0; i < resultVector.size(); i++)
        qDebug("%f", resultVector[i]);

    double numberL = 0;
    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        numberL += getValue(i, L);
    }

    short LCounter = 0;
    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        for (int  j = 0; j <= getValue(i, L); j++)
        {
        double N1 = (getValue(i,A)/getValue(i,L))*(resultVector[i+1] - resultVector[i]) +  getValue(i,q) * getValue(i,L) / 2 * (1 - 2 * (j/getValue(i,L)));
        resultMap.push_back({LCounter, N1});
        if (j != getValue(i, L)) LCounter += 1;
        }

    }
    NxMap = resultMap;

    int j = 0;
    int x = 0;
    for (int i = 0; i < NxMap.size(); i++)
    {
        GxMap.push_back({NxMap[i].first, (NxMap[i].second / ui->tableWidget->item(j, A)->text().toDouble())});
        x++;
        if (x == getValue(j, L)+1)
        {
            j++;
            x = 0;
        }
    }

    double currentL = 0;
    for (int element = 0; element < ui->tableWidget->rowCount(); element++)
    {
        double currentX = 0;
        double beginElement = resultVector[element];
        double endElement   = resultVector[element + 1];
        while(currentX <= getValue(element, L))
        {
          double currentY = beginElement + (currentX / getValue(element, L)) * (endElement - beginElement) +
                   getValue(element, q) * (getValue(element, L) * getValue(element, L)) / (2 * getValue(element, E) * getValue(element, A)) *
                  (currentX / getValue(element, L)) * (1 - (currentX/getValue(element, L)));
           UxMap.push_back({currentL, currentY});
           currentL += 0.001;
           currentX += 0.001;
        }
        currentX = 0;
    }

    mapToTable(NxMap, 0);
    mapToTable(UxMap, 1);
    mapToTable(GxMap, 2);

   }
}

