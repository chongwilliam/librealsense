# License: Apache 2.0. See LICENSE file in root directory.
# Copyright(c) 2019 Intel Corporation. All Rights Reserved.
#  minimum required cmake version: 3.1.0
cmake_minimum_required(VERSION 3.1.0)

project(redis-stream)

if(BUILD_GRAPHICAL_EXAMPLES)
	# redis-stream
    add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
    target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES})
    target_link_libraries(${PROJECT_NAME} ${SAI2_COMMON_LIBRARIES})
    include_directories(${PROJECT_NAME} ../ ../../third-party/tclap/include)
    set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "Examples")
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    # redis-show
    add_executable(redis-show redis-show.cpp)
    set_property(TARGET redis-show PROPERTY CXX_STANDARD 11)
    target_link_libraries(redis-show ${DEPENDENCIES})
    target_link_libraries(redis-show ${SAI2_COMMON_LIBRARIES})
    include_directories(redis-show ../ ../../third-party/tclap/include)
    set_target_properties (redis-show PROPERTIES FOLDER "Examples")
	install(TARGETS redis-show RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    
endif()
