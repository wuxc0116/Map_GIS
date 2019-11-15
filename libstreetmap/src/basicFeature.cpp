#include <basicFeature.h>

using namespace std;

extern std::vector<LatLon> pointOfInterest_position;
extern std::string str_tolower(std::string s);
extern std::map<int, double>features;
extern std::vector<intersection_path> intersection_info;
extern int width_in_pixels;
extern int height_in_pixels;
extern double width;
extern double height;

//convert longitude to x, same method used in m1.cpp 
double convert_x_from_lon(float lon){
    double x, lon_rad, lat_avg;
    lon_rad = lon*DEG_TO_RAD;
    lat_avg = ((min_lat + max_lat)/2)*DEG_TO_RAD;
    x = lon_rad*cos(lat_avg);
    return x;
}

//convert latitude to y
double convert_y_from_lat(float lat){
    double lat_rad;
    lat_rad = lat*DEG_TO_RAD;
    return lat_rad;
}

//convert y to latitude
double convert_lat_from_y(double y){
    double lat = y/DEG_TO_RAD;
    return lat;
}

//convert x to longitude
double convert_lon_from_x(double x){
    double lon, lon_rad, lat_avg;
    lat_avg = ((min_lat + max_lat)/2)*DEG_TO_RAD;
    lon_rad = x/cos(lat_avg);
    lon = lon_rad/DEG_TO_RAD;
    return lon;
}

int find_zoom_level(ezgl::renderer &g){
    double constant = 0.36; //this constant is used for determine the zoom in level
    if(g.get_visible_world().area() >= original_area){
        return 1; //origional level
    }
    if(g.get_visible_world().area() <= original_area * pow(constant, 9)){
        return 10;
    }
    if(g.get_visible_world().area() <= original_area * pow(constant, 8)){
        return 9;
    }
    if(g.get_visible_world().area() <= original_area * pow(constant, 7)){
        return 8;
    }
    if(g.get_visible_world().area() <= original_area * pow(constant, 6)){
        return 7;
    }
    if(g.get_visible_world().area() <= original_area * pow(constant, 5)){
        return 6;
    }
    if(g.get_visible_world().area() <= original_area * pow(constant, 4)){
        return 5;
    }
    if(g.get_visible_world().area() <= original_area * pow(constant, 3)){
        return 4;
    }
    if(g.get_visible_world().area() <= original_area * pow(constant, 2)){
        return 3;
    }
    if(g.get_visible_world().area() <= original_area * pow(constant, 1)){
        return 2;
    }
    else{
        return 1;
    }
    
}


void draw_all_features(ezgl::renderer &g){
    int zoom = find_zoom_level(g);
    unsigned featuresNum = getNumFeatures();
    for (unsigned i=0; i<featuresNum; i++){
        if (getFeatureType(i) == Lake){
            ezgl::color feature_color(165,197,247,255);
            draw_feature(g, i, feature_color, 1);
        }
        else if (getFeatureType(i) == Park){
            ezgl::color feature_color(196,226,152,255);
            draw_feature(g, i, feature_color, 1);
        }
        else if (getFeatureType(i) == Beach){
            ezgl::color feature_color(249,245,112,255);
            draw_feature(g, i, feature_color, 1);
        }
        
        else if (getFeatureType(i) == River){
            ezgl::color feature_color(23,142,211,255);
            draw_feature(g, i, feature_color, 2);
        }
        else if (getFeatureType(i) == Island){
            ezgl::color feature_color(242,235,225,255);
            draw_feature(g, i, feature_color, 2);
        }
        else if (getFeatureType(i) == Greenspace){
            ezgl::color feature_color(196,226,152,255);
            draw_feature(g, i, feature_color, 1);
            //draw_feature(g, i, ezgl::DARK_GREEN, 1);
        }
        else if (getFeatureType(i) == Golfcourse){
            ezgl::color feature_color(196,226,152,255);
            draw_feature(g, i, feature_color, 1);
        }
        else if (getFeatureType(i) == Building && zoom>=6){
            draw_feature(g, i, ezgl::GREY_55, 1);
        }
        else if (getFeatureType(i) == Stream && zoom>=7){
            ezgl::color feature_color(165,197,247,255);
            draw_feature(g, i, feature_color, 2);
        }
        
    }
}

double polygonArea(std::vector<double>X, std::vector<double>Y, int numPoints){
    double area = 0;         // Accumulates area in the loop
    int j = numPoints-1;  // The last vertex is the 'previous' one to the first

    for (int i=0; i<numPoints; i++){
        area = area +  (X[j]+X[i]) * (Y[j]-Y[i]); 
        j = i;  //j is previous vertex to i
      }
    return area/2;
}



void draw_feature(ezgl::renderer &g, FeatureIndex featureIdx, ezgl::color feature_color, int line_width){
    g.set_line_width(line_width);
    //find the number of points in feature polygon
    int num_of_points = getFeaturePointCount(featureIdx);
    //create a new vector to store location of each vertex
    std::vector<ezgl::point2d> vertex;
    for(int i=0; i<num_of_points; i++){
        LatLon location = getFeaturePoint(i, featureIdx);
        ezgl::point2d points(convert_x_from_lon(location.lon()), convert_y_from_lat(location.lat()));
        vertex.push_back(points);
        //now, vertex holds the all vertex points
    }
    if(vertex.size()==1){
        //only one vertex poly is not exist
        //do nothing to avoid error
    }
    //Change the color for subsequent draw calls
    else{
        
        g.set_color(feature_color);

        //check whether it is open feature
        if(vertex[0] == vertex[num_of_points - 1]){
            // draw close features
            g.fill_poly(vertex);
        } else {
            // draw open features, which is river and stream
            for(int j=0; j<num_of_points-1; j++){
                g.draw_line(vertex[j], vertex[j+1]);
            }
        }
    }
}



