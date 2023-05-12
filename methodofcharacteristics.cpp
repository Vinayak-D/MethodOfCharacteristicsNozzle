#include "methodofcharacteristics.h"
#include "ui_methodofcharacteristics.h"

methodofcharacteristics::methodofcharacteristics(QWidget *parent, engine Engine, std::string option) :
    QWidget(parent),
    ui(new Ui::methodofcharacteristics)
{
    EngineCopy = Engine;
    ui->setupUi(this);
    ui->MeLabel->setText(QString::number(EngineCopy.exit_Mach));
    ui->GamLabel->setText(QString::number(EngineCopy.gamma));
    if (option == "one"){
        ui->PcLabel->setText(QString::number(EngineCopy.chamber_pressure));
        ui->PoLabel->setText(QString::number(EngineCopy.exit_pressure));
    }
}

methodofcharacteristics::~methodofcharacteristics()
{
    delete ui;
}

//design the nozzle
void methodofcharacteristics::on_pushButton_clicked()
{
    //reset
    EngineCopy.reset();

    //get number of points, initialize them
    EngineCopy.nLines = std::min(ui->nField->text().toInt(), 45);
    if (EngineCopy.nLines < 3){
        EngineCopy.nLines = 3;
    }
    EngineCopy.initializeCharacteristicPoints();
    //find max wall angle (deg)
    EngineCopy.max_wall_angle = eq.getPMAngle(EngineCopy.gamma, EngineCopy.exit_Mach)/2;
    //find wall angle divisions
    std::vector<float> thtDivisions = eq.interpolateAngle(EngineCopy.max_wall_angle, EngineCopy.nLines);
    //find wall and centerline points
    std::vector<int> wallPoints;
    std::vector<int> centerPoints;
    for (int i = 0; i < EngineCopy.nPoints; i++){
        if (EngineCopy.characteristicPoints[i].is_wall_location){
            wallPoints.push_back(EngineCopy.characteristicPoints[i].index);
        }
        if(EngineCopy.characteristicPoints[i].is_centerline_location){
            centerPoints.push_back(EngineCopy.characteristicPoints[i].index);
        }
    }

    //begin method of characteristics

    //conversions
    float r2d = 180/M_PI;
    float d2r = M_PI/180;

    //point a (throat)
    float xa = 0; //x location (cm)
    EngineCopy.nozzle_throat_radius = ui->RtField->text().toFloat(); //y location (cm)
    float tht_a = 0.19; //kickoff angle (deg)
    float pm_a = 0.19; //same as kickoff angle (deg)
    float M_a = 1.026; //Mach at a
    float slope_a = -75.23; //slope at a (deg)

    //point 1
    EngineCopy.characteristicPoints[0].x_location = EngineCopy.nozzle_throat_radius*tan((90+slope_a)*d2r);
    EngineCopy.characteristicPoints[0].pm_angle = tht_a + pm_a;
    EngineCopy.characteristicPoints[0].Mach = eq.getMachFromPM(M_a, EngineCopy.characteristicPoints[0].pm_angle, EngineCopy.gamma);
    EngineCopy.characteristicPoints[0].mu = asin(1/EngineCopy.characteristicPoints[0].Mach)*r2d;

    //points 2 to n+1, characteristicPoints[1] to cP[n] - the first reflection
    for (int i = 1; i <= EngineCopy.nLines; i++){
        //assign flow and pm angle (equal for first reflection)
        if (!EngineCopy.characteristicPoints[i].is_wall_location){
            EngineCopy.characteristicPoints[i].flow_angle = thtDivisions[i];
            EngineCopy.characteristicPoints[i].pm_angle = thtDivisions[i];
            EngineCopy.characteristicPoints[i].Mach = eq.getMachFromPM(EngineCopy.characteristicPoints[i-1].Mach, EngineCopy.characteristicPoints[i].pm_angle, EngineCopy.gamma);
            EngineCopy.characteristicPoints[i].mu = asin(1/EngineCopy.characteristicPoints[i].Mach)*r2d;
            //find theta_ax = nu_ax and the Mach angle
            float tv_ax = (2*(thtDivisions[i]) + (EngineCopy.characteristicPoints[i-1].flow_angle - EngineCopy.characteristicPoints[i-1].pm_angle))/2;
            float M_ax = eq.getMachFromPM(1.001, tv_ax, EngineCopy.gamma);
            float mu_ax = asin(1/M_ax)*r2d;
            //find left and right running characteristic slopes
            float tht_b = (EngineCopy.characteristicPoints[i-1].flow_angle + EngineCopy.characteristicPoints[i-1].mu + EngineCopy.characteristicPoints[i].flow_angle + EngineCopy.characteristicPoints[i].mu)/2;
            float tht_t = (tv_ax - mu_ax + EngineCopy.characteristicPoints[i].flow_angle - EngineCopy.characteristicPoints[i].mu)/2;
            //find xy location of point i
            std::pair<float, float> xy = eq.returnXYIntersectionPoint(xa, EngineCopy.nozzle_throat_radius, tht_t, EngineCopy.characteristicPoints[i-1].x_location, EngineCopy.characteristicPoints[i-1].y_location, tht_b);
            EngineCopy.characteristicPoints[i].x_location = xy.first;
            EngineCopy.characteristicPoints[i].y_location = xy.second;
        }
        //extend for the wall location
        else{
            EngineCopy.characteristicPoints[i].flow_angle = EngineCopy.characteristicPoints[i-1].flow_angle;
            EngineCopy.characteristicPoints[i].pm_angle = EngineCopy.characteristicPoints[i-1].pm_angle;
            EngineCopy.characteristicPoints[i].Mach = EngineCopy.characteristicPoints[i-1].Mach;
            EngineCopy.characteristicPoints[i].mu = EngineCopy.characteristicPoints[i-1].mu;
            //find left and second left running (both point up), the first left is the interpolated wall angle (max for first reflection)
            float tht_t = EngineCopy.max_wall_angle;
            float tht_b = (EngineCopy.characteristicPoints[i-1].flow_angle + EngineCopy.characteristicPoints[i-1].mu + EngineCopy.characteristicPoints[i].flow_angle + EngineCopy.characteristicPoints[i].mu)/2;
            //find xy location of the wall point
            std::pair<float, float> xy = eq.returnXYIntersectionPoint(xa, EngineCopy.nozzle_throat_radius, tht_t, EngineCopy.characteristicPoints[i-1].x_location, EngineCopy.characteristicPoints[i-1].y_location, tht_b);
            EngineCopy.characteristicPoints[i].x_location = xy.first;
            EngineCopy.characteristicPoints[i].y_location = xy.second;
        }
    }

    //remaining points
    int j = 0;
    int k = 1;
    for (int i = EngineCopy.nLines + 1; i < EngineCopy.nPoints; i++){
        if (EngineCopy.characteristicPoints[i].is_centerline_location){
            EngineCopy.characteristicPoints[i].pm_angle = EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].flow_angle + EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].pm_angle;
            EngineCopy.characteristicPoints[i].Mach = eq.getMachFromPM(EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].Mach, EngineCopy.characteristicPoints[i].pm_angle, EngineCopy.gamma);
            EngineCopy.characteristicPoints[i].mu = asin(1/EngineCopy.characteristicPoints[i].Mach)*r2d;
            float tht_t = (EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].flow_angle - EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].mu + EngineCopy.characteristicPoints[i].flow_angle - EngineCopy.characteristicPoints[i].mu)/2;
            float tht_b = 0;
            //find xy location of centerline point
            std::pair<float, float> xy = eq.returnXYIntersectionPoint(EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].x_location, EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].y_location, tht_t, EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j+1)].x_location, EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j+1)].y_location, tht_b);
            EngineCopy.characteristicPoints[i].x_location = xy.first;
            EngineCopy.characteristicPoints[i].y_location = xy.second;
        }
        if (!EngineCopy.characteristicPoints[i].is_centerline_location && !EngineCopy.characteristicPoints[i].is_wall_location){
            EngineCopy.characteristicPoints[i].flow_angle = thtDivisions[k];
            EngineCopy.characteristicPoints[i].pm_angle = (EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].flow_angle + EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].pm_angle - (EngineCopy.characteristicPoints[i-1].flow_angle - EngineCopy.characteristicPoints[i-1].pm_angle))/2;
            EngineCopy.characteristicPoints[i].Mach = eq.getMachFromPM(EngineCopy.characteristicPoints[i-1].Mach, EngineCopy.characteristicPoints[i].pm_angle, EngineCopy.gamma);
            EngineCopy.characteristicPoints[i].mu = asin(1/EngineCopy.characteristicPoints[i].Mach)*r2d;
            float tht_t = (EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].flow_angle - EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].mu + EngineCopy.characteristicPoints[i].flow_angle - EngineCopy.characteristicPoints[i].mu)/2;
            float tht_b = (EngineCopy.characteristicPoints[i-1].flow_angle + EngineCopy.characteristicPoints[i-1].mu + EngineCopy.characteristicPoints[i].flow_angle + EngineCopy.characteristicPoints[i].mu)/2;
            //find xy location of point
            std::pair<float, float> xy = eq.returnXYIntersectionPoint(EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].x_location, EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].y_location, tht_t, EngineCopy.characteristicPoints[i-1].x_location, EngineCopy.characteristicPoints[i-1].y_location, tht_b);
            EngineCopy.characteristicPoints[i].x_location = xy.first;
            EngineCopy.characteristicPoints[i].y_location = xy.second;
            k += 1;
        }
        if (EngineCopy.characteristicPoints[i].is_wall_location){
            EngineCopy.characteristicPoints[i].flow_angle = EngineCopy.characteristicPoints[i-1].flow_angle;
            EngineCopy.characteristicPoints[i].pm_angle = EngineCopy.characteristicPoints[i-1].pm_angle;
            EngineCopy.characteristicPoints[i].Mach = EngineCopy.characteristicPoints[i-1].Mach;
            EngineCopy.characteristicPoints[i].mu = EngineCopy.characteristicPoints[i-1].mu;
            float tht_t = EngineCopy.characteristicPoints[i-1].flow_angle;
            float tht_b = (EngineCopy.characteristicPoints[i-1].flow_angle + EngineCopy.characteristicPoints[i-1].mu + EngineCopy.characteristicPoints[i].flow_angle + EngineCopy.characteristicPoints[i].mu)/2;
            //find xy location of the wall point
            std::pair<float, float> xy = eq.returnXYIntersectionPoint(EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].x_location, EngineCopy.characteristicPoints[i-(EngineCopy.nLines-j)].y_location, tht_t, EngineCopy.characteristicPoints[i-1].x_location, EngineCopy.characteristicPoints[i-1].y_location, tht_b);
            EngineCopy.characteristicPoints[i].x_location = xy.first;
            EngineCopy.characteristicPoints[i].y_location = xy.second;
            k = 1;
            j += 1;
        }
    }

    //plot all points on widget
    plotNozzlePoints(ui->customPlot);

    //output to file
    outputToFile();

    //update labels
    ui->nPointsLabel->setText(QString::number(EngineCopy.nPoints));
    ui->thetaMaxLabel->setText(QString::number(EngineCopy.max_wall_angle));
    ui->thetaDivLabel->setText(QString::fromStdString(eq.vectorToString(thtDivisions)));
    ui->centerPointsLabel->setText(QString::fromStdString(eq.vectorToString(centerPoints)));
    ui->wallPointsLabel->setText(QString::fromStdString(eq.vectorToString(wallPoints)));
    ui->lengthLabel->setText(QString::number(EngineCopy.characteristicPoints[EngineCopy.nPoints-1].x_location));
    float aR = (pow(EngineCopy.characteristicPoints[EngineCopy.nPoints-1].y_location,2))/(pow(EngineCopy.nozzle_throat_radius,2));
    ui->areaRatioLabel->setText(QString::number(aR));
}

