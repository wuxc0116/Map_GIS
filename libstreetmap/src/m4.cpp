#include "m4.h"
#include "m4_helper.h"
#include "m1.h"
#include "m3.h"
#include "StreetsDatabaseAPI.h"
#include <time.h>
#include <algorithm>
#include <vector>
#include "m1_helper.h"
#include <limits.h>
#include <unordered_map>
#include <queue>

#define time_limit = 30;
using namespace std;

unordered_map<unsigned, unordered_map<unsigned, DPath> > all_path_structure;

//struct deliver

//greedy algorithm that find the nearest intersection (must satisfy conditions: 
//if it is a pick up point, the item weight must be smaller than the remaining capacity
//if it is a drop off point, it must be picked up first)
std::vector<CourierSubpath> traveling_courier(
        const std::vector<DeliveryInfo>& deliveries,
               const std::vector<unsigned>& depots,
        const float right_turn_penalty,
        const float left_turn_penalty,
        const float truck_capacity){
    
    //check if the position is marked
    std::vector<CourierSubpath> all_path;
    std::vector<delivery_info>intersection_info;
    std::vector<unsigned> intersections;
    std::vector<int>picked_up_id;
    std::vector<unsigned> depots_copy = depots;
    intersection_info.resize(deliveries.size()*2);
    intersections.resize(deliveries.size()*2);
    int visited = 0;
    float currentCapacity = 0;
    // go for any depot to nearest pickup, P
    //order of M*N
    double shortestDistance = 0;
    unsigned startIntersection;
    int firstpickup;
    int startDepotIndex;
    int startdepot;
    int prevIntersection;
    int prevDeliveryID;
    std::string prevType;
    //go through every depots
//    for(int k = 0; k < depots_copy.size(); k++){
        for(int j = 0; j < depots_copy.size(); j++){
            for(int i = 0; i < deliveries.size(); i++){
                //pickUp goes before drop off
                intersections[i*2] = deliveries[i].pickUp;
                intersections[i*2 + 1] = deliveries[i].dropOff;
                intersection_info[i*2].pickUp = deliveries[i].pickUp;
                intersection_info[i*2].itemWeight = deliveries[i].itemWeight;
                intersection_info[i*2].deliverID = i;
                intersection_info[i*2 + 1].dropOff = deliveries[i].dropOff;
                intersection_info[i*2 + 1].itemWeight = deliveries[i].itemWeight;
                intersection_info[i*2 + 1].deliverID = i;

                //consider about the capacity
                if(deliveries[i].itemWeight > truck_capacity){
                    //cannot pick the delivery up
                }
                else{
                    //find the distance between the first depots and all the deliveries position
                    double distance = find_distance_between_two_points(getIntersectionPosition(depots_copy[j]), 
                            getIntersectionPosition(deliveries[i].pickUp));
                    //the first intersection
                    if(j == 0 && i == 0){
                        shortestDistance = distance;
                        startIntersection = deliveries[i].pickUp;
                        firstpickup = i;
                        startdepot = depots_copy[j];
                        startDepotIndex = j;
                    } 
                    else{
                        if(shortestDistance > distance){
                            shortestDistance = distance;
                            startIntersection = deliveries[i].pickUp;
                            firstpickup = i;
                            startdepot = depots_copy[j];
                            startDepotIndex = j;
                        }      
                    }
                }
            }
        }
        //need to consider about the corner case that the depots are not accessible
        //if only one depot is unavailable, use the rest of the depots to compute
        //if only one depot is in the depot list, return empty
//        std::vector<unsigned> path = find_path_between_intersections(startdepot, startIntersection, 
//                right_turn_penalty, left_turn_penalty);
//        if(path.size() == 0 && startdepot != startIntersection){
//            //this depot is not available
//            depots_copy.erase(depots_copy.begin() + startDepotIndex);
//            if(depots_copy.size() == 0){
//                std::vector<CourierSubpath> empty;
//                return empty;
//            }
//            k--;
//        }
//        else{
//            //the path between the depot and the nearest intersection is available
//            break;
//        }
//    }
//    intersections.push_back(startdepot);
//    unordered_map<unsigned, unordered_map<unsigned, DPath>> all_path_structure;
    find_all_path (intersections, right_turn_penalty, left_turn_penalty);
    
    visited = 1;

    CourierSubpath subPathf;
    subPathf.start_intersection = startdepot;
    subPathf.end_intersection = startIntersection;
    subPathf.subpath = find_path_between_intersections(startdepot, startIntersection, right_turn_penalty, left_turn_penalty);
//    subPathf.subpath = all_path_structure[startdepot][startIntersection].dpath;
//    std::cout << subPathf.subpath.size() << std::endl;
//    std::cout << "length of the first subPath (depot start)::  " << subPathf.subpath.size() << std::endl;
    //consider about the corner case that the intersection cannot be accessed
//    if(subPathf.subpath.size() == 0 && startdepot != startIntersection){
//        std::vector<CourierSubpath> empty;
//        return empty;
//    }
    all_path.push_back(subPathf);
    currentCapacity = truck_capacity - deliveries[firstpickup].itemWeight;
    intersection_info.erase(intersection_info.begin() + firstpickup*2);
    picked_up_id.push_back(firstpickup);
    //while package to deliver
//        while the size of vector that stores all valid intersections is not empty
    while(intersection_info.size() != 0){       
        //if the current capacity is smaller than the next nearest intersection pick up point
        //then chose to deliver. before dropping off, see if the item is picked up
        //what to pass in to the Dijkstra_find_path function
        //how to make sure the deliver point is already visited
        //resize or initialize the return vector
        std::vector<delivery_info>condition_intersections;
        condition_intersections = intersection_info;
        std::vector<unsigned> deliveries_index;
        double distance;
        DPath pathTime;
        double time;
        int currIntersection;
        std::string type;
        std::string shortestType;
        int deliveryID;
        CourierSubpath subPath;
        float weight;
        if(visited == 1){
            prevIntersection = startIntersection;
            prevDeliveryID = firstpickup;
            prevType = "pickUp";
        }
        //if there is only on intersection left in the list
        //just pull it in the end of the all_path vector
        if(intersection_info.size() == 1){
            currIntersection = intersection_info[0].dropOff;
            currentCapacity = currentCapacity + weight;
            subPath.start_intersection = prevIntersection;
            subPath.end_intersection = currIntersection;
//            subPath.subpath = find_path_between_intersections(prevIntersection, currIntersection, right_turn_penalty, left_turn_penalty); 
            subPath.subpath = all_path_structure[prevIntersection][currIntersection].dpath;
//            std::cout << "length of the last subPath: " << subPath.subpath.size() << std::endl;
//            if(subPath.subpath.size() == 0 && prevIntersection != currIntersection){
//                std::vector<CourierSubpath> empty;
//                return empty;
//            }
            if(prevType == "pickUp"){
                subPath.pickUp_indices.push_back(prevDeliveryID);
            }
            else{
                subPath.pickUp_indices = deliveries_index;
            }
            all_path.push_back(subPath);
            visited = visited + 1;
            prevIntersection = currIntersection;
            prevDeliveryID = intersection_info[0].deliverID;
            prevType = "dropOff";
            break;
        }          
        //for pick-up, need to consider about the capacity
        //for drop off, need to see if it is picked up first
        //erase all the un-satisfied intersections
        for(int i = 0; i < condition_intersections.size();){
            unsigned pick = condition_intersections[i].pickUp;
            //if that intersection is a pick-up point
            if(pick != 0){
                if(condition_intersections[i].itemWeight > currentCapacity){
                    //remaining weight is not able to pick up that deliveries
                    condition_intersections.erase(condition_intersections.begin() + i);
                }
                else{
                    //can be put onto the truck, check the distance
                    i++;
                }
            }
            //the intersection is a drop-off point
            else{
                bool flag = false;
                for(int j = 0; j < picked_up_id.size(); j++){
                    //if the picked up intersection exists in the list that the truck has been to
                    //reserve in the vector, otherwise, erase the element
                    if(condition_intersections[i].deliverID == picked_up_id[j]){
                        flag = true;
                        break;
                    }
                }
                if(flag){
                    i++;
                }
                else{
                    condition_intersections.erase(condition_intersections.begin() + i);
                }
            }
        }
        //start finding the closest valid intersection to the previous intersection
        for(int i = 0; i < condition_intersections.size(); i++){
            double subShortestDistance;
            double subTime;
            if(condition_intersections[i].pickUp != 0){
                //for pick up
//                distance = find_distance_between_two_points(getIntersectionPosition(prevIntersection), 
//                        getIntersectionPosition(condition_intersections[i].pickUp));
//                distance = 
                pathTime = all_path_structure[prevIntersection][condition_intersections[i].pickUp];
                
                
                time = pathTime.time;
                type = "pickUp";
            }
            else{
                //for drop off
//                distance = find_distance_between_two_points(getIntersectionPosition(prevIntersection), 
//                        getIntersectionPosition(condition_intersections[i].dropOff));
                pathTime = all_path_structure[prevIntersection][condition_intersections[i].dropOff];
                            
                
                time = pathTime.time;
                type = "dropOff";
            }
            //compare to find the shortest distance
            if(i == 0){
                shortestType = type;
//                subShortestDistance = distance;
                subTime = time;
                if(type == "pickUp"){
                    currIntersection = condition_intersections[i].pickUp;
                    deliveryID = condition_intersections[i].deliverID;
                    weight = condition_intersections[i].itemWeight;
                }
                else{
                    currIntersection = condition_intersections[i].dropOff;
                    deliveryID = condition_intersections[i].deliverID;
                    weight = condition_intersections[i].itemWeight;
                }
            }
            else{
//                if(subShortestDistance > distance){
                if(subTime > time){
//                    subShortestDistance = distance;
                    subTime = time;
                    if(type == "pickUp"){
                        currIntersection = condition_intersections[i].pickUp;
                        deliveryID = condition_intersections[i].deliverID;
                        weight = condition_intersections[i].itemWeight;
                        shortestType = type;
                    }
                    else{
                        currIntersection = condition_intersections[i].dropOff;
                        deliveryID = condition_intersections[i].deliverID;
                        weight = condition_intersections[i].itemWeight;
                        shortestType = type;
                    }
                }
            }
        }
        //put the two for loop together
        //another loop to 
        //erase the intersection in the intersection_info
        //change the size of the intersection_info
        for(int i = 0; i < intersection_info.size(); i++){
            //find the corresponding intersection id that is the closest to 
            //the previous intersection, need to erase it out of the loop
            if(intersection_info[i].deliverID == deliveryID){
                //check if it is pick up or drop off
                intersection_info.erase(intersection_info.begin() + i);
                break;
            }
        }
        //consider the delivery change on the truck
        //if it is a pick up, put the ID in picked_up_id vector
        if(shortestType == "pickUp"){
            currentCapacity = currentCapacity - weight;
            picked_up_id.push_back(deliveryID);
        }
        else{
            currentCapacity = currentCapacity + weight;
        }
        //put the intersection in the returned back vector
        subPath.start_intersection = prevIntersection;
        subPath.end_intersection = currIntersection;
//        subPath.subpath = find_path_between_intersections(prevIntersection, currIntersection, right_turn_penalty, left_turn_penalty);
        subPath.subpath = all_path_structure[prevIntersection][currIntersection].dpath;
        //subPath.subpath = find_path_between_intersections(prevIntersection, currIntersection, right_turn_penalty, left_turn_penalty);
        
//        std::cout << "middle subPath: " << subPath.subpath.size() << std::endl;
//        if(subPath.subpath.size() == 0 && prevIntersection != currIntersection){
//            std::vector<CourierSubpath> empty;
//            return empty;
//        }
        if(prevType == "pickUp"){
            subPath.pickUp_indices.push_back(prevDeliveryID);
        }
        else{
            subPath.pickUp_indices.clear();
        }
        all_path.push_back(subPath);
        subPath.pickUp_indices.clear();
        visited = visited + 1;
        prevIntersection = currIntersection;
        prevDeliveryID = deliveryID;
        prevType = shortestType;
    }
    //go to nearest depot, order of size of depot
    //the items on the truck should be 0
    double finalShortest;
    int finalDepot;
    int finalDepotIndex;
//    for(int j = 0; j < depots_copy.size(); j++){
        for(int i = 0; i < depots_copy.size(); i++){
            //find the distance between the all the  depots and all the last intersection
            double distance = find_distance_between_two_points(getIntersectionPosition(prevIntersection), 
                    getIntersectionPosition(depots_copy[i]));
//            double distance = all_path_structure[prevIntersection][depots_copy[i]].dpath;
            //the first intersection
            if(i == 0){
                finalShortest = distance;
                finalDepot = depots_copy[i];
                finalDepotIndex = i;
            } 
            else{
                if(finalShortest > distance){
                    finalShortest = distance;
                    finalDepot = depots_copy[i];
                    finalDepotIndex = i;
                }      
            }
        }
        //consider about the corner case
        //depot intersection is not accessible
//        std::vector<unsigned> path2 = find_path_between_intersections(prevIntersection, finalDepot, 
//                right_turn_penalty, left_turn_penalty);
//        if(path2.size() == 0 && prevIntersection != finalDepot){
//            //this depot is not available
//            depots_copy.erase(depots_copy.begin() + finalDepotIndex);
//            if(depots_copy.size() == 0){
//                std::vector<CourierSubpath> empty;
//                return empty;
//            }
//            j--;
//        }
//        else{
//            //the path between the depot and the nearest intersection is available
//            break;
//        }
//    }
    
    subPathf.start_intersection = prevIntersection;
    subPathf.end_intersection = finalDepot;
    subPathf.subpath = find_path_between_intersections(prevIntersection, finalDepot, right_turn_penalty, left_turn_penalty);
//    std::cout << "last subPath (final depot): " << subPathf.subpath.size() << std::endl;
//    if(subPathf.subpath.size() == 0 && prevIntersection != finalDepot){
//        std::vector<CourierSubpath> empty;
//        return empty;
//    }
    all_path.push_back(subPathf);
    
    
    
//    for(int i = 0; i < all_path.size(); i++){
//        std::cout << "start intersection: " << all_path[i].start_intersection << std::endl;
//        std::cout << "end intersection: " << all_path[i].end_intersection << std::endl;
//
//        std::cout << "path size: " << all_path[i].subpath.size() << std::endl;
//    }

    
//    std::cout << "**********************************about to call two_opt**********************************" << endl;
//    two_opt(all_path, deliveries, truck_capacity);
    
    return all_path;
}

