#include <uiButtons.h>
using namespace std;

int info_flag = 0;
extern bool find_POI;
extern bool find_street;
extern bool find_path_ui;
extern bool find_path;
extern string street1;
extern string street2;
extern string from_street_1;
extern string from_street_2;
extern string to_street_1;
extern string to_street_2;
extern string find_info;
extern unsigned intersection1;
extern unsigned intersection2;
extern double total_travel_time;
extern std::vector<unsigned> path;
extern std::vector<std::string> street_names;
extern void load_map_name(std::string mapName, ezgl::application *application);


/********************  below are callback functions to load new maps   ***************************/

void load_map_beijing(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    string loadMapName = "/cad2/ece297s/public/maps/beijing_china.streets.bin";
    load_map_name(loadMapName, application);
}

void load_map_hongkong(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    string loadMapName = "/cad2/ece297s/public/maps/hong-kong_china.streets.bin";
    load_map_name(loadMapName, application);
}

void load_map_toronto(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    string loadMapName = "/cad2/ece297s/public/maps/toronto_canada.streets.bin";
    load_map_name(loadMapName, application);
}

void load_map_newyork(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    string loadMapName = "/cad2/ece297s/public/maps/new-york_usa.streets.bin";
    load_map_name(loadMapName, application);
}

void load_map_london(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    string loadMapName = "/cad2/ece297s/public/maps/london_england.streets.bin";
    load_map_name(loadMapName, application);
}

void load_map_sydney(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    string loadMapName = "/cad2/ece297s/public/maps/sydney_australia.streets.bin";
    load_map_name(loadMapName, application);
}

void load_map_singapore(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    string loadMapName = "/cad2/ece297s/public/maps/singapore.streets.bin";
    load_map_name(loadMapName, application);
}

void load_map_cairo(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    string loadMapName = "/cad2/ece297s/public/maps/cairo_egypt.streets.bin";
    load_map_name(loadMapName, application);
}

void load_map_iceland(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    string loadMapName = "/cad2/ece297s/public/maps/iceland.streets.bin";
    load_map_name(loadMapName, application);
}

void load_map_tokyo(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    string loadMapName = "/cad2/ece297s/public/maps/tokyo_japan.streets.bin";
    load_map_name(loadMapName, application);
}

/***********************************************************************************************/





/****************** below are callback functions for the new buttons created ************************/

//callback function to send signal to quit finding
void quit_find(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    //update message when test button pressed
    application->update_message("Quit Find Button Pressed");
    find_street = false;
    find_POI = false;
    find_path = false;
    find_path_ui = false;
    intersection1 = 0;
    intersection2 = 0;
    application->refresh_drawing();
}

//callback function to display the associated find poi window
void search_poi(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    //update the status bar message
    application->update_message("POI Button Pressed");

    //get a pointer to point to the search poi window, and have it displayed
    GtkWindow *poi_window = (GtkWindow*) application->get_object("POIwindow");
    gtk_window_present (poi_window);
}

//callback function to display the associated find intersection window
void search_intersection(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    //update the status bar message
    application->update_message("Intersection Button Pressed");
    //get a pointer point to the new window and have it displayed
    GtkWindow *intersection_window = (GtkWindow*) application->get_object("IntersectionWindow");
    gtk_window_present(intersection_window);
}

//callback function to display the associated find path window
void display_find_path(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    //update the status bar message
    application->update_message("Path Button Pressed");
    //get a pointer to point to the new window and have it displayed
    GtkWindow *path_message_window = (GtkWindow *) application->get_object("PathMessageWindow");
    gtk_window_present (path_message_window);
}

//callback function to set the flag for finding poi
//and close the current find poi window
void find_poi_go(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    // Get the GtkEntry object
    GtkEntry* text_entry = (GtkEntry *) application->get_object("SearchEntryPOI");
    // Get the text written in the widget
    const char *text = gtk_entry_get_text(text_entry);
    find_info = text;
    // Update the status bar message
    application->update_message(text);
    // Redraw the graphics
    application->refresh_drawing();
    
    //update message when test button pressed
    application->update_message("Search Button pressed.");
    //set the flag
    find_POI = true;
    //close the window
    GtkWidget *poi_window = (GtkWidget*) application->get_object("POIwindow");
    gtk_widget_hide (poi_window);
    application->refresh_drawing();
}

