/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/*
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
//#include "m4_helper.h"
using namespace std;


DPath find_path_between_intersections_m4(
                const unsigned intersect_id_start, 
                const unsigned intersect_id_end,
                const double right_turn_penalty, 
                const double left_turn_penalty);

void find_all_path (const vector<unsigned>& intersections,
                    const float right_turn_penalty,
                    const float left_turn_penalty){
#pragma omp parallel for
    for(unsigned i=0; i<intersections.size(); i++){
//#pragma omp critical
        for(unsigned j=0; j<intersections.size(); j++){
            unsigned start_id = intersections[i];
            unsigned end_id = intersections[j];
            DPath temp = find_path_between_intersections_m4(start_id, end_id, right_turn_penalty, left_turn_penalty);
            #pragma omp critical
            all_path_structure [start_id][end_id] = temp;
        }
        
   }
}


class compare_new {
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

DPath find_path_between_intersections_m4(
                const unsigned intersect_id_start, 
                const unsigned intersect_id_end,
                const double right_turn_penalty, 
                const double left_turn_penalty){
    unordered_map<unsigned, Node> close_set_new;
    unsigned interNum = getNumIntersections();
    close_set_new.clear();
    vector <Node> open_set;
    open_set.resize(interNum);
    priority_queue<Node, vector<Node>, compare_new> pri_queue;
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
        close_set_new.insert(make_pair(current_node.intersection_id,current_node));
        

        //if found the path, trace back
        if(current_node.intersection_id==intersect_id_end){  
        vector<unsigned> path;
            Node current=current_node;
            while(current.intersection_id != start.intersection_id){
                path.insert(path.begin(),close_set_new.find(current.intersection_id)->second.path_segment);
                current.intersection_id=current.parent_node;
                current.parent_node=close_set_new.find(current.intersection_id)->second.parent_node;
                }
            DPath time_with_path;
            time_with_path.dpath = path;
            time_with_path.time = current_node.gscore;
        return time_with_path;
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
    DPath empty;
    return empty;
}
 */ 