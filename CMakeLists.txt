cmake_minimum_required(VERSION 3.13)
project(conductor)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib/impresario-utils)
add_subdirectory(test)
include_directories(lib/serialization/generated)
include_directories(src)

set(
        SOURCE_FILES
        src/Main.cpp
        src/Config.h

        src/audioStream/AudioStream.h
        src/audioStream/AudioStreamManager.cpp src/audioStream/AudioStreamManager.h
        src/audioStream/RingBuffer.cpp src/audioStream/RingBuffer.h

        src/audioProcessor/AudioProcessor.h
        src/audioProcessor/AudioProcessorSuite.cpp src/audioProcessor/AudioProcessorSuite.h
        src/audioProcessor/AudioStreamSource.cpp src/audioProcessor/AudioStreamSource.h

        src/audioProcessor/onsetProcessor/OnsetProcessor.cpp src/audioProcessor/onsetProcessor/OnsetProcessor.h
        src/audioProcessor/onsetProcessor/OnsetAggregator.cpp src/audioProcessor/onsetProcessor/OnsetAggregator.h
)

add_executable(conductor ${SOURCE_FILES})
target_link_libraries(conductor LINK_PUBLIC impresario-utils)
target_link_libraries(conductor LINK_PUBLIC aubio)
target_link_libraries(conductor LINK_PUBLIC portaudio)
target_link_libraries(conductor LINK_PUBLIC zmq)
target_link_libraries(conductor LINK_PUBLIC pthread)
