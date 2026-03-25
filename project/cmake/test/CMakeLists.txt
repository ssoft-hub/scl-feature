# Generate tests for feature: one test target per subdirectory under module/feature/test/*
# Supports GTest, doctest, and Catch2 depending on availability/toggles.

include(${CMAKE_SOURCE_DIR}/project/cmake/SCLDependencies.cmake)

get_filename_component(SCL_FEATURE_TEST_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../test" ABSOLUTE)

if (EXISTS "${SCL_FEATURE_TEST_DIR}")
    file(GLOB SCL_FEATURE_TEST_SUBDIRS LIST_DIRECTORIES true "${SCL_FEATURE_TEST_DIR}/*")

    foreach(subdir IN LISTS SCL_FEATURE_TEST_SUBDIRS)
        if (IS_DIRECTORY "${subdir}")
            get_filename_component(name "${subdir}" NAME)


            # --- GoogleTest ---
            if (GTEST_FOUND)
                file(GLOB_RECURSE srcs CONFIGURE_DEPENDS
                    "${subdir}/*gtest.c" "${subdir}/*gtest.cc" "${subdir}/*gtest.cpp"
                )
                if (srcs)
                    set(tgt "feature_${name}_gtest")
                    add_executable(${tgt} ${srcs})
                    target_link_libraries(${tgt} PRIVATE scl::feature GTest::gtest_main)
                    target_compile_features(${tgt} PRIVATE cxx_std_20)
                    include(GoogleTest)            # Provides gtest_discover_tests
                    gtest_discover_tests(${tgt} DISCOVERY_TIMEOUT 30 DISCOVERY_MODE PRE_TEST)
                endif()
            endif()

            # --- doctest ---
            # Expect tests to either provide their own main or use:
            #   #define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
            if (DOCTEST_FOUND)
                file(GLOB_RECURSE srcs CONFIGURE_DEPENDS
                    "${subdir}/*doctest.c" "${subdir}/*doctest.cc" "${subdir}/*doctest.cpp"
                )
                if (srcs)
                    set(tgt "feature_${name}_doctest")
                    add_executable(${tgt} ${srcs})
                    target_link_libraries(${tgt} PRIVATE scl::feature doctest::doctest)
                    target_compile_features(${tgt} PRIVATE cxx_std_20)
                    add_test(NAME ${tgt} COMMAND ${tgt})
                endif()
            endif()

            # --- Catch2 ---
            if (CATCH2_FOUND)
                file(GLOB_RECURSE srcs CONFIGURE_DEPENDS
                    "${subdir}/*catch2.c" "${subdir}/*catch2.cc" "${subdir}/*catch2.cpp"
                )
                if (srcs)
                    if (TARGET Catch2::Catch2WithMain)
                        set(tgt "feature_${name}_catch2")
                        add_executable(${tgt} ${srcs})
                        target_link_libraries(${tgt} PRIVATE scl::feature Catch2::Catch2WithMain)
                        target_compile_features(${tgt} PRIVATE cxx_std_20)
                        include(Catch)              # Provides catch_discover_tests for Catch2 v3
                        catch_discover_tests(${tgt})
                    elseif (TARGET Catch2::Catch2)
                        # For Catch2 v2, tests must supply their own main()
                        set(tgt "feature_${name}_catch2_v2")
                        add_executable(${tgt} ${srcs})
                        target_link_libraries(${tgt} PRIVATE scl::feature Catch2::Catch2)
                        target_compile_features(${tgt} PRIVATE cxx_std_20)
                        add_test(NAME ${tgt} COMMAND ${tgt})
                    endif()
                endif()
            endif()
        endif()
    endforeach()
endif()
