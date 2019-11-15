/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "m1.h"
#include "m1.cpp"
#include "m2.h"
#include "m3.h"
#include "m4.h"
#include <uiButtons.h>
#include <pathDisplay.h>
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "StreetsDatabaseAPI.h"
#include <iostream>
#include <sstream>
#include <string>
#include <basicFeature.h>
#include <mouse.h>
#include "math.h"
#include <vector>
#include <iomanip>

using namespace std;

//declare functions here
void draw_main_canvas (ezgl::renderer &g);
void map_bound();
void setup_streets();
void find_button(GtkWidget *widget, ezgl::application *application);
void quit_find_button(GtkWidget *widget, ezgl::application *application);
void load_map_button(GtkWidget *widget, ezgl::application *application);
void on_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data);
void initial_setup(ezgl::application *application);
void load_map_name(std::string mapName, ezgl::application *application);

         

//declare global variables here
std::vector<intersection_data> intersections;
std::vector<street_data> streets;
std::vector<LatLon> pointOfInterest_position;
intersection_data highlight_point;
std::vector<num_click_intersection> count_clicked_intersection; 
std::vector<intersection_data> highlight_intersection_text;

std::vector<unsigned> path;
std::vector<intersection_path> from_intersection_to;
std::vector<intersection_path> intersection_info;
std::vector<unsigned> intersectionID_clicked;


double max_lon;
double min_lon;
double max_lat;
double min_lat;
double original_area;
bool run_flag = false; // used to set the flag, so that the value in original_area will no longer change

LatLon previousClicked;

string street1;
string street2;
string from_street_1;
string from_street_2;
string to_street_1;
string to_street_2;
string find_info;
bool find_POI = false;
bool find_street = false;
bool load_map_pressed = false;
bool find_path = false;
bool find_path_ui = false;
int width_in_pixels;
int height_in_pixels;
double height;
double width;
unsigned clickedIntersection_1 = 0;
unsigned clickedIntersection_2 = 0;
double total_travel_time;
unsigned intersection1 = 0;
unsigned intersection2 = 0;

//define global variable to store application and canvas
ezgl::application* appPtr;
ezgl::canvas* canvasPtr;

//Draws the map which has been loaded with load_map().
//Your main() program should do the same.
void draw_map(){
    map_bound();
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";
    ezgl::application application(settings);
    appPtr = &application;
    
    double max_lon_x = convert_x_from_lon(max_lon);
    double max_lat_y = convert_y_from_lat(max_lat);
    double min_lon_x = convert_x_from_lon(min_lon);
    double min_lat_y = convert_y_from_lat(min_lat);
    ezgl::rectangle initial_world({min_lon_x, min_lat_y}, {max_lon_x, max_lat_y}); 

    canvasPtr = application.add_canvas("MainCanvas", draw_main_canvas, initial_world);

    DeliveryInfo info1(22066, 41932, 156.64452);
    DeliveryInfo info2(41932, 69463, 100.59782);
    DeliveryInfo info3(71507, 7087, 157.35182);
    DeliveryInfo info4(654, 265, 65);

    std::vector<DeliveryInfo> deliveries;
    deliveries.push_back(info1); 
    deliveries.push_back(info2); deliveries.push_back(info3); deliveries.push_back(info4);
    traveling_courier(deliveries, {35855, 9003, 37807}, 15, 25, 9747.506835938);



//    std::vector<unsigned> test;
//    std::vector<unsigned> another_test;
//    test.resize(5);
//    for (unsigned i=0; i<5; i++){
//        test[i]=i;
//        
//    }
//    extern std::vector<unsigned>two_opt_swap(std::vector<unsigned> existing_route, int i, int k);
//    another_test = two_opt_swap(test, 1, 3);
//    for(unsigned i=0; i<5; i++){
//       std::cout << "++++++++++++++++++++ test vector " << i << ": " << another_test[i] << endl;
//    }
    
    application.run(initial_setup, act_on_mouse_press, act_on_mouse_move, act_on_key_press);

}