int judge_name_count(int zoom){
    if(zoom<=3){
        return 450;
    }
    else if(zoom>3 && zoom<=5){
        return 50;
    }
//    else if(zoom>5 && zoom<=7){
//        return 26;
//    }
    else if(zoom>5){
        return 10;
    }
    else
        return 100;
}


void draw_all_st_name(ezgl::renderer &g){
    int name_count = 1;
    int zoom = find_zoom_level(g);
    // first get all segment on this street
    //std::vector<unsigned> segment_list = find_street_street_segments(st_id);
    int seg_num = getNumStreetSegments();
    //draw a line between each segment
    for (int i=0; i<seg_num; i++){
        float speed_lim = getInfoStreetSegment(i).speedLimit;
        if((speed_lim>=80 && zoom<=5) ){
            bool check_oneway = getInfoStreetSegment(i).oneWay;
            LatLon seg_start = getIntersectionPosition(getInfoStreetSegment(i).from);
            LatLon seg_end = getIntersectionPosition(getInfoStreetSegment(i).to);
           
            //find the distance between seg_start and seg_end
//            int distance = find_distance_between_two_points(seg_start, seg_end);
            if(name_count == judge_name_count(zoom)){
                //get the street name
                unsigned st_id = getInfoStreetSegment(i).streetID;
                string st_name = getStreetName(st_id);
                if(st_name != "<unknown>"  && st_name.size()<=30){
                    draw_street_name(seg_start, seg_end, st_name, check_oneway, zoom, g);
                    name_count=0;
                }
            }
            name_count++;
        }
        else if(zoom>5 && speed_lim>40){
            float speed_lim_2 = getInfoStreetSegment(i).speedLimit;
            //unsigned num_curve = getInfoStreetSegment(i).curvePointCount;
            bool check_oneway = getInfoStreetSegment(i).oneWay;
            LatLon seg_start = getIntersectionPosition(getInfoStreetSegment(i).from);
            LatLon seg_end = getIntersectionPosition(getInfoStreetSegment(i).to);
           
            //find the distance between seg_start and seg_end
            int distance = find_distance_between_two_points(seg_start, seg_end);
            if(distance > 400 && name_count>=5 && speed_lim_2<60){
                //get the street name
                unsigned st_id = getInfoStreetSegment(i).streetID;
                string st_name = getStreetName(st_id);
                if(st_name != "<unknown>"  && st_name.size()<=30){
                    draw_street_name(seg_start, seg_end, st_name, check_oneway, zoom, g);
                    name_count=0;
                }
            }
            name_count++;
        }
    }
}


