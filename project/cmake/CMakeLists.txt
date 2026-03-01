# Header-only library: feature
# Produces targets:
#   - scl-feature (real interface target)
#   - scl::feature (ALIAS in build tree)
# Exported/imported name is set to "feature" so installed import is scl::feature.

# Resolve module root directory (module/feature)
get_filename_component(SCL_FEATURE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

set(MODULE feature)

# ---- Make headers visible in IDE ----
set(SCL_FEATURE_SRC_DIR "${SCL_FEATURE_ROOT}/src")
file(GLOB_RECURSE SCL_FEATURE_ALL_HEADERS CONFIGURE_DEPENDS
    "${SCL_FEATURE_SRC_DIR}/*.h"
    "${SCL_FEATURE_SRC_DIR}/*.hpp"
    "${SCL_FEATURE_SRC_DIR}/*.hh"
    "${SCL_FEATURE_SRC_DIR}/*.hxx"
    "${SCL_FEATURE_SRC_DIR}/*.inl"
)
set(SCL_FEATURE_PUBLIC_HEADERS "${SCL_FEATURE_ALL_HEADERS}")
list(FILTER SCL_FEATURE_PUBLIC_HEADERS EXCLUDE REGEX ".*/private/.*")

# ---- Private headers and sources (implementation files) ----
file(GLOB_RECURSE SCL_FEATURE_PRIVATE_SOURCES CONFIGURE_DEPENDS
    "${SCL_FEATURE_SRC_DIR}/*.c"
    "${SCL_FEATURE_SRC_DIR}/*.cpp"
)

# ---- Make target scl-${MODULE}: INTERFACE/STATIC/SHARED ----
if (SCL_FEATURE_PRIVATE_SOURCES)
    if (BUILD_SHARED_LIBS)
        add_library(scl-${MODULE} SHARED)
    else()
        add_library(scl-${MODULE} STATIC)
    endif()
    set(SCL_FEATURE_SCOPE PUBLIC)
else()
    add_library(scl-${MODULE} INTERFACE)
    set(SCL_FEATURE_SCOPE INTERFACE)
endif()

add_library(scl::${MODULE} ALIAS scl-${MODULE})

# Requires for C++
target_compile_features(scl-${MODULE} ${SCL_FEATURE_SCOPE} cxx_std_20)

# Ensure the imported/install name is scl::feature
set_target_properties(scl-${MODULE} PROPERTIES EXPORT_NAME feature)

target_include_directories(scl-${MODULE}
    ${SCL_FEATURE_SCOPE}
        $<BUILD_INTERFACE:${SCL_FEATURE_ROOT}/src>
        $<INSTALL_INTERFACE:include>
)

# Dependency on scl::utility:
if (NOT TARGET scl::utility)
    find_package(scl CONFIG REQUIRED COMPONENTS utility)
endif()

target_link_libraries(scl-${MODULE} INTERFACE scl::utility)

# ---- Add implementation sources to the target normally ----
target_sources(scl-${MODULE} PRIVATE
    ${SCL_FEATURE_PRIVATE_SOURCES}
)

# Prefer CMake 3.23+ FILE_SET HEADERS so IDEs (VS, Xcode) attach headers to the target itself
# IMPORTANT:
# - The HEADERS file set must always keep a single scope across the entire project.
# - We use PUBLIC for the HEADERS file set in this target.
# - Do NOT add private headers to the HEADERS file set; that causes scope conflicts.
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.23")
    target_sources(scl-${MODULE} ${SCL_FEATURE_SCOPE}
        FILE_SET HEADERS
            BASE_DIRS ${SCL_FEATURE_SRC_DIR}
            FILES ${SCL_FEATURE_PUBLIC_HEADERS}
    )
endif()

# --- IDE-only aggregation target (Qt Creator, etc., will show these files) ---
add_custom_target(scl_feature ALL
    SOURCES
        ${SCL_FEATURE_ALL_HEADERS}
        ${SCL_FEATURE_PRIVATE_SOURCES}
)

# ---- Group files nicely in IDEs (Visual Studio/Xcode) ----
source_group(TREE "${SCL_FEATURE_SRC_DIR}"
    FILES
        ${SCL_FEATURE_ALL_HEADERS}
        ${SCL_FEATURE_PRIVATE_SOURCES}
)
