#include "equations.h"
//constructor
equations::equations(){}

//methods
std::pair<float,float> equations::returnOutsideTemperatureAndPressure(float h){
    float T;
    float P_o;
    if (11000 > h & h < 25000){
        T = -56.46;
        P_o = 1000*(22.65*exp(1.73-0.000157*h));
    }
    else if (h >= 25000){
        T = -131.21 + 0.00299*h ;
        P_o = 1000*(2.488*pow((T+273.1)/216.6,-11.388));
    }
    else {
        T = 15.04 - 0.00649*h;
        P_o = 1000*(101.29*(pow((T+273.1)/288.08,5.256)));
    }
    std::pair<float,float> result(T, P_o);
    return result;
}

float equations::getExitMach(float &Tc, float &Pc, float &Pe, float &gam, float &R){
    //set defaults for any edge cases
    if (gam <= 1){
        gam = 1.4;
    }
    if (Pe/Pc >= 1){
        float prTemp = 0.40;
        Pc = Pe/prTemp;
    }
    float pr = pow((Pe/Pc),((gam-1)/gam));
    float exit_temperature = Tc*pr;
    float exit_sound_speed = sqrt(gam*R*exit_temperature);
    float exit_velocity = sqrt((2*gam*R*Tc*(1-pr))/(gam-1));
    float exit_Mach = exit_velocity/exit_sound_speed;
    return exit_Mach;
}

float equations::getPMAngle(float gam, float M){
    if (gam <= 1){
        return 0;
    }
    float angle = sqrt((gam+1)/(gam-1))*atan(sqrt(((gam-1)*(pow(M,2)-1))/(gam+1)))-atan(sqrt(pow(M,2)-1));
    return angle*(180/M_PI);
}

float equations::getMachFromPM(float Mi, float pmAngle, float gam){
    double nu = pmAngle*(M_PI/180);
    while (Mi <= 15){
        double x = sqrt((gam+1)/(gam-1))*atan(sqrt(((gam-1)*(pow(Mi,2)-1))/(gam+1)))-atan(sqrt(pow(Mi,2)-1));
        double residual = nu - x;
        if (residual < 0.0001){
            return Mi;
        }
        Mi += 0.001;
    }
    return 1.0;
}

std::vector<float> equations::interpolateAngle(float theta, int n){
    std::vector<float> result = {0};
    float temp = theta - (1+fmod(theta,1));
    float delta = temp/(n-2);
    for (int i = 0; i < n-2; i++){
        result.push_back(result[i]+delta);
    }
    result.push_back(theta);
    return result;
}

std::string equations::vectorToString(std::vector<float> v){
    std::string str;
    for (unsigned int i = 0; i < v.size(); i++){
        str.append(std::to_string(v[i]));
        str.append(" ");
    }
    return str;
}

std::string equations::vectorToString(std::vector<int> v){
    std::string str;
    for (unsigned int i = 0; i < v.size(); i++){
        str.append(std::to_string(v[i]));
        str.append(" ");
    }
    return str;
}

std::pair<float,float> equations::returnXYIntersectionPoint(float xt, float yt, float tht_to, float xb, float yb, float tht_b){
    tht_to = tht_to*(M_PI/180);
    tht_b = tht_b*(M_PI/180);
    float x = (xt*tan(tht_to)-xb*tan(tht_b)+yb-yt)/(tan(tht_to)-tan(tht_b));
    float y = (tan(tht_to)*tan(tht_b)*(xt-xb)+tan(tht_to)*yb-tan(tht_b)*yt)/(tan(tht_to)-tan(tht_b));
    std::pair<float,float> result(x,y);
    return result;
}
