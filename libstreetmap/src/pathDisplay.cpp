#include <pathDisplay.h>
#include <math.h>

using namespace std;

//print out the shortest path according to user input
//"pathSegment" is a vector that stores the segment ID by order
void print_the_path(std::vector<unsigned> pathSegment, ezgl::renderer &g){
//    int zoom = find_zoom_level(g);
    // first get all segment on this street
    //draw a line between each segment
    for (unsigned i = 0; i < pathSegment.size(); i++){
//        std::cout << pathSegment[i] << std::endl;
            unsigned num_curve = getInfoStreetSegment(pathSegment[i]).curvePointCount;
            //the start and end point of the segment
            LatLon seg_start = getIntersectionPosition(getInfoStreetSegment(pathSegment[i]).from);
            LatLon seg_end = getIntersectionPosition(getInfoStreetSegment(pathSegment[i]).to);
            //do not need the distance in this part
//            int distance = find_distance_between_two_points(seg_start, seg_end);
            
            g.set_color(ezgl::BLUE);
            //use 4 actually
//            g.set_line_width(4);
            g.set_line_width(4);
//            ezgl::rectangle currentView = g.get_visible_world();
//            double lineWidth = currentView.width() * 5000;
//            std::cout << lineWidth << std::endl;
//            g.set_line_width(lineWidth);
           
           
            //draw the street seg according to the curve point number
            if(num_curve == 0){
                ezgl::point2d seg_start_alter  (convert_x_from_lon(seg_start.lon()), convert_y_from_lat(seg_start.lat()));
                ezgl::point2d seg_end_alter  (convert_x_from_lon(seg_end.lon()), convert_y_from_lat(seg_end.lat()));
                g.draw_line(seg_start_alter, seg_end_alter);
            }
            else if(num_curve == 1){
                LatLon inflection = getStreetSegmentCurvePoint(0, pathSegment[i]);
                ezgl:: point2d seg_start_alter (convert_x_from_lon(seg_start.lon()), convert_y_from_lat(seg_start.lat()));
                ezgl:: point2d seg_inflection_alter (convert_x_from_lon(inflection.lon()), convert_y_from_lat(inflection.lat()));
                ezgl::point2d seg_end_alter  (convert_x_from_lon(seg_end.lon()), convert_y_from_lat(seg_end.lat()));
                g.draw_line(seg_start_alter, seg_inflection_alter);
                g.draw_line(seg_inflection_alter, seg_end_alter); 
            }
            else{
                //first draw the first curve_segment
                LatLon inflection = getStreetSegmentCurvePoint(0, pathSegment[i]);
                ezgl:: point2d seg_start_alter (convert_x_from_lon(seg_start.lon()), convert_y_from_lat(seg_start.lat()));
                ezgl:: point2d seg_inflection_alter (convert_x_from_lon(inflection.lon()), convert_y_from_lat(inflection.lat()));
                g.draw_line(seg_start_alter, seg_inflection_alter);
                //then draw the rest
                for(unsigned j=0; j<num_curve-1; j++){
                    LatLon inflection_1 = getStreetSegmentCurvePoint(j, pathSegment[i]);
                    LatLon inflection_2 = getStreetSegmentCurvePoint(j+1, pathSegment[i]);
                    //more code are needed
                    ezgl:: point2d seg_inflection_1_alter (convert_x_from_lon(inflection_1.lon()), convert_y_from_lat(inflection_1.lat()));
                    ezgl:: point2d seg_inflection_2_alter (convert_x_from_lon(inflection_2.lon()), convert_y_from_lat(inflection_2.lat()));
                    g.draw_line (seg_inflection_1_alter, seg_inflection_2_alter);
                }
                //and finally draw the last segment
                LatLon inflection_second_last = getStreetSegmentCurvePoint(num_curve-1, pathSegment[i]);
                ezgl::point2d seg_second_last_alter  (convert_x_from_lon(inflection_second_last.lon()), convert_y_from_lat(inflection_second_last.lat()));
                ezgl::point2d seg_end_alter  (convert_x_from_lon(seg_end.lon()), convert_y_from_lat(seg_end.lat()));
                g.draw_line(seg_second_last_alter, seg_end_alter);
            }
//        }
    }
}


double find_seg_angle(LatLon seg1_start, LatLon seg1_end, LatLon seg2_start, LatLon seg2_end){
    double seg1_dx = seg1_end.lon() - seg1_start.lon();
    double seg1_dy = seg1_end.lat() - seg1_start.lat();
    double seg2_dx = seg2_end.lon() - seg2_start.lon();
    double seg2_dy = seg2_end.lat() - seg2_start.lat();
    double cross = seg1_dx * seg2_dy - seg2_dx * seg1_dy;
    
    double seg1_length = sqrt(seg1_dx + seg1_dy);
    double seg2_length = sqrt(seg2_dx + seg2_dy);
    
    double sinTheta = cross / (seg1_length * seg2_length);
    double angle = asin(sinTheta);
    
    return angle;
} 

