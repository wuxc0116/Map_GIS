/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   node.h
 * Author: wuxiangc
 *
 * Created on March 17, 2019, 10:57 AM
 */

#ifndef NODE_H
#define NODE_H

class Node{
 public:
    
    unsigned intersection_id;
    bool visited=false;
    bool in_openlist;
    double gscore;
    double hscore;
    unsigned parent_node;
    unsigned path_segment;
    
//    Node(unsigned intersection_id, double gscore, double hscore, unsigned parent_node){
//        intersection_id = intersection_id;
//        gscore = gscore;
//        hscore = hscore;
//        parent_node = parent_node;
//    }
    
    
    
    double fscore(){
        return gscore + (hscore/48);
    }
    
   
    ~Node(){
        ;
    }
    
};


#endif /* NODE_H */

