cmake_minimum_required( VERSION 3.14 )

set( TARGET scl-feature-test-stream )
project( ${TARGET} )

#find_package( ScL REQUIRED )

set( MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../" )
set( SOURCE_DIR "${MODULE_DIR}/test/Stream" )

file( GLOB SOURCE_FILES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.h" )
add_executable( ${TARGET} ${SOURCE_FILES} )

target_include_directories( ${TARGET} PUBLIC ${SCL_INCLUDE_DIR} )
target_link_libraries( ${TARGET} PRIVATE ScL::feature )
