#include "engine.h"

//constructors
engine::engine(){}

//methods
void engine::initializeCharacteristicPoints(){
    //initialize empty array of characteristic points
    nPoints = nLines + nLines*(nLines+1)/2;
    int j = 1 + nLines;
    int k = 0;
    for (int i = 1; i <= nPoints; i++){
        cPoint point(i);
        //set wall points accordingly
        if (i == j + k){
            point.is_wall_location = true;
            k = k + 1;        
            j = j + nLines - k;
        }
        characteristicPoints.push_back(point);
    }
    //set centerline points accordingly (0 flow angle and y location)
    for (int i = 0; i < nPoints; i++){
        if (characteristicPoints[i].index == 1){
            characteristicPoints[i].is_centerline_location = true;
            characteristicPoints[i].flow_angle = 0;
            characteristicPoints[i].y_location = 0;
        }
        if (i >= 1){
            if (characteristicPoints[i-1].is_wall_location){
                characteristicPoints[i].is_centerline_location = true;
                characteristicPoints[i].flow_angle = 0;
                characteristicPoints[i].y_location = 0;
            }
        }
    }
}

void engine::reset(){
    //reset all
    characteristicPoints.clear();
}
