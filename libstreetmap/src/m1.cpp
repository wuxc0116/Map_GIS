/* 
 * Copyright 2019 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "math.h"
#include <vector>
#include <unordered_map>
#include <map>
#include <iostream>
#include <string>
#include <locale>
#include <algorithm>
#include "m1_helper.h"

using namespace std;
//extern double polygonArea(std::vector<double>X, std::vector<double>Y, int numPoints);
//extern double convert_x_from_lon(float lon);
//extern double convert_y_from_lat(float lat);

//define unordered_map
std::map <std::string, unsigned> street; //street name + ID
std::multimap <std::string, unsigned> street_multi; //street name + ID, used for find street using partial name
std::unordered_map <unsigned, vector<unsigned>> street_segment; //street ID + all its segments(in a vector)
std::unordered_map <unsigned, vector<unsigned>> intersection_segment; //intersection + all its segments(in a vector)
std::unordered_map <unsigned, vector<unsigned>> street_intersection; //street + all its intersection


std::vector<std::vector<unsigned>> intersection_street_segments;
std::vector<std::vector<std::string>>intersection_street_names;
std::vector<std::string>intersection_name;
std::vector<std::vector<unsigned>> all_street_intersection;
std::vector<std::string> street_names;
std::vector<std::vector<LatLon>> curve_points;
std::vector<double> travel_time;
std::vector<LatLon>intersection_position;
std::vector<OSMID>intersection_OSMNodeID;
//std::vector<std::string>POI_type;
std::map<int, double>features;
std::vector<std::vector<unsigned>> adjIntersection;
std::vector<unsigned> segmentsAtIntersection;


//convert all upper case to lower case
//this implementation function is from cppreference.com, std::tolower
std::string str_tolower(std::string s); // this line is for decalaration
std::string str_tolower(std::string s) {
    std::transform(s.begin(), 
                   s.end(), 
                   s.begin(),
                   [](unsigned char c) -> unsigned char { return std::tolower(c);}
                   );
    return s;
}


bool load_map(std::string map_name/*map_path*/) {
    //bool load_successful = false; //Indicates whether the map has loaded 
                                  //successfully

    bool load_successful = loadStreetsDatabaseBIN(map_name);
    //
    //Load your map related data structures here
    //
    if (!load_successful)
        return false;

    //load street name with its id into a multi_map
    //this data structure is used for find_street_from_partial_name
    for(int i=0; i<getNumStreets(); i++){
        //convert all upper case to lower case, then put it into multi map
        street_multi.insert(std::make_pair(str_tolower(getStreetName(i)), i));
    }
    
    //load street (with its ID) into the unordered_map
    for(int i=0; i<getNumStreets(); i++){
        street.insert(std::make_pair(getStreetName(i), i));
    }
    
    
    //load street_segment (street + segment pair together)
    std::vector<unsigned> segment; 
    for(int i=0; i<getNumStreets(); i++){
        street_segment.insert(std::make_pair(i, segment));
        //the vector "segment" here is still an empty vector
        //waiting for inserting elements in the next for_loop
    }
    //push_back all segments into the vector "segment"
    for(int i=0; i<getNumStreetSegments(); i++){
        //string st_name = getStreetName(getInfoStreetSegment(i).streetID);
        street_segment[getInfoStreetSegment(i).streetID].push_back(i);
    }
    
    
    //resize all vector, avoid crush
    //intersection
    int numIntersection = getNumIntersections();
//    int numPointOfInterests = getNumPointsOfInterest();
    intersection_street_segments.resize(numIntersection);
    intersection_street_names.resize(numIntersection);
    intersection_name.resize(numIntersection);
    intersection_position.resize(numIntersection);
    intersection_OSMNodeID.resize(numIntersection);
    adjIntersection.resize(numIntersection);
//    POI_type.resize();
    
    //street
    int numStreet = getNumStreets();
    all_street_intersection.resize(numStreet);
    street_names.resize(numStreet);
    
    /////////////////////////////////*intersection*/////////////////////////////
    //loop through the intersections
    for(int intersection = 0; intersection < numIntersection; intersection++){
        
        unsigned segCountAtIntersection = getIntersectionStreetSegmentCount(intersection);
        //push in related information at each intersection
        intersection_name.push_back(getIntersectionName(intersection));
        intersection_position.push_back(getIntersectionPosition(intersection));
        intersection_OSMNodeID.push_back(getIntersectionOSMNodeID(intersection));
        
        //loop through the segments at each intersections, insert in the vector created
        for(unsigned i = 0; i < segCountAtIntersection; i++){
            unsigned segAtIntersection = getIntersectionStreetSegment(i, intersection);
            
            intersection_street_segments[intersection].push_back(segAtIntersection);
            
            InfoStreetSegment infoStreetSeg = getInfoStreetSegment(segAtIntersection);
            //store the intersection id to the corresponding street id
            //have duplicated, need to solve
            all_street_intersection[infoStreetSeg.streetID].push_back(infoStreetSeg.from);
            all_street_intersection[infoStreetSeg.streetID].push_back(infoStreetSeg.to);

        }
        
        
        
//        std::vector<unsigned>eachAdjIntersection;
//        //get the vector of street segments for the given intersection
//        
////        segmentsAtIntersection.resize(getNumStreetSegments());
//        segmentsAtIntersection = find_intersection_street_segments(intersection);
//        
////        std::cout << segmentsAtIntersection.size() << std::endl;
////        std::cout << getNumStreetSegments() << std::endl;
//        for(unsigned i = 0; i < segmentsAtIntersection.size(); i++){
////            std::cout << segmentsAtIntersection[i] << "+++++++++" << std::endl;
////            std::cout << segmentsAtIntersection.size() << "-----------" << std::endl;
//            //get the struct of number i street segment by the intersection
//            InfoStreetSegment infoSeg = getInfoStreetSegment(segmentsAtIntersection[i]);
//            //if the intersection is the start point(from) and the segment is a oneway segment
//            if((static_cast<unsigned>(infoSeg.from) == intersection) && (infoSeg.oneWay == true)){
//                eachAdjIntersection.push_back(infoSeg.to);
//            }
//            //intersection point is from and segment is not oneway segment
//            else if((static_cast<unsigned>(infoSeg.from)== intersection) && (infoSeg.oneWay == false)){
//                eachAdjIntersection.push_back(infoSeg.to);
//            }
//            //intersection is to and segment is oneway segment
//            else if((static_cast<unsigned>(infoSeg.to) == intersection) && (infoSeg.oneWay == true)){
//                //cannot travel, do nothing
//            }
//            //intersection is to and segment is both way segment
//            else if((static_cast<unsigned>(infoSeg.to) == intersection) && (infoSeg.oneWay == false)){
//                eachAdjIntersection.push_back(infoSeg.from);
//            }
//        }
//
//        segmentsAtIntersection.clear();
//        segmentsAtIntersection.resize(0);
//        //erase the duplicated intersections
//            std::sort(eachAdjIntersection.begin(), eachAdjIntersection.end()); //sort the vector first
//            //erase the closest two duplicated adjacent intersection
//            eachAdjIntersection.erase(std::unique(eachAdjIntersection.begin(), eachAdjIntersection.end()), eachAdjIntersection.end());
//
//    //        return adjIntersection;
//            adjIntersection[intersection] = eachAdjIntersection;
        
        
        
        for(int i = 0; i < getIntersectionStreetSegmentCount(intersection); i++){
            StreetSegmentIndex a = getIntersectionStreetSegment(i, intersection);
            InfoStreetSegment count = getInfoStreetSegment(a);
            if(static_cast<int> (intersection) == count.from) {
                adjIntersection[intersection].push_back(count.to);
            }
            else{
                if(count.oneWay){}
                else{
                    adjIntersection[intersection].push_back(count.from);
                }
            }

        }
        std::sort(adjIntersection[intersection].begin(), adjIntersection[intersection].end()); //sort the vector first
            //erase the closest two duplicated adjacent intersection
        adjIntersection[intersection].erase(std::unique(adjIntersection[intersection].begin(), adjIntersection[intersection].end()), adjIntersection[intersection].end());
        
    }
    
    
    
    ///////////////////////////////*Segment*////////////////////////////////////
    //pre loads the travel time and curve points of all street segments into vectors
    int segNum = getNumStreetSegments();
    curve_points.resize(segNum);
    travel_time.resize(segNum);
    InfoStreetSegment info;
    int curvePointNum;
    float speed_limit;
    double street_segment_length;
    
    for(int i = 0; i < segNum; i++){  //loading curve points here
        info = getInfoStreetSegment(i);
        curvePointNum = info.curvePointCount;
        
        if(curvePointNum != 0){
            for(int j = 0; j < curvePointNum; j++){
                curve_points[i].push_back(getStreetSegmentCurvePoint(j, i));
            }
        }
    }
    
    for(int i = 0; i < segNum; i++){ //loading street segment travel time here
        info = getInfoStreetSegment(i);
        speed_limit = info.speedLimit*1000/3600;
        street_segment_length = find_street_segment_length(i);
        travel_time[i] = street_segment_length/speed_limit;

        
    }
    
    
    ///////////////////////////////*Street*/////////////////////////////////////
    
    //loop through all streets
    for(int streetID = 0; streetID < numStreet; streetID++){
        
        street_names.push_back(getStreetName(streetID));
        //deal with duplicated issue
        //erase the duplicated intersection ID
        std::sort(all_street_intersection[streetID].begin(), all_street_intersection[streetID].end()); //sort the vector first
        //erase the closest two duplicated intersections
        all_street_intersection[streetID].erase(std::unique(all_street_intersection[streetID].begin(), all_street_intersection[streetID].end()), all_street_intersection[streetID].end());
    }
    
    
    //load_successful = true; //Make sure this is updated to reflect whether
                            //loading the map succeeded or failed
    
    ///////////////////////////*Point of Interests*/////////////////////////////
//    bool exist = false;
//    int numPointOfInterests = getNumPointsOfInterest();
//    for(int i=0; i<numPointOfInterests; i++){
//        if(i==0)
//            POI_type.push_back(getPointOfInterestType(i));
//        if(i!=0){
//            for(int j=0; j<i; j++){
//                if(getPointOfInterestType(i)== POI_type[j]){
//                    exist =true;
//                    j = i;
//                }
//            }
//            if(exist == false){
//                POI_type.push_back(getPointOfInterestType(i));
//            }
//            exist = false;
//        }
//    }
    
/////////////////////////////*Features*////////////////////////////////////////
//    std::map<int, double>features; //the first integer is the featureID,
                                //second integer is the area of the feature
//    int featuresNum = getNumFeatures();
//    double polygon_Area;
//    int numPoints;
//    std::vector<double>x;
//    std::vector<double>y;
//    
//     for(int i=0; i<featuresNum; i++){
//        numPoints = getFeaturePointCount(i);
//        for(int j=0; j<numPoints; j++){
//            double x_coord = getFeaturePoint(j, i).lon();
//            double y_coord = getFeaturePoint(j, i).lat();
//            x.push_back(convert_x_from_lon(x_coord));
//            y.push_back(convert_y_from_lat(y_coord));
//        }
//        polygon_Area = polygonArea(x, y, numPoints);
////        features[i] = polygon_Area;
//        features.insert(std::make_pair(i, polygon_Area));
//    }
    return load_successful;
}

