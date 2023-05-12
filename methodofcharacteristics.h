#ifndef METHODOFCHARACTERISTICS_H
#define METHODOFCHARACTERISTICS_H
#include "src/engine.h"
#include "src/equations.h"
#include <QWidget>
#include "src/qcustomplot.h"
#include <QFile>
#include <QTextStream>

namespace Ui {
class methodofcharacteristics;
}

class methodofcharacteristics : public QWidget
{
    Q_OBJECT

public:
    explicit methodofcharacteristics(QWidget *parent = nullptr, engine Engine = engine(), std::string option = "none");
    ~methodofcharacteristics();

private slots:
    void on_pushButton_clicked();
    void plotNozzlePoints(QCustomPlot *customPlot);
    void outputToFile();

private:
    Ui::methodofcharacteristics *ui;
    engine EngineCopy;
    equations eq;
};

#endif // METHODOFCHARACTERISTICS_H
