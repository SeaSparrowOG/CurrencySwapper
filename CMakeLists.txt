cmake_minimum_required(VERSION 3.24)

option(FULL_RELEASE "Finalizes the build, compressing everything into a BSA." OFF)
option(OUTDATED "Applies 1.5.97 offsets/IDs." OFF)

# -------- Project ----------
project(
	CurrencySwapper
	VERSION 2.0.0
	LANGUAGES CXX
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

get_filename_component(
	Skyrim64Path
	"[HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Bethesda Softworks\\Skyrim Special Edition;installed path]"
	ABSOLUTE CACHE
)

list(APPEND CMAKE_IGNORE_PATH "${PROJECT_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}/tools/SKSE-CMakeModules/Modules"
)
list(APPEND CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}/tools/Bethesda-CMakeModules/Modules"
)
include(SKSEPlugin)
set(SKSE_USE_XBYAK ON)
add_compile_definitions(SKSE_SUPPORT_XBYAK)

if(OUTDATED)
	add_compile_definitions(SKYRIM_OUTDATED)
else()
	add_compile_definitions(SKYRIM_AE)
	add_compile_definitions(SKYRIM_SUPPORT_AE)
endif()

SKSEPlugin_Add(
	${PROJECT_NAME}
	SOURCE_DIR src
	SOURCES
		README.md
		vcpkg.json
	PRECOMPILE_HEADERS
		src/common/PCH.h
)

set_target_properties(CommonLibSSE PROPERTIES
	FOLDER External
)

set(DATA_DIR "${PROJECT_SOURCE_DIR}/data")

include(components.cmake)

# ---- Post build ----
if(DEFINED ENV{SKYRIM_MODS_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_MODS_FOLDER}")
	if(OUTDATED)
		set(OUTPUT_FOLDER "$ENV{SKYRIM_MODS_FOLDER}/${PROJECT_NAME} - Outdated")
	elseif (FULL_RELEASE)
		set(OUTPUT_FOLDER "$ENV{SKYRIM_MODS_FOLDER}/${PROJECT_NAME} - Full Release")
	else()
		set(OUTPUT_FOLDER "$ENV{SKYRIM_MODS_FOLDER}/${PROJECT_NAME} - Test")
	endif()
endif()

# ---- Compile Papyrus Scripts and Pack Into Archive ----
include(Papyrus)
include(BSArchive)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Data/Source/Scripts")
	file(GLOB ${PROJECT_NAME}_SOURCES
		LIST_DIRECTORIES false
		CONFIGURE_DEPENDS
		"Data/Source/Scripts/*.psc"
	)

	Papyrus_Add(
		"Papyrus"
		GAME ${Skyrim64Path}
		IMPORTS ${CMAKE_CURRENT_SOURCE_DIR}/Data/Source/Scripts
		SOURCES ${${PROJECT_NAME}_SOURCES}
		OPTIMIZE ANONYMIZE
	)

	BSArchive_Add(
		"BSA"
		OUTPUT "${PROJECT_NAME}.bsa"
		FORMAT SSE
		FILES ${Papyrus_OUTPUT}
	)
else()
	message(STATUS "No scripts to compile found, won't compress into BSA.")
endif()

# ---- Automatically copy artifacts to MO2 ----
if(DEFINED OUTPUT_FOLDER)
    set(DLL_FOLDER "${OUTPUT_FOLDER}/SKSE/Plugins")

    add_custom_command(
        TARGET "${PROJECT_NAME}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"

		COMMAND "${CMAKE_COMMAND}"
			-DSRC_DIR="${PROJECT_SOURCE_DIR}/Data"
			-DDST_DIR="${OUTPUT_FOLDER}"
			-DPROJECT_NAME="${PROJECT_NAME}"
			-P "${PROJECT_SOURCE_DIR}/CMake/Scripts/CMake-Copy-Artifacts.cmake"
    )

	# Loose scripts for non-release builds (why? Because I said so smile)
    if(NOT FULL_RELEASE)
        add_custom_command(
        TARGET "${PROJECT_NAME}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}"
            -DSOURCE_SCRIPTS="${PROJECT_SOURCE_DIR}/Data/Source/Scripts"
            -DCOMPILED_SCRIPTS="${CMAKE_BINARY_DIR}/Scripts"
            -DOUTPUT_FOLDER="${OUTPUT_FOLDER}"
            -P "${PROJECT_SOURCE_DIR}/CMake/Scripts/CMake-Copy-Scripts.cmake"
        VERBATIM
    )
    endif()
endif()