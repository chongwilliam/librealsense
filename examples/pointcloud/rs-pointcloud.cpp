// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015-2017 Intel Corporation. All Rights Reserved.

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include "example.hpp"          // Include short list of convenience functions for rendering

#include <algorithm>            // std::min, std::max

// #include <hiredis/hiredis.h>    // Include Redis API
#include "redis/RedisClient.h"
// Create redis keys 
// const char *PC_KEY = "pc::capture";
const std::string PC_KEY = "pc::capture";

// Helper functions
void register_glfw_callbacks(window& app, glfw_state& app_state);

int main(int argc, char * argv[]) try
{
    /*
    // Connect to redis server given by argument IP 
    redisContext *c;
    redisReply *reply;
    const char *hostname = "127.0.0.1";
    int port = 6379;
    struct timeval timeout = { 2, 0 }; // 2 seconds
    c = redisConnectWithTimeout(hostname, port, timeout);
    if (c == NULL || c->err) {
       std::cerr << "Something bad happened" << std::endl;
       exit(1);
    }
    */
    auto redis_client = RedisClient();
    redis_client.connect();
    redis_client.set(PC_KEY, "0");
    std::string save_command = "0";

    /*
    // Initialize key     
    reply = (redisReply*)redisCommand(c, "SET %s %s", PC_KEY, "0");
    freeReplyObject(reply); 
    
    reply = (redisReply*)redisCommand(c, "GET %s", PC_KEY);
    std::cout << reply->len << std::endl;
    freeReplyObject(reply); 
    */
    
    // Create a simple OpenGL window for rendering:
    window app(1280, 720, "RealSense Pointcloud Example");
    // Construct an object to manage view state
    glfw_state app_state;
    // register callbacks to allow manipulation of the pointcloud
    register_glfw_callbacks(app, app_state);

    // Declare pointcloud object, for calculating pointclouds and texture mappings
    rs2::pointcloud pc;
    // We want the points object to be persistent so we can display the last cloud when a frame drops
    rs2::points points;

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    // Start streaming with default recommended configuration
    pipe.start();

    while (app) // Application still alive?
    {
        // Wait for the next set of frames from the camera
        auto frames = pipe.wait_for_frames();

        auto color = frames.get_color_frame();

        // For cameras that don't have RGB sensor, we'll map the pointcloud to infrared instead of color
        if (!color)
            color = frames.get_infrared_frame();

        // Tell pointcloud object to map to this color frame
        pc.map_to(color);

        auto depth = frames.get_depth_frame();

        // Generate the pointcloud and texture mappings
        points = pc.calculate(depth);

        // Upload the color frame to OpenGL
        app_state.tex.upload(color);

        // Draw the pointcloud
        draw_pointcloud(app.width(), app.height(), app_state, points);        
        
        // Save the pointcloud to .ply file if redis key is on
        /*
        reply = (redisReply*)redisCommand(c,"GET %s", PC_KEY);
        std::cout << reply->str << std::endl;
        if (std::stoi(reply->str) == 1) {        	       
            freeReplyObject(reply);
	    points.export_to_ply("test.ply", color);
	    reply = (redisReply*)redisCommand(c,"SET %s %s", PC_KEY, "0");
	    freeReplyObject(reply);
        } 
        */        
        save_command = redis_client.get(PC_KEY);
        std::cout << save_command << std::endl;
        if (save_command == "1") {
            points.export_to_ply("test.ply", color);
            redis_client.set(PC_KEY, "0");
        }
        
    }

    return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception & e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}
