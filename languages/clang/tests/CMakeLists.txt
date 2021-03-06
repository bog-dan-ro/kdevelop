add_executable(clang-parser
    clang-parser.cpp
)

target_link_libraries(clang-parser
    KDev::Tests
    kdevclangduchain
)

add_executable(clang-minimal-visitor
    minimal_visitor.cpp
)

target_link_libraries(clang-minimal-visitor
    ${CLANG_CLANG_LIB}
)

ecm_add_test(test_buddies.cpp
    TEST_NAME test_buddies-clang
    LINK_LIBRARIES
        KDev::Tests
        Qt5::Test
)

ecm_add_test(test_codecompletion.cpp
    TEST_NAME test_codecompletion
    LINK_LIBRARIES
        KDev::Tests
        Qt5::Test
        kdevclangcodecompletion
        kdevclangduchain
)

ecm_add_test(test_assistants.cpp
    TEST_NAME test_assistants
    LINK_LIBRARIES
        KDev::Tests
        Qt5::Test
        kdevclangduchain
        kdevclangutil
)

ecm_add_test(test_clangutils.cpp
    TEST_NAME test_clangutils
    LINK_LIBRARIES
        KDev::Tests
        Qt5::Test
        ${CLANG_CLANG_LIB}
        kdevclangduchain
)

ecm_add_test(test_duchain.cpp
    TEST_NAME test_duchain-clang
    LINK_LIBRARIES
        KDev::Tests
        Qt5::Test
        kdevclangduchain
)

ecm_add_test(test_duchainutils.cpp
    TEST_NAME test_duchainutils
    LINK_LIBRARIES
        KDev::Tests
        Qt5::Test
        kdevclangcodecompletion
        kdevclangduchain
)

ecm_add_test(test_problems.cpp
    TEST_NAME test_problems
    LINK_LIBRARIES
        KDev::Tests
        Qt5::Test
        kdevclangduchain
)

configure_file("testfilepaths.h.cmake" "testfilepaths.h" ESCAPE_QUOTES)
ecm_add_test(test_files.cpp
TEST_NAME test_files-clang
LINK_LIBRARIES
    Qt5::Test
    Qt5::Core
    KDev::Language
    KDev::Tests
    kdevclangduchain
)