void draw_all_street_seg(ezgl::renderer &g){
    int zoom = find_zoom_level(g);
    // first get all segment on this street
    int seg_num = getNumStreetSegments();
    //draw a line between each segment
    for (int i=0; i<seg_num; i++){
        float speed_lim = getInfoStreetSegment(i).speedLimit;
        if((speed_lim>=80 && zoom<=3) || (zoom>=4)){
            unsigned num_curve = getInfoStreetSegment(i).curvePointCount;
            //bool check_oneway = getInfoStreetSegment(i).oneWay;
            LatLon seg_start = getIntersectionPosition(getInfoStreetSegment(i).from);
            LatLon seg_end = getIntersectionPosition(getInfoStreetSegment(i).to);
            int distance = find_distance_between_two_points(seg_start, seg_end);
           

            //set the line width and color according to the zoom level
            {
                if(zoom <=2){
                    g.set_color(ezgl::YELLOW_ORANGE);
                    g.set_line_width(4);
                }
                else if(zoom <=5){
                    if(speed_lim <= 40 || distance<50){
                        g.set_color(ezgl::WHITE);
                        g.set_line_width(1);
                    }
                    else{
                        g.set_color(ezgl::YELLOW_ORANGE);
                        g.set_line_width(2);
                    }
                }
                else if(zoom <=7){
                    if(speed_lim <= 40 || distance<50){
                        g.set_color(ezgl::WHITE);
                        g.set_line_width(2);
                    }
                    else{
                        g.set_color(ezgl::YELLOW_ORANGE);
                        g.set_line_width(3);
                    }
                }
                else if(zoom <=8){
                    if(speed_lim <= 40 || distance<30){
                        g.set_color(ezgl::WHITE);
                        g.set_line_width(2);
                    }
                    else {
                        g.set_color(ezgl::YELLOW_ORANGE);
                        g.set_line_width(6);
                    }
                }
                else if(zoom <=9){
                    if(speed_lim <= 40 || distance<30){
                        g.set_color(ezgl::WHITE);
                        g.set_line_width(2);
                    }
                    else{
                        g.set_color(ezgl::YELLOW_ORANGE);
                        g.set_line_width(9);
                    }
                }
                else if(zoom >=9 || distance<30){
                    if(speed_lim <= 40){
                        g.set_color(ezgl::WHITE);
                        g.set_line_width(2);
                    }
                    else{
                        g.set_color(ezgl::YELLOW_ORANGE);
                        g.set_line_width(11);
                    }
                }
            }
           
            //draw the street seg according to the curve point number
            if(num_curve == 0){
                ezgl::point2d seg_start_alter  (convert_x_from_lon(seg_start.lon()), convert_y_from_lat(seg_start.lat()));
                ezgl::point2d seg_end_alter  (convert_x_from_lon(seg_end.lon()), convert_y_from_lat(seg_end.lat()));
                g.draw_line(seg_start_alter, seg_end_alter);
            }
            else if(num_curve == 1){
                LatLon inflection = getStreetSegmentCurvePoint(0, i);
                ezgl:: point2d seg_start_alter (convert_x_from_lon(seg_start.lon()), convert_y_from_lat(seg_start.lat()));
                ezgl:: point2d seg_inflection_alter (convert_x_from_lon(inflection.lon()), convert_y_from_lat(inflection.lat()));
                ezgl::point2d seg_end_alter  (convert_x_from_lon(seg_end.lon()), convert_y_from_lat(seg_end.lat()));
                g.draw_line(seg_start_alter, seg_inflection_alter);
                g.draw_line(seg_inflection_alter, seg_end_alter);
            }
            else{
                //first draw the first curve_segment
                LatLon inflection = getStreetSegmentCurvePoint(0, i);
                ezgl:: point2d seg_start_alter (convert_x_from_lon(seg_start.lon()), convert_y_from_lat(seg_start.lat()));
                ezgl:: point2d seg_inflection_alter (convert_x_from_lon(inflection.lon()), convert_y_from_lat(inflection.lat()));
                g.draw_line(seg_start_alter, seg_inflection_alter);
                //then draw the rest
                for(unsigned j=0; j<num_curve-1; j++){
                    LatLon inflection_1 = getStreetSegmentCurvePoint(j, i);
                    LatLon inflection_2 = getStreetSegmentCurvePoint(j+1, i);
                    //more code are needed
                    ezgl:: point2d seg_inflection_1_alter (convert_x_from_lon(inflection_1.lon()), convert_y_from_lat(inflection_1.lat()));
                    ezgl:: point2d seg_inflection_2_alter (convert_x_from_lon(inflection_2.lon()), convert_y_from_lat(inflection_2.lat()));
                    g.draw_line (seg_inflection_1_alter, seg_inflection_2_alter);
                }
                //and finally draw the last segment
                LatLon inflection_second_last = getStreetSegmentCurvePoint(num_curve-1, i);
                ezgl::point2d seg_second_last_alter  (convert_x_from_lon(inflection_second_last.lon()), convert_y_from_lat(inflection_second_last.lat()));
                ezgl::point2d seg_end_alter  (convert_x_from_lon(seg_end.lon()), convert_y_from_lat(seg_end.lat()));
                g.draw_line(seg_second_last_alter, seg_end_alter);
            }
        }
    }
}




void draw_street_name(LatLon inter_start, LatLon inter_end, string text_content, bool oneway, int zoom, ezgl::renderer &g){
    //first convert all passed-in variables to x-y coord
    double inter_start_x = convert_x_from_lon(inter_start.lon());
    double inter_start_y = convert_y_from_lat(inter_start.lat());
    double inter_end_x = convert_x_from_lon(inter_end.lon());
    double inter_end_y = convert_y_from_lat(inter_end.lat());
   
    // find the distance between two intersections
    double delta_x = inter_end_x - inter_start_x;
    double delta_y = inter_end_y - inter_start_y;
   
    //set the rotation angle of text
        double degree;
        if(delta_x == 0){
            degree = 90; //vertical situation
        }
        else{
            degree = atan(delta_y / delta_x) / DEG_TO_RAD;
        }
        g.set_text_rotation(degree);
        g.set_color(ezgl::BLACK);
       
        //store the location into a new variable, having type point2d
        double position_to_draw_x = (inter_start_x + inter_end_x)/2;
        double position_to_draw_y = (inter_start_y + inter_end_y)/2;
        ezgl::point2d position_to_draw (position_to_draw_x, position_to_draw_y);

   
    //if zoom<=5, do not tell the oneway
    if(zoom <= 5){
        g.draw_text(position_to_draw, text_content);
    }
    else{
       

        //draw the street name
        if(oneway == true){
            if(delta_x > 0){ //case when the direction is to the right
                g.draw_text(position_to_draw, text_content + "-->");
            } else {
                g.draw_text(position_to_draw, text_content + "<--");
            }
        } else{
            g.draw_text(position_to_draw, text_content);
        }
        //reset the angle for remainder
        g.set_text_rotation(0);
    }
}

//function to check if a point is of the given search type
//this function is also useful when user inputs partial word
bool search_keyword_point_of_interest(int POI_index, string keyword){
    std::string poi_name_lower;
    std::string keyword_lower;
    
    //first convert poi names and input string to lower cases
    poi_name_lower = str_tolower(getPointOfInterestName(POI_index));
    keyword_lower = str_tolower(keyword);

    //do the comparison
    std::size_t found_name = poi_name_lower.find(keyword_lower);
    if (found_name != std::string::npos) return true;
    else return false;
}

