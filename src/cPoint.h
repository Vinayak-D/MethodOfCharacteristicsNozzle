#ifndef CPOINT_H
#define CPOINT_H

#pragma once

class cPoint{
    public:
        //blank constructor
        cPoint(int i);

        //variables
        int index;
        float flow_angle; //deg
        float pm_angle; //deg
        float Mach; //unitless
        float mu; //deg
        float x_location; //cm
        float y_location; //cm
        bool is_wall_location; 
        bool is_centerline_location;
};

#endif
