cmake_minimum_required( VERSION 3.14 )

set( TARGET scl-feature-test-cast )
project( ${TARGET} )

# find_package( ScL REQUIRED )
# find_package(doctest REQUIRED)


set( MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../" )
set( SOURCE_DIR "${MODULE_DIR}/test/Cast" )
set( DOCTEST_INCLUDE_DIR "${COMPLEX_BASE_DIR}/3rdparty/doctest/doctest")


file( GLOB SOURCE_FILES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.h" )
add_executable( ${TARGET} ${SOURCE_FILES} )

target_include_directories( ${TARGET} PUBLIC ${SCL_INCLUDE_DIR} ${DOCTEST_INCLUDE_DIR})
target_link_libraries( ${TARGET} PRIVATE ScL::feature)