void draw_main_canvas (ezgl::renderer &g) {
    
    intersections.resize(getNumIntersections());
    intersection_info.resize(getNumIntersections());

    for(int id = 0; id < getNumIntersections(); id++){
        intersections[id].position = getIntersectionPosition(id);
        intersections[id].name = getIntersectionName(id);
        intersection_info[id].intersectionID = id;
        intersection_info[id].name = getIntersectionName(id);
        intersection_info[id].position = getIntersectionPosition(id);
    }
    
    pointOfInterest_position.resize(getNumPointsOfInterest());
    for(int index = 0; index < getNumPointsOfInterest(); index++){
        pointOfInterest_position[index] = getPointOfInterestPosition(index);
    }
    
    
    if(run_flag == false){
    original_area = g.get_visible_world().area();

    }
    // set the flag to true, so that the value in original_area will no longer change
    run_flag=true;
    int zoom = find_zoom_level(g); // find the zoom level
    //draw background
    ezgl::color background_color(237,237,237,248); //grey
    g.set_color(background_color);
    g.draw_rectangle({convert_x_from_lon(min_lon), convert_y_from_lat(min_lat)}, {convert_x_from_lon(max_lon), convert_y_from_lat(max_lat)});
    g.fill_rectangle({convert_x_from_lon(min_lon), convert_y_from_lat(min_lat)}, {convert_x_from_lon(max_lon), convert_y_from_lat(max_lat)});
    
    draw_all_features(g);
    draw_all_street_seg(g);
    draw_all_st_name(g);

    //draw intersection
    g.set_color(ezgl::RED);
    for(size_t i = 0; i < intersections.size(); i++){

        double x = convert_x_from_lon(intersections[i].position.lon());
        double y = convert_y_from_lat(intersections[i].position.lat());

        ezgl::point2d center(x,y);
        //modify the size of the intersections when zooming in and out
        if(zoom >= 6){
            if(original_area >= 7){
                g.fill_arc(center,0.0000005, 0, 360);
            }
            else if(original_area <= 9/*0.000005*/){
                g.fill_arc(center,0.000001, 0, 360);
            }
        }       
    } 

    ezgl::rectangle View = g.get_visible_world();
    height = View.height();
    width = View.width();

        

    //to allow the user to click on the intersections and find the shortest path
    //'find_street' variable will be changed
    //user want to find the path of two intersections by clicking on the intersections
    if(find_path){
        //pass in intersection id
        //need to press quit_find in the end

//        clickedIntersection_1
        if(clickedIntersection_1 != 0){
            g.set_color(ezgl::FIRE_BRICK);

            ezgl::point2d point1(convert_x_from_lon(getIntersectionPosition(clickedIntersection_1).lon()), 
                convert_y_from_lat(getIntersectionPosition(clickedIntersection_1).lat()));
            g.fill_arc(point1, height * 0.01, 0, 360);
        }
        if(clickedIntersection_2 != 0){
            g.set_color(ezgl::FIRE_BRICK);
            ezgl::point2d point2(convert_x_from_lon(getIntersectionPosition(clickedIntersection_2).lon()), 
                convert_y_from_lat(getIntersectionPosition(clickedIntersection_2).lat()));
            g.fill_arc(point2, height * 0.01, 0, 360);
        }
        //two intersections are clicked
        if(clickedIntersection_1 != 0 && clickedIntersection_2 != 0){
            path = find_path_between_intersections(clickedIntersection_1, clickedIntersection_2, 15, 25);
            print_the_path(path, g);
        }  

         total_travel_time = round_double(compute_path_travel_time(path, 15, 25)/60);
         intersection1 = clickedIntersection_1;
         intersection2 = clickedIntersection_2;
    }
    //if the user want to find path by entering the name of the two street 
    //to find intersection first, then find the path of two intersections
    else if(find_path_ui){
        //find the intersection between two streets first
        int intersect_1_id = find_intersection_id_by_two_streets_name(from_street_1, from_street_2);

        int intersect_2_id = find_intersection_id_by_two_streets_name(to_street_1, to_street_2);
        g.set_color(ezgl::FIRE_BRICK);
        //first intersection
        ezgl::point2d point1(convert_x_from_lon(getIntersectionPosition(intersect_1_id).lon()), 
            convert_y_from_lat(getIntersectionPosition(intersect_1_id).lat()));
        
        ezgl::rectangle currentView = g.get_visible_world();
        double height_intersect = currentView.height() * 0.005;
        g.fill_arc(point1, height_intersect, 0, 360);

//        //second intersection
        ezgl::point2d point2(convert_x_from_lon(getIntersectionPosition(intersect_2_id).lon()), 
            convert_y_from_lat(getIntersectionPosition(intersect_2_id).lat()));
        g.fill_arc(point2, height_intersect, 0, 360);
        path = find_path_between_intersections(intersect_1_id, intersect_2_id, 15, 25);
        print_the_path(path, g);
        
        int zoom_lev = find_zoom_level(g);
        if(zoom_lev > 6){
            double visible_width = g.get_visible_world().width();
            double visible_height = g.get_visible_world().height();
            double text_box_width= convert_x_from_lon(visible_width) * 15;
            double text_box_height = convert_y_from_lat(visible_height) * 10;
            //draw a text box
            g.set_color(ezgl::WHITE);
            g.fill_rectangle(point1, text_box_width, text_box_height);
            g.fill_rectangle(point2, text_box_width, text_box_height);
            //get the text position
            //for the first intersection
            double text_x_f = convert_x_from_lon(getIntersectionPosition(intersect_1_id).lon()) + text_box_width/2;
            double text_y_1_f = convert_y_from_lat(getIntersectionPosition(intersect_1_id).lon()) + text_box_height*3/4;
            double text_y_2_f = convert_y_from_lat(getIntersectionPosition(intersect_1_id).lat()) + text_box_height*2/4;
            double text_y_3_f = convert_y_from_lat(getIntersectionPosition(intersect_1_id).lat()) + text_box_height*1/4;
            //for the second intersection
            double text_x_s = convert_x_from_lon(getIntersectionPosition(intersect_2_id).lon()) + text_box_width/2;
            double text_y_1_s = convert_y_from_lat(getIntersectionPosition(intersect_2_id).lon()) + text_box_height*3/4;
            double text_y_2_s = convert_y_from_lat(getIntersectionPosition(intersect_2_id).lat()) + text_box_height*2/4;
            double text_y_3_s = convert_y_from_lat(getIntersectionPosition(intersect_2_id).lat()) + text_box_height*1/4;
            //put the info into the text box
            //for the first intersection
            g.set_color(ezgl::BLACK);
            g.draw_text({text_x_f, text_y_1_f}, getIntersectionName(intersect_1_id));
            g.draw_text({text_x_f, text_y_2_f}, "Lon is: " + std::to_string(getIntersectionPosition(intersect_1_id).lon()));
            g.draw_text({text_x_f, text_y_3_f}, "Lat is: " + std::to_string(getIntersectionPosition(intersect_1_id).lat()));
            //for the second intersection
            g.draw_text({text_x_s, text_y_1_s}, getIntersectionName(intersect_2_id));
            g.draw_text({text_x_s, text_y_2_s}, "Lon is: " + std::to_string(getIntersectionPosition(intersect_2_id).lon()));
            g.draw_text({text_x_s, text_y_3_s}, "Lat is: " + std::to_string(getIntersectionPosition(intersect_2_id).lat()));
        }
        
        intersection1 = intersect_1_id;
        intersection2 = intersect_2_id;
    }
    else{
        if(highlight_point.name != ""){
  
                g.set_color(ezgl::PURPLE);
                ezgl::point2d point(convert_x_from_lon(highlight_point.position.lon()), 
                        convert_y_from_lat(highlight_point.position.lat()));
                //modified the size of intersections when user zooming
                if(zoom >= 6){
                    if(original_area >= 7){
                        g.fill_arc(point,0.0000005, 0, 360);
                    }
                    else if(original_area <= 9){
                        g.fill_arc(point,0.000001, 0, 360);
                    }
                    for(unsigned i = 0; i < count_clicked_intersection.size(); i++){
                        if(count_clicked_intersection[i].position.lat() == highlight_point.position.lat() &&
                                count_clicked_intersection[i].position.lon() == highlight_point.position.lon()){
                            if(count_clicked_intersection[i].ifClick == true){
                                double visible_width = g.get_visible_world().width();
                                    double visible_height = g.get_visible_world().height();
                                    double text_box_width= convert_x_from_lon(visible_width) * 15;
                                    double text_box_height = convert_y_from_lat(visible_height) * 10;
                                    //draw a text box
                                    g.set_color(ezgl::WHITE);
                                    g.fill_rectangle(point, text_box_width, text_box_height);
                                    //get the text position
                                    double text_x = convert_x_from_lon(highlight_point.position.lon()) + text_box_width/2;
                                    double text_y_1 = convert_y_from_lat(highlight_point.position.lat()) + text_box_height*3/4;
                                    double text_y_2 = convert_y_from_lat(highlight_point.position.lat()) + text_box_height*2/4;
                                    double text_y_3 = convert_y_from_lat(highlight_point.position.lat()) + text_box_height*1/4;
                                    //put the info into the text box
                                    g.set_color(ezgl::BLACK);
                                    g.draw_text({text_x, text_y_1}, highlight_point.name);
                                    g.draw_text({text_x, text_y_2}, "Lon is: " + std::to_string(highlight_point.position.lon()));
                                    g.draw_text({text_x, text_y_3}, "Lat is: " + std::to_string(highlight_point.position.lat()));
                            }
                        }

                    }

                }
        }
    }
    
    
   
    //draw points that are of type searched by user
    if(find_POI && (find_info != "")){
        std::string canvas_id = appPtr->get_main_canvas_id();
        width_in_pixels = appPtr->get_canvas(canvas_id)->width();
        height_in_pixels = appPtr->get_canvas(canvas_id)->height();
        draw_point_of_interest(g, find_info);
    }
       
    //draw intersections according to the input names
    if(find_street && (street1 != "") && (street2 != "")){
        draw_intersections_by_two_streets_name(g, street1, street2);
    }
    
    g.set_color(ezgl::FIRE_BRICK);
    ezgl::point2d point1(convert_x_from_lon(getIntersectionPosition(22066).lon()), 
            convert_y_from_lat(getIntersectionPosition(22066).lat()));
    g.fill_arc(point1, height * 0.01, 0, 360);
    
    ezgl::point2d point2(convert_x_from_lon(getIntersectionPosition(41932).lon()), 
        convert_y_from_lat(getIntersectionPosition(41932).lat()));
    g.fill_arc(point2, height * 0.01, 0, 360);
    path = find_path_between_intersections(22066, 41932, 15, 25);
    print_the_path(path, g);
}
 
