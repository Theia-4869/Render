#pragma once

#include <string>
#include <vector>

#include "graphics/camera.h"
#include "graphics/framebuffer.h"
#include "graphics/platform.h"
#include "graphics/scene.h"

namespace VCL {
enum class BUTTON : unsigned char { Left = 0, Right, Middle, NUM };

class Renderer {
 public:
  VWindow* window_ = nullptr;
  Framebuffer* framebuffer_ = nullptr;
  Camera* camera_ = nullptr;

  Scene scene_;

  Vec2f last_mouse_pos_;
  bool button_pressed_[size_t(BUTTON::NUM)] = {};

  int width_ = 800;
  int height_ = 600;

  bool isFix_ = false;
  int lightMode_ = -1;
  int cameraMode_ = -1;
  int tracingMode_ = false;
  
  void Init(const std::string& title, int width, int height,
            bool isFix, int lightMode, int cameraMode, bool tracingMode);
  void Progress(int &x, int &y, Color **buffer, int **cnt, const bool MonteCarlo);
  void MainLoop();
  void Destroy();


  // callbacks
  void MouseBottonCallback(BUTTON button, bool pressed);
};
};  // namespace VCL