// return the closest intersection (away from the startID intersection) for pick up or drop off
DNode Dijkstra_find_path (unsigned startID,
        const std::vector<DeliveryInfo>& deliveries,
        const float right_turn_penalty,
        const float left_turn_penalty){
   
}


/*
vector<vector<DPath>> find_all_path (const vector<unsigned>& intersections,
                                                const float right_turn_penalty,
                                                const float left_turn_penalty){
    vector<vector<DPath>> all_paths(intersections.size());
//#pragma omp parallel for
    for(unsigned i=0; i=intersections.size(); i++){
        all_paths[i] = find_paths_from_intersections_dijkstra (intersections[i], intersections,
                                                            right_turn_penalty, left_turn_penalty);
    }
    return all_paths;
}
*/
/*
class compare {
public:
    bool operator()(DNode& a, DNode& b)
    {
        if(a.gscore>=b.gscore) {
            return true;
        }
        return false;
    }
};

unordered_map<unsigned, DNode> close_set_m4;
vector<DPath> find_paths_from_intersections_dijkstra(unsigned intersect_id_start, vector<unsigned> intersection_list,
                                                                const float right_turn_penalty, const float left_turn_penalty){
    unsigned interNum = intersection_list.size();
    vector<DPath> result_path(interNum);
    close_set_m4.clear();
    vector <DNode> open_set;
    open_set.resize(interNum);
    priority_queue<DNode, vector<DNode>, compare> pri_queue;
    // put the starting node on the open list, and leave all score to 0
    DNode start;
    start.intersection_id=intersect_id_start;
    start.parent_node=intersect_id_start;
    start.gscore=0;
    start.path_segment = intersect_id_start;
   
    pri_queue.push(start);
    open_set[intersect_id_start] = start;

    bool done=false;
    while(!pri_queue.empty()){
        if(done){
            return result_path;
        }
        if(!done){ // this should be the condition to stop iteration, where all delivery intersections have been visited
            //find the node with the least f on the open list, call it "current node"
            DNode current_node;
            current_node=pri_queue.top();
            //pop off this node from the open list and open set
            pri_queue.pop();                       
            int curr_intersect_id = current_node.intersection_id;
            open_set[curr_intersect_id] = DNode();
            //set it into close set
            open_set[curr_intersect_id].visited = true;
            close_set_m4.insert(make_pair(current_node.intersection_id,current_node));
           
            //if reach the intersection in the delivery list, trace back
            for(unsigned i=0; i<interNum; i++){
                if(current_node.intersection_id==intersection_list[i]){
                    DPath path_with_time;
                    vector<unsigned> path;
                    DNode current=current_node;
                    while(current.intersection_id != start.intersection_id){
                        path.insert(path.begin(),close_set_m4.find(current.intersection_id)->second.path_segment);
                        current.intersection_id=current.parent_node;
                        current.parent_node=close_set_m4.find(current.intersection_id)->second.parent_node;
                        }
                    path_with_time.dpath = path;
                    path_with_time.time = current_node.gscore;
                    result_path[i] = path_with_time;
                    intersection_list.erase(intersection_list.begin()+i); //remove this intersection from the list
                }
            }
            if(intersection_list.size()==0){
                done = true;
            }
           
           
            //generate "current node" all neighbors and set their parent to the "current node"
            vector<unsigned> neighbor_list=find_adjacent_intersections(current_node.intersection_id); 
            unsigned adjacent_size = neighbor_list.size();
            for(unsigned i=0; i<adjacent_size; i++){  //traverse all intersections

                unsigned neighbor_intersect_id = neighbor_list[i];
                DNode neighborNode = open_set[neighbor_list[i]];

                if(neighborNode.visited == true){
                    continue;
                }
                //check if the neighbor is in the open list
                bool in_open_set = neighborNode.in_openlist;
                if (in_open_set==false){
                    //if neighbor is not in open set
                    //set gscore, hscore, parent_node, and put it into open set
                    neighborNode.intersection_id=neighbor_intersect_id;  
                    neighborNode.parent_node = current_node.intersection_id;

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
                        neighborNode.path_segment = seg2;
                        //neighborNode.in_openlist = true;
                        pri_queue.push(neighborNode);
                    }
                }
            }
           
        }
    }
    //if not found any, return an empty vector
    vector<DPath> empty;
    return empty;
}

 */ 




