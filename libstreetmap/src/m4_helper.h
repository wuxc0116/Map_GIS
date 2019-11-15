/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   m4_helper.h
 * Author: wuxiangc
 *
 * Created on March 23, 2019, 6:37 PM
 */

#ifndef M4_HELPER_H
#define M4_HELPER_H
#include <vector>
#include <unordered_map>
#include <string>


using namespace std;

struct locationInfo{
    unsigned intersectionID;
    std::string type;
    float itemWeight;
    std::vector<unsigned> pickupIndices;
};

struct pickupInfo{
    unsigned id;
    bool erased;
    
};

//DNode is the class for Dijkstra Algorithm
class DNode{
 public:
   
    unsigned intersection_id;
    bool visited=false;
    bool in_openlist = false;
    double gscore;
    unsigned parent_node;
    unsigned path_segment; 
  
    ~DNode(){
        ;
    }
   
};


class DPath{
public:
    double time;
    vector<unsigned> dpath;
};

/*
vector<vector<DPath>> find_all_path (const vector<unsigned>& intersections,
                                                const float right_turn_penalty,
                                                const float left_turn_penalty);

vector<DPath> find_paths_from_intersections_dijkstra(unsigned intersect_id_start, vector<unsigned> intersection_list,
                                                                const float right_turn_penalty, const float left_turn_penalty);

unsigned find_shortest_segment(unsigned curr_intersect_id,unsigned next_intersect_id);
*/
unordered_map<unsigned, DPath> find_paths_from_intersections_dijkstra (unsigned intersect_id_start, vector<unsigned> intersection_list,
                                                                const float right_turn_penalty, const float left_turn_penalty);

void find_all_path (const vector<unsigned>& intersections,
                                                const float right_turn_penalty,
                                                const float left_turn_penalty);

struct delivery_info{
    //std::string type;
    unsigned pickUp = 0;
    unsigned dropOff = 0;
    float itemWeight;
    int deliverID;
};


// return the closest intersection (away from the startID intersection) for pick up or drop off
DNode Dijkstra_find_path (unsigned startID,
        const std::vector<DeliveryInfo>& deliveries,
        const float right_turn_penalty,
        const float left_turn_penalty);
#endif /* M4_HELPER_H */


void two_opt(std::vector<CourierSubpath> &existing_route, const std::vector<DeliveryInfo>& deliveries, const float truck_capacity);
void two_opt_swap(std::vector<locationInfo> &route, std::vector<locationInfo> &new_route, const std::vector<DeliveryInfo>& deliveries,
                    int i, int k, const float truck_capacity, bool &valid_route);

