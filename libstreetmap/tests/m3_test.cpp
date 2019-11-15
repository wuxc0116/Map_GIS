#include <random>
#include <unittest++/UnitTest++.h>

#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include "m3.h"

#include "unit_test_util.h"
#include "path_verify.h"

using ece297test::relative_error;
using ece297test::path_is_legal;


SUITE(inter_inter_path_func_public) {
struct MapFixture {
    MapFixture() {
        rng = std::minstd_rand(4);
        rand_intersection = std::uniform_int_distribution<unsigned>(0, getNumIntersections()-1);
        rand_street = std::uniform_int_distribution<unsigned>(1, getNumStreets()-1);
        rand_segment = std::uniform_int_distribution<unsigned>(0, getNumStreetSegments()-1);
        rand_poi = std::uniform_int_distribution<unsigned>(0, getNumPointsOfInterest()-1);
        rand_lat = std::uniform_real_distribution<double>(46.650653839, 46.730262756);
        rand_lon = std::uniform_real_distribution<double>(7.667298794, 7.962547302);
        rand_turn_penalty = std::uniform_real_distribution<double>(0., 30.);
    }

    std::minstd_rand rng;
    std::uniform_int_distribution<unsigned> rand_intersection;
    std::uniform_int_distribution<unsigned> rand_street;
    std::uniform_int_distribution<unsigned> rand_segment;
    std::uniform_int_distribution<unsigned> rand_poi;
    std::uniform_real_distribution<double> rand_lat;
    std::uniform_real_distribution<double> rand_lon;
    std::uniform_real_distribution<double> rand_turn_penalty;
};
    TEST(find_path_between_intersections_legality) {
        std::vector<unsigned> path;

        path = find_path_between_intersections(20425454, 6484, 15, 25);
        CHECK(path_is_legal(0, 589, path));
        
        //path = find_path_between_intersections_2(203, 204, 0.00000000000000000, 0.00000000000000000);
        //CHECK(path_is_legal(0, 589, path));

        

    } //find_path_between_intersections_optimality

} //inter_inter_path_func_public

