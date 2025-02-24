cmake_minimum_required(VERSION 3.24)
project(SearchEngine)

set(CMAKE_CXX_STANDARD 17)
set(HEAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(SearchEngine main.cpp)
add_executable(BuildDict test/testBuildDict.cc src/DictProducer.cc src/Configuration.cc src/SplitToolCppJieba.cc src/SplitTool.cc src/DirScanner.cc)
add_executable(jieBaTest test/testJieBa.cc src/SplitTool.cc src/SplitToolCppJieba.cc)
add_executable(XMLParser test/testXML.cc src/tinyxml2.cc src/Configuration.cc src/DirScanner.cc src/PageLib.cc)
add_executable(server test/Server.cc src/SearchEngineServer.cc src/KeyRecommender.cc src/Dictionary.cc src/CandidateResult.cc src/WebPageQuery.cc src/WebPage.cpp src/tinyxml2.cc src/Configuration.cc src/SplitToolCppJieba.cc src/SplitTool.cc src/WebInfo.cc)
add_executable(WebSearchModel test/testWebSearchModel.cc src/PageLibPreprocessor.cc src/Configuration.cc src/SplitToolCppJieba.cc src/SplitTool.cc src/tinyxml2.cc)
target_include_directories(BuildDict PUBLIC ${HEAD_DIR})
target_include_directories(SearchEngine PUBLIC ${HEAD_DIR})
target_include_directories(jieBaTest PUBLIC ${HEAD_DIR})
target_include_directories(XMLParser PUBLIC ${HEAD_DIR})
target_include_directories(server PUBLIC ${HEAD_DIR})
target_include_directories(WebSearchModel PUBLIC ${HEAD_DIR})
target_link_libraries(server -lworkflow -lwfrest)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")