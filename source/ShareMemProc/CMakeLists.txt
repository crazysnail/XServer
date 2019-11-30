PROJECT(ShareMemProc CXX)

SET(CMAKE_CXX_FLAGS "-g -O2 -Wall -std=c++14")
AUX_SOURCE_DIRECTORY(. SRC_LIST)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} GameCommon boost_program_options boost_system boost_chrono boost_timer boost_date_time boost_thread zxero protobuf mysqlcppconn m pthread)
