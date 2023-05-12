#ifndef ENGINE_H
#define ENGINE_H
#include "cPoint.h"
#include <vector>
#pragma once

class engine{
    public:
        //constructors
        engine();

        //methods
        void initializeCharacteristicPoints();
        void reset();

        //variables: user defined
        float chamber_pressure; //Pa
        float chamber_temperature;  //K
        float gamma; //unitless
        float R_gas; //J/kgK
        float nozzle_throat_radius; //m
        int nPoints; //number of characteristic points
        int nLines; //number of characteristic lines

        //variables: calculated
        float exit_pressure; //Pa
        float exit_temperature; //K
        float exit_sound_speed; //m/s
        float exit_Mach; //unitless
        float max_wall_angle; //deg
        std::vector<cPoint> characteristicPoints;

};


#endif
