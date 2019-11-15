#pragma once //protects against multiple inclusions of this header file

/*this file is to implement the user interface of the path and draw on the map*/

#include "m1.h"
#include "m2.h"
#include "m3.h"
#include <basicFeature.h>
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "StreetsDatabaseAPI.h"
#include <math.h>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
using namespace std;

//print out the shortest path according to user input
void print_the_path(std::vector<unsigned> pathSegment, ezgl::renderer &g);
double find_seg_angle(LatLon seg1_start, LatLon seg1_end, LatLon seg2_start, LatLon seg2_end);
bool seg_is_straight(unsigned street_segment1, unsigned street_segment2);