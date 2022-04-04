cmake_minimum_required(VERSION 3.8)	 # Проверка версии CMake.
										# Если версия установленой программы
										# старее указаной, произайдёт аварийный выход.
set(PROJECT_NAME socket)

project(${PROJECT_NAME})			# Название проекта

set(CMAKE_C_FLAGS "$CMAKE_C_FLAGS /std:c++20")

set(SOURCES main.cpp socket_wrapper/src/socket.cpp socket_wrapper/src/socket_wrapper.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})		# Создает исполняемый файл с именем main
									            # из исходника main.cpp
