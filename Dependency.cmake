include(ExternalProject)

set(DEP_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/Dependency)
set(DEP_PROJECTS dep_clipp dep_spdlog dep_catch)

ExternalProject_Add(
    dep_clipp
    GIT_REPOSITORY "https://github.com/muellan/clipp.git"
    GIT_TAG "v1.2.2"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND
        ${CMAKE_COMMAND} -E make_directory ${DEP_INSTALL_DIR}/include &&
        ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_BINARY_DIR}/dep_clipp-prefix/src/dep_clipp/include/clipp.h
            ${DEP_INSTALL_DIR}/include
    TEST_COMMAND ""
)

ExternalProject_Add(
    dep_spdlog
    GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
    GIT_TAG "v1.x"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DSPDLOG_BUILD_EXAMPLES=OFF
        -DSPDLOG_BUILD_TESTS=OFF
        -DSPDLOG_INSTALL=ON
    TEST_COMMAND ""
)

ExternalProject_Add(
    dep_catch
    GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
    GIT_TAG "v2.7.0"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_COMMAND
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DCATCH_BUILD_TESTING=OFF
        -DCATCH_INSTALL_DOCS=OFF
        -DCATCH_INSTALL_HELPERS=OFF
    TEST_COMMAND ""
)
