#pragma once

#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "basicFeature.h"
#include "pathDisplay.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "StreetsDatabaseAPI.h"
#include <math.h>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>

using namespace std;


void load_map_beijing(GtkWidget *widget, ezgl::application* application);
void load_map_hongkong(GtkWidget *widget, ezgl::application* application);
void load_map_toronto(GtkWidget *widget, ezgl::application* application);
void load_map_newyork(GtkWidget *widget, ezgl::application* application);
void load_map_london(GtkWidget *widget, ezgl::application* application);
void load_map_sydney(GtkWidget *widget, ezgl::application* application);
void load_map_singapore(GtkWidget *widget, ezgl::application* application);
void load_map_cairo(GtkWidget *widget, ezgl::application* application);
void load_map_iceland(GtkWidget *widget, ezgl::application* application);
void load_map_tokyo(GtkWidget *widget, ezgl::application* application);
void quit_find(GtkWidget *widget, ezgl::application* application);
void search_poi(GtkWidget *widget, ezgl::application* application);
void search_intersection(GtkWidget *widget, ezgl::application* application);
void display_find_path(GtkWidget *widget, ezgl::application* application);
void find_poi_go(GtkWidget *widget, ezgl::application* application);
void find_intersection_go(GtkWidget *widget, ezgl::application* application);
void find_path_go(GtkWidget *widget, ezgl::application* application);
void find_poi_cancel(GtkWidget *widget, ezgl::application* application);
void find_intersection_cancel(GtkWidget *widget, ezgl::application* application);
void find_path_cancel(GtkWidget *widget, ezgl::application* application);
void click_on_map(GtkWidget *widget, ezgl::application* application);
void enter_intersections(GtkWidget *widget, ezgl::application* application);
void show_help(GtkWidget *widget, ezgl::application* application);
void close_help(GtkWidget *widget, ezgl::application* application);
void display_travel_info(GtkWidget *widget, ezgl::application* application);