//plot all nozzle points
void methodofcharacteristics::plotNozzlePoints(QCustomPlot *customPlot){

    QVector<double> x(EngineCopy.nPoints), y(EngineCopy.nPoints); // initialize with entries 0..100
    x[0] = 0;
    y[0] = EngineCopy.nozzle_throat_radius;
    for (int i = 1; i < EngineCopy.nPoints; i++)
    {
      x[i] = EngineCopy.characteristicPoints[i].x_location; // x goes from -1 to 1
      y[i] = EngineCopy.characteristicPoints[i].y_location; // let's plot a quadratic function
    }
    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QColor(50, 50, 50, 255));
    customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
    customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    customPlot->xAxis->setLabel("X-Axis");
    customPlot->yAxis->setLabel("Y-Axis");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(0, EngineCopy.characteristicPoints[EngineCopy.nPoints-1].x_location + 45);
    customPlot->yAxis->setRange(-10, EngineCopy.characteristicPoints[EngineCopy.nPoints-1].y_location + 15);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    customPlot->replot();
}

//output to file
void methodofcharacteristics::outputToFile(){
    //change "/Users/vinayakdeshpande/Desktop/" in below line to your desired output directory
    QFile File ("C/Users/vinay/Desktop/nozzlePoints.txt");
    if (File.open(QIODevice::ReadWrite)){
        QTextStream stream (&File);
        stream << "Index" << "\t\t" << "X" << "\t\t" << "Y" << "\t\t" << "Mach" << "\t\t" << "FlowAngle" << "\t" << "P-MAngle" << "\n";
        for (int i = 0; i < EngineCopy.nPoints; i++){
            stream << EngineCopy.characteristicPoints[i].index << "\t\t" << EngineCopy.characteristicPoints[i].x_location << "\t\t" << EngineCopy.characteristicPoints[i].y_location << "\t\t" << EngineCopy.characteristicPoints[i].Mach << "\t\t" << EngineCopy.characteristicPoints[i].flow_angle << "\t\t" << EngineCopy.characteristicPoints[i].pm_angle << "\n";
        }
    }
}