//function to draw the point of interest that are found
void draw_point_of_interest(ezgl::renderer &g, string keyword){

    bool keywordFound = false;
    int zoom = find_zoom_level(g);

    g.set_text_rotation(0);
    
     //calculation to find the length and width of a single pixel
    double pixel_width = width/width_in_pixels;
    double pixel_height = height/height_in_pixels;  
     
     //get the bounds of the window
    ezgl::point2d bottom_left = g.get_visible_world().bottom_left();
    ezgl::point2d top_right = g.get_visible_world().top_right();
    
        //traverse all points of interest to search
    for(unsigned i = 0; i < pointOfInterest_position.size(); i++){
        keywordFound = search_keyword_point_of_interest(i, keyword);

        //search word found, but the map will only draw these points when it's zoomed in
        if( /*(count >= 9) && */(keywordFound) && (zoom >= 2)){  
            //png size: width-->40 pixels, height-->40 pixels                 
            //need to move up 40 pixels and move left 20 pixels

            //first convert LatLon to xy, then move the png image
            double x = convert_x_from_lon(pointOfInterest_position[i].lon()); 
            double y = convert_y_from_lat(pointOfInterest_position[i].lat());
            double png_x = x - 20*pixel_width; 
            double png_y = y + 40*pixel_height; 

            ezgl::point2d point_center(x,y);
            ezgl::point2d png_coord(png_x,png_y);
            
            //check if poi is in window bound
            //only draw the points that are inside the bound
            if((png_coord.x > bottom_left.x) && (png_coord.x < top_right.x) &&
               (png_coord.y > bottom_left.y) && (png_coord.y < top_right.y)){
               
                ezgl::surface *png_surface;
                png_surface = ezgl::renderer::load_png("libstreetmap/resources/map_marker2.png");
                g.draw_surface(png_surface, png_coord);
                ezgl::renderer::free_surface(png_surface);
                g.set_font_size(14);
                g.set_color(ezgl::BLACK);
                g.draw_text (point_center, getPointOfInterestName(i));
            }
        }
    }
}

//function to display some points of interest when the program is first started
//void display_poi_by_default(ezgl::renderer &g){
//    int zoom = find_zoom_level(g);
//    int count = 19;
//    g.set_text_rotation(0);
//    
//    for(unsigned i = 0; i < pointOfInterest_position.size(); i++){
//
//        if( (count >= 19) && (zoom >= 4)){  //png size: width-->30 pixels, height-->30 pixels
//            double x = convert_x_from_lon(pointOfInterest_position[i].lon());
//            double y = convert_y_from_lat(pointOfInterest_position[i].lat());
//            ezgl::point2d point_center(x,y);
//            ezgl::point2d png_coord(x-0.0000015,y+0.0000025);
//            ezgl::surface *png_surface;
//            
//            //first check their types
//            if(getPointOfInterestType(i)=="hospital" || getPointOfInterestType(i)=="pharmacy" ||
//               getPointOfInterestType(i)=="ambulance_station" || getPointOfInterestType(i)=="dentist" ||
//               getPointOfInterestType(i)=="doctors")
//                png_surface = ezgl::renderer::load_png("libstreetmap/resources/hospital4.png");
//            else if((getPointOfInterestType(i)=="fast_food") || (getPointOfInterestType(i)=="cafe") || (getPointOfInterestType(i)=="restaurant") )
//                png_surface = ezgl::renderer::load_png("libstreetmap/resources/restaurant6.png");
//            else 
//            png_surface = ezgl::renderer::load_png("libstreetmap/resources/map_marker2.png");
//            
//            //draw the points and also display their names
//            g.draw_surface(png_surface, png_coord);
//            ezgl::renderer::free_surface(png_surface);
//            g.set_font_size(14);
//            g.set_color(ezgl::BLACK);
//            g.draw_text (point_center, getPointOfInterestName(i));
//            count = 0;
//        }
//
//        count = count +1;
//
//    }
//}



