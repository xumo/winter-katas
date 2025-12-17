cmake_minimum_required(VERSION 3.31)
project(bolt-playground)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

get_filename_component(CINDER_PATH $ENV{CINDER_PATH} ABSOLUTE)
get_filename_component(APP_PATH ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

ci_make_app(
        SOURCES     ${APP_PATH}/BoltPlayground.cpp
        CINDER_PATH ${CINDER_PATH}
        RESOURCES   ${APP_PATH}/assets/shaders/shader.vert
        ASSETS_PATH  ${APP_PATH}/assets
)