void close_map() {
    //Clean-up your map related data structures here
    street_multi.clear();    
    curve_points.clear();
    street_segment.clear();
    travel_time.clear();
    adjIntersection.clear();
    
    intersection_street_segments.clear();
    intersection_street_names.clear();
    intersection_name.clear();
    all_street_intersection.clear();
    street_names.clear();
//std::vector<std::vector<LatLon>> curve_points;
//std::vector<double> travel_time;
    intersection_position.clear();
    intersection_OSMNodeID.clear();
//std::vector<std::string>POI_type;
    features.clear();
//std::vector<std::vector<unsigned>> adjIntersection;

    closeStreetDatabase();
}

//Returns the street names at the given intersection (includes duplicate street 
//names in returned vector)
std::vector<std::string> find_intersection_street_names(unsigned intersection_id){
    //create an empty vector of type string
    std::vector<std::string>streetName;
    
    //get the number of segment related to the intersection_id
    unsigned num = getIntersectionStreetSegmentCount(intersection_id);
    for(unsigned i = 0; i < num; i++){
        //get each segment index
        StreetSegmentIndex ssi = getIntersectionStreetSegment(i, intersection_id);
        //get all the information related to the segment by the streetSegmentIndex
        InfoStreetSegment infoSeg = getInfoStreetSegment(ssi);
        //start pushing strings into the vector streetName
        streetName.push_back(getStreetName(infoSeg.streetID));
    }
    return streetName;
}


