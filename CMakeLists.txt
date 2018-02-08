cmake_minimum_required(VERSION 3.9)
project(vocableTrainer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_AUTOMOC ON)
find_package(Qt5 REQUIRED
        Widgets
        )
find_package(XercesC REQUIRED)

set(SourceFiles
        main.cpp
        VocableTrainer.h VocableTrainer.cpp
        VocableList.h VocableList.cpp
        Vocable.h
        stringUtils.h
        MyTableModel.tcc MyTableModel.h
        MyTableView.tcc MyTableView.h
        Optional.h
        lect2-pskel.cxx lect2-pskel.hxx
        Lect2Parser.h parsingEntities.h Info.h Finally.h Trainer.cpp Trainer.h)

add_executable(vocableTrainer ${SourceFiles})
target_link_libraries(vocableTrainer Qt5::Widgets xerces-c)
