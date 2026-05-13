cmake_minimum_required(VERSION 3.12)

project(njako C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_executable(main
	${PROJECT_SOURCE_DIR}/AvlMap.c
	${PROJECT_SOURCE_DIR}/hashArray.c
	${PROJECT_SOURCE_DIR}/pVoidArray.c
	${PROJECT_SOURCE_DIR}/main.c
	${PROJECT_SOURCE_DIR}/serialize_to_bytes.c
)