//the searching bar
//void draw_searching_bar(ezgl::renderer &g){
//    //set the hight and width of the bar 
//    //change the size of the bar according to the view
//    ezgl::rectangle rect = g.get_visible_world();
//    double height = rect.height() * 0.03;
//    double width = rect.width() * 0.5;
//    
//    //set the left bottom position of the searching bar
//    ezgl::point2d start(rect.bottom_left().x + rect.height() * 0.0001, rect.bottom_left().y + rect.height()*0.97);
//    g.set_line_width(1);
//    //set the border of the bar with color black
//    g.set_color(ezgl::GREY_75);
//    ezgl::rectangle searchingBar(start, width, height);
//    //put the value of x y position in global variables
//    //these variables are used in mouse.cpp to check user click position
//    searchingBarBotLeft_x = searchingBar.bottom_left().x;
//    searchingBarBotLeft_y = searchingBar.bottom_left().y;
//    searchingBarTopRight_x = searchingBar.top_right().x;
//    searchingBarTopRight_y = searchingBar.top_right().y;
//
//    //searchingBar.
//    g.draw_rectangle(searchingBar);
//    //inner color of the bar is grey
//    g.set_color(ezgl::GREY_75);
//    g.fill_rectangle(start, width, height);
//    
//    //the text inside the searching bar
//    ezgl::point2d searchingTextPosition(searchingBar.center_x(), searchingBar.center_y());
//    g.set_color(ezgl::GREY_55);
//    g.draw_text(searchingTextPosition, "Search point of interest here", 11, 13);
//
//    if(searchingBarClicked){
//        g.draw_rectangle(searchingBar);
//        g.set_color(ezgl::GREY_75);
//        g.fill_rectangle(start, width, height);
//        //the searching bar is empty
//        if(searchingText.length() == 0 && searchingBarTyped == false){
//            g.set_color(ezgl::GREY_55);
//            g.draw_text(searchingTextPosition, "Enter the POI, features name (type quit to exit)", 11, 13);
//        }
//        if(searchingBarTyped){
//            //user has input in the searching bar
//            g.set_color(ezgl::BLACK);
//            g.set_font_size(13);
//            //limit the size of the input string
//            if(searchingText.length() >= 50){
//                string subString = searchingText.substr(searchingText.length() - 50, 50);
//                g.draw_text(searchingTextPosition, subString, 13, 13);
//            }
//            else{
//                
//                //if the user input is space
//                if(keyCode == 65){
//                    searchingText = searchingText + " ";
//                    g.draw_text(searchingTextPosition, searchingText, 13, 13);
//                }
//                //if the user input is return
//                else if(keyCode == 36){
//                    //stop entering the string, clear the bar, start searching
//                    //display the default searching bar
//                    g.set_color(ezgl::GREY_55);
//                    g.draw_text(searchingTextPosition,"Enter the POI, features name (type quit to exit)", 11, 13);
//                    searchingInfo = searchingText;
//                    startSearching = true;
//                    searchingText.clear();
//                }
//                //if the user input is backspace
//                else if(keyCode == 22){
//                    //if the string is empty
//                    if(searchingText.length() == 0){
//                        g.set_color(ezgl::GREY_55);
//                        g.draw_text(searchingTextPosition, "Enter the POI, features name (type quit to exit)", 11, 13);
//                    }
//                    else{
//                        searchingText.pop_back();
//                        if(searchingText.length() == 0){
//                            g.set_color(ezgl::GREY_55);
//                            g.draw_text(searchingTextPosition, "Enter the POI, features name (type quit to exit)", 11, 13);
//                        }
//                        else
//                            g.draw_text(searchingTextPosition, searchingText, 13, 13);
//                    } 
//                }
//                //if the user input is shift + dash(in the middle or lower dash)
//                else if(keyCode == 50){
//                    //std::cout << ifShiftPressed << std::endl;
//                    previousPressedChar = 50;
//                }
//                //if the user input is middle dash (-)
//                else if(keyCode == 20){
//                    //std::cout << ifShiftPressed << std::endl;
//                    //if the user did not press shift before
//                    //std::cout << previousPressedChar << std::endl;
//                    if(previousPressedChar == 50){
//                        searchingText = searchingText + "_";
//                        g.draw_text(searchingTextPosition, searchingText, 13, 13);
//                        previousPressedChar = 0;
//                    }
//                    else{
//                        searchingText = searchingText + "-";
//                        g.draw_text(searchingTextPosition, searchingText, 13, 13);
//                    }
//                }
//                //if the user input is comma (59)
//                else if(keyCode == 59){
//                    searchingText = searchingText + ",";
//                    g.draw_text(searchingTextPosition, searchingText, 13, 13);
//                }
//                else{
//                    //put the input char in the string, concat each char
//                    searchingText = searchingText + everyChar;
//                    //everyChar = NULL;
//                    //replace the default string
//                    g.draw_text(searchingTextPosition, searchingText, 13, 13);
//                }
//                
//            }
//            //need a button
//        }
//    }
//}