//Returns all intersections reachable by traveling down one street segment 
//from given intersection (hint: you can't travel the wrong way on a 1-way street)
//the returned vector should NOT contain duplicate intersections
std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id){
//    std::vector<unsigned>adjIntersection;
//    
//    //get the vector of street segments for the given intersection
//    std::vector<unsigned>segmentsAtIntersection;
//    segmentsAtIntersection = find_intersection_street_segments(intersection_id);
//    
//    for(unsigned i = 0; i < segmentsAtIntersection.size(); i++){
//        //get the struct of number i street segment by the intersection
//        InfoStreetSegment infoSeg = getInfoStreetSegment(segmentsAtIntersection[i]);
//        //if the intersection is the start point(from) and the segment is a oneway segment
//        if((static_cast<unsigned>(infoSeg.from) == intersection_id) && (infoSeg.oneWay == true)){
//            adjIntersection.push_back(infoSeg.to);
//        }
//        //intersection point is from and segment is not oneway segment
//        else if((static_cast<unsigned>(infoSeg.from)== intersection_id) && (infoSeg.oneWay == false)){
//            adjIntersection.push_back(infoSeg.to);
//        }
//        //intersection is to and segment is oneway segment
//        else if((static_cast<unsigned>(infoSeg.to) == intersection_id) && (infoSeg.oneWay == true)){
//            //cannot travel, do nothing
//        }
//        //intersection is to and segment is both way segment
//        else if((static_cast<unsigned>(infoSeg.to) == intersection_id) && (infoSeg.oneWay == false)){
//            adjIntersection.push_back(infoSeg.from);
//        }
//    }
//    
//    //erase the duplicated intersections
//    std::sort(adjIntersection.begin(), adjIntersection.end()); //sort the vector first
//    //erase the closest two duplicated adjacent intersection
//    adjIntersection.erase(std::unique(adjIntersection.begin(), adjIntersection.end()), adjIntersection.end());
//    
//    return adjIntersection;
    return adjIntersection[intersection_id];
}


