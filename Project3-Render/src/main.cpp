#include <iostream>
#include "renderer/renderer.h"
#include <spdlog/spdlog.h>
#include "common/getopt.h"
#include "common/helperfunc.h"

using namespace VCL;

bool isFix = false;
int lightMode = -1;
int cameraMode = -1;
std::string tracing = "ray";
bool tracingMode = false;

void PrintHelp()
{
    std::cout <<
            "--fix:               Fix the objects (default false)\n"
            "--light <mode>:      Set mode of light (0, 1, 2, 3)\n"
            "--camera <mode>:     Set view of camera (0, 1, 2, 3)\n"
            "--tracing <mode>:    Set tracing mode (ray, path)\n"
            "--help:              Show help\n";
    exit(1);
}

void ProcessArgs(int argc, char** argv)
{
    const char* const short_opts = "fl:c:t:h";
    const option long_opts[] = {
            {"fix", no_argument, nullptr, 'f'},
            {"light", required_argument, nullptr, 'l'},
            {"camera", required_argument, nullptr, 'c'},
            {"tracing", required_argument, nullptr, 't'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, no_argument, nullptr, 0}
    };

    while (true)
    {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt)
            break;

        switch (opt)
        {
        case 'f':
            isFix = true;
            std::cout << "Fix is set to true\n" << std::endl;
            break;

        case 'l':
            lightMode = std::stoi(optarg);
            if (lightMode < 0 || lightMode > 3)
            {
              std::cout << "Mode of light should be (0, 1, 2, 3), not " << lightMode << "\n" << std::endl;
              exit(1);
            }
            std::cout << "Mode of light set to: " << lightMode << "\n" << std::endl;
            break;

        case 'c':
            cameraMode = std::stoi(optarg);
            if (cameraMode < 0 || cameraMode > 3)
            {
              std::cout << "Mode of camera should be (0, 1, 2, 3), not " << cameraMode << "\n" << std::endl;
              exit(1);
            }
            std::cout << "Mode of camera set to: " << cameraMode << "\n" << std::endl;
            break;

        case 't':
            tracing = std::string(optarg);
            if (tracing == "ray")
            {
              tracingMode = false;
              std::cout << "Choose ray tracing mode" << "\n" << std::endl;
            }
            else if (tracing == "path")
            {
              tracingMode = true;
              std::cout << "Choose path tracing mode" << "\n" << std::endl;
            }
            else
            {
              std::cout << "Tracing mode should be either ray or path, not " << tracing << "\n" << std::endl;
              exit(1);
            }
            break;

        case 'h': // -h or --help
        case '?': // Unrecognized option
        default:
            PrintHelp();
            break;
        }
    }
}

int main(int argc, char **argv) {
  spdlog::set_pattern("[%^%l%$] %v");
#ifdef NDEBUG
  spdlog::set_level(spdlog::level::info);
#else
  spdlog::set_level(spdlog::level::debug);
#endif
  ProcessArgs(argc, argv);
  std::cout << "is position fix: " << std::boolalpha << isFix << "\n"
            << "light mode: " << lightMode << "\n"
            << "camera mode: " << cameraMode << "\n"
            << "tracing mode: " << tracing << " tracing\n";
  Renderer renderer;
  renderer.Init("Visual Computing", 800, 600,
                isFix, lightMode, cameraMode, tracingMode);
  renderer.MainLoop();
  renderer.Destroy();
  return 0;
}