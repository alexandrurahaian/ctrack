cmake_minimum_required(VERSION 4.3)
project(ctrack)

set(CMAKE_CXX_STANDARD 17)

file(COPY settings.cfg DESTINATION .)
file(COPY dates.ctrack DESTINATION .)
file(COPY monitored.ctrack DESTINATION .)

add_executable(ctrack main.cpp
        Objects/Date.h
        Headers/ConfigurationHandler.h
        Headers/OutColour.h
        Headers/Terminal.h
        Sources/OutColour.cpp
        Sources/Terminal.cpp
        Headers/TrackedDatesHandler.h
        Sources/TrackedDatesHandler.cpp
        Headers/Monitor.h
        Sources/Monitor.cpp
)