//Returns all intersections along the a given street
std::vector<unsigned> find_all_street_intersections(unsigned street_id){
    //use the vector created in load function
    return all_street_intersection[street_id];
}


//Return all intersection ids for two intersecting streets
//This function will typically return one intersection id.
std::vector<unsigned> find_intersection_ids_from_street_ids(unsigned street_id1, 
                                                              unsigned street_id2){
    //need a helper function to find street ids from name (more convenient)
    //loop through all intersections on the given street
    //then find the same intersection id, consider duplicated
    std::vector<unsigned> intersectionID_from_streetID;
    std::vector<unsigned> street_id1_intersection;
    std::vector<unsigned> street_id2_intersection;
    
    //all intersection id on street 1 is stored in street_id1_intersection
    street_id1_intersection = find_all_street_intersections(street_id1);
    street_id2_intersection = find_all_street_intersections(street_id2);
    
    //loop through all intersection on the street_id1 and 2, find same intersection
    for(unsigned i = 0; i < street_id1_intersection.size(); i++){
        for(unsigned j = 0; j < street_id2_intersection.size(); j++){
            if(street_id1_intersection[i] == street_id2_intersection[j]){
                intersectionID_from_streetID.push_back(street_id1_intersection[i]);
            }
        }
    }
    return intersectionID_from_streetID;
}


