#include <array>
#include <functional>
#include <iostream>
#include <optional>
#include <cstddef>
#include <string>

#include <random>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>



// This file will be generated automatically when you run the CMake
// configuration step. It creates a namespace called `audioProcessor`. You can modify
// the source template at `configured_files/config.hpp.in`.
#include <internal_use_only/config.hpp>

#include <audioProcessor/audioProcessor.hpp>


audioProcessor::audioProcessor ap;

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, const char **argv)
{
    try {
        CLI::App app{ fmt::format(
            "{} version {}", audioProcessor::cmake::project_name, audioProcessor::cmake::project_version) };

        std::optional<std::string> message;
        app.add_option("-m,--message", message, "A message to print back out");
        bool show_version = false;
        app.add_flag("--version", show_version, "Show version information");

        bool is_turn_based = false;
        auto *turn_based = app.add_flag("--turn_based", is_turn_based);

        bool is_loop_based = false;
        auto *loop_based = app.add_flag("--loop_based", is_loop_based);

        turn_based->excludes(loop_based);
        loop_based->excludes(turn_based);


        CLI11_PARSE(app, argc, argv);

        if (show_version) {
            fmt::print("{}\n", audioProcessor::cmake::project_version);
            return EXIT_SUCCESS;
        }

        audioProcessor::audioProcessor::AudioFrame audioFrame;
        
        audioProcessor::audioProcessor::ProcessResult result;

        ap.init();
        ap.processAudioFrame(audioFrame, result);
        fmt::print("{}  \n", audioProcessor::cmake::project_version);




    } catch (const std::exception &e) {
        spdlog::error("Unhandled exception in main: {}", e.what());
    }
}