bool seg_is_straight(unsigned street_segment1, unsigned street_segment2){
    // first judge whether two segments intersect
    unsigned seg1_from = getInfoStreetSegment(street_segment1).from;
    unsigned seg1_to = getInfoStreetSegment(street_segment1).to;
    unsigned seg2_from = getInfoStreetSegment(street_segment2).from;
    unsigned seg2_to = getInfoStreetSegment(street_segment2).to;
    bool intersect = (seg1_from==seg2_from) || (seg1_from==seg2_to) || (seg1_to==seg2_from) || (seg1_to==seg2_to);
    //bool intersect = (seg1_to == seg2_from) || (seg1_from == seg2_from) || (seg1_to == seg2_to) || (seg1_from == seg2_to);
    if(!intersect){
        return false;
    }
    
    // second, check whether two segments have the same street ID
    if(getInfoStreetSegment(street_segment1).streetID == getInfoStreetSegment(street_segment2).streetID){
        return true;
    }
    
    // if not belongs to both two situation above, calculate the turn type, using cross product
    //declare the variables that store the position of starting & ending points
    LatLon seg1_start, seg1_end, seg2_start, seg2_end;
    // calculate the curve point count for each segment
    int num_cur_1 = getInfoStreetSegment(street_segment1).curvePointCount;
    int num_cur_2 = getInfoStreetSegment(street_segment2).curvePointCount;
    //case 1, seg1_to == seg2_from
    if(seg1_to == seg2_from){
        //if seg1 has curve points, set the start point to the last curve point
        if(num_cur_1!=0){
            seg1_start = getStreetSegmentCurvePoint(num_cur_1-1, street_segment1);
            seg1_end = getIntersectionPosition(seg1_to);
        } else if(num_cur_1==0) {
            seg1_start = getIntersectionPosition(seg1_from);
            seg1_end = getIntersectionPosition(seg1_to);
        }
        //if seg2 has curve points, set the end point to the first curve point
        if(num_cur_2!=0){
            seg2_start = getIntersectionPosition(seg2_from);
            seg2_end = getStreetSegmentCurvePoint(0, street_segment2);
        } else if(num_cur_2==0) {
            seg2_start = getIntersectionPosition(seg2_from);
            seg2_end = getIntersectionPosition(seg2_to);
        }
        //find the cross product
        double value = find_seg_angle(seg1_start, seg1_end, seg2_start, seg2_end);
        if(value <= 0.15 || value >= -0.15)
            return true; // is straight
        else
            return false;
    }
    
    //case 2, 
    else if(seg1_from == seg2_from){
        //if seg1 has curve points, set the start point to the first curve point
        if(num_cur_1!=0){
            seg1_start = getStreetSegmentCurvePoint(0, street_segment1);
            seg1_end = getIntersectionPosition(seg1_from);
        } else if(num_cur_1==0) {
            //if seg1 don't have curve points, start=to, end=from
            seg1_start = getIntersectionPosition(seg1_to);
            seg1_end = getIntersectionPosition(seg1_from);
        }
        //if seg2 has curve points, set the end point to the first curve point
        if(num_cur_2!=0){
            seg2_start = getIntersectionPosition(seg2_from);
            seg2_end = getStreetSegmentCurvePoint(0, street_segment2);
        } else if(num_cur_2==0) {
            seg2_start = getIntersectionPosition(seg2_from);
            seg2_end = getIntersectionPosition(seg2_to);
        }
        //find the cross product
        double value = find_seg_angle(seg1_start, seg1_end, seg2_start, seg2_end);
        if(value <= 0.15 || value >= -0.15)
            return true; // is straight
        else
            return false;
    }
    
    //case 3
    else if(seg1_to == seg2_to){
        //if seg1 has curve points, set the start point to the last curve point
        if(num_cur_1!=0){
            seg1_start = getStreetSegmentCurvePoint(num_cur_1-1, street_segment1);
            seg1_end = getIntersectionPosition(seg1_to);
        } else if(num_cur_1==0) {
            seg1_start = getIntersectionPosition(seg1_from);
            seg1_end = getIntersectionPosition(seg1_to);
        }
        //if seg2 has curve points, set the start point to the last curve point
        if(num_cur_2!=0){
            seg2_start = getIntersectionPosition(seg2_to);
            seg2_end = getStreetSegmentCurvePoint(num_cur_2-1, street_segment2);
        } else if(num_cur_2==0) {
            seg2_start = getIntersectionPosition(seg2_to);
            seg2_end = getIntersectionPosition(seg2_from);
        }
        //find the cross product
        double value = find_seg_angle(seg1_start, seg1_end, seg2_start, seg2_end);
        if(value <= 0.15 || value >= -0.15)
            return true; // is straight
        else
            return false;
    }
    
    //case 4
    else if(seg1_from == seg2_to){
        //if seg1 has curve points, set the start point to the first curve point
        if(num_cur_1!=0){
            seg1_start = getStreetSegmentCurvePoint(0, street_segment1);
            seg1_end = getIntersectionPosition(seg1_from);
        } else if(num_cur_1==0) {
            //if seg1 don't have curve points, start=to, end=from
            seg1_start = getIntersectionPosition(seg1_to);
            seg1_end = getIntersectionPosition(seg1_from);
        }
        //if seg2 has curve points, set the start point to the last curve point
        if(num_cur_2!=0){
            seg2_start = getIntersectionPosition(seg2_to);
            seg2_end = getStreetSegmentCurvePoint(num_cur_2-1, street_segment2);
        } else if(num_cur_2==0) {
            seg2_start = getIntersectionPosition(seg2_to);
            seg2_end = getIntersectionPosition(seg2_from);
        }
        //find the cross product
        double value = find_seg_angle(seg1_start, seg1_end, seg2_start, seg2_end);
        if(value <= 0.15 || value >= -0.15)
            return true; // is straight
        else
            return false;
    }
    
    //default case
    else
        return true;
}