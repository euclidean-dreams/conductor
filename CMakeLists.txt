cmake_minimum_required(VERSION 3.13)
project(conductor VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wno-psabi)

include(lib/cmake-utils/dependency-integrator.cmake)
integrate_cmake_dependency(
        NAME googletest
        GIT_URL git@github.com:google/googletest.git
        GIT_TAG 703bd9caab50b139428cea1aaff9974ebee5742e
)
integrate_cmake_dependency(
        NAME impresario-utils
        GIT_URL git@github.com:impresario-systems/impresario-utils.git
        GIT_TAG origin/dev
)

# conductor
set(
        SOURCE_FILES
        src/Config.h
        src/Bootstrapper.cpp src/Bootstrapper.h

        src/audioStream/AudioStream.h
        src/audioStream/AudioInput.cpp src/audioStream/AudioInput.h
        src/audioStream/RingBuffer.cpp src/audioStream/RingBuffer.h
        src/audioStream/AudioUtils.h src/audioStream/AudioUtils.cpp

        src/audioProcessor/AudioProcessor.h
        src/audioProcessor/AudioProcessorSuite.cpp src/audioProcessor/AudioProcessorSuite.h
        src/audioProcessor/AudioStreamSource.cpp src/audioProcessor/AudioStreamSource.h
        src/audioProcessor/AudioProcessorSink.cpp src/audioProcessor/AudioProcessorSink.h
        src/audioProcessor/OnsetProcessor.cpp src/audioProcessor/OnsetProcessor.h
        src/audioProcessor/PitchProcessor.cpp src/audioProcessor/PitchProcessor.h

        src/packet/PacketConduit.cpp src/packet/PacketConduit.h
        src/packet/PacketCollectionManager.h
        src/packet/PacketSpout.cpp src/packet/PacketSpout.h
        src/packet/PacketCollection.cpp src/packet/PacketCollection.h

        src/packet/Packet.h
        src/packet/NotSerializableException.h
        src/packet/AudioPacket.cpp src/packet/AudioPacket.h
        src/packet/OnsetPacket.cpp src/packet/OnsetPacket.h
        src/packet/PitchPacket.cpp src/packet/PitchPacket.h

)
list(APPEND TARGETS conductor)
add_executable(conductor src/Main.cpp ${SOURCE_FILES})

# test
set(
        TEST_FILES

        test/audioStream/RingBufferTest.cpp
        test/packet/PacketMock.h
        test/packet/PacketCollectionManagerMock.h
        test/packet/AudioPacketTest.cpp
        test/packet/PacketConduitTest.cpp

)
list(APPEND TARGETS test-conductor)
add_executable(test-conductor ${TEST_FILES} ${SOURCE_FILES})
target_link_libraries(test-conductor PRIVATE gtest_main)
target_link_libraries(test-conductor PRIVATE gmock)

# finalize
find_package(cppzmq)
foreach (target IN LISTS TARGETS)
    target_include_directories(${target} PRIVATE src)
    target_link_libraries(${target} PRIVATE impresario-utils)
    target_link_libraries(${target} PRIVATE aubio)
    target_link_libraries(${target} PRIVATE portaudio)
    target_link_libraries(${target} PRIVATE cppzmq)
    target_link_libraries(${target} PRIVATE pthread)
endforeach ()
