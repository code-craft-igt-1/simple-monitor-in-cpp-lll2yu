cmake_minimum_required(VERSION 3.14)
project(bms-monitor)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")

include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP true
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

find_package(Boost REQUIRED COMPONENTS locale)

set(LANGUAGES en ger ja)
set(PO_DIR ${CMAKE_SOURCE_DIR}/locales)
set(MO_DIR ${CMAKE_BINARY_DIR}/locales)

file(MAKE_DIRECTORY ${MO_DIR})
foreach(LANG ${LANGUAGES})
    add_custom_command(
        OUTPUT ${MO_DIR}/${LANG}/LC_MESSAGES/vitalchecker.mo
        COMMAND ${CMAKE_COMMAND} -E make_directory ${MO_DIR}/${LANG}/LC_MESSAGES
        COMMAND msgfmt -o ${MO_DIR}/${LANG}/LC_MESSAGES/vitalchecker.mo ${PO_DIR}/${LANG}/vitalchecker.po
        DEPENDS ${PO_DIR}/${LANG}/vitalchecker.po
        COMMENT "Compiling ${LANG} .po to .mo"
        VERBATIM
    )
    add_custom_target(${LANG}_mo ALL DEPENDS ${MO_DIR}/${LANG}/LC_MESSAGES/vitalchecker.mo)
endforeach()

enable_testing()

file(GLOB SOURCES "*.cpp" "include/*.h")
add_executable(
    test-monitor
  ${SOURCES}
)
target_link_libraries(
    test-monitor
  GTest::gtest_main
  GTest::gmock_main
  Boost::locale
)

include_directories(${Boost_INCLUDE_DIRS})
include(GoogleTest)
gtest_discover_tests(test-monitor)
