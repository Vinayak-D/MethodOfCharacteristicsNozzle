#ifndef EQUATIONS_H
#define EQUATIONS_H
#include <vector>
#include <math.h>
#include <string> 

#pragma once

class equations{
    public:
        //constructor
        equations();

        //methods
        std::pair<float,float> returnOutsideTemperatureAndPressure(float h); //T in K, P in Pa
        float getExitMach(float &Tc, float &Pc, float &Pe, float &gam, float &R);
        float getPMAngle(float gam, float M); //degrees
        float getMachFromPM(float Mi, float pmAngle, float gam); //pmAngle in degrees
        std::vector<float> interpolateAngle(float theta, int n); //angles are in degrees
        std::string vectorToString(std::vector<float> v);
        std::string vectorToString(std::vector<int> v);
        std::pair<float,float> returnXYIntersectionPoint(float xt, float yt, float tht_to, float xb, float yb, float tht_b); //angles in degrees

};

#endif
