include_directories(../include)
aux_source_directory(. DIR_LIB_SRCS)
add_library (LnStlLibrary ${DIR_LIB_SRCS})


