rock_library(arc_driver
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(arc_driver_bin Main.cpp
    DEPS arc_driver)

