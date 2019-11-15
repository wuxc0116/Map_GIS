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
#include <iostream>
#include <unittest++/UnitTest++.h>
#include "m1.h"

/*
 * This is the main that drives running
 * unit tests.
 */
//int main(int /*argc*/, char** /*argv*/) {
//    //Run the unit tests
//    int num_failures = UnitTest::RunAllTests();
//
//    return num_failures;
//}

int main(int argc, char** argv) {
    //Load the test map
    std::string map_name =  "/cad2/ece297s/public/maps/toronto_canada.streets.bin";
    if(!load_map(map_name)) {
        std::cout << "ERROR: Could not load map file: '" << map_name << "'!";
        std::cout << " Subsequent tests will likely fail." << std::endl;
    }

    //Run the unit tests
    int num_failures = UnitTest::RunAllTests();

    //Cleanup
    close_map();

    return num_failures;
}