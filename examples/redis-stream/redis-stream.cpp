// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include "example.hpp"          // Include short list of convenience functions for rendering
#include <hiredis/hiredis.h>
#include <opencv2/opencv.hpp>   // Include OpenCV API

// Create redis keys 
const std::string RGB_KEY = "rsd455::rgb";
const std::string DEPTH_KEY = "rsd455::depth";

// Capture Example demonstrates how to
// capture depth and color video streams and render them to the screen
int main(int argc, char * argv[]) try
{
    rs2::log_to_console(RS2_LOG_SEVERITY_ERROR);

    // Create a simple OpenGL window for rendering:
    window app(1280, 720, "RealSense Capture Example");

    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map;

    // Declare rates printer for showing streaming rates of the enabled streams.
    rs2::rates_printer printer;

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;

    // Declare pipeline configuration 
    rs2::config pipe_config;

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

    // Start streaming with default recommended configuration
    // The default video configuration contains Depth and Color streams
    // If a device is capable to stream IMU data, both Gyro and Accelerometer are enabled by default

    pipe.start();

    while (app) // Application still alive?
    {
        // rs2::frameset data = pipe.wait_for_frames().    // Wait for next set of frames from the camera
        //                      apply_filter(printer).     // Print each enabled stream frame rate
        //                      apply_filter(color_map);   // Find and colorize the depth data

        rs2::frameset data = pipe.wait_for_frames();

        // The show method, when applied on frameset, break it to frames and upload each frame into a gl textures
        // Each texture is displayed on different viewport according to it's stream unique id
        app.show(data);

        // Get frames
        rs2::frame rgb = data.get_color_frame();
        rs2::frame depth = data.get_depth_frame().apply_filter(color_map);

        // Query frame size (width and height)
        const int w = depth.as<rs2::video_frame>().get_width();
        const int h = depth.as<rs2::video_frame>().get_height();

        // Create OpenCV matrix of size (w,h) from the colorized depth data
        cv::Mat rgb_image(cv::Size(w, h), CV_8UC3, (void*)depth.get_data(), cv::Mat::AUTO_STEP);
        cv::Mat depth_image(cv::Size(w, h), CV_8UC3, (void*)depth.get_data(), cv::Mat::AUTO_STEP);

        // Stream data to redis server
        reply = (redisReply*)redisCommand(c,"SET rsd455::rgb",(char*)rgb_image.data, h*w*3);
        freeReplyObject(reply);
        reply = (redisReply*)redisCommand(c,"SET rsd455::depth",(char*)depth_image.data, h*w*3);
        freeReplyObject(reply);
    }

    return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}