//unordered_map<unsigned, unordered_map<unsigned, DPath>> all_path_structure;
void find_all_path (const vector<unsigned>& intersections,
                    const float right_turn_penalty,
                    const float left_turn_penalty){
#pragma omp parallel for
    for(unsigned i=0; i<intersections.size(); i++){
        //std::cout << "out loop: " << intersections[i] << endl;
        
        unordered_map<unsigned, DPath> path_from_intersection = find_paths_from_intersections_dijkstra (intersections[i], intersections,right_turn_penalty, left_turn_penalty);
        //all_path_structure.insert(std::make_pair(i, path_from_intersection));
        unsigned intersection_id = intersections[i];
        #pragma omp critical
        all_path_structure[intersection_id] = path_from_intersection;
   }
}


class compare {
public:
    bool operator()(DNode& a, DNode& b)
    {
        if(a.gscore>=b.gscore) {
            return true;
        }
        return false;
    }
};


unsigned find_shortest_segment(unsigned curr_intersect_id,unsigned next_intersect_id);

unordered_map<unsigned, DPath> find_paths_from_intersections_dijkstra (unsigned intersect_id_start, vector<unsigned> intersection_list,
                                                                const float right_turn_penalty, const float left_turn_penalty){
    unordered_map<unsigned, DNode> close_set_m4;
    unordered_map<unsigned, DNode> open_set;
    unsigned interNum = intersection_list.size();
    unordered_map<unsigned, DPath> result_path(interNum);
    close_set_m4.clear();
    priority_queue<DNode, vector<DNode>, compare> pri_queue;
    // put the starting node on the open list, and leave all score to 0
    DNode start;
    start.intersection_id=intersect_id_start;
    start.parent_node=intersect_id_start;
    start.gscore=0;
    start.path_segment = intersect_id_start;
   
    //copy to a new vector
    vector<unsigned> intersection_list_copy (interNum);
    intersection_list_copy = intersection_list;
    unsigned count = interNum;
    
    pri_queue.push(start);
    open_set[intersect_id_start] = start;
    
    while(!pri_queue.empty()){ 

            //find the node with the least f on the open list, call it "current node"
            DNode current_node;
            current_node=pri_queue.top();
            //pop off this node from the open list and open set
            pri_queue.pop();                       
            int curr_intersect_id = current_node.intersection_id;
            open_set[curr_intersect_id] = DNode();
            //set it into close set
            open_set[curr_intersect_id].visited = true;
            close_set_m4.insert(make_pair(current_node.intersection_id,current_node));
           
            //if reach the intersection in the delivery list, trace back
            for(unsigned i=0; i<interNum; i++){
                if(current_node.intersection_id==intersection_list[i]){
                    DPath path_with_time;
                    vector<unsigned> path;
                    DNode current=current_node;
                    while(current.intersection_id != start.intersection_id){
                        path.insert(path.begin(),close_set_m4.find(current.intersection_id)->second.path_segment);
                        current.intersection_id=current.parent_node;
                        current.parent_node=close_set_m4.find(current.intersection_id)->second.parent_node;
                        }
                    path_with_time.dpath = path;
                    path_with_time.time = current_node.gscore;
                    unsigned end_intersection_id = intersection_list[i];
                    result_path[end_intersection_id] = path_with_time;
                    count--;
                    //vector<unsigned>::iterator it = std::find(intersection_list_copy.begin(), intersection_list_copy.end(), end_intersection_id);
                    //intersection_list_copy.erase(intersection_list_copy.begin()+i); //remove this intersection from the list
                    //intersection_list_copy.erase(it);
                    continue;
                }
            }
            if(count==0){
                return result_path;
            }
           
           
            //generate "current node" all neighbors and set their parent to the "current node"
            vector<unsigned> neighbor_list=find_adjacent_intersections(current_node.intersection_id); 
            unsigned adjacent_size = neighbor_list.size();
            for(unsigned i=0; i<adjacent_size; i++){  //traverse all intersections

                unsigned neighbor_intersect_id = neighbor_list[i];
                DNode neighborNode = open_set[neighbor_list[i]];

                if(neighborNode.visited == true){
                    continue;
                }
                //check if the neighbor is in the open list
                bool in_open_set = neighborNode.in_openlist;
                if (in_open_set==false){
                    //if neighbor is not in open set
                    //set gscore, hscore, parent_node, and put it into open set
                    neighborNode.intersection_id=neighbor_intersect_id;  
                    neighborNode.parent_node = current_node.intersection_id;

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
                        neighborNode.path_segment = seg2;
                        neighborNode.in_openlist = true;
                        //pri_queue.push(neighborNode);
                    }
                }
            }
           
        }
    //if not found any, return an empty vector
    unordered_map<unsigned, DPath> empty;
    return empty;
}

