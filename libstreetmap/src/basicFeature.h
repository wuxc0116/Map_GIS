#pragma once //protects against multiple inclusions of this header file

// This header files includes all helper functions to draw basic maps
// such as streets, intersections, POI, features (lake, buildings, etc)

#include "m1.h"
#include "m2.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "StreetsDatabaseAPI.h"
#include <math.h>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
using namespace std;

//declare global variables here
extern double original_area;
extern double max_lat;
extern double min_lat;

// functions to transfer coordnidates
double convert_x_from_lon(float lon);
double convert_y_from_lat(float lat);
double convert_lon_from_x(double x);
double convert_lat_from_y(double y);
int find_zoom_level(ezgl::renderer &g);


void draw_all_features(ezgl::renderer &g);
void draw_feature(ezgl::renderer &g, FeatureIndex featureIdx, ezgl::color feature_color, int line_width);

void draw_street_name(LatLon inter_start, LatLon inter_end, string text_content, bool oneway, int zoom, ezgl::renderer &g);
void draw_all_st_name(ezgl::renderer &g);
int judge_name_count(int zoom);
void draw_all_street_seg(ezgl::renderer &g);

//void draw_searching_bar(ezgl::renderer &g);
//void draw_searching_mapName_bar(ezgl::renderer &g);

std::string ignore_blank(std::string input);
//void draw_intersections_by_two_streets_name(ezgl::renderer &g, std::string streetNames);

//void draw_intersections_by_two_streets_name(ezgl::renderer &g, std::string twoStreetNames);
//bool search_partial_street_names(int StreetIndex, string streetName);
void draw_intersections_by_two_streets_name(ezgl::renderer &g, std::string name1, std::string name2);

struct intersection_data {
    LatLon position;
    std::string name;
};

struct num_click_intersection{
    LatLon position;
    bool ifClick = false;
};


struct street_data {
    std::string st_name;
    std::vector<unsigned> segID;
    std::vector<LatLon> curve_points_m2; //'m2' is mean to differ from that used in m1
};

struct intersection_path{
    int intersectionID;
    std::string name;
    LatLon position;
};

//struct feature_data{
//    int featureID;
//    int area;
//    int point_counts;
//};

bool search_keyword_point_of_interest(int POI_index, string keyword);
void draw_point_of_interest(ezgl::renderer &g, string keyword);

double polygonArea(std::vector<double>X, std::vector<double>Y, int numPoints);
void draw_all_features_test(ezgl::renderer &g);
int find_intersectionID_by_data(intersection_data clickedIntersection);intersection_path find_intersection_info_by_two_street_names(std::string name1, std::string name2);
int find_intersection_id_by_two_streets_name(std::string name1, std::string name2);
float round_double(float var);