//Returns the street segments for the given intersection 
std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id){

    return intersection_street_segments[intersection_id];
}

const std::vector<unsigned>& find_intersection_street_segments_ref(unsigned intersection_id){

    return intersection_street_segments[intersection_id];
}


//Returns the distance between two coordinates in meters
double find_distance_between_two_points(LatLon point1, LatLon point2){
    double x1, x2, y1, y2;
    double lat1_rad, lat2_rad, lon1_rad, lon2_rad;
    double lat_avg;
    double distance;
    
    //convert latitude & longitude in degree to radians
    lat1_rad = point1.lat()*DEG_TO_RAD;
    lat2_rad = point2.lat()*DEG_TO_RAD;
    lon1_rad = point1.lon()*DEG_TO_RAD;
    lon2_rad = point2.lon()*DEG_TO_RAD;
            
    //convert (lat,lon) to (x,y))
    lat_avg = ((point1.lat()+point2.lat())/2)*DEG_TO_RAD;
    x1 = lon1_rad*cos(lat_avg);
    y1 = lat1_rad;
    x2 = lon2_rad*cos(lat_avg);
    y2 = lat2_rad;
    
    //calculate the distance
    distance = EARTH_RADIUS_IN_METERS * sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1));

    return distance;
}

//Returns the length of the given street segment in meters
double find_street_segment_length(unsigned street_segment_id){
    InfoStreetSegment seg_info;
    LatLon point1, point2; 
    int curvePointsCount;
    double length;
    
    //fetch the intersections and curve points (if there's any) of the street segment
    seg_info = getInfoStreetSegment(street_segment_id);  
    curvePointsCount = seg_info.curvePointCount; 
    point1 = getIntersectionPosition(seg_info.from);  
    point2 = getIntersectionPosition(seg_info.to);
    
    //straight street segment, calculate the distance between the end points
    if(curvePointsCount == 0){
        length = find_distance_between_two_points(point1, point2);
        return length;
    }
    
    //calculate the length of each segment, and add them up
    length = find_distance_between_two_points(point1, curve_points[street_segment_id][0]) +
            find_distance_between_two_points(point2, curve_points[street_segment_id][curvePointsCount-1]);
    for(int i=0; i<curvePointsCount-1; i++){
        length = length + find_distance_between_two_points(curve_points[street_segment_id][i], curve_points[street_segment_id][i+1]);
    }
    return length;
}

//Returns the length of the specified street in meters
double find_street_length(unsigned street_id){
    std::vector<unsigned> street_segments;
    double length = 0;
    
    //fetch all the segment ids from the given street
    street_segments = find_street_street_segments(street_id);

    //find the length of each street segment, and add them up
    for(unsigned i=0; i<street_segments.size(); i++){
        length = length + find_street_segment_length(street_segments[i]);
    }
    return length;
    
}

//Returns the travel time to drive a street segment in seconds 
//(time = distance/speed_limit)
double find_street_segment_travel_time(unsigned street_segment_id){

    return travel_time[street_segment_id];
}


//Returns all street segments for the given street
std::vector<unsigned> find_street_street_segments(unsigned street_id){
    return street_segment[street_id];
}