//callback function to set the flag for finding intersections
//and close the current find intersection window
void find_intersection_go(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    // Get the GtkEntry object
    GtkEntry* text_entry1 = (GtkEntry *) application->get_object("SearchStreet1");
    GtkEntry* text_entry2 = (GtkEntry *) application->get_object("SearchStreet2");
    // Get the text written in the widget
    const char *street_1 = gtk_entry_get_text(text_entry1);
    const char *street_2 = gtk_entry_get_text(text_entry2);
    street1 = street_1;
    street2 = street_2;
    
    //update message when test button pressed
    application->update_message("Go button pressed.");
    //set the flag to true
    find_street = true;
    //close the window
    GtkWidget *intersection_window = (GtkWidget*) application->get_object("IntersectionWindow");
    gtk_widget_hide (intersection_window);
    application->refresh_drawing();
}

//callback function to set the flag for finding path, and close the current find path window
void find_path_go(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    // Get the GtkEntry object
    GtkEntry* text_from1 = (GtkEntry *) application->get_object("FromStreet1");
    GtkEntry* text_from2 = (GtkEntry *) application->get_object("FromStreet2");
    GtkEntry* text_to1 = (GtkEntry *) application->get_object("ToStreet1");
    GtkEntry* text_to2 = (GtkEntry *) application->get_object("ToStreet2");
    //Get the street names for finding path between two intersections
    const char *from_1 = gtk_entry_get_text(text_from1);
    const char *from_2 = gtk_entry_get_text(text_from2);
    const char *to_1 = gtk_entry_get_text(text_to1);
    const char *to_2 = gtk_entry_get_text(text_to2);
    from_street_1 = from_1;
    from_street_2 = from_2;
    to_street_1 = to_1;
    to_street_2 = to_2;
    
    //update message when test button pressed
    application->update_message("Go button pressed.");
    //set the flag to find path
    find_path_ui = true;
    //close the window to display path
    GtkWidget *find_path_window = (GtkWidget*) application->get_object("FindPathWindow");
    gtk_widget_hide (find_path_window);
    application->refresh_drawing();
}

//callback function to close the find poi window
void find_poi_cancel(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    //update the status bar
    application->update_message("Find point of interest window closed");
    //close the window
    GtkWidget *poi_window = (GtkWidget*) application->get_object("POIwindow");
    gtk_widget_hide (poi_window);
    application->refresh_drawing();
}

//callback function to close the find intersection window
void find_intersection_cancel(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    //update the status bar
    application->update_message("Find intersection window closed");
    //close the window
    GtkWidget *intersection_window = (GtkWidget*) application->get_object("IntersectionWindow");
    gtk_widget_hide (intersection_window);
    application->refresh_drawing();
}

//callback function to close the find path window
void find_path_cancel(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    //update the status bar
    application->update_message("Find path window closed");
    //close the window
    GtkWidget *find_path_window = (GtkWidget*) application->get_object("FindPathWindow");
    gtk_widget_hide (find_path_window);
    application->refresh_drawing();
}

//callback function to enable user to click on intersections to find path
void click_on_map(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    //update the status bar
    application->update_message("Click on map option chose");
    //first close the message window
    GtkWidget *path_message_window = (GtkWidget *) application->get_object("PathMessageWindow");
    gtk_widget_hide (path_message_window);
    //set the find path flag to true
    find_path = true;
}

//callback function to display the find path window to get intersection names
void enter_intersections(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    //update the status bar
    application->update_message("Enter intersections option chose");
    //first close the message window
    GtkWidget *path_message_window = (GtkWidget *) application->get_object("PathMessageWindow");
    gtk_widget_hide (path_message_window);
    //then display the find path window
    GtkWindow *find_path_window = (GtkWindow *) application->get_object("FindPathWindow");
    gtk_window_present (find_path_window);
}

//callback function to display the Help window
void show_help(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    //update the status bar
    application->update_message("About menu clicked");
    //show the overlay item
    GtkWindow *help = (GtkWindow *) application->get_object("HelpWindow");
    gtk_window_present (help);
    
}

