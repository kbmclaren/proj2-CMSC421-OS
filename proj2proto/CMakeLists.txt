cmake_minimum_required(VERSION 3.0.0)
project(mail_box VERSION 0.1.0)

# You can use this line to change the C standard in use. Valid values are:
# 90, 99, 11
set(CMAKE_C_STANDARD 11)

# You can use this line to define the version of C++ to use. Acceptable values
# are: 98, 11, 14 and MAYBE 17 and 20 if you have a relatively new version of GCC
set(CMAKE_CXX_STANDARD 14)

# This will make sure that all of the executables are placed in the root of this
# folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_compile_options(-Wall -Wextra -Wno-unused-parameter -Werror)

# You can add all your c or cpp files here. Just use a new line and the path to
# the file. So for instance:
# set(421_SOURCE_FILES
#     src/main.c
#     src/potato/another_folder_maybe/functions.c
# )
# DO NOT INCLUDE ANY HEADER FILES HERE
set(421_SOURCE_FILES
        src/proto.c
        )


add_executable(mail_box ${421_SOURCE_FILES})