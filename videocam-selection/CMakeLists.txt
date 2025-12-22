cmake_minimum_required(VERSION 3.31)
project(videocam-selection)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

get_filename_component(CINDER_PATH $ENV{CINDER_PATH} ABSOLUTE)
get_filename_component(APP_PATH ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set(INCLUDE_FILES /opt/homebrew/Cellar/libusb/1.0.29/include/libusb-1.0/ ${APP_PATH}/dependencies/PS3EYEDriver/src)
# Add libusb from brew for ps3eye and apple's Security framework
set(LIBS_FILES /opt/homebrew/Cellar/libusb/1.0.29/lib/libusb-1.0.a "-framework Security")
set(SOURCE_FILES  ${APP_PATH}/VideoCamSelection.cpp ${APP_PATH}/dependencies/PS3EYEDriver/src/ps3eye.cpp ${APP_PATH}/dependencies/PS3EYEDriver/src/ps3eye.h)


ci_make_app(
  SOURCES     ${SOURCE_FILES}
  CINDER_PATH ${CINDER_PATH}
  ASSETS_PATH  ${APP_PATH}/assets
  INCLUDES    ${INCLUDE_FILES}
  LIBRARIES   ${LIBS_FILES}
)