/*
class compare {
public:
    bool operator()(DNode* a, DNode* b)
    {
        if(a->gscore>=b->gscore) {
            return true;
        }
        return false;
    }
};
unsigned find_shortest_segment(unsigned curr_intersect_id,unsigned next_intersect_id);

unordered_map<unsigned, DPath> find_paths_from_intersections_dijkstra (unsigned intersect_id_start, vector<unsigned> intersection_list,
                                                                const float right_turn_penalty, const float left_turn_penalty){
    
    unsigned interNum = intersection_list.size();
    unordered_map <unsigned, DNode> close_set_m4;
    close_set_m4.clear();
    unordered_map <unsigned, DNode> open_set;
    //open_set.resize(interNum);
    priority_queue<DNode*, vector<DNode*>, compare> pri_queue;
    // put the starting node on the open list, and leave all score to 0
    DNode start;
    start.intersection_id=intersect_id_start;
    start.parent_node=intersect_id_start;
    start.gscore=0;
    start.path_segment = intersect_id_start;
   
    pri_queue.push(&start);
    open_set[intersect_id_start] = start;
   
    int count = interNum;  // count is used to judge whether all intersections in the list has pre-computed
    unordered_map <unsigned, DPath> result_path;
    while(!pri_queue.empty()){
        //find the node with the least f on the open list, call it "current node"
        DNode* current_node;
        current_node=pri_queue.top();
        current_node->visited = true;
        //pop off this node from the open list and open set
        pri_queue.pop();                       
        int curr_intersect_id = current_node->intersection_id;
        //open_set[curr_intersect_id] = DNode();
        //set it into close set
        //open_set[curr_intersect_id].visited = true;
        close_set_m4.insert(make_pair(current_node->intersection_id, *current_node));
       
        //check whether current_node id = intersection id in the list
        for(int i=0; i<intersection_list.size(); i++){
            //if find one end, trace back this path. Put path and time into result_path
            if(intersection_list[i] == curr_intersect_id){
                double temp_time = current_node->gscore;
                vector<unsigned> path;
                DNode* current = current_node;
                while(current->intersection_id != start.intersection_id){
                    //trace back the path
                    path.insert(path.begin(),close_set_m4[current->intersection_id].path_segment);
                    //current->intersection_id=current->parent_node;
                    //current->parent_node=close_set_m4[current->intersection_id].parent_node;
                    unsigned parent_id = close_set_m4[current->intersection_id].parent_node;
                    current = &close_set_m4 [parent_id];
                }
                DPath path_with_time;
                path_with_time.dpath = path;
                
                    
                //path_with_time.time = current_node->gscore;
                path_with_time.time = temp_time;
                result_path[curr_intersect_id] = path_with_time;
                count--; 
            }
        }
       
       
        //generate "current node" all neighbors and set their parent to the "current node"
        vector<unsigned> neighbor_list=find_adjacent_intersections(current_node->intersection_id); 
        unsigned adjacent_size = neighbor_list.size();
        for(unsigned i=0; i<adjacent_size; i++){  //traverse all intersections
            
            unsigned neighbor_intersect_id = neighbor_list[i];
            DNode* neighborNode = &open_set[neighbor_list[i]];
           
            if(neighborNode->visited == true){
                continue;
            }
            

           
            //check if the neighbor is in the open list
            bool in_open_set = neighborNode->in_openlist;
            if (!in_open_set){
                //if neighbor is not in open set
                //set gscore, hscore, parent_node, and put it into open set
                neighborNode->intersection_id = neighbor_intersect_id;  
                neighborNode->parent_node = current_node->intersection_id;

                unsigned seg1 = current_node->path_segment; // if the first time, "start" is the current_node, path_seg is itself
                unsigned seg2 = find_shortest_segment(current_node->intersection_id, neighborNode->intersection_id);
                neighborNode->path_segment = seg2;
                // compute g_score, using path travel time
                double neighbor_seg_travel_time = find_street_segment_travel_time(seg2);
                TurnType turn = find_turn_type(seg1, seg2);
                if(turn == TurnType::LEFT){
                    neighbor_seg_travel_time = neighbor_seg_travel_time + left_turn_penalty;
                } else if(turn == TurnType::RIGHT){
                    neighbor_seg_travel_time = neighbor_seg_travel_time + right_turn_penalty;
                }
                neighborNode->gscore = neighbor_seg_travel_time + current_node->gscore;
               
                // put this node into open list and open set
                neighborNode->in_openlist = true;
                pri_queue.push(neighborNode);
                open_set[neighborNode->intersection_id] = *neighborNode;  
                if(neighborNode->intersection_id == 18552){
                }
            }
            else{
                // if the neighbor node is in the open set
                // calculate the g_score from current_node to neighbor_node
                // if this g_score is smaller than previously calculated g value
                // set the parent_node of neighbor_node to current_node
                // and also update the gscore, hscore
                neighborNode=&open_set[neighbor_intersect_id];
                unsigned seg1 = current_node->path_segment;
                unsigned seg2 = find_shortest_segment(current_node->intersection_id, neighborNode->intersection_id);
                double neighbor_seg_travel_time = find_street_segment_travel_time(seg2);
                    TurnType turn = find_turn_type(seg1, seg2);
                    if(turn == TurnType::LEFT){
                        neighbor_seg_travel_time = neighbor_seg_travel_time + left_turn_penalty;
                    } else if(turn == TurnType::RIGHT){
                        neighbor_seg_travel_time = neighbor_seg_travel_time + right_turn_penalty;
                    }
                double new_gscore = neighbor_seg_travel_time + current_node->gscore;
                //if found that the score is lower, update all info to this neighbor node
                
                if(new_gscore<neighborNode->gscore){     
                   
                    neighborNode->intersection_id=neighbor_intersect_id;
                    neighborNode->gscore=new_gscore;
                    neighborNode->parent_node=current_node->intersection_id;
                    neighborNode->path_segment = seg2;
                    neighborNode->in_openlist = true;
                    pri_queue.push(neighborNode);
                }
            }
        }
       
          
           
    }
    //for(unsigned i=0; i<result_path.size(); i++){
    
        //cout<<"start: "<<intersect_id_start<<" end:  time: "<<result_path[41932].time<<", "<<endl;;
    //}

    return result_path;
    
    //if not found any, return an empty vector
    unordered_map<unsigned, DPath> empty;
    return empty;
}

*/





