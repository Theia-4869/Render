#include "renderer.h"
#include <iostream>
#include <random>

#include "common/helperfunc.h"
#include "graphics/globillum.h"

namespace VCL {
void Renderer::Init(const std::string& title, int width, int height,
                    bool isFix, int lightMode, int cameraMode, bool tracingMode) {
  width_ = width;
  height_ = height;
  isFix_ = isFix;
  lightMode_ = lightMode;
  cameraMode_ = cameraMode;
  tracingMode_ = tracingMode;
  InitPlatform();
  window_ = CreateVWindow(title, width_, height_, this);
  framebuffer_ = new Framebuffer(width_, height_);

  camera_ = new Camera;
  const float c_y = 1.5;
  const float c_z = 1.5 + 1.5 * std::sqrt(2);
  if (cameraMode_ == -1) cameraMode_ = std::floor(rand01() * 4);
  if (cameraMode_ == 0) 
  {
    camera_->InitData((float)width_ / height_, 0.25f * PI_, 1.0f, 1000.0f, c_z,
                        1.0f * PI_, 0.5f * PI_, Vec3f(0.0f, c_y, -4.0f));
  }
  else if (cameraMode_ == 1) 
  {
    camera_->InitData((float)width_ / height_, 0.25f * PI_, 1.0f, 1000.0f, c_z,
                        0.0f, 0.5f * PI_, Vec3f(0.0f, c_y, 0.0f));
  }
  else if (cameraMode_ == 2) 
  {
    camera_->InitData((float)width_ / height_, 0.25f * PI_, 1.0f, 1000.0f, c_z,
                        -0.5f * PI_, 0.5f * PI_, Vec3f(-2.0f, c_y, -2.0f));
  }
  else if (cameraMode_ == 3) 
  {
    camera_->InitData((float)width_ / height_, 0.25f * PI_, 1.0f, 1000.0f, c_z,
                        0.5f * PI_, 0.5f * PI_, Vec3f(2.0f, c_y, -2.0f));
  }

  auto &mats = scene_.mats_;
	auto &objs = scene_.objs_;
  auto &lights = scene_.lights_;
	// Initialize materials.
	mats["ceiling"] = std::make_unique<Material>(Color(102.0f, 8.0f, 116.0f) / 255);
	mats["floor"] = std::make_unique<Material>(Color(140.0f, 0.0f, 0.0f) / 255);
	mats["wall1"] = std::make_unique<Material>(Color(122.0f, 255.0f, 206.0f) / 255);
	mats["wall2"] = std::make_unique<Material>(Color(134.0f, 151.0f, 255.0f) / 255);
  mats["wall3"] = std::make_unique<Material>(Color(255.0f, 241.0f, 67.0f) / 255);
	mats["wall4"] = std::make_unique<Material>(Color(255.0f, 192.0f, 203.0f) / 255);
	mats["wood"] = std::make_unique<Material>(Color(98.0f, 42.0f, 29.0f) / 255);
  mats["glaze"] = std::make_unique<Material>(Color(26.0f, 79.0f, 163.0f) / 255);
	mats["metal"] = std::make_unique<Material>(Color(0, 0, 0), Color(.8f, .8f, .8f), 30);
	mats["mirror"] = std::make_unique<Material>(Color(37.2f, 24.4f, 13.2f) / 255, Color(.6f, .6f, .6f), -1);
	mats["light"] = std::make_unique<Material>(Color(20, 20, 20), true);
  if (cameraMode_ == 0) mats["wall4"] = std::make_unique<Material>(Color(37.2f, 24.4f, 13.2f) / 255, Color(.6f, .6f, .6f), -1);
  else if (cameraMode_ == 1) mats["wall3"] = std::make_unique<Material>(Color(37.2f, 24.4f, 13.2f) / 255, Color(.6f, .6f, .6f), -1);
  else if (cameraMode_ == 2) mats["wall2"] = std::make_unique<Material>(Color(37.2f, 24.4f, 13.2f) / 255, Color(.6f, .6f, .6f), -1);
  else if (cameraMode_ == 3) mats["wall1"] = std::make_unique<Material>(Color(37.2f, 24.4f, 13.2f) / 255, Color(.6f, .6f, .6f), -1);
	// Set boundaries.
	objs.emplace_back(std::make_unique<Plane>(
		mats["ceiling"].get(),
		Vec3(0, 3, 0), Vec3(0, -1, 0)));
	objs.emplace_back(std::make_unique<Plane>(
		mats["floor"].get(),
		Vec3(0, 0, 0), Vec3(0, 1, 0)));
	objs.emplace_back(std::make_unique<Plane>(
		mats["wall1"].get(),
		Vec3(-2, 0, 0), Vec3(1, 0, 0)));
	objs.emplace_back(std::make_unique<Plane>(
		mats["wall2"].get(),
		Vec3(2, 0, 0), Vec3(-1, 0, 0)));
	objs.emplace_back(std::make_unique<Plane>(
		mats["wall3"].get(),
		Vec3(0, 0, -4), Vec3(0, 0, 1)));
  objs.emplace_back(std::make_unique<Plane>(
		mats["wall4"].get(),
		Vec3(0, 0, 0), Vec3(0, 0, -1)));
	// Set the light.
	const real dl = real(2) / 3;
	const real rl = 5;
	const real hl = std::sqrt(rl * rl - dl * dl);
  if (lightMode_ == -1) lightMode_ = std::floor(rand01() * 4);
  if (lightMode_ == 0)
  {
    objs.emplace_back(std::make_unique<Sphere>(
      mats["light"].get(),
      Vec3(0, 3 + hl, -2), rl));
    objs.emplace_back(std::make_unique<Sphere>(
      mats["light"].get(),
      Vec3(-1, 3, -3), real(.01)));
    objs.emplace_back(std::make_unique<Sphere>(
      mats["light"].get(),
      Vec3(1, 3, -3), real(.01)));
    objs.emplace_back(std::make_unique<Sphere>(
      mats["light"].get(),
      Vec3(-1, 3, -1), real(.01)));
    objs.emplace_back(std::make_unique<Sphere>(
      mats["light"].get(),
      Vec3(1, 3, -1), real(.01)));
    lights.emplace_back(std::make_unique<Light>(
      Vec3(0, 3, -2),
      Color(1, 1, 1) * 2.0));
    lights.emplace_back(std::make_unique<Light>(
      Vec3(-1, 3, -3),
      Color(1, 1, 1) * 0.5));
    lights.emplace_back(std::make_unique<Light>(
      Vec3(1, 3, -3),
      Color(1, 1, 1) * 0.5));
    lights.emplace_back(std::make_unique<Light>(
      Vec3(-1, 3, -1),
      Color(1, 1, 1) * 0.5));
    lights.emplace_back(std::make_unique<Light>(
      Vec3(1, 3, -1),
      Color(1, 1, 1) * 0.5));
  }
  else if (lightMode_ == 1)
  {
    objs.emplace_back(std::make_unique<Sphere>(
      mats["light"].get(),
      Vec3(0, 3 + hl, -2), rl));
    objs.emplace_back(std::make_unique<Sphere>(
      mats["light"].get(),
      Vec3(0, 3, -3.25), real(.01)));
    objs.emplace_back(std::make_unique<Sphere>(
      mats["light"].get(),
      Vec3(0, 3, -0.75), real(.01)));
    objs.emplace_back(std::make_unique<Sphere>(
      mats["light"].get(),
      Vec3(-1.25, 3, -2), real(.01)));
    objs.emplace_back(std::make_unique<Sphere>(
      mats["light"].get(),
      Vec3(1.25, 3, -2), real(.01)));
    lights.emplace_back(std::make_unique<Light>(
      Vec3(0, 3, -2),
      Color(1, 1, 1) * 2.0));
    lights.emplace_back(std::make_unique<Light>(
      Vec3(0, 3, -3.25),
      Color(1, 1, 1) * 0.5));
    lights.emplace_back(std::make_unique<Light>(
      Vec3(0, 3, -0.75),
      Color(1, 1, 1) * 0.5));
    lights.emplace_back(std::make_unique<Light>(
      Vec3(-1.25, 3, -2),
      Color(1, 1, 1) * 0.5));
    lights.emplace_back(std::make_unique<Light>(
      Vec3(1.25, 3, -2),
      Color(1, 1, 1) * 0.5));
  }
  else if (lightMode_ == 2)
  {
    objs.emplace_back(std::make_unique<Sphere>(
      mats["light"].get(),
      Vec3(0, 3, -2), real(.01)));
    lights.emplace_back(std::make_unique<Light>(
      Vec3(0, 3, -2),
      Color(1, 1, 1) * 2.0));
    if (cameraMode_ == 0 || cameraMode_ == 1)
    {
      objs.emplace_back(std::make_unique<Cuboid>(
        mats["light"].get(),
        Vec3(-2, 1.5, -2),
        real(0.02), real(1.0), real(1.0)));
      objs.emplace_back(std::make_unique<Cuboid>(
        mats["light"].get(),
        Vec3(2, 1.5, -2),
        real(0.02), real(1.0), real(1.0)));
      lights.emplace_back(std::make_unique<Light>(
        Vec3(-2, 1.5, -2),
        Color(1, 1, 1) * 1.0));
      lights.emplace_back(std::make_unique<Light>(
        Vec3(2, 1.5, -2),
        Color(1, 1, 1) * 1.0));
    }
    else if (cameraMode_ == 2 || cameraMode_ == 3)
    {
      objs.emplace_back(std::make_unique<Cuboid>(
        mats["light"].get(),
        Vec3(0, 1.5, -4),
        real(1.0), real(1.0), real(0.02)));
      objs.emplace_back(std::make_unique<Cuboid>(
        mats["light"].get(),
        Vec3(0, 1.5, 0),
        real(1.0), real(1.0), real(0.02)));
      lights.emplace_back(std::make_unique<Light>(
        Vec3(0, 1.5, -4),
        Color(1, 1, 1) * 1.0));
      lights.emplace_back(std::make_unique<Light>(
        Vec3(0, 1.5, 0),
        Color(1, 1, 1) * 1.0));
    }
  }
  else if (lightMode_ == 3)
  {
    objs.emplace_back(std::make_unique<Cuboid>(
      mats["light"].get(),
      Vec3(0, 3, -2),
      real(1.0), real(0.02), real(1.0)));
    lights.emplace_back(std::make_unique<Light>(
      Vec3(0, 3, -2),
      Color(1, 1, 1) * 2.0));
    if (cameraMode_ == 0 || cameraMode_ == 1)
    {
      objs.emplace_back(std::make_unique<Sphere>(
        mats["light"].get(),
        Vec3(-2, 1.5, -2), real(.01)));
      objs.emplace_back(std::make_unique<Sphere>(
        mats["light"].get(),
        Vec3(2, 1.5, -2), real(.01)));
      lights.emplace_back(std::make_unique<Light>(
        Vec3(-2, 1.5, -2),
        Color(1, 1, 1) * 1.0));
      lights.emplace_back(std::make_unique<Light>(
        Vec3(2, 1.5, -2),
        Color(1, 1, 1) * 1.0));
    }
    else if (cameraMode_ == 2 || cameraMode_ == 3)
    {
      objs.emplace_back(std::make_unique<Sphere>(
        mats["light"].get(),
        Vec3(0, 1.5, -4), real(.01)));
      objs.emplace_back(std::make_unique<Sphere>(
        mats["light"].get(),
        Vec3(0, 1.5, 0), real(.01)));
      lights.emplace_back(std::make_unique<Light>(
        Vec3(0, 1.5, -4),
        Color(1, 1, 1) * 1.0));
      lights.emplace_back(std::make_unique<Light>(
        Vec3(0, 1.5, 0),
        Color(1, 1, 1) * 1.0));
    }
  }
  if (isFix_)
  {
    objs.emplace_back(std::make_unique<Cuboid>(
      mats["wood"].get(),
      Vec3(1, 1, real(-3.2)),
      real(.8), real(2.0), real(.8)));
    objs.emplace_back(std::make_unique<Sphere>(
      mats["metal"].get(),
      Vec3(-1, real(.8), -2), real(.8)));
    objs.emplace_back(std::make_unique<Tetrahedron>(
      mats["glaze"].get(),
      Vec3(0, 0, -2),
      Vec3(real(-0.866), 0, real(-0.5)),
      Vec3(real(0.866), 0, real(-0.5)),
      Vec3(0, real(1.414), -1)));
  }
  else
  {
    real c1 = rand01() + 0.5;
    objs.emplace_back(std::make_unique<Cuboid>(
      mats["wood"].get(),
      Vec3(c1, 1, real(-3.2)),
      real(.8), real(2.0), real(.8)));
    real c2 = -rand01() / 5.0 - 0.9;
    objs.emplace_back(std::make_unique<Sphere>(
      mats["metal"].get(),
      Vec3(c2, real(.8), -2), real(.8)));
    real c3 = rand01() - 0.5;
    objs.emplace_back(std::make_unique<Tetrahedron>(
      mats["glaze"].get(),
      Vec3(c3, 0, -2),
      Vec3(real(-0.866)+c3, 0, real(-0.5)),
      Vec3(real(0.866)+c3, 0, real(-0.5)),
      Vec3(c3, real(1.414), -1)));
  }
  
  scene_.ambient_light_ = Color(0.05, 0.05, 0.05);
}

void Renderer::Progress(int &x, int &y, Color **buffer, int **cnt, const bool MonteCarlo) {
  const real dx = real(1) / width_;
	const real dy = real(1) / height_;

  const real lx = dx * x;
  const real ly = dy * y;

  const real sx = lx + rand01() * dx;
  const real sy = ly + rand01() * dy;

  if (!MonteCarlo) {
    buffer[y][x] += (GlobIllum::RayTrace(scene_, camera_->GenerateRay(sx, sy)) - buffer[y][x]) / (++cnt[y][x]);    
  }
  else {
    buffer[y][x] += (GlobIllum::PathTrace(scene_, camera_->GenerateRay(sx, sy)) - buffer[y][x]) / (++cnt[y][x]);
  }

  int idx = (y * width_ + x) * 4;
  for (int i = 0; i < 3; i++) framebuffer_->color_[idx + i] = std::round(std::pow(std::clamp(buffer[y][x][i], 0.0f, 1.0f), 1 / 2.2f) * 255);

  x++;
  if (x == width_) {
    x = 0;
    y++;
    if (y == height_) y = 0;
  }
}

void Renderer::MainLoop() {
  // switch between ray tracing and path tracing
  const bool MonteCarlo = tracingMode_;

  int x;
  int y;

  Color **buffer = new Color *[height_];
  int **cnt = new int *[height_];

  for (y = 0; y < height_; y++) {
    buffer[y] = new Color[width_];
    cnt[y] = new int[width_];
    for (x = 0; x < width_; x++) {
      buffer[y][x] = Color::Zero();
      cnt[y][x] = 0;
    }
  }

  x = y = 0;
  int idx = 0;
  const int buffer_size = height_ * width_;
  int patch_size = 50000;
  patch_size = patch_size > buffer_size ? buffer_size : patch_size;
  while (!window_->should_close_) {
    PollInputEvents();

    // serial version
    // for (int i = 0; i < 10; ++i) {
    //   Progress(x, y, buffer, cnt, MonteCarlo);
    // }

    // parallel version
    # pragma omp parallel for
    for (int i = 0; i < patch_size; ++i) {
      int p = (idx + i) % buffer_size;
      int px = p % width_;
      int py = p / width_;
      Progress(px, py, buffer, cnt, MonteCarlo);
    }
    idx = (idx + patch_size) % buffer_size;

    window_->DrawBuffer(framebuffer_);
  }

  for (y = 0; y < height_; y++) {
    delete[] buffer[y];
    delete[] cnt[y];
  }
  delete[] buffer;
  delete[] cnt;
}

void Renderer::Destroy() {
  if (camera_) delete camera_;
  if (framebuffer_) delete framebuffer_;
  window_->Destroy();
  if (window_) delete window_;
  DestroyPlatform();
}
};  // namespace VCL