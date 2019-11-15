/*
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
    std::vector<int>picked_up_id;
    intersection_info.resize(deliveries.size()*2);
    int visited = 0;
    float currentCapacity = 0;
    // go for any depot to nearest pickup, P
    //order of M*N
    double shortestDistance = 0;
    unsigned startIntersection;
    int firstpickup;
    int startdepot;
    int prevIntersection;
    int prevDeliveryID;
    std::string prevType;
    //go through every depots
    for(int j = 0; j < depots.size(); j++){
        for(int i = 0; i < deliveries.size(); i++){
            //pickUp goes before drop off
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
                double distance = find_distance_between_two_points(getIntersectionPosition(depots[j]), 
                        getIntersectionPosition(deliveries[i].pickUp));
                //the first intersection
                if(j == 0 && i == 0){
                    shortestDistance = distance;
                    startIntersection = deliveries[i].pickUp;
                    firstpickup = i;
                    startdepot = depots[j];
                } 
                else{
                    if(shortestDistance > distance){
                        shortestDistance = distance;
                        startIntersection = deliveries[i].pickUp;
                        firstpickup = i;
                        startdepot = depots[j];
                    }      
                }
            }
        }
    }

    visited = 1;

    CourierSubpath subPathf;
    subPathf.start_intersection = startdepot;
    subPathf.end_intersection = startIntersection;
    subPathf.subpath = find_path_between_intersections(startdepot, startIntersection, right_turn_penalty, left_turn_penalty);
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
            subPath.subpath = find_path_between_intersections(prevIntersection, currIntersection, right_turn_penalty, left_turn_penalty);
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
            if(condition_intersections[i].pickUp != 0){
                //for pick up
                distance = find_distance_between_two_points(getIntersectionPosition(prevIntersection), 
                getIntersectionPosition(condition_intersections[i].pickUp));
                type = "pickUp";

            }
            else{
                //for drop off
                distance = find_distance_between_two_points(getIntersectionPosition(prevIntersection), 
                getIntersectionPosition(condition_intersections[i].dropOff));
                type = "dropOff";
            }
            //compare to find the shortest distance
            if(i == 0){
                shortestType = type;
                subShortestDistance = distance;
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
                if(subShortestDistance > distance){
                    subShortestDistance = distance;
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
        subPath.subpath = find_path_between_intersections(prevIntersection, currIntersection, right_turn_penalty, left_turn_penalty);
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
    for(int i = 0; i < depots.size(); i++){
        //find the distance between the all the  depots and all the last intersection
        double distance = find_distance_between_two_points(getIntersectionPosition(prevIntersection), 
                getIntersectionPosition(depots[i]));
        //the first intersection
        if(i == 0){
            finalShortest = distance;
            finalDepot = depots[i];
        } 
        else{
            if(finalShortest > distance){
                finalShortest = distance;
                finalDepot = depots[i];
            }      
        }
    }
    subPathf.start_intersection = prevIntersection;
    subPathf.end_intersection = finalDepot;
    subPathf.subpath = find_path_between_intersections(prevIntersection, finalDepot, right_turn_penalty, left_turn_penalty);
    all_path.push_back(subPathf);
    return all_path;
}

// return the closest intersection (away from the startID intersection) for pick up or drop off
DNode Dijkstra_find_path (unsigned startID,
        const std::vector<DeliveryInfo>& deliveries,
        const float right_turn_penalty,
        const float left_turn_penalty){
   
}



vector<vector<DPath>> find_all_path (const vector<unsigned>& intersections,
                                                const float right_turn_penalty,
                                                const float left_turn_penalty){
    vector<vector<DPath>> all_paths(intersections.size());
    for(unsigned i=0; i=intersections.size(); i++){
        all_paths[i] = find_paths_from_intersections_dijkstra (intersections[i], intersections,
                                                            right_turn_penalty, left_turn_penalty);
    }
    return all_paths;
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
}



//unsigned find_shortest_segment(unsigned curr_intersect_id,unsigned next_intersect_id){
//    const vector<unsigned>& segments=find_intersection_street_segments_ref(curr_intersect_id);
//    double shorest_time = UINT_MAX;
//    unsigned shortest_segid = 0;
//    for(unsigned i=0; i<segments.size(); i++){
//       
//        //if two streets is connected
//        if(static_cast<unsigned>(getInfoStreetSegment(segments[i]).from)==next_intersect_id || static_cast<unsigned>(getInfoStreetSegment(segments[i]).to)==next_intersect_id){
//            if(static_cast<unsigned>(getInfoStreetSegment(segments[i]).oneWay) && static_cast<unsigned>(getInfoStreetSegment(segments[i]).to) !=next_intersect_id){
//                //if curr_intersect cannot reach next_intersect due to one way, skip it
//                continue;
//            }
//            double seg_time = find_street_segment_travel_time(segments[i]);
//            if(seg_time<shorest_time){
//                shorest_time = seg_time;
//                shortest_segid = segments[i];
//            }
//        }
//    }
//    return shortest_segid;
//}



void two_opt(std::vector<CourierSubpath> existing_route){
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
    
    //store the start/end intersections into a new vector for swapping
    std::vector<unsigned> route;
    route.resize(existing_route.size()+1);
    route[0]=existing_route[0].start_intersection;
    for(int m=0; m<existing_route.size(); m++){
        route[m] = existing_route[m].end_intersection;

    }
    
    int numOfNodes = route.size();
    double route_travel_time=0;
    double new_travel_time=0;
    std::vector<unsigned> new_route;
    std::vector<unsigned> path;
    
    //compute the travel time of the route found by greedy algorithm
    for(int i=0; i<numOfNodes-1; i++){
        path = find_path_between_intersections(route[i], route[i+1], 15, 25);
        for(int j=0; j<path.size(); j++){
            route_travel_time = route_travel_time + 
                    find_street_segment_travel_time(path[j]);
        }
    }

    //do the 2-opt swap
    for(int i=0; i<numOfNodes; i++){
        for(int k=i+1; k<numOfNodes; k++){
            new_route = two_opt_swap(route, i, k);
        }
        
        
        //need to check if the new route is legal
        //...
        //...
        //...
        
        
        //new route is legal
        //compute the new travel time
        for(int m=0; m<numOfNodes-1; m++){
            path = find_path_between_intersections(new_route[m], new_route[m+1], 15, 25);
            for(int n=0; n<path.size(); n++){
                new_travel_time = new_travel_time + 
                        find_street_segment_travel_time(path[n]);
            }
        }
        
        //compare the new travel time with the existing travel time
        if(new_travel_time < route_travel_time) route = new_route;
    }
    
}



std::vector<unsigned>two_opt_swap(std::vector<unsigned> route, int i, int k){

    int route_size = route.size();
    std::vector<unsigned> new_route;
    new_route.resize(route_size);
    int b=k;
    for(int a=0; a<route_size; a++){
        //take route[1] to route[i-1] and add them in order to new_route
        if(a<i) new_route[a]=route[a];
        
        //take route[i] to route[k] and add them in reverse order to new_route
        if((a>=i)&&(a<=k)){
            new_route[a]=route[b];
            b = b-1;
        }
        
        //take route[k+1] to end and add them in order to new_route
        if(a>k) new_route[a]=route[a];
        std::cout<< "======================" << a << ": " << new_route[a] << endl;
    }
    return new_route;
}
*/