PROJECT(GameServer)

SET(CMAKE_CXX_FLAGS "-g -O2 -Wall -std=c++14")
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)

SET(CMAKE_EXE_LINKER_FLAGS "-L/usr/lib64/mysql -L/usr/lib")
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} GameCommon z dl lua boost_program_options boost_serialization boost_chrono boost_timer boost_date_time boost_system boost_thread boost_container zxero protobuf mysqlcppconn curl m pthread)
