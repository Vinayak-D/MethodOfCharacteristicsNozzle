#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->MeField->setEnabled(false);
    ui->HField->setEnabled(false);
    ui->PcField->setEnabled(false);
    ui->TcField->setEnabled(false);
    ui->GamField->setEnabled(false);
    ui->GamField2->setEnabled(false);
    ui->RField->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_allParamsOption_toggled(bool checked)
{
    if (checked){
        ui->MeField->setEnabled(false);
        ui->GamField2->setEnabled(false);
        ui->HField->setEnabled(true);
        ui->PcField->setEnabled(true);
        ui->TcField->setEnabled(true);
        ui->GamField->setEnabled(true);
        ui->RField->setEnabled(true);
    }
    else{
        ui->MeField->setEnabled(true);
        ui->GamField2->setEnabled(true);
    }
}

void MainWindow::on_exitMachOption_toggled(bool checked)
{
    if (checked){
        ui->MeField->setEnabled(true);
        ui->GamField2->setEnabled(true);
        ui->HField->setEnabled(false);
        ui->PcField->setEnabled(false);
        ui->TcField->setEnabled(false);
        ui->GamField->setEnabled(false);
        ui->RField->setEnabled(false);
    }
    else{
        ui->MeField->setEnabled(true);
        ui->GamField2->setEnabled(true);
    }
}

void MainWindow::on_pushButton_clicked()
{
    std::string option;
    if (ui->allParamsOption->isChecked()){
        Engine.chamber_pressure = ui->PcField->text().toFloat();
        Engine.chamber_temperature = ui->TcField->text().toFloat();
        Engine.gamma = ui->GamField->text().toFloat();
        Engine.R_gas = ui->RField->text().toFloat();
        float altitude = ui->HField->text().toFloat();
        std::pair<float,float> tP = eq.returnOutsideTemperatureAndPressure(altitude);
        Engine.exit_pressure = tP.second;
        Engine.exit_Mach = eq.getExitMach(Engine.chamber_temperature, Engine.chamber_pressure, Engine.exit_pressure, Engine.gamma, Engine.R_gas);
        option = "one";
    }
    else if (ui->exitMachOption->isChecked()){
        try{
            Engine.exit_Mach = ui->MeField->text().toFloat();
            Engine.gamma = ui->GamField2->text().toFloat();
        }
        catch (float defaultMe) {
            defaultMe = 2.0;
            Engine.exit_Mach = defaultMe;
            option = "two";
        }
    }
    this->close();
    //for widgets, setting parent argument to 0 opens widget in a new window
    mocdesign = new methodofcharacteristics(0, Engine, option);
    mocdesign->show();
}

