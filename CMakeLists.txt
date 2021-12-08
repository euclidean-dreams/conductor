cmake_minimum_required(VERSION 3.13)
project(conductor VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(USE_MUFFT "use mufft instead of ne10 for fft, recommended for x86 architectures" OFF)

add_compile_options(-Wno-psabi)

include(lib/cmake-utils/dependency-integrator.cmake)

if (USE_MUFFT)
    add_compile_definitions(USE_MUFFT)
    SET(MUFFT_ENABLE_FFTW OFF)
    integrate_cmake_dependency(
            NAME mufft
            GIT_URL git@github.com:Themaister/muFFT.git
            GIT_TAG 47bb08652eab399c2c7d460abe5184857110f130
    )
else ()
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
endif ()

integrate_cmake_dependency(
        NAME impresario-utils
        GIT_URL git@github.com:impresario-systems/impresario-utils.git
        GIT_TAG origin/dev
)

# conductor
set(
        SOURCE_FILES

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
        src/audioProcessor/STFTProcessor.cpp src/audioProcessor/STFTProcessor.h
        src/audioProcessor/HarmonicTransformProcessor.cpp src/audioProcessor/HarmonicTransformProcessor.h
        src/audioProcessor/SpectrogramProcessor.cpp src/audioProcessor/SpectrogramProcessor.h
        src/audioProcessor/DisplaySignalProcessor.cpp src/audioProcessor/DisplaySignalProcessor.h
        src/audioProcessor/MelFilterBankProcessor.cpp src/audioProcessor/MelFilterBankProcessor.h
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
        src/packet/Signal.h
        src/packet/AudioPacket.cpp src/packet/AudioPacket.h
        src/packet/RawAudioPacket.cpp src/packet/RawAudioPacket.h
        src/packet/STFTPacket.cpp src/packet/STFTPacket.h
        src/packet/SpectrogramPacket.cpp src/packet/SpectrogramPacket.h
        src/packet/HarmonicTransformPacket.cpp src/packet/HarmonicTransformPacket.h
        src/packet/DisplaySignalPacket.cpp src/packet/DisplaySignalPacket.h
        src/packet/MelSignalPacket.cpp src/packet/MelSignalPacket.h
        src/audioProcessor/EqualizerProcessor.cpp src/audioProcessor/EqualizerProcessor.h
        src/audioProcessor/MaterializationProcessor.cpp src/audioProcessor/MaterializationProcessor.h
        src/Bootstrapper.cpp src/Bootstrapper.h src/Config.cpp src/Config.h)
list(APPEND TARGETS conductor)
add_executable(conductor src/Main.cpp ${SOURCE_FILES})

# finalize
find_package(cppzmq)
target_include_directories(conductor PRIVATE src)
target_link_libraries(conductor PRIVATE cppzmq)
target_link_libraries(conductor PRIVATE impresario-utils)
target_link_libraries(conductor PRIVATE portaudio)
target_link_libraries(conductor PRIVATE pthread)
target_link_libraries(conductor PRIVATE yaml-cpp)

if (USE_MUFFT)
    target_link_libraries(conductor PRIVATE muFFT)
else ()
    target_include_directories(conductor PRIVATE ${CMAKE_BINARY_DIR}/ne10-src/inc)
    target_link_libraries(conductor PRIVATE NE10)
endif ()
