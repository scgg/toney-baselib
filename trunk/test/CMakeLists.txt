
# test cmakelists ,test 的cmake文件

file(GLOB_RECURSE test_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)
#source_group("appmain"  REGULAR_EXPRESSION appmain)

include_directories(
  ${PROJECT_SOURCE_DIR}/algorithm
  ${PROJECT_SOURCE_DIR}/base
  ${PROJECT_SOURCE_DIR}/config
  ${PROJECT_SOURCE_DIR}/net
  ${PROJECT_SOURCE_DIR}/system
  
)

link_directories(
  ${PROJECT_SOURCE_DIR}/lib
)

ADD_EXECUTABLE(test ${test_SRCS})

target_link_libraries(test
	TYLib
  
)
