//Returns true if you can get from intersection1 to intersection2 using a single 
//street segment (hint: check for 1-way streets too)
//corner case: an intersection is considered to be connected to itself
bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2){
    //corner case, intersection connected to itself
    //cout<<getIntersectionName(43755)<<", "<<getIntersectionName(9004)<<endl;
    if(intersection_id1 == intersection_id2){
        return true;
    }
    //store all intersection_street_segment into a new vector
    std::vector<unsigned> target_stSeg = find_intersection_street_segments(intersection_id1);
    
    for(unsigned i=0; i<target_stSeg.size(); i++){
        //store all the information into a vector
        InfoStreetSegment target_stSeg_info = getInfoStreetSegment(target_stSeg[i]);
        
        //check whether it is one-way road
        if(target_stSeg_info.oneWay == true){
            //check whether this target stSeg is connected to intersection 2
            if(static_cast<unsigned>(target_stSeg_info.to) == intersection_id2){
                return true;
            }
        }
        else if(target_stSeg_info.oneWay == false){
            //if it is not a one way road
            //then from and to can have four possibilities:
            //(from 1, to 2) (from 1, to 1) (from 2, to 1) (from 2, to 2)
            if((static_cast<unsigned>(target_stSeg_info.from)==intersection_id1 && static_cast<unsigned>(target_stSeg_info.to)==intersection_id2) || 
               (static_cast<unsigned>(target_stSeg_info.from)==intersection_id1 && static_cast<unsigned>(target_stSeg_info.to)==intersection_id1) ||
               (static_cast<unsigned>(target_stSeg_info.to)==intersection_id1 && static_cast<unsigned>(target_stSeg_info.from)==intersection_id2) ||
               (static_cast<unsigned>(target_stSeg_info.to)==intersection_id2 && static_cast<unsigned>(target_stSeg_info.from)==intersection_id2) ){
                return true;
            }
                       
        }
    }
    return false;
}


//Returns the nearest point of interest to the given position
unsigned find_closest_point_of_interest(LatLon my_position){
    unsigned target_id = 0;
    //initialize the smallest distance to the first element in the intersection vector
    double smallestDistance = find_distance_between_two_points(my_position, getPointOfInterestPosition(0));
    
    // Iterate the list, update the smallest distance if this distance this smaller than the previous one
    for(int i=0; i<getNumPointsOfInterest(); i++){
        double distance = find_distance_between_two_points(my_position, getPointOfInterestPosition(i));
        if(distance < smallestDistance){
            smallestDistance = distance;
            target_id = i;
        }
    }
    return target_id;
}
 


//Returns the nearest intersection to the given position
unsigned find_closest_intersection(LatLon my_position){
    unsigned target_id = 0;
    //initialize the smallest distance to the first element in the intersection vector
    double smallestDistance = find_distance_between_two_points(my_position, getIntersectionPosition(0));
    
    // Iterate the list, update the smallest distance if this distance this smaller than the previous one
    for(int i=0; i<getNumIntersections(); i++){
        double distance = find_distance_between_two_points(my_position, getIntersectionPosition(i));
        if(distance < smallestDistance){
            smallestDistance = distance;
            target_id = i;
        }
    }
    return target_id;
}


//Returns all street ids corresponding to street names that start with the given prefix
//The function should be case-insensitive to the street prefix. For example, 
//both "bloo" and "BloO" are prefixes to "Bloor Street East".
//If no street names match the given prefix, this routine returns an empty (length 0) 
//vector.
//You can choose what to return if the street prefix passed in is an empty (length 0) 
//string, but your program must not crash if street_prefix is a length 0 string.

std::vector<unsigned> find_street_ids_from_partial_street_name(std::string street_prefix){
    //create a new vector for storing street ids
    std::vector<unsigned> street_ids;
    //if the multi map is empty, directly return an empty vector
    if(street_prefix.empty())
       return street_ids;
    //change all street_prefix to lower case
    for(auto& street_prefix_lower: street_prefix){ 
        street_prefix_lower = std::tolower(street_prefix_lower);
    }
    multimap<string, unsigned>::iterator range;
    //find lower and upper bound, to locate where we should start and stop extract elements from multi map
    multimap<string, unsigned>::iterator lower = street_multi.lower_bound(street_prefix );
    multimap<string, unsigned>::iterator upper = street_multi.upper_bound(street_prefix + '{');
    for(range = lower; range!=upper; range++){
        street_ids.push_back(range->second);
    }
    return street_ids;
}


