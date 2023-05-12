#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "src/engine.h"
#include "src/equations.h"
#include "methodofcharacteristics.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_allParamsOption_toggled(bool checked);
    void on_exitMachOption_toggled(bool checked);
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    methodofcharacteristics *mocdesign;
    engine Engine;
    equations eq;
};
#endif // MAINWINDOW_H
