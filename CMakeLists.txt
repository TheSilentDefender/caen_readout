cmake_minimum_required(VERSION 3.10)

project(CAEN_PHA_Readout VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME}
        main.cpp
        ClassDigitizer2Gen.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE CAEN_FELib)

install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION bin
)

