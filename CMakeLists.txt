cmake_minimum_required(VERSION 3.0)
project(Glitter)
# 设置C++标准
set(CMAKE_CXX_STANDARD 17)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(Glitter/Vendor/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(Glitter/Vendor/assimp)

option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
add_subdirectory(Glitter/Vendor/bullet)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

include_directories(Glitter/Headers/
                    Glitter/Vendor/assimp/include/
                    Glitter/Vendor/bullet/src/
                    Glitter/Vendor/glad/include/
                    Glitter/Vendor/glfw/include/
                    Glitter/Vendor/glm/
                    Glitter/Vendor/stb/)

file(GLOB VENDORS_SOURCES Glitter/Vendor/glad/src/glad.c)
file(GLOB PROJECT_HEADERS Glitter/Headers/*.hpp)
file(GLOB PROJECT_SOURCES Glitter/Sources/*.cpp Glitter/Sources/*.hpp)
file(GLOB PROJECT_SHADERS Glitter/Shaders/*.comp
                          Glitter/Shaders/*.frag
                          Glitter/Shaders/*.geom
                          Glitter/Shaders/*.vert)
file(GLOB PROJECT_RESOURCES Glitter/Resources/*.jpg
                            Glitter/Resources/*.png)
# 获取资源文件夹中的文件列表
# file(GLOB RESOURCE_FILES "Glitter/resources/*")

# 复制资源文件夹中的文件到构建目录
# file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)

file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})
source_group("Resources" FILES ${PROJECT_RESOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES})
target_link_libraries(${PROJECT_NAME}
        assimp
        glfw
        ${GLFW_LIBRARIES}
        ${CMAKE_SOURCE_DIR}/Glitter/lib-x86_64/libGLEW.a
        ${GLAD_LIBRARIES}
        BulletDynamics
        BulletCollision
        LinearMath
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

#message("zjzjzj CMAKE_CURRENT_BINARY_DIR is ${CMAKE_BINARY_DIR}")
#message("zjzjzj Glitter path is${CMAKE_BINARY_DIR}/${PROJECT_NAME}")
message("zjzjzj GLFW_LIBRARIES ${GLFW_LIBRARIES}")


# 定义源文件夹和目标文件夹的路径
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/Glitter/Resources)
set(DESTINATION_DIR ${PROJECT_NAME}/Resources)

# 自定义函数，用于递归地复制文件夹及其子文件夹，并保持相对路径结构
function(copy_directory_with_relative_path SOURCE DESTINATION)
    file(GLOB_RECURSE FILES RELATIVE "${SOURCE}" "${SOURCE}/*")
    foreach(FILE ${FILES})
        set(SRC "${SOURCE}/${FILE}")
        set(DST "${DESTINATION}/${FILE}")
        if(IS_DIRECTORY "${SRC}")
            file(MAKE_DIRECTORY "${DST}")
            copy_directory_with_relative_path("${SRC}" "${DST}")
        else()
            configure_file("${SRC}" "${DST}" COPYONLY)
        endif()
    endforeach()
endfunction()

# 调用自定义函数进行递归复制文件夹
copy_directory_with_relative_path(${SOURCE_DIR} ${DESTINATION_DIR})

set(SRC_SHADERS_DIR ${CMAKE_SOURCE_DIR}/Glitter/Shaders)
set(DES_SHADERS_DIR ${PROJECT_NAME}/Shaders)

copy_directory_with_relative_path(${SRC_SHADERS_DIR} ${DES_SHADERS_DIR})


#add_custom_command(
#    TARGET ${PROJECT_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Glitter/Shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders
#    COMMENT "Copying resources to build directory"
#    #DEPENDS ${PROJECT_SHADERS} ${PROJECT_RESOURCES}
#)

#file(GLOB_RECURSE RESOURCES_FILES ${CMAKE_SOURCE_DIR}/Resources/*)
#foreach(file ${RESOURCES_FILES})
#    file(COPY ${file} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#endforeach()
#
#function(print_directory_structure TARGET_DIR)
#    message("Directory structure of ${TARGET_DIR}:")
#    file(GLOB_RECURSE files "${TARGET_DIR}/*")
#    foreach(file ${files})
#        file(RELATIVE_PATH relative_path "${TARGET_DIR}" "${file}")
#        message("  ${relative_path}")
#    endforeach()
#endfunction()

# 调用函数打印目标文件目录结构
# print_directory_structure(${CMAKE_BINARY_DIR})