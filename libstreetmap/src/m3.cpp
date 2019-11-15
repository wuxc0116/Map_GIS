/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "m1.h"
#include "m3.h"
#include "StreetsDatabaseAPI.h"
#include <queue>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <limits.h>
#include <node.h>
#include "m1_helper.h"

using namespace std;
double cross(LatLon seg1_start, LatLon seg1_end, LatLon seg2_start, LatLon seg2_end);


TurnType find_turn_type(unsigned street_segment1, unsigned street_segment2){
    // first judge whether two segments intersect
    unsigned seg1_from = getInfoStreetSegment(street_segment1).from;
    unsigned seg1_to = getInfoStreetSegment(street_segment1).to;
    unsigned seg2_from = getInfoStreetSegment(street_segment2).from;
    unsigned seg2_to = getInfoStreetSegment(street_segment2).to;
    bool intersect = (seg1_from==seg2_from) || (seg1_from==seg2_to) || (seg1_to==seg2_from) || (seg1_to==seg2_to);
    //bool intersect = (seg1_to == seg2_from) || (seg1_from == seg2_from) || (seg1_to == seg2_to) || (seg1_from == seg2_to);
    if(!intersect){
        return TurnType::NONE;
    }
    
    // second, check whether two segments have the same street ID
    if(getInfoStreetSegment(street_segment1).streetID == getInfoStreetSegment(street_segment2).streetID){
        return TurnType::STRAIGHT;
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
        double value = cross(seg1_start, seg1_end, seg2_start, seg2_end);
        if(value <= 0)
            return TurnType::RIGHT;
        else
            return TurnType::LEFT;
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
        double value = cross(seg1_start, seg1_end, seg2_start, seg2_end);
        if(value <= 0)
            return TurnType::RIGHT;
        else
            return TurnType::LEFT;
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
        double value = cross(seg1_start, seg1_end, seg2_start, seg2_end);
        if(value <= 0)
            return TurnType::RIGHT;
        else
            return TurnType::LEFT;
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
        double value = cross(seg1_start, seg1_end, seg2_start, seg2_end);
        if(value <= 0)
            return TurnType::RIGHT;
        else
            return TurnType::LEFT;
    }
    
    //default case
    else
        return TurnType::NONE;
}


double cross(LatLon seg1_start, LatLon seg1_end, LatLon seg2_start, LatLon seg2_end){
    double seg1_dx = seg1_end.lon() - seg1_start.lon();
    double seg1_dy = seg1_end.lat() - seg1_start.lat();
    double seg2_dx = seg2_end.lon() - seg2_start.lon();
    double seg2_dy = seg2_end.lat() - seg2_start.lat();
    double result = seg1_dx * seg2_dy - seg2_dx * seg1_dy;
    return result;
}


double compute_path_travel_time(const std::vector<unsigned>& path, 
                                const double right_turn_penalty, 
                                const double left_turn_penalty){
    double time = 0;
    unsigned size = path.size();
    //trivial cases
    if(path.size()==0){
        return time;
    }
    
    //other cases when the vector size is non-zero
    time = time + find_street_segment_travel_time(path[0]);
    for(unsigned i=1; i<size; i++){
        if(find_turn_type(path[i-1], path[i]) == TurnType::RIGHT){
            time = time + right_turn_penalty;
        }
        if(find_turn_type(path[i-1], path[i]) == TurnType::LEFT){
            time = time + left_turn_penalty;
        }
        time = time + find_street_segment_travel_time(path[i]);
    }
    
    
    return time;
}

