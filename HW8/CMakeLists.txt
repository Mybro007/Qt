cmake_minimum_required(VERSION 3.5)

project(HW8 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent Sql)

# Путь к папке с CMakeLists.txt (текущая директория)
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Укажите директорию для поиска библиотек
link_directories(${LIB_DIR})

# Укажите исходники проекта
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h

        database.h
        database.cpp
        dbmanager.h
        dbmanager.cpp
        dbdata.ui
)

add_executable(HW8
        ${PROJECT_SOURCES}
        dbdata.h dbdata.cpp
        mainwindow.ui
)

# Добавьте библиотеки (не указывая полный путь, только имя)
target_link_libraries(HW8 PRIVATE
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Sql
    libcrypto-3-x64        # Имя библиотеки без пути и расширения
    libiconv-2
    libintl-9
    libpq
    libssl-3-x64
)
