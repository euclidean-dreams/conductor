cmake_minimum_required(VERSION 3.13)
project(conductor VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wno-psabi)

include(lib/cmake-utils/dependency-integrator.cmake)

SET(INSTALL_GTEST OFF)
integrate_cmake_dependency(
        NAME googletest
        GIT_URL git@github.com:google/googletest.git
        GIT_TAG 703bd9caab50b139428cea1aaff9974ebee5742e
)

integrate_cmake_dependency(
        NAME json
        GIT_URL git@github.com:nlohmann/json.git
        GIT_TAG db78ac1d7716f56fc9f1b030b715f872f93964e4
)

SET(YAML_CPP_BUILD_TESTS OFF)
SET(YAML_CPP_INSTALL OFF)
SET(YAML_MSVC_SHARED_RT OFF)
SET(YAML_CPP_BUILD_TOOLS OFF)
SET(YAML_CPP_BUILD_CONTRIB OFF)
integrate_cmake_dependency(
        NAME yaml
        GIT_URL git@github.com:jbeder/yaml-cpp.git
        GIT_TAG 9a3624205e8774953ef18f57067b3426c1c5ada6
)

integrate_cmake_dependency(
        NAME iir1
        GIT_URL git@github.com:berndporr/iir1.git
        GIT_TAG 67591c8eac591c576b9aabe9a2f288296bb263f0
)

SET(GNULINUX_PLATFORM ON)
SET(NE10_LINUX_TARGET_ARCH armv7)
set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)
SET(NE10_ENABLE_MATH OFF)
SET(NE10_ENABLE_PHYSICS OFF)
SET(NE10_ENABLE_IMGPROC OFF)
SET(NE10_BUILD_EXAMPLES OFF)
integrate_cmake_dependency(
        NAME ne10
        GIT_URL git@github.com:projectNe10/Ne10.git
        GIT_TAG 1f059a764d0e1bc2481c0055c0e71538470baa83
)

integrate_cmake_dependency(
        NAME impresario-utils
        GIT_URL git@github.com:impresario-systems/impresario-utils.git
        GIT_TAG origin/dev
)

# conductor
set(
        SOURCE_FILES
        src/Config.cpp src/Config.h
        src/Bootstrapper.cpp src/Bootstrapper.h

        src/audioStream/AudioStream.h
        src/audioStream/AudioInput.cpp src/audioStream/AudioInput.h
        src/audioStream/RingBuffer.cpp src/audioStream/RingBuffer.h
        src/audioStream/AudioFileReader.cpp src/audioStream/AudioFileReader.h
        src/audioStream/AudioFileStream.cpp src/audioStream/AudioFileStream.h
        src/audioStream/AudioUtils.h src/audioStream/AudioUtils.cpp

        src/audioProcessor/AudioProcessor.h
        src/audioProcessor/PacketTypeConverter.h
        src/audioProcessor/AudioProcessorSuite.cpp src/audioProcessor/AudioProcessorSuite.h
        src/audioProcessor/AudioStreamSource.cpp src/audioProcessor/AudioStreamSource.h
        src/audioProcessor/AudioProcessorSink.cpp src/audioProcessor/AudioProcessorSink.h
        src/audioProcessor/BandpassProcessor.cpp src/audioProcessor/BandpassProcessor.h
        src/audioProcessor/STFTProcessor.cpp src/audioProcessor/STFTProcessor.h
        src/audioProcessor/onsetProcessor/SpecFluxOnsetProcessor.cpp src/audioProcessor/onsetProcessor/SpecFluxOnsetProcessor.h
        src/audioProcessor/onsetProcessor/CuriousOnsetDetector.cpp src/audioProcessor/onsetProcessor/CuriousOnsetDetector.h
        src/audioProcessor/onsetProcessor/CuriousPerceptron.cpp src/audioProcessor/onsetProcessor/CuriousPerceptron.h

        src/audioProcessor/FileWriter.h

        src/packet/Cleanable.h
        src/packet/PacketConduitCurator.cpp src/packet/PacketConduitCurator.h
        src/packet/PacketConduit.h
        src/packet/PacketReceiver.h
        src/packet/PacketDispatcher.h
        src/packet/PacketCollection.h

        src/packet/Packet.h
        src/packet/Serializable.h
        src/packet/FileWritable.h
        src/packet/AudioPacket.cpp src/packet/AudioPacket.h
        src/packet/RawAudioPacket.cpp src/packet/RawAudioPacket.h
        src/packet/STFTPacket.cpp src/packet/STFTPacket.h
        src/packet/SpectrogramPacket.cpp src/packet/SpectrogramPacket.h
        src/packet/OnsetPacket.cpp src/packet/OnsetPacket.h
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

# exp
set(
        EXP
        src/packet/Packet.h
        src/packet/PacketCollection.h

)
list(APPEND TARGETS exp-conductor)
add_executable(exp-conductor ${EXP})
target_link_libraries(exp-conductor PRIVATE gtest_main)

# finalize
find_package(cppzmq)
foreach (target IN LISTS TARGETS)
    target_include_directories(${target} PRIVATE src)
    target_link_libraries(${target} PRIVATE cppzmq)
    target_link_libraries(${target} PRIVATE iir_static)
    target_link_libraries(${target} PRIVATE impresario-utils)
    target_link_libraries(${target} PRIVATE portaudio)
    target_link_libraries(${target} PRIVATE pthread)
    target_link_libraries(${target} PRIVATE yaml-cpp)
    target_link_libraries(${target} PRIVATE nlohmann_json::nlohmann_json)

    target_include_directories(${target} PRIVATE ${CMAKE_BINARY_DIR}/ne10-src/inc)
    target_link_libraries(${target} PRIVATE NE10)
endforeach ()
