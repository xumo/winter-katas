cmake_minimum_required(VERSION 3.31)
project(space-wrench)

set(CMAKE_CXX_STANDARD 23)

get_filename_component( CINDER_PATH $ENV{CINDER_PATH}  ABSOLUTE )
get_filename_component( APP_PATH ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

ci_make_app(
        SOURCES     ${APP_PATH}/SpaceWrench.cpp
        CINDER_PATH ${CINDER_PATH}
        RESOURCES   ${APP_PATH}/assets/shader.vert
                    ${APP_PATH}/assets/shader.frag
                    ${APP_PATH}/assets/wrench.obj
)
