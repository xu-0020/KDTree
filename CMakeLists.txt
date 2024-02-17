
cmake_minimum_required(VERSION 3.28)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)


# libLAS library setup
set(LIBLAS_INCLUDE_DIR "C:/OSGeo4W/include")
set(LIBLAS_LIBRARY_DIR "C:/OSGeo4W/lib")

include_directories(${LIBLAS_INCLUDE_DIR})
link_directories(${LIBLAS_LIBRARY_DIR})

# Boost library setup
set(Boost_INCLUDE_DIR "C:/Boost/boost")
set(Boost_LIBRARY_DIR "C:/Boost/boost/stage/lib")  
find_package(Boost REQUIRED) 

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})


# Create your executable
add_executable(kd main.cpp KdTree.cpp DataParser.cpp)  
# Link libraries
target_link_libraries(kd ${LIBLAS_LIBRARY_DIR}/liblas.lib ${Boost_LIBRARIES})


