// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include "example.hpp"          // Include short list of convenience functions for rendering
#include <hiredis/hiredis.h>
#include <opencv2/opencv.hpp>   // Include OpenCV API
#include <opencv2/imgcodecs.hpp>

// Create redis keys 
const std::string RGB_KEY = "rsd455::rgb";
const std::string DEPTH_KEY = "rsd455::depth";

using namespace cv;

// Capture Example demonstrates how to
// capture depth and color video streams and render them to the screen
int main(int argc, char * argv[]) try
{
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

    bool show_rgb = false;
    bool show_depth = false;
    if (atoi(argv[1]) == 0) {
        show_rgb = true;
    }
    else if (atoi(argv[1]) == 1) {
        show_depth = true;
    }
    else {
        std::cerr << "Invalid input (either 0 or 1)" << std::endl;
    }

    const auto window_name = "Display Image";
    namedWindow(window_name, WINDOW_AUTOSIZE);
    int bufferSize = 640 * 480 * 3;

    while (waitKey(1) < 0 && getWindowProperty(window_name, WND_PROP_AUTOSIZE) >= 0)
    {
        if (show_rgb) {
            // RGB
            reply = (redisReply*)redisCommand(c, "GET %s", "rsd455::rgb");
            // reply = (redisReply*)redisCommand(c, "GET rsd455::rgb");

            // Convert to vector of byte            
            std::vector<char> vectordata(reply->str, reply->str + bufferSize);

            std::cout << reply->str << std::endl;

            // Create mat
            Mat data_mat(vectordata, true);

            // Convert to cv mat
            // Mat image = imdecode(cv::Mat(1, reply->len, CV_8UC1, reply->str), IMREAD_UNCHANGED);  // default reads color
            Mat image(imdecode(data_mat, 1));

            // Update the window with new data
            // imshow(window_name, image);
            freeReplyObject(reply);
        }
        else if (show_depth) {
            // Depth
            reply = (redisReply*)redisCommand(c,"GET %s", "rsd455::depth");
            Mat image = imdecode(cv::Mat(1, bufferSize, CV_8UC1, reply->str), IMREAD_UNCHANGED);  // default reads color
            // Update the window with new data
            imshow(window_name, image);
            freeReplyObject(reply);
        }
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