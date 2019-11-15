// This file contains functions on mouse and key.
// All related functions should be implemented here

#pragma once //protects against multiple inclusions of this header file

#include <ezgl/application.hpp>
#include <ezgl/graphics.hpp>

#include <iostream>

void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y);
void act_on_mouse_move(ezgl::application *application, GdkEventButton *event, double x, double y);
void act_on_key_press(ezgl::application *application, GdkEventKey *event, char *key_name);