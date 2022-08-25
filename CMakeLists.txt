cmake_minimum_required(VERSION 3.21)
project(zksc)

set(CMAKE_CXX_STANDARD 14)

# find a boost install with the libraries filesystem and system
find_package(Boost REQUIRED)
include_directories(${BOOST_INCLUDE_DIRS})

# check if boost was found
if (Boost_FOUND)
    message("LOG: Boost found")
else ()
    message(FATAL_ERROR "Cannot find Boost")
endif ()

# find openssl install with the libraries filesystem and system
find_package(OpenSSL REQUIRED)


# check if boost was found
if (OpenSSL_FOUND)
    message("LOG: OpenSSL found")
else ()
    message(FATAL_ERROR "Cannot find OpenSSL")
endif ()

############################################################
# Create the zeta generators
############################################################

# Add an executable with the above sources
add_executable(zeta_gen32 details/zeta32.cpp)

# link the new target with the target
target_link_libraries(zeta_gen32 PRIVATE ${Boost_LIBRARIES})
target_link_libraries(zeta_gen32 PRIVATE OpenSSL::SSL)

# Add an executable with the above sources
add_executable(zeta_gen56 details/zeta56.cpp)

# link the new target with the target
target_link_libraries(zeta_gen56 PRIVATE ${Boost_LIBRARIES})
target_link_libraries(zeta_gen56 PRIVATE OpenSSL::SSL)

############################################################
# Create libraries
############################################################

set(ENABLE_56INT 1)
set(DEBUG_MODE 0)
add_definitions( -DENABLE_56INT=${ENABLE_56INT})
add_definitions( -DDEBUG_MODE=${DEBUG_MODE})

if(ENABLE_56INT EQUAL 1)
    message("Compiling for 56 bit numbers")
    set(LATTICE_SOURCES
            src/lattice256/params56.h
            src/lattice256/structs56.h
            src/util.h)
else()
    message("Compiling for 32 bit numbers")
    set(LATTICE_SOURCES
            src/lattice256/params32.h
            src/lattice256/structs32.h
            src/util.h)
endif()

add_library(lattice256_ntt SHARED src/lattice256/ntt.cpp ${LATTICE_SOURCES})
add_library(lattice256::ntt ALIAS lattice256_ntt)
target_include_directories(lattice256_ntt PUBLIC ${PROJECT_SOURCE_DIR}/src/lattice256)
target_link_libraries(lattice256_ntt PRIVATE ${Boost_LIBRARIES})

add_library(lattice256_fips202 SHARED src/lattice256/fips202.cpp)
add_library(lattice256::fips202 ALIAS lattice256_fips202)
target_include_directories(lattice256_fips202 PUBLIC ${PROJECT_SOURCE_DIR}/src/lattice256)

add_library(lattice256_shake SHARED src/lattice256/shake.cpp ${LATTICE_SOURCES})
add_library(lattice256::shake ALIAS lattice256_shake)
target_include_directories(lattice256_shake PUBLIC ${PROJECT_SOURCE_DIR}/src/lattice256)
target_link_libraries(lattice256_shake PRIVATE lattice256::fips202)
target_link_libraries(lattice256_shake PRIVATE ${Boost_LIBRARIES})

add_library(lattice256_poly SHARED src/lattice256/poly.cpp ${LATTICE_SOURCES})
add_library(lattice256::poly ALIAS lattice256_poly)
target_include_directories(lattice256_poly PUBLIC ${PROJECT_SOURCE_DIR}/src/lattice256)
target_link_libraries(lattice256_poly PRIVATE lattice256::ntt)
target_link_libraries(lattice256_poly PRIVATE lattice256::fips202)
target_link_libraries(lattice256_poly PRIVATE lattice256::shake)
target_link_libraries(lattice256_poly PRIVATE ${Boost_LIBRARIES})

add_library(lattice256_random SHARED src/lattice256/random.cpp ${LATTICE_SOURCES})
add_library(lattice256::random ALIAS lattice256_random)
target_include_directories(lattice256_random PUBLIC ${PROJECT_SOURCE_DIR}/src/lattice256)
target_link_libraries(lattice256_random PRIVATE lattice256::poly)
target_link_libraries(lattice256_random PRIVATE ${Boost_LIBRARIES})

add_library(lattice256 SHARED src/lattice256/polyvec.cpp ${LATTICE_SOURCES})
add_library(lattice256::lib ALIAS lattice256)
target_include_directories(lattice256 PUBLIC ${PROJECT_SOURCE_DIR}/src/lattice256)
target_link_libraries(lattice256 PUBLIC lattice256::fips202)
target_link_libraries(lattice256 PUBLIC lattice256::shake)
target_link_libraries(lattice256 PUBLIC lattice256::poly)
target_link_libraries(lattice256 PUBLIC lattice256::random)

# packing
add_library(cip_sis_pack SHARED src/pack/pack.cpp)
add_library(cip_sis::pack ALIAS cip_sis_pack)
target_include_directories(cip_sis_pack PUBLIC ${PROJECT_SOURCE_DIR}/src/pack)
target_link_libraries(cip_sis_pack PRIVATE lattice256::lib)
target_link_libraries(cip_sis_pack PRIVATE m)
target_link_libraries(cip_sis_pack PRIVATE ${Boost_LIBRARIES})

# inputs
add_library(cip_sis_input SHARED src/input/input.cpp)
add_library(cip_sis::input ALIAS cip_sis_input)
target_include_directories(cip_sis_input PUBLIC ${PROJECT_SOURCE_DIR}/src/input)
target_link_libraries(cip_sis_input PRIVATE lattice256::lib)
target_link_libraries(cip_sis_input PRIVATE cip_sis::pack)

# algo
add_library(cip_sis_algo SHARED src/algo/arithmetic.cpp src/algo/statements.cpp src/algo/serializer.cpp)
add_library(cip_sis::algo ALIAS cip_sis_algo)
target_include_directories(cip_sis_algo PUBLIC ${PROJECT_SOURCE_DIR}/src/algo)
target_link_libraries(cip_sis_algo PRIVATE lattice256::lib)
target_link_libraries(cip_sis_algo PRIVATE cip_sis::pack)
target_link_libraries(cip_sis_algo PRIVATE cip_sis::input)
target_link_libraries(cip_sis_algo PRIVATE ${Boost_LIBRARIES})

# lang
set(LANG_SOURCES
        src/lang/compiling_functions.h
        src/lang/executing_functions.h
        src/lang/parser_gen.cpp
        src/lang/parser_ver.cpp
        src/lang/pre_compiler.cpp
        src/lang/lexer.cpp
        src/lang/lang_utils.h)
add_library(cip_sis_lang SHARED ${LANG_SOURCES})
add_library(cip_sis::lang ALIAS cip_sis_lang)
target_include_directories(cip_sis_lang PUBLIC ${PROJECT_SOURCE_DIR}/src/lang)
target_link_libraries(cip_sis_lang PRIVATE lattice256::lib)
target_link_libraries(cip_sis_lang PRIVATE cip_sis::pack)
target_link_libraries(cip_sis_lang PRIVATE cip_sis::input)
target_link_libraries(cip_sis_lang PRIVATE cip_sis::algo)
target_link_libraries(cip_sis_lang PRIVATE ${Boost_LIBRARIES})

# main
add_library(cip_sis SHARED src/rahas.cpp)
add_library(cip_sis::lib ALIAS cip_sis)
target_include_directories(cip_sis PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(cip_sis PUBLIC ${Boost_LIBRARIES})
target_link_libraries(cip_sis PUBLIC OpenSSL::SSL)
target_link_libraries(cip_sis PUBLIC m)
target_link_libraries(cip_sis PUBLIC lattice256::lib)
target_link_libraries(cip_sis PUBLIC cip_sis::pack)
target_link_libraries(cip_sis PUBLIC cip_sis::input)
target_link_libraries(cip_sis PUBLIC cip_sis::algo)
target_link_libraries(cip_sis PUBLIC cip_sis::lang)


############################################################
# Create the tests
############################################################

set(TESTS
        src/lattice256/tests_lattice.hpp
        src/input/tests_inputs.hpp
        src/algo/tests_algo.hpp
        src/lang/tests_lang.hpp)

# Add an executable with the above sources
add_executable(tests src/tests.cpp ${TESTS})

# link the new target with the target
target_link_libraries(tests PUBLIC lattice256::lib)
target_link_libraries(tests PRIVATE cip_sis::lib)
target_link_libraries(tests PRIVATE ${Boost_LIBRARIES})
target_link_libraries(tests PRIVATE OpenSSL::SSL)
