#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QPainter;
class QJsonObject;




struct state
{

bool drawState = 0;
bool leftBase  = 0;
bool rightBase = 0;

bool Nx        = 0;
};

enum tableNames
{
    L,
    A,
    E,
    sigma,
    q,
    F
};



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void open();
    void write();
    void fromJsonToTable(QJsonObject &obj);
    void fromTxtToTable();

    bool validation();

    void add();
    void remove();
    void removeAll();

    void paintEvent(QPaintEvent* );
    inline void paintQ(QPainter &painter, int x, int centerY, qreal width, QString q);

    QMap<quint32, double> NxMap;

    void showNx(QPainter &painter, qreal startX, qreal startY);
    Ui::MainWindow *ui;

    state wigetState;
    float offsetX;

    void calc();
    QVector <double> gauss(QVector<QVector<double>> matrixA, QVector<double> matrixB);

    void initRow();

private:
    bool getDrawState();
    bool getLeftBase ();
    bool getRightBase();
    bool getNx       ();

    double    getValue    (int element, int type);

private:
    void setDrawState(bool state);
    void setLeftBase (bool state);
    void setRightBase(bool state);
    void setNx       (bool state);
};
#endif // MAINWINDOW_H