void two_opt(std::vector<CourierSubpath> &existing_route, const std::vector<DeliveryInfo>& deliveries, const float truck_capacity){
//    start_again:
//       best_distance = calculateTotalDistance(existing_route)
//       for (i = 0; i < number of nodes eligible to be swapped - 1; i++) {
//           for (k = i + 1; k < number of nodes eligible to be swapped; k++) {
//               new_route = 2optSwap(existing_route, i, k)
//               new_distance = calculateTotalDistance(new_route)
//               if (new_distance < best_distance) {
//                   existing_route = new_route
//                   goto start_again
//               }
//           }
//       }
    
    
    int size = existing_route.size();
    bool valid_route=false;
    int numOfNodes;
    double route_travel_time=0;
    //    double new_travel_time=0;
    
    
    
    std::vector<locationInfo> route;
    std::vector<locationInfo> new_route;
    std::vector<unsigned> path;
    route.resize(size-1);
//    std::cout << "existing route size: " << size << endl;
//    std::cout << "route size: " << route.size() << endl;
    
    //store the start/end intersections into a new vector for swapping
    unsigned start_depot = existing_route[0].start_intersection;
    unsigned end_depot = existing_route[size-1].end_intersection;
    for(int i=1; i<size; i++){
        route[i-1].intersectionID = existing_route[i].start_intersection;
//        route[i-1].pickupIndices = existing_route[i].pickUp_indices;
        //check if the intersection is a pick-up or drop-off
        if(existing_route[i].pickUp_indices.size()==0){//drop off location
            route[i-1].type = "drop off";
            int pickup_id;
            int dropoff_id = route[i-1].intersectionID;
            bool item_found = false;
            //loop through deliveries to get the corresponding pick up id
            for(int j=0; j<deliveries.size(); j++){
                if(deliveries[j].dropOff == dropoff_id){
                    pickup_id = deliveries[j].pickUp;
                    //check if this item has been picked up
                    for(int k=1; k<i; k++){
                        if(existing_route[i].start_intersection == pickup_id){
                            route[i-1].itemWeight = -(deliveries[j].itemWeight);
                            item_found = true;
                            break;
                        }
                            
                    }
                    if(item_found) break;
                }
            }
            
        }
        else{ //pick up location
            route[i-1].type = "pick up";
            route[i-1].pickupIndices = existing_route[i].pickUp_indices;
            int index = existing_route[i].pickUp_indices[0];
            route[i-1].itemWeight = deliveries[index].itemWeight;
        }
        
    }
    numOfNodes = route.size();
    new_route.resize(numOfNodes);
    
   
    //compute the travel time of the old route
    //first calculate the travel time between start depot and the first intersection
    path = find_path_between_intersections(start_depot, route[0].intersectionID, 15, 25);
    for(int i=0; i<path.size(); i++){
        route_travel_time = route_travel_time + find_street_segment_travel_time(path[i]);
    }
    //then calculate the travel time between the last intersection and the end depot
    path = find_path_between_intersections(route[size-2].intersectionID, end_depot, 15, 25);
    for(int i=0; i<path.size(); i++){
        route_travel_time = route_travel_time + find_street_segment_travel_time(path[i]);
    }
    //then calculate the travel time of the route
    for(int i=0; i<numOfNodes-1; i++){
        path = find_path_between_intersections(route[i].intersectionID, route[i+1].intersectionID, 15, 25);
        for(int j=0; j<path.size(); j++){
            route_travel_time = route_travel_time + find_street_segment_travel_time(path[j]);
        }
    }

//    std::cout << "++++++++++++++++++ old time: " << route_travel_time << endl;
    
    //do the 2-opt swap
    for(int i=0; i<numOfNodes; i++){
        for(int k=i+1; k<numOfNodes; k++){
            
            two_opt_swap(route, new_route, deliveries, i, k, truck_capacity, valid_route);
           
            if(valid_route){
                //new route is valid, compute the new travel time
                double new_travel_time=0;
                int new_route_size = new_route.size();
                //first calculate the travel time between start depot and the first intersection
                path = find_path_between_intersections(start_depot, new_route[0].intersectionID, 15, 25);
                for(int n=0; n<path.size(); n++){
                    new_travel_time = new_travel_time + find_street_segment_travel_time(path[n]);
                }
                //then calculate the travel time between the end depot and the last intersection
                path = find_path_between_intersections(end_depot, new_route[new_route_size-1].intersectionID, 15, 25);
                for(int n=0; n<path.size(); n++){
                    new_travel_time = new_travel_time + find_street_segment_travel_time(path[n]);
                }
                //now calculate the travel time of the new route
                for(int n=0; n<new_route_size-1; n++){
                    path = find_path_between_intersections(new_route[i].intersectionID, new_route[i+1].intersectionID, 15, 25);
                    for(int m=0; m<path.size(); m++){
                        new_travel_time = new_travel_time + find_street_segment_travel_time(path[m]);
                    }
                }


                //check if the new_route is a better solution
                //also need to check if the remaining time is enough to update the route
                //...
                //...
                //...
                //if yes, then overwrite the existing route
                if(new_travel_time < route_travel_time){
                    //update the travel time
                    route_travel_time = new_travel_time;
                    //update the first node of the route
                    existing_route[0].end_intersection = new_route[0].intersectionID;
                    existing_route[0].subpath = find_path_between_intersections(start_depot, new_route[0].intersectionID, 15, 25);
                    //update the existing route
                    for(int i=0; i<new_route.size(); i++){
                        existing_route[i].end_intersection = new_route[i].intersectionID;
                        existing_route[i+1].start_intersection = new_route[i].intersectionID;
//                        existing_route[i+1].pickUp_indices = new_route[i].pickupIndices;
                        existing_route[i+1].subpath = find_path_between_intersections(
                                                        existing_route[i+1].start_intersection,
                                                        existing_route[i+1].end_intersection,
                                                        15, 25);
                       
                    }
                }
            }
                
            
        }
    }
//    std::cout << "++++++++++++++++++ new time: " << new_travel_time << endl;
}