//the mapName searchingBar
//allow the user to input the mapName and load it
//void draw_searching_mapName_bar(ezgl::renderer &g){
//    g.set_text_rotation(0);
//    //same procedure as drawing the top searching bar
//    //set the hight and width of the bar 
//    //change the size of the bar according to the view
//    ezgl::rectangle rect = g.get_visible_world();
//    double height = rect.height() * 0.03;
//    double width = rect.width() * 0.3;
//    
//    //set the left bottom position of the searching bar
//    ezgl::point2d start(rect.bottom_left().x + rect.height() * 0.0001, rect.bottom_left().y);
//    g.set_line_width(1);
//    //set the border of the bar with color black
//    g.set_color(ezgl::GREY_75);
//    ezgl::rectangle searchingBar(start, width, height);
//    //put the value of x y position in global variables
//    //these variables are used in mouse.cpp to check user click position
//    mapNameBarBotLeft_x = searchingBar.bottom_left().x;
//    mapNameBarBotLeft_y = searchingBar.bottom_left().y;
//    mapNameBarTopRight_x = searchingBar.top_right().x;
//    mapNameBarTopRight_y = searchingBar.top_right().y;
//
//    //searchingBar.
//    g.draw_rectangle(searchingBar);
//    //inner color of the bar is grey
//    g.set_color(ezgl::GREY_75);
//    g.fill_rectangle(start, width, height);
//    
//    //the text inside the searching bar
//    ezgl::point2d searchingTextPosition(searchingBar.center_x(), searchingBar.center_y());
//    g.set_color(ezgl::GREY_55);
//    g.draw_text(searchingTextPosition, "Enter the map name: ", 11, 13);
//    
//    if(mapNameBarClicked){
//        g.draw_rectangle(searchingBar);
//        g.set_color(ezgl::GREY_75);
//        g.fill_rectangle(start, width, height);
//        //the searching bar is empty
//        if(mapNameText.length() == 0 && mapNameBarTyped == false){
//            g.set_color(ezgl::GREY_55);
//            g.draw_text(searchingTextPosition, "Enter the map name: ", 11, 13);
//        }
//        if(mapNameBarTyped){
//            //user has input in the searching bar
//            g.set_color(ezgl::BLACK);
//            g.set_font_size(13);
//            //limit the size of the input string
//            if(mapNameText.length() >= 40){
//                string subString = mapNameText.substr(mapNameText.length() - 40, 40);
//                g.draw_text(searchingTextPosition, subString, 13, 13);
//            }
//            else{
//                //if the user input is space
//                if(keyCodeMapName == 65){
//                    mapNameText = mapNameText + " ";
//                    g.draw_text(searchingTextPosition, mapNameText, 13, 13);
//                }
//                //if the user input is return
//                else if(keyCodeMapName == 36){
//                    //stop entering the string, clear the bar, start searching
//                    //display the default searching bar
//                    g.set_color(ezgl::GREY_55);
//                    g.draw_text(searchingTextPosition, "Enter the map name: ", 11, 13);
//                    mapNameStored = mapNameText;
//                    mapNameLoaded = true;
//                    mapNameText.clear();
//                }
//                //if the user input is backspace
//                else if(keyCodeMapName == 22){
//                    //if the string is empty
//                    if(mapNameText.length() == 0){
//                        g.set_color(ezgl::GREY_55);
//                        g.draw_text(searchingTextPosition, "Enter the map name: ", 11, 13);
//                    }
//                    else{
//                        mapNameText.pop_back();
//                        if(mapNameText.length() == 0){
//                            g.set_color(ezgl::GREY_55);
//                            g.draw_text(searchingTextPosition, "Enter the map name: ", 11, 13);
//                        }
//                        else
//                            g.draw_text(searchingTextPosition, mapNameText, 13, 13);
//                    } 
//                }
//                //if the user input is shift + dash(in the middle or lower dash)
//                else if(keyCodeMapName == 50){
//                    //std::cout << ifShiftPressed << std::endl;
//                    previousPressedMapName = 50;
//                }
//                //if the user input is middle dash (-)
//                else if(keyCodeMapName == 20){
//                    //std::cout << ifShiftPressed << std::endl;
//                    //if the user did not press shift before
//                    if(previousPressedMapName == 50){
//                        mapNameText = mapNameText + "_";
//                        g.draw_text(searchingTextPosition, mapNameText, 13, 13);
//                        previousPressedMapName = 0;
//                    }
//                    else{
//                        mapNameText = mapNameText + "-";
//                        g.draw_text(searchingTextPosition, mapNameText, 13, 13);
//                    }
//                }
//                //if the user input is comma (59)
//                else if(keyCodeMapName == 59){
//                    mapNameText = mapNameText + ",";
//                    g.draw_text(searchingTextPosition, mapNameText, 13, 13);
//                }
//                else{
//                    //put the input char in the string, concat each char
//                    mapNameText = mapNameText + everyCharMapName;
//                    //everyCharMapName = NULL;
//                    //replace the default string
//                    g.draw_text(searchingTextPosition, mapNameText, 13, 13);
//                }
//            }
//            //need a button
//        }
//    }
//}



//ignore the blank spaces in the strings
std::string ignore_blank(std::string input){
    for (int i = input.length()-1; i >= 0;--i){ 
     if(input[i] == ' ')
         input.erase(i, 1); 
    }
    return input;
}


//print all intersections on both streets
//void draw_intersections_by_two_streets_name(ezgl::renderer &g, std::string twoStreetNames){
//    std::string name1;
//    std::string name2;
//    std::vector<unsigned> street_one_ids;
//    std::vector<unsigned> street_two_ids;
//    std::size_t locationToSeparate = 0;
//    std::vector<unsigned> all_intersections;
////    int zoom = find_zoom_level(g);
//    
//    //if the string is not empty
//    if(twoStreetNames.length() != 0){
//        //comma is found
//        if(twoStreetNames.find(",") != std::string::npos){
//            locationToSeparate = twoStreetNames.find(",");
//        }
//        //use & to search
//        else if (twoStreetNames.find("&") != std::string::npos){
//            locationToSeparate = twoStreetNames.find("&");
//        }
//        //blank space in front and after the string
//        //can only separate 'a' not 'and'
////        else if (twoStreetNames.find("and") != std::string::npos){
////            locationToSeparate = twoStreetNames.find("and");
////        }
//        if(locationToSeparate != std::string::npos){
//            //start searching intersections
//            name1 = twoStreetNames.substr(0, locationToSeparate);
////            name1 = ignore_blank(name1);
//            name2 = twoStreetNames.substr(locationToSeparate + 1);
////            name2 = ignore_blank(name2);
//            std::cout << name1 << std::endl;
//            std::cout << name2 << std::endl;
//            //for one street
//            street_one_ids = find_street_ids_from_partial_street_name(name1);
//            street_two_ids = find_street_ids_from_partial_street_name(name2);
////            for(int i = 0; i < street_one_ids.size(); i++){
////                std::cout << street_one_ids[i] << "!!!!!!!!!!!!!!!!!!" << std::endl;
////            }
//            //if the two street names are valid
//            if(street_one_ids.size() != 0){
//                if(street_two_ids.size() != 0){
//                    //two valid input names
////                        std::vector<unsigned> find_intersection_ids_from_street_ids(unsigned street_id1, 
////                                                              unsigned street_id2);
//                    for(unsigned i = 0; i < street_one_ids.size(); i++){
//                        for(unsigned j = 0; j < street_two_ids.size(); j++){
//                            std::vector<unsigned> store = find_intersection_ids_from_street_ids
//                                                        (street_one_ids[i], street_two_ids[j]);
////                            std::cout <<  << std::endl;
//                            for(unsigned k = 0; k < store.size(); k++){
////                                std::cout << getIntersectionName(store[k]) << std::endl;
//                                all_intersections.push_back(store[k]);
////                                std::cout << all_intersections[k] << "----------" << std::endl;
//                            }
//                        }
//                    }
//                    //highlight intersections
//                    //deal with duplicated
//                    for(IntersectionIndex i = 0; i < all_intersections.size(); i++){
////                        std::cout << getIntersectionName(all_intersections[i]) << "--------" << std::endl;
//                        g.set_color(ezgl::MAGENTA);
//                        //position of point need to highlighted
//                        ezgl::point2d point(convert_x_from_lon(getIntersectionPosition(all_intersections[i]).lon()), 
//                                convert_y_from_lat(getIntersectionPosition(all_intersections[i]).lat()));
//                        //make the radius of the intersection remains the same with the view
//                        ezgl::rectangle currentView = g.get_visible_world();
//                        double height = currentView.height() * 0.02;
//                        g.fill_arc(point, height, 0, 360);
//                        
////                        if(zoom >= 9){
////                            g.fill_arc(point,0.000002, 0, 360);
////                        }
////                        else if(zoom >= 7/*0.000005*/){
////                            g.fill_arc(point,0.000005, 0, 360);
////                        }
////                        else if(zoom >= 4/*0.00001*/){
////                            g.fill_arc(point,0.00002, 0, 360);
////                        }
////                        else if(zoom >= 2){
////                            g.fill_arc(point,0.00005, 0, 360);
////                        }
////                        else if(zoom >= 1){
////                            g.fill_arc(point,0.00015, 0, 360);
////                        }
//                    }
//                }
//                else
//                    std::cout << "The first name is invalid" << std::endl;
//            }
//            else
//                std::cout << "The first name is invalid" << std::endl;
//        }
//    }
//    else
//        std::cout<< "----Input string is empty----" << std::endl;
//}

