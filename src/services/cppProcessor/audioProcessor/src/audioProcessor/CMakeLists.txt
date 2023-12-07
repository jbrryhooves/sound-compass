add_executable(intro main.cpp "audioProcessor.cpp" )

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
          

          #lefticus::tools
          #ftxui::screen
          #ftxui::dom
          #ftxui::component
)

target_include_directories(intro PRIVATE 

"${CMAKE_BINARY_DIR}/configured_files/include"
"${PROJECT_SOURCE_DIR}/include"

)