//callback function to close the Help window
void close_help(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    //update the status bar
    application->update_message("Help window closed");
    //close the help window
    GtkWidget *help_window = (GtkWidget *) application->get_object("HelpWindow");
    gtk_widget_hide(help_window);
}

//callback function to display the travel information when user clicks Travel Info button
void display_travel_info(GtkWidget *widget, ezgl::application* application){
    widget = widget;
    const gchar *info_text;
    string from_intersection;
    string to_intersection;
    string street_name;
    string turn_type;
    string str;
    string intersection_2;
    double seg_length = 0;
    double seg_length_accum = 0;
    int seg_time = static_cast<int> (total_travel_time);
    bool no_turn = false;
    bool street_straight;
    
    //set the flag to determine whether to close or open the info
    info_flag = (info_flag +1) % 2;
    //finding path, gather travel info and ready for display
    if(find_path || find_path_ui){
        from_intersection = getIntersectionName(intersection1);
        to_intersection = getIntersectionName(intersection2);
        
        //display an overview of the path
        //starting point, destination, and total travel time
        str = "From:   " + from_intersection + "\n";
        str = str + "To:   " + to_intersection + "\n";
        str = str + "Travel time:   " + std::to_string(seg_time) + " min \n \n";
        
        //for loop to check if there's a turn
        for (unsigned i=0; i<path.size(); i++){
            unsigned seg_id = path[i];
            unsigned intersect2ID = getInfoStreetSegment(seg_id).to; 
            unsigned street_id = getInfoStreetSegment(seg_id).streetID;
            street_name = getStreetName(street_id);
            seg_length = find_street_segment_length(seg_id);
            
            //not reached the last street segment
            //check the turn type
            if(i < (path.size()-1)){
                unsigned seg_id_2 = path[i+1];
                unsigned street_id_2 = getInfoStreetSegment(seg_id_2).streetID;
                street_straight = seg_is_straight(seg_id, seg_id_2);
                
                //compare both the street ids and the angle between two street segments
                if(street_id == street_id_2 || street_straight){
                    //same street, go straight
                    seg_length_accum = seg_length_accum + seg_length;
                    no_turn = true;
                }
                else{ //need to take a turn, get the direction to turn
                    no_turn = false;
                    intersection_2 = getIntersectionName(intersect2ID);
                    seg_length_accum = seg_length_accum + seg_length;
                    if(find_turn_type(path[i], path[i+1]) == TurnType::LEFT)
                        turn_type = "turn left";
                    else if(find_turn_type(path[i], path[i+1]) == TurnType::RIGHT)
                        turn_type = "turn right";
                }
                
                //not a straight street, display the street segment info
                //and turn direction first
                if(!no_turn){
                    int length = static_cast<int>(seg_length_accum);
                    str = str + "Go along " + street_name + " for "
                            + std::to_string(length) + "m \n"
                            + "Then " + turn_type + " at " 
                            + intersection_2 + "\n \n";
                    seg_length_accum = 0;
                }
                
            }
            else{//gone through an entire straight street, display the info
                seg_length_accum = seg_length_accum + seg_length;
                int length = static_cast<int> (seg_length_accum);
                str = str + "Go along " + street_name + " for "
                            + std::to_string(length) + "m \n \n"
                            + "Arrived at destination \n";
            }
            
        }
        
        info_text = str.c_str();
    }
    else //this is for when not in find path mode
        info_text = " Please select two intersections to begin ";
    
    //get pointers to point to Gtk widgets, and show the pop up menu
    GtkTextBuffer *info_buffer = (GtkTextBuffer *) application->get_object("InfoBuffer");
    gtk_text_buffer_set_text(info_buffer, info_text, -1);
    GtkPopover *popover = (GtkPopover *) application->get_object("ScrollablePop");
    if(info_flag == 1){
        //update the status bar, and open the menu
        application->update_message("Displaying travel information");
        gtk_popover_popup(popover);
    } 
    else{
        //update the status bar, and close the menu
        application->update_message("Travel information closed");
        gtk_popover_popdown(popover);
    }
}