/*
//part of the algorithm in A* refers to chenjunt's github
//website is: https: //github.com/chenjuntu/Resume/blob/master/ECE297/libstreetmap/src/m3.cpp
//but the code is different

class compare {
public:
    bool operator()(Node& a, Node& b)
    {
        if(a.fscore()>=b.fscore()) {
            return true;
        }
        return false;
    }
};

int find_shortest_segment(unsigned curr_intersect_id,unsigned next_intersect_id);

unordered_map<unsigned, Node> close_set;

std::vector<unsigned> find_path_between_intersections(
                const unsigned intersect_id_start, 
                const unsigned intersect_id_end,
                const double right_turn_penalty, 
                const double left_turn_penalty){
    unsigned interNum = getNumIntersections();
    close_set.clear();
    vector <Node> open_set;
    open_set.resize(interNum);
    priority_queue<Node, vector<Node>, compare> pri_queue;
    // put the starting node on the open list, and leave all score to 0
    Node start;
    start.intersection_id=intersect_id_start;
    start.parent_node=intersect_id_start;
    start.gscore=0;
    start.hscore=0;
    start.path_segment = intersect_id_start;
    
    pri_queue.push(start);
    open_set[intersect_id_start] = start;
        
    //while the open list is not empty
    // find the node with least f on the open list
    // pop off this node from the open list and open set
    // set it into close set
    while(!pri_queue.empty()){
        //find the node with the least f on the open list, call it "current node"
        Node current_node;
        current_node=pri_queue.top();
        //pop off this node from the open list and open set
        pri_queue.pop();                        
        int curr_intersect_id = current_node.intersection_id;
        open_set[curr_intersect_id] = Node();
        //set it into close set
        open_set[curr_intersect_id].visited = true;
        close_set.insert(make_pair(current_node.intersection_id,current_node));
        

        //if found the path, trace back
        if(current_node.intersection_id==intersect_id_end){  
        vector<unsigned> path;
            Node current=current_node;
            while(current.intersection_id != start.intersection_id){
                path.insert(path.begin(),close_set.find(current.intersection_id)->second.path_segment);
                current.intersection_id=current.parent_node;
                current.parent_node=close_set.find(current.intersection_id)->second.parent_node;
                }
        return path;
        }
        
        //generate "current node" all neighbors and set their parent to the "current node"
        vector<unsigned> neighbor_list=find_adjacent_intersections(current_node.intersection_id);  
        unsigned adjacent_size = neighbor_list.size();
        for(unsigned i=0; i<adjacent_size; i++){  //traverse all intersections
            
            unsigned neighbor_intersect_id = neighbor_list[i];
            Node neighborNode = open_set[neighbor_list[i]];
            
            if(neighborNode.visited == true){
                continue;
            }
            LatLon end_loc=getIntersectionPosition(intersect_id_end);
            //check if the neighbor is in the open list
            bool in_open_set = neighborNode.in_openlist;
            if (in_open_set==false){
                //if neighbor is not in open set
                //set gscore, hscore, parent_node, and put it into open set
                neighborNode.intersection_id=neighbor_intersect_id;   
                neighborNode.parent_node = current_node.intersection_id;
                neighborNode.hscore=find_distance_between_two_points(getIntersectionPosition(neighbor_intersect_id),end_loc);

                unsigned seg1 = current_node.path_segment; // if the first time, "start" is the current_node, path_seg is itself
                unsigned seg2 = find_shortest_segment(current_node.intersection_id, neighborNode.intersection_id);
                neighborNode.path_segment = seg2;
                // compute g_score, using path travel time
                double neighbor_seg_travel_time = find_street_segment_travel_time(seg2);
                TurnType turn = find_turn_type(seg1, seg2);
                if(turn == TurnType::LEFT){
                    neighbor_seg_travel_time = neighbor_seg_travel_time + left_turn_penalty;
                } else if(turn == TurnType::RIGHT){
                    neighbor_seg_travel_time = neighbor_seg_travel_time + right_turn_penalty;
                }
                neighborNode.gscore = neighbor_seg_travel_time + current_node.gscore;
                
                // put this node into open list and open set
                neighborNode.in_openlist = true;
                pri_queue.push(neighborNode); 
                open_set[neighborNode.intersection_id] = neighborNode;   
            }
            else{ 
                // if the neighbor node is in the open set
                // calculate the g_score from current_node to neighbor_node
                // if this g_score is smaller than previously calculated g value
                // set the parent_node of neighbor_node to current_node
                // and also update the gscore, hscore
                neighborNode=open_set[neighbor_intersect_id];
                unsigned seg1 = current_node.path_segment;
                unsigned seg2 = find_shortest_segment(current_node.intersection_id, neighborNode.intersection_id);
                double neighbor_seg_travel_time = find_street_segment_travel_time(seg2);
                    TurnType turn = find_turn_type(seg1, seg2);
                    if(turn == TurnType::LEFT){
                        neighbor_seg_travel_time = neighbor_seg_travel_time + left_turn_penalty;
                    } else if(turn == TurnType::RIGHT){
                        neighbor_seg_travel_time = neighbor_seg_travel_time + right_turn_penalty;
                    }
                double new_gscore = neighbor_seg_travel_time + current_node.gscore;
                //if found that the score is lower, update all info to this neighbor node
                if(new_gscore<neighborNode.gscore){                    
                    neighborNode.intersection_id=neighbor_intersect_id;
                    neighborNode.gscore=new_gscore;
                    neighborNode.parent_node=current_node.intersection_id;
                    neighborNode.hscore = find_distance_between_two_points(getIntersectionPosition(neighborNode.intersection_id), end_loc);
                    neighborNode.path_segment = seg2;
                    //neighborNode.in_openlist = true;
                    pri_queue.push(neighborNode);
                }
            }
        }
    }
    //if not found any, return an empty vector
    vector<unsigned> empty;
    return empty;
}



//function to find the shortest segment between two intersections
int find_shortest_segment(unsigned curr_intersect_id,unsigned next_intersect_id){
    const vector<unsigned>& segments=find_intersection_street_segments_ref(curr_intersect_id);
    double shorest_time = UINT_MAX;
    int shortest_segid = 0;
    for(unsigned i=0; i<segments.size(); i++){
        
        //if two streets is connected
        if(static_cast<unsigned>(getInfoStreetSegment(segments[i]).from)==next_intersect_id || static_cast<unsigned>(getInfoStreetSegment(segments[i]).to)==next_intersect_id){
            if(static_cast<unsigned>(getInfoStreetSegment(segments[i]).oneWay) && static_cast<unsigned>(getInfoStreetSegment(segments[i]).to) !=next_intersect_id){
                //if curr_intersect cannot reach next_intersect due to one way, skip it
                continue;
            }
            double seg_time = find_street_segment_travel_time(segments[i]);
            if(seg_time<shorest_time){
                shorest_time = seg_time;
                shortest_segid = segments[i];
            }
        }
    }
    return shortest_segid;
}
*/

