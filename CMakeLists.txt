cmake_minimum_required(VERSION 3.22.1)
project(FIA_WEB)

set(CMAKE_CXX_STANDARD 17)

add_executable(FIA_SERV Server/FiaServ.cpp
)
add_executable(FIA_CLIT Client/FiaClit.cpp
)