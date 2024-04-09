cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# http://www.levelofindirection.com/journal/2010/12/28/unit-testing-in-c-and-objective-c-just-got-easier.html
# Thanks for CATCH!

project(EventBusTest)

# Dependencies
enable_testing()
if(NOT TARGET Dexode::EventBus)
	find_package(EventBus CONFIG REQUIRED)
endif()

include(FetchContent)
fetchcontent_declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG v2.13.10
	OVERRIDE_FIND_PACKAGE
)

fetchcontent_makeavailable(Catch2)

find_package(Catch2 2 REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(unit)
add_subdirectory(integration)