void map_bound(){
    max_lat = getIntersectionPosition(0).lat();
    min_lat = max_lat;
    max_lon = getIntersectionPosition(0).lon();
    min_lon = max_lon;
    intersections.resize(getNumIntersections());
    
    for(int id = 0; id < getNumIntersections(); id++){
        intersections[id].position = getIntersectionPosition(id);
        intersections[id].name = getIntersectionName(id);
        
        max_lat = std::max(max_lat, intersections[id].position.lat());
        min_lat = std::min(min_lat, intersections[id].position.lat());
        max_lon = std::max(max_lon, intersections[id].position.lon());
        min_lon = std::min(min_lon, intersections[id].position.lon());
    }
}



void load_map_name(std::string mapName, ezgl::application *application){

    
        close_map();
        
        bool ifsuccess = load_map(mapName);
        if(!ifsuccess){
            //if the map loaded is fail
            std::cout << "failure to load the map '" << mapName << "'" << std::endl;
        }
        
        clickedIntersection_1 = 0;
        clickedIntersection_2 = 0;
        intersections.clear();
        intersection_info.clear();
        streets.clear();
        pointOfInterest_position.clear();
//intersection_data highlight_point;
        count_clicked_intersection.clear(); 

        highlight_intersection_text.clear();

        path.clear();
        from_intersection_to.clear();
        intersection_info.clear();
        intersectionID_clicked.clear();
        
        //set camera size
        map_bound();
        ezgl::point2d origion(convert_x_from_lon(min_lon), convert_y_from_lat(min_lat));
        ezgl::point2d top_right(convert_x_from_lon(max_lon), convert_y_from_lat(max_lat));
        ezgl::rectangle bound (origion, top_right);
        canvasPtr->get_camera().set_world(bound);
                
        
        application->refresh_drawing();
}