//this function is to find the intersection id when user clicking on it
int find_intersectionID_by_data(intersection_data clickedIntersection){
//    std::cout << intersection_info.size() << std::endl;
    for(unsigned i = 0; i < intersection_info.size(); i++){
        //match with the vector to find the id
        if(intersection_info[i].position.lat() == clickedIntersection.position.lat() && 
                intersection_info[i].position.lon() == clickedIntersection.position.lon()){
//            std::cout << intersection_info[i].intersectionID << "+++++++++" << std::endl;
            return intersection_info[i].intersectionID;
        }
    }
    return 0;
}

//re-implemented version of draw_intersection function to adapt to the new find feature in ui
void draw_intersections_by_two_streets_name(ezgl::renderer &g, std::string name1, std::string name2){
    std::vector<unsigned> street_one_ids;
    std::vector<unsigned> street_two_ids;
//    std::size_t locationToSeparate = 0;
    std::vector<unsigned> all_intersections;
    
    //if the string is not empty
    if((name1.length()!=0) && (name2. length()!=0)){

//            std::cout << name1 << std::endl;
//            std::cout << name2 << std::endl;
            //for one street
            street_one_ids = find_street_ids_from_partial_street_name(name1);
            street_two_ids = find_street_ids_from_partial_street_name(name2);

            //if the two street names are valid
            if(street_one_ids.size() != 0){
                if(street_two_ids.size() != 0){

                    for(unsigned i = 0; i < street_one_ids.size(); i++){
                        for(unsigned j = 0; j < street_two_ids.size(); j++){
                            std::vector<unsigned> store = find_intersection_ids_from_street_ids
                                                        (street_one_ids[i], street_two_ids[j]);
//                            std::cout <<  << std::endl;
                            for(unsigned k = 0; k < store.size(); k++){
//                                std::cout << getIntersectionName(store[k]) << std::endl;
                                all_intersections.push_back(store[k]);
//                                std::cout << all_intersections[k] << "----------" << std::endl;
                            }
                        }
                    }
                    //highlight intersections
                    //deal with duplicated
                    for(unsigned i = 0; i < all_intersections.size(); i++){
//                        std::cout << getIntersectionName(all_intersections[i]) << "--------" << std::endl;
                        g.set_color(ezgl::MAGENTA);
                        //position of point need to highlighted
                        ezgl::point2d point(convert_x_from_lon(getIntersectionPosition(all_intersections[i]).lon()), 
                                convert_y_from_lat(getIntersectionPosition(all_intersections[i]).lat()));
                        //make the radius of the intersection remains the same with the view
                        ezgl::rectangle currentView = g.get_visible_world();
                        double height_point = currentView.height() * 0.02;
                        g.fill_arc(point, height_point, 0, 360);
                        
                    }
                }
                else{
                    std::cout << "The second name is invalid" << std::endl;
//                    all_intersections.clear();
                }
            }
            else{
                std::cout << "The first name is invalid" << std::endl;
//                all_intersections.clear();
            }
    }
    else
        std::cout<< "----Input string is empty----" << std::endl;
}

//intersection_path find_intersection_info_by_two_street_names(std::string name1, std::string name2){
//    std::vector<unsigned> street_one_ids;
//    std::vector<unsigned> street_two_ids;
//    std::vector<unsigned> all_intersections;
////    int intersection_searched;
//    
//    //if the string is not empty
//    if((name1.length()!=0) && (name2. length()!=0)){
//        street_one_ids = find_street_ids_from_partial_street_name(name1);
//        street_two_ids = find_street_ids_from_partial_street_name(name2);
//
//        //if the two street names are valid
//        if(street_one_ids.size() != 0){
//            if(street_two_ids.size() != 0){
//                for(unsigned i = 0; i < street_one_ids.size(); i++){
//                    for(unsigned j = 0; j < street_two_ids.size(); j++){
//                        std::vector<unsigned> store = find_intersection_ids_from_street_ids
//                                                    (street_one_ids[i], street_two_ids[j]);
////                            std::cout <<  << std::endl;
//                        for(unsigned k = 0; k < store.size(); k++){
////                                std::cout << getIntersectionName(store[k]) << std::endl;
//                            all_intersections.push_back(store[k]);
////                            intersection_searched = store[k];
////                            break;
//                                std::cout << all_intersections[k] << "==========" << std::endl;
//                        }
//                    }
//                }
//                
//                //highlight intersections
//                //deal with duplicated
////                for(IntersectionIndex i = 0; i < all_intersections.size(); i++){
//////                        std::cout << getIntersectionName(all_intersections[i]) << "--------" << std::endl;
////                    g.set_color(ezgl::MAGENTA);
////                    //position of point need to highlighted
////                    ezgl::point2d point(convert_x_from_lon(getIntersectionPosition(all_intersections[i]).lon()), 
////                            convert_y_from_lat(getIntersectionPosition(all_intersections[i]).lat()));
////                    //make the radius of the intersection remains the same with the view
////                    ezgl::rectangle currentView = g.get_visible_world();
////                    double height = currentView.height() * 0.02;
////                    g.fill_arc(point, height, 0, 360);
////
////                }
//                intersection_path infoOfClickedIntersection;
//                int intersectionID = all_intersections[2];
//                std::cout << all_intersections[1] << "+++++++++++++" << std::endl;
//                infoOfClickedIntersection.intersectionID = intersectionID;
//                infoOfClickedIntersection.name = getIntersectionName(intersectionID);
//                infoOfClickedIntersection.position = getIntersectionPosition(intersectionID);
//                return infoOfClickedIntersection;
////                intersection_path infoOfClickedIntersection;
////                infoOfClickedIntersection.intersectionID = intersection_searched;
////                infoOfClickedIntersection.name = getIntersectionName(intersection_searched);
////                infoOfClickedIntersection.position = getIntersectionPosition(intersection_searched);
////                return infoOfClickedIntersection;
//            }
//            else
//                std::cout << "The second name is invalid" << std::endl;
//        }
//        else
//            std::cout << "The first name is invalid" << std::endl;
//    }
//    else
//        std::cout<< "----Input string is empty----" << std::endl;
////    return NULL;
//}


int find_intersection_id_by_two_streets_name(std::string name1, std::string name2){
    std::vector<unsigned> street_one_ids;
    std::vector<unsigned> street_two_ids;
//    std::size_t locationToSeparate = 0;
    std::vector<unsigned> all_intersections;
    
    //if the string is not empty
    if((name1.length()!=0) && (name2. length()!=0)){

//            std::cout << name1 << std::endl;
//            std::cout << name2 << std::endl;
            //for one street
            street_one_ids = find_street_ids_from_partial_street_name(name1);
            street_two_ids = find_street_ids_from_partial_street_name(name2);

            //if the two street names are valid
            if(street_one_ids.size() != 0){
                if(street_two_ids.size() != 0){

                    for(unsigned i = 0; i < street_one_ids.size(); i++){
                        for(unsigned j = 0; j < street_two_ids.size(); j++){
                            std::vector<unsigned> store = find_intersection_ids_from_street_ids
                                                        (street_one_ids[i], street_two_ids[j]);
//                            std::cout <<  << std::endl;
                            for(unsigned k = 0; k < store.size(); k++){
//                                std::cout << getIntersectionName(store[k]) << std::endl;
                                all_intersections.push_back(store[k]);
//                                std::cout << all_intersections[k] << "----------" << std::endl;
                                return all_intersections[k];
                            }
                        }
                    }
                    //highlight intersections
                    //deal with duplicated
//                    for(IntersectionIndex i = 0; i < all_intersections.size(); i++){
////                        std::cout << getIntersectionName(all_intersections[i]) << "--------" << std::endl;
//                        g.set_color(ezgl::MAGENTA);
//                        //position of point need to highlighted
//                        ezgl::point2d point(convert_x_from_lon(getIntersectionPosition(all_intersections[i]).lon()), 
//                                convert_y_from_lat(getIntersectionPosition(all_intersections[i]).lat()));
//                        //make the radius of the intersection remains the same with the view
//                        ezgl::rectangle currentView = g.get_visible_world();
//                        double height = currentView.height() * 0.02;
//                        g.fill_arc(point, height, 0, 360);
//                        
//                    }
                }
                else{
                    std::cout << "The second name is invalid" << std::endl;
//                    all_intersections.clear();
                }
            }
            else{
                std::cout << "The first name is invalid" << std::endl;
//                all_intersections.clear();
            }
    }
    else{
        std::cout<< "----Input string is empty----" << std::endl;
    }
    return 0;
}


float round_double(float var){
    float value = (int)(var * 100 + .5);
    return (float)value / 100;
}