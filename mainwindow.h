#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <unordered_map>

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
bool Ux        = 0;
bool Gx        = 0;
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

    QVector <QPair<int, double>>  NxMap;
    QVector <QPair<int, double>>  UxMap;
    QVector <QPair<int, double>>  GxMap;
                                                                          // tableName -> N(x) - 0, U(x) - 1, G(x) - 2
    void showGraph(QPainter &painter, QVector <QPair<int, double>>  &Map, qreal startX, qreal startY, short tableName);
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
    bool getUx       ();
    bool getGx       ();

    double    getValue    (int element, int type);

private:
    void setDrawState(bool state);
    void setLeftBase (bool state);
    void setRightBase(bool state);
    void setNx       (bool state);
    void setUx       (bool state);
    void setGx       (bool state);
};
#endif // MAINWINDOW_H