void initial_setup(ezgl::application *application){
  // Update the status bar message
  application->update_message("EZGL Application");
 
  // Connect load_map_beijing function to the Beijing button
  GObject *beijing_button = application->get_object("Beijing");
  g_signal_connect(beijing_button, "clicked", G_CALLBACK(load_map_beijing), application);
  
  // Connect load_map_hongkong function to the Hongkong button
  GObject *hongkong_button = application->get_object("HongKong");
  g_signal_connect(hongkong_button, "clicked", G_CALLBACK(load_map_hongkong), application);
  
  // Connect load_map_toronto function to the Toronto button
  GObject *toronto_button = application->get_object("Toronto");
  g_signal_connect(toronto_button, "clicked", G_CALLBACK(load_map_toronto), application);
  
  // Connect load_map_newyork function to the Newyork button
  GObject *newyork_button = application->get_object("Newyork");
  g_signal_connect(newyork_button, "clicked", G_CALLBACK(load_map_newyork), application);
  
  // Connect load_map_london function to the London button
  GObject *london_button = application->get_object("London");
  g_signal_connect(london_button, "clicked", G_CALLBACK(load_map_london), application);
  
  // Connect load_map_sydney function to the Sydney button
  GObject *sydney_button = application->get_object("Sydney");
  g_signal_connect(sydney_button, "clicked", G_CALLBACK(load_map_sydney), application);
  
  // Connect load_map_singapore function to the Singapore button
  GObject *singapore_button = application->get_object("Singapore");
  g_signal_connect(singapore_button, "clicked", G_CALLBACK(load_map_singapore), application);
  
  // Connect load_map_cairo function to the Cairo button
  GObject *cairo_button = application->get_object("Cairo");
  g_signal_connect(cairo_button, "clicked", G_CALLBACK(load_map_cairo), application);
  
  // Connect load_map_iceland function to the Iceland button
  GObject *iceland_button = application->get_object("Iceland");
  g_signal_connect(iceland_button, "clicked", G_CALLBACK(load_map_iceland), application);
  
  // Connect load_map_tokyo function to the Tokyo button
  GObject *tokyo_button = application->get_object("Tokyo");
  g_signal_connect(tokyo_button, "clicked", G_CALLBACK(load_map_tokyo), application);
  
  // Connect quit_find function to the Quit Find button
  GObject *quit_find_button = application->get_object("QuitFind");
  g_signal_connect(quit_find_button, "clicked", G_CALLBACK(quit_find), application);
  
  // Connect search_poi function to the POI button
  GObject *poi_button = application->get_object("POIButton");
  g_signal_connect(poi_button, "clicked", G_CALLBACK(search_poi), application);
  
  // Connect search_intersection function to the Intersection button
  GObject *intersection_button = application->get_object("IntersectionButton");
  g_signal_connect(intersection_button, "clicked", G_CALLBACK(search_intersection), application);
  
  // Connect display_find_path function to the Path button
  GObject *path_button = application->get_object("PathButton");
  g_signal_connect(path_button, "clicked", G_CALLBACK(display_find_path), application);
  
  // Connect find_poi_go function to the Search button in the new find window
  GObject *search_button = application->get_object("SearchPOIButton");
  g_signal_connect(search_button, "clicked", G_CALLBACK(find_poi_go), application);
  
  // Connect find_intersection_go function to the Go button in the find intersection window
  GObject *intersection_go_button = application->get_object("IntersectionGoButton");
  g_signal_connect(intersection_go_button, "clicked", G_CALLBACK(find_intersection_go), application);
 
  // Connect find_path_go function to the Go button in the find path window
  GObject *path_go_button = application->get_object("PathGoButton");
  g_signal_connect(path_go_button, "clicked", G_CALLBACK(find_path_go), application);
  
  // Connect find_poi_cancel function to the Cancel button in the find poi window
  GObject *poi_cancel_button = application->get_object("CancelButtonPOI");
  g_signal_connect(poi_cancel_button, "clicked", G_CALLBACK(find_poi_cancel), application);
  
  // Connect find_intersection_cancel function to the Cancel button in the find intersection window
  GObject *intersection_cancel_button = application->get_object("IntersectionCancelButton");
  g_signal_connect(intersection_cancel_button, "clicked", G_CALLBACK(find_intersection_cancel), application);
  
  // Connect find_path_cancel function to the Cancel button in the find path window
  GObject *path_cancel_button = application->get_object("PathCancelButton");
  g_signal_connect(path_cancel_button, "clicked", G_CALLBACK(find_path_cancel), application);
  
  // Connect click_on_map function to the Click on map button in the message window
  GObject *click_on_map_button = application->get_object("MessageWindowClick");
  g_signal_connect(click_on_map_button, "clicked", G_CALLBACK(click_on_map), application);
  
  // Connect enter_intersections function to the Enter intersection button in the message window
  GObject *enter_intersection_button = application->get_object("MessageWindowEnter");
  g_signal_connect(enter_intersection_button, "clicked", G_CALLBACK(enter_intersections), application);
  
  // Connect show_help function to the  About menu 
  GObject *help_menu = application->get_object("HelpButton");
  g_signal_connect(help_menu, "clicked", G_CALLBACK(show_help), application);
  
   // Connect close_help function to the OK button
  GObject *OK_button = application->get_object("OKButton");
  g_signal_connect(OK_button, "clicked", G_CALLBACK(close_help), application);

   // Connect display_travel_info function to the Travel Info button
  GObject *travel_info_button = application->get_object("TravelInfoButton");
  g_signal_connect(travel_info_button, "clicked", G_CALLBACK(display_travel_info), application);
  
  
}
