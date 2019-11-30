PROJECT(AccountServer CXX)

SET(CMAKE_CXX_FLAGS "-g -O2 -Wall -std=c++14 -L/usr/lib64/mysql")
AUX_SOURCE_DIRECTORY(. SRC_LIST)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} GameCommon zxero boost_program_options boost_timer boost_date_time boost_thread protobuf mysqlcppconn mysqlclient m pthread)
