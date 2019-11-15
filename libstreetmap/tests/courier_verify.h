/* 
 * Copyright 2018 University of Toronto
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
#pragma once
#include <vector>
#include <map>

#include "m3.h"
#include "m4.h"

#define FLOAT_EPSILON 3e-2


namespace ece297test {

bool courier_path_is_legal(const std::vector<DeliveryInfo>& deliveries,
                          const std::vector<unsigned>& depots,
                          const std::vector<unsigned>& path);

bool courier_path_is_legal_with_capacity(const std::vector<DeliveryInfo>& deliveries,
                          const std::vector<unsigned>& depots,
                          const std::vector<CourierSubpath>& path,
                          const float truck_capacity);

bool valid_courier_problem(const std::vector<DeliveryInfo>& deliveries,
                           const std::vector<unsigned>& depots);

bool is_depot(const std::vector<unsigned>& depots,
              const unsigned intersection_id);


void pick_up_at_intersection(const std::vector<DeliveryInfo>& deliveries,
                             const std::multimap<unsigned,size_t>& intersections_to_pick_up,
                             const unsigned curr_intersection,
                             std::vector<bool>& deliveries_picked_up);

void drop_off_at_intersection(const std::vector<DeliveryInfo>& deliveries,
                              const std::multimap<unsigned,size_t>& intersections_to_drop_off,
                              const std::vector<bool>& deliveries_picked_up,
                              const unsigned curr_intersection,
                              std::vector<bool>& deliveries_dropped_off);

bool pick_up_at_intersection_capacity(const std::vector<DeliveryInfo>& deliveries,
                             const std::multimap<unsigned,size_t>& intersections_to_pick_up,
                             const std::vector<unsigned> & pickup_indices,
                             const unsigned start_intersection,
                             std::vector<bool>& deliveries_picked_up, 
                             float & current_total_weight);

bool drop_off_at_intersection_capacity(const std::vector<DeliveryInfo>& deliveries,
                              const std::multimap<unsigned,size_t>& intersections_to_drop_off,
                              const std::vector<bool>& deliveries_picked_up,
                              const unsigned curr_intersection,
                              std::vector<bool>& deliveries_dropped_off, 
                              float& current_total_weight);


bool delivered_all_packages(const std::vector<DeliveryInfo>& deliveries,
                            const std::vector<bool>& deliveries_picked_up,
                            const std::vector<bool>& deliveries_dropped_off);

bool determine_start_intersection(const std::vector<unsigned>& path,
                                  const std::vector<unsigned>& depots,
                                  unsigned& start_intersection);

bool traverse_segment(const std::vector<unsigned>& path,
                      const unsigned path_idx,
                      const unsigned curr_intersection,
                      unsigned& next_intersection);

bool is_start_intersection_correct(const CourierSubpath& cs);

bool is_end_intersection_correct(const CourierSubpath& cs);

double compute_courier_path_travel_time(const std::vector<CourierSubpath>& courier_route, 
                                        const float right_turn_penalty, 
                                        const float left_turn_penalty);
}