//function to do the swap for 2-opt
//returns a vector of the swapped intersections
void two_opt_swap(std::vector<locationInfo> &route, std::vector<locationInfo> &new_route, 
                  const std::vector<DeliveryInfo>& deliveries, int i, int k, 
                  const float truck_capacity, bool &valid_route){

    int route_size = route.size();
    int b=k;
    
    
    
    /////////////////////* checking swap legality here */////////////////////////////
    
    //illegal to swap the first intersection with a drop off
    if(i==0){
        if(route[k].type!="pick up"){
            valid_route = false;
            return;
        }
    }
    //illegal to swap the first intersection with the last, simply return
    if((i==0)&&(k==route_size-1)){
        valid_route = false;
        return;
    }
    //illegal to swap the last intersection with a pick up
    if(k==route_size-1){
        if(route[i].type!="drop off"){
            valid_route = false;
            return;
        }
    }
    
    
    
    //a temporary vector to store the intersections between i and k
    std::vector<locationInfo> temp;
    int temp_size = k-i+1;
    float total_picked_up_weight = 0;
    float total_item_weight = 0;
    temp.resize(temp_size);
    
    //check the legality of swapping intersections between i and k
    for(int n=0; n<temp_size; n++){
        //pick up location, check truck capacity
        if(route[k-n].type=="pick up"){
            //find the item weight to be picked up
            total_item_weight = total_item_weight + route[k-n].itemWeight;
//            int index = route[k-n].pickupIndices[0];
//            total_item_weight = total_item_weight + deliveries[index].itemWeight;
            
            //find the accumulated item weight before this point
            for(int j=0; j<i; j++){
              total_item_weight = total_item_weight + route[j].itemWeight;
            }
            if(total_item_weight < truck_capacity){//legal swap, store in temp
                temp[n].intersectionID = route[k-n].intersectionID;
                temp[n].type = "pick up";
                temp[n].itemWeight = route[k-n].itemWeight;
                temp[n].pickupIndices = route[k-n].pickupIndices;
            }
            else{//illegal swap, return from this function
                valid_route = false;
                return;
            }
        }
        //drop off location, check if picked up
        if(route[k-n].type=="drop off"){
            int dropoff_id = route[k-n].intersectionID;
            int temp_id;
            pickupInfo temp_pickupInfo;
            std::vector<pickupInfo> pickup_id;
            //might be the drop off location for multiple items
            //store all the pick up id in a vector
            for(int j=0; j<deliveries.size(); j++){
                if(deliveries[j].dropOff == dropoff_id){
//                    temp_id = deliveries[j].pickUp;
                    temp_pickupInfo.id = deliveries[j].pickUp;
                    temp_pickupInfo.erased = false;
                    pickup_id.push_back(temp_pickupInfo);
                    
                }
            }
            //check if the pick up location has been visited
            bool visited = false;
            for(int j=0; j<i; j++){
                for(int m=0; m<pickup_id.size(); m++){
                    if(pickup_id[m].erased == false){
                        if(route[j].intersectionID == pickup_id[m].id){
                            visited = true;
                            pickup_id[m].erased = true;
                            break;
                        }
                    }
                        
                }
                if(visited) break;
            }
            
            if(visited){//legal swap, store this point in temp
                temp[n].intersectionID = route[k-n].intersectionID;
                temp[n].type = "drop off";
                temp[n].itemWeight = route[k-n].itemWeight;
                temp[n].pickupIndices = route[k-n].pickupIndices;
                //subtract the item weight from the total weight
                total_item_weight = total_item_weight + route[k-n].itemWeight;
            }  
        }
        
    }
    
    //the new route is valid, now update the existing route
    for(int a=0; a<route_size; a++){
        //take route[0] to route[i-1] and add them in order to new_route
        if(a<i) new_route[a]=route[a];
        //take route[i] to route[k] and add them in reverse order to new_route
        if((a>=i)&&(a<=k)){
            new_route[a]=temp[a];
//            b = b-1;
        }
        //take route[k+1] to end and add them in order to new_route
        if(a>k) new_route[a]=route[a];
    }
    
    valid_route = true;
}