class compare {
public:
    bool operator()(Node& a, Node& b)
    {
        if(a.fscore()>=b.fscore()) {
            return true;
        }
        return false;
    }
};

int find_shortest_segment(unsigned curr_intersect_id,unsigned next_intersect_id);

unordered_map<unsigned, Node> close_set;

std::vector<unsigned> find_path_between_intersections(
                const unsigned intersect_id_start, 
                const unsigned intersect_id_end,
                const double right_turn_penalty, 
                const double left_turn_penalty){
    unsigned interNum = getNumIntersections();
    close_set.clear();
    unordered_map <unsigned, Node> pri_queue_map;
    unordered_map <unsigned, Node> open_set;
    priority_queue<Node, vector<Node>, compare> pri_queue;
    // put the starting node on the open list, and leave all score to 0
    Node start;
    start.intersection_id=intersect_id_start;
    start.parent_node=intersect_id_start;
    start.gscore=0;
    start.hscore=0;
    start.path_segment = intersect_id_start;
    
    pri_queue.push(start);
    open_set[intersect_id_start] = start;
        
    //while the open list is not empty
    // find the node with least f on the open list
    // pop off this node from the open list and open set
    // set it into close set
    while(!pri_queue.empty()){
    //while(!pri_queue_map.empty()){
        //function to find the best g_score in open list
        unsigned temp_best_intersection_id;
        for(unsigned i=0; i<pri_queue_map.size(); i++){
            double temp_gscore = 99999;
            temp_best_intersection_id = pri_queue_map[i].intersection_id;
            if(pri_queue_map[i].gscore < temp_gscore){
                temp_gscore = pri_queue_map[temp_best_intersection_id].gscore;
                temp_best_intersection_id = pri_queue_map[temp_best_intersection_id].intersection_id;
            }
        }
        
        
        //find the node with the least f on the open list, call it "current node"
        Node current_node;
        current_node=pri_queue.top();
        //current_node = open_set [temp_best_intersection_id];
        //pop off this node from the open list and open set
        pri_queue.pop();                        
        int curr_intersect_id = current_node.intersection_id;
        open_set[curr_intersect_id] = Node();
        //set it into close set
        open_set[curr_intersect_id].visited = true;
        close_set.insert(make_pair(current_node.intersection_id,current_node));
        

        //if found the path, trace back
        if(current_node.intersection_id==intersect_id_end){  
        vector<unsigned> path;
            Node current=current_node;
            while(current.intersection_id != start.intersection_id){
                path.insert(path.begin(),close_set.find(current.intersection_id)->second.path_segment);
                current.intersection_id=current.parent_node;
                current.parent_node=close_set.find(current.intersection_id)->second.parent_node;
                }
        return path;
        }
        
        //generate "current node" all neighbors and set their parent to the "current node"
        vector<unsigned> neighbor_list=find_adjacent_intersections(current_node.intersection_id);  
        unsigned adjacent_size = neighbor_list.size();
        for(unsigned i=0; i<adjacent_size; i++){  //traverse all intersections
            
            unsigned neighbor_intersect_id = neighbor_list[i];
            Node neighborNode = open_set[neighbor_list[i]];
            
            if(neighborNode.visited == true){
                continue;
            }
            LatLon end_loc=getIntersectionPosition(intersect_id_end);
            //check if the neighbor is in the open list
            bool in_open_set = neighborNode.in_openlist;
            if (in_open_set==false){
                //if neighbor is not in open set
                //set gscore, hscore, parent_node, and put it into open set
                neighborNode.intersection_id=neighbor_intersect_id;   
                neighborNode.parent_node = current_node.intersection_id;
                neighborNode.hscore=find_distance_between_two_points(getIntersectionPosition(neighbor_intersect_id),end_loc);

                unsigned seg1 = current_node.path_segment; // if the first time, "start" is the current_node, path_seg is itself
                unsigned seg2 = find_shortest_segment(current_node.intersection_id, neighborNode.intersection_id);
                neighborNode.path_segment = seg2;
                // compute g_score, using path travel time
                double neighbor_seg_travel_time = find_street_segment_travel_time(seg2);
                TurnType turn = find_turn_type(seg1, seg2);
                if(turn == TurnType::LEFT){
                    neighbor_seg_travel_time = neighbor_seg_travel_time + left_turn_penalty;
                } else if(turn == TurnType::RIGHT){
                    neighbor_seg_travel_time = neighbor_seg_travel_time + right_turn_penalty;
                }
                neighborNode.gscore = neighbor_seg_travel_time + current_node.gscore;
                
                // put this node into open list and open set
                neighborNode.in_openlist = true;
                pri_queue.push(neighborNode); 
                open_set[neighborNode.intersection_id] = neighborNode;   
            }
            else{ 
                // if the neighbor node is in the open set
                // calculate the g_score from current_node to neighbor_node
                // if this g_score is smaller than previously calculated g value
                // set the parent_node of neighbor_node to current_node
                // and also update the gscore, hscore
                neighborNode=open_set[neighbor_intersect_id];
                unsigned seg1 = current_node.path_segment;
                unsigned seg2 = find_shortest_segment(current_node.intersection_id, neighborNode.intersection_id);
                double neighbor_seg_travel_time = find_street_segment_travel_time(seg2);
                    TurnType turn = find_turn_type(seg1, seg2);
                    if(turn == TurnType::LEFT){
                        neighbor_seg_travel_time = neighbor_seg_travel_time + left_turn_penalty;
                    } else if(turn == TurnType::RIGHT){
                        neighbor_seg_travel_time = neighbor_seg_travel_time + right_turn_penalty;
                    }
                double new_gscore = neighbor_seg_travel_time + current_node.gscore;
                //if found that the score is lower, update all info to this neighbor node
                if(new_gscore<neighborNode.gscore){                    
                    neighborNode.intersection_id=neighbor_intersect_id;
                    neighborNode.gscore=new_gscore;
                    neighborNode.parent_node=current_node.intersection_id;
                    neighborNode.hscore = find_distance_between_two_points(getIntersectionPosition(neighborNode.intersection_id), end_loc);
                    neighborNode.path_segment = seg2;
                    //neighborNode.in_openlist = true;
                    pri_queue.push(neighborNode);
                }
            }
        }
    }
    //if not found any, return an empty vector
    vector<unsigned> empty;
    return empty;
}



//function to find the shortest segment between two intersections
int find_shortest_segment(unsigned curr_intersect_id,unsigned next_intersect_id){
    const vector<unsigned>& segments=find_intersection_street_segments_ref(curr_intersect_id);
    double shorest_time = UINT_MAX;
    int shortest_segid = 0;
    for(unsigned i=0; i<segments.size(); i++){
        
        //if two streets is connected
        if(static_cast<unsigned>(getInfoStreetSegment(segments[i]).from)==next_intersect_id || static_cast<unsigned>(getInfoStreetSegment(segments[i]).to)==next_intersect_id){
            if(static_cast<unsigned>(getInfoStreetSegment(segments[i]).oneWay) && static_cast<unsigned>(getInfoStreetSegment(segments[i]).to) !=next_intersect_id){
                //if curr_intersect cannot reach next_intersect due to one way, skip it
                continue;
            }
            double seg_time = find_street_segment_travel_time(segments[i]);
            if(seg_time<shorest_time){
                shorest_time = seg_time;
                shortest_segid = segments[i];
            }
        }
    }
    return shortest_segid;
}