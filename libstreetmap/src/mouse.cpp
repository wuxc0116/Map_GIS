#include <mouse.h>
#include <m2.h>
//#include <m2.cpp>
#include <basicFeature.h>

extern std::vector<intersection_data> intersections;
//extern std::vector<intersection_data> highlight_intersection;
extern intersection_data highlight_point;
extern std::vector<intersection_data> highlight_intersection_text;
extern std::vector<num_click_intersection> count_clicked_intersection; 
extern bool show_info_box;
extern LatLon previousClicked;
extern bool find_path;
extern std::vector<intersection_path> from_intersection_to;
extern std::vector<unsigned> intersectionID_clicked;
int count_click = 0;
extern double height;
extern unsigned clickedIntersection_1;
extern unsigned clickedIntersection_2;
//extern bool searchingBarClicked;
//extern bool mapNameBarClicked;
//extern bool searchingBarTyped;
//extern bool mapNameBarTyped;
//extern double searchingBarBotLeft_x;
//extern double searchingBarBotLeft_y;
//extern double searchingBarTopRight_x;
//extern double searchingBarTopRight_y;
//extern double mapNameBarBotLeft_x;
//extern double mapNameBarBotLeft_y;
//extern double mapNameBarTopRight_x;
//extern double mapNameBarTopRight_y;
//extern string searchingText;
//extern char everyChar;
//extern char everyCharMapName;
//extern int keyCode;
//extern int keyCodeMapName;
//extern string searchingInfo;
//extern bool startSearching;
//extern bool mapNameLoaded;
//extern std::string mapNameStored;
//extern void load_map_name(std::string mapName);
//extern int previousPressedChar;
//bool mouse_click=false;
//extern int mouse_click_count;

void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y)
{
    application->update_message("Mouse Clicked");
    event = event;

    //highlight the intersections when user click on it
    for(unsigned i = 0; i < intersections.size(); i++){
        if(find_path){
            double newHeight = height * 0.01;
//            std::cout << height << std::endl;
            if(convert_lon_from_x(x-newHeight) <= intersections[i].position.lon() 
               && convert_lon_from_x(x+newHeight) >= intersections[i].position.lon()){
                if(convert_lat_from_y(y-newHeight) <= intersections[i].position.lat() 
                   && convert_lat_from_y(y+newHeight) >= intersections[i].position.lat()){
                    
                    if(count_click >= 2){
                        count_click = 0;
                        clickedIntersection_1 = 0;
                        clickedIntersection_2 = 0;
                    }
                    //user click on the first intersection
                    if(count_click == 0){
                        clickedIntersection_1 = find_intersectionID_by_data(intersections[i]);
                        count_click = count_click + 1;
                        std::cout << "User click at intersection: " << intersections[i].name << std::endl;
                        break;
                    } 
                    //user click on the second intersection
                    if(count_click == 1){
                        clickedIntersection_2 = find_intersectionID_by_data(intersections[i]);
                        count_click = count_click + 1;
                        std::cout << "User click at intersection: " << intersections[i].name << std::endl;
                        break;
                    }
                }
            }        
        }
        else{
        //the click is valid in a range of radius
        if(convert_lon_from_x(x-0.000001) <= intersections[i].position.lon() 
           && convert_lon_from_x(x + 0.000001) >= intersections[i].position.lon()){
            if(convert_lat_from_y(y-0.000001) <= intersections[i].position.lat() 
               && convert_lat_from_y(y + 0.000001) >= intersections[i].position.lat()){
                //put the highlighted intersections into the vectors 
//                highlight_intersection.push_back(intersections[i]);
                
                highlight_point = intersections[i];
                std::cout << "User click at intersection: " << intersections[i].name << std::endl;
                //see if the intersection is highlighted previously
                bool flag = false;
                for(unsigned j = 0; j < count_clicked_intersection.size(); j++){
                    //the intersection is clicked before
                    if(count_clicked_intersection[j].position.lat() == highlight_point.position.lat() &&
                            count_clicked_intersection[j].position.lon() == highlight_point.position.lon()){
                        //did not click on previous one
                        if(previousClicked.lat() != highlight_point.position.lat() ||
                                previousClicked.lon() != highlight_point.position.lon()){
                            count_clicked_intersection[j].ifClick = true;
                            flag = true;
                            previousClicked = highlight_point.position;
                            break;
                        }
                        //the one clicked before
                        else{
                            if(count_clicked_intersection[j].ifClick == true){
                                count_clicked_intersection[j].ifClick = false;
                                flag = true;
                                previousClicked = highlight_point.position;
                                break;
                            }
                            else{
                                count_clicked_intersection[j].ifClick = true;
                                flag = true;
                                previousClicked = highlight_point.position;
                                break;
                            }
                        }

                    }
                }
                //the intersection is not clicked before
                if(flag == false){
                    num_click_intersection clicked_intersect;
                    clicked_intersect.position = highlight_point.position;
                    clicked_intersect.ifClick = true;
                    previousClicked = highlight_point.position;
                    count_clicked_intersection.push_back(clicked_intersect);
                }
            }
        }
        }
    }
    application->refresh_drawing();
    
}

/**
* Function to handle mouse move event
* The current mouse position in the main canvas' world coordinate system is returned
* A pointer to the application and the entire GDK event are also returned
*/
void act_on_mouse_move(ezgl::application *application, GdkEventButton *event, double x, double y)
{
    application = application;
    event = event;
    x = x;
    y = y;
    //    std::cout << "Mouse move at coordinates (" << x << "," << y << ") "<< std::endl;
}

/**
* Function to handle keyboard press event
* The name of the key pressed is returned (0-9, a-z, A-Z, Up, Down, Left, Right, Shift_R, Control_L, space, Tab, ...)
* A pointer to the application and the entire GDK event are also returned
*/
void act_on_key_press(ezgl::application *application, GdkEventKey *event, char *key_name)
{
    application->update_message("Key Pressed");
    
    application = application;
    event = event;
    key_name = key_name;

//    std::cout << *key_name <<" key is pressed" << std::endl;
    //std::cout << event->hardware_keycode << std::endl;
 
}