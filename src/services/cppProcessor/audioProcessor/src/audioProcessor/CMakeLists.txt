add_executable(intro main.cpp "beamProcessor.cpp" "app.cpp" )

target_link_libraries(
  intro
  PRIVATE myproject::myproject_options
          myproject::myproject_warnings)

target_link_system_libraries(
  intro
  PRIVATE
          CLI11::CLI11
          fmt::fmt
          spdlog::spdlog
          kfr
          #threadpool
          
)

target_include_directories(intro PRIVATE 

"${CMAKE_BINARY_DIR}/configured_files/include"
"${PROJECT_SOURCE_DIR}/include"
#"${thread_pool_SOURCE_DIR}/include"
"${PROJECT_SOURCE_DIR}/libs/thread-pool/include"


)
