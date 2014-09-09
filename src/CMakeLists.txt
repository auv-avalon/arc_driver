rock_library(arc_driver
    SOURCES AmberDevice.cpp ArcDriver.cpp ArcDriverFilterToken.cpp ArcTokenDriver.cpp arc_packet.c 
    HEADERS AmberDevice.hpp ArcDriver.hpp ArcDriverFilterToken.hpp ArcTokenDriver.hpp arc_packet.h ArcInterface.hpp
    DEPS_PKGCONFIG base-types iodrivers_base)

rock_executable(arc_driver_reader ArcTestReader.cpp
    DEPS arc_driver)
rock_executable(arc_driver_sender ArcTestSender.cpp
    DEPS arc_driver)
rock_executable(arc_token_driver_reader ArcTestTokenReader.cpp
    DEPS arc_driver)

