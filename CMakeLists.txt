cmake_minimum_required(VERSION 3.13)
project(conductor VERSION 0.1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wno-psabi)

add_subdirectory(lib/googletest)
add_subdirectory(lib/impresario-serialization)
add_subdirectory(lib/impresario-utils)

# conductor
set(
        SOURCE_FILES
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
list(APPEND TARGETS conductor)
add_executable(conductor src/Main.cpp ${SOURCE_FILES})

# test
set(
        TEST_FILES
        ${SOURCE_FILES}

        test/stream/RingBufferTest.cpp
)
list(APPEND TARGETS test-conductor)
add_executable(test-conductor ${TEST_FILES})
target_link_libraries(test-conductor PRIVATE gtest_main)

# finalize
foreach(target IN LISTS TARGETS)
    target_include_directories(${target} PRIVATE src)
    target_link_libraries(${target} PRIVATE impresario-utils)
    target_link_libraries(${target} PRIVATE impresario-serialization)
    target_link_libraries(${target} PRIVATE aubio)
    target_link_libraries(${target} PRIVATE portaudio)
    target_link_libraries(${target} PRIVATE zmq)
    target_link_libraries(${target} PRIVATE pthread)
endforeach()
