#if defined(_WIN32) || defined(_WIN64)
  #define _CRT_SECURE_NO_WARNINGS 1
  #pragma warning(disable: 4244) // conversion from 'double' to 'float', possible loss of data
  #pragma warning(disable: 4100) // '(void)argc; (void)argv;' or '(void)mods; (void)scancode;'
  #pragma warning(disable: 4996)
  #pragma warning(disable: 4458) // declaration of method argument hides a class member
#endif // _WIN32 || _WIN64

#define _USE_MATH_DEFINES
#include <cmath>

#include <cassert>
#include <cstdlib>

#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <fmt/core.h>

// GLEW
#include <GL/glew.h> // "glew.h" must be included before "gl.h"
#define GL_GLEXT_PROTOTYPES

// OpenGL
#if defined(__APPLE__)
  #include <OpenGL/gl.h>
#elif defined(linux)
  #include <GL/gl.h>
#else
  #include <GL/GL.h>
#endif // __APPLE__

// GLFW3
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// ImGUI
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

constexpr char kWindowTitle[]{"GLFW3 + ImGui"};
constexpr size_t kScreenWidth{800};
constexpr size_t kScreenHeight{600};
constexpr size_t screen_width{kScreenWidth};
constexpr size_t screen_height{kScreenHeight};

/**
 * @brief
 *
 */
struct DeleteWindow {
  void operator()(GLFWwindow *window) {
    if (window != nullptr) {
      fmt::print("Destroying window '{}'\n", reinterpret_cast<void *>(window));
      glfwDestroyWindow(window);
    }
  }
};

std::tuple<int, int, std::string> get_glsl_version() {
#if defined(__APPLE__)
  // GLFW Context Version: 3.2+ only
  constexpr int kMajorVersion{3};
  constexpr int kMinorVersion{2};
  const std::string kGlslVersion{"#version 150"};
#else
  // GLFW Context Version: 3.0
  constexpr int kMajorVersion{3};
  constexpr int kMinorVersion{3};
  const std::string kGlslVersion{"#version 130"};
#endif // __APPLE__
  return std::tuple<int, int, std::string>(kMajorVersion, kMinorVersion, kGlslVersion);
}

/**
 * @brief
 *
 * @param argc
 * @param argv
 * @return EXIT_SUCCES, EXIT_FAILURE
 */
int32_t main(int32_t argc, char** argv) {
  glfwInit();
  auto [ major_version, minor_version, glsl_version ] = get_glsl_version();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);
  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
  // 3.0+ only, required on MAC
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac
#endif // __APPLE__
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

  std::unique_ptr<GLFWwindow, DeleteWindow> g_window;
  g_window.reset(glfwCreateWindow(screen_width, screen_height, kWindowTitle, nullptr, nullptr));

  glfwMakeContextCurrent(g_window.get());
  glfwSetWindowAttrib(g_window.get(), GLFW_DECORATED, GLFW_TRUE);
  glewInit();
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::GetIO().IniFilename = nullptr;
  ImGui::GetIO().FontGlobalScale = 2.0f;
  ImGui_ImplGlfw_InitForOpenGL(g_window.get(), true);
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());

  // Prepare Data
  std::vector<std::pair<float, float>> points_;
  constexpr float kMaxValue{100.0f};
  for(float x = -1.0f * kMaxValue; x < kMaxValue; x = x + 0.1f) {
      float y = std::pow(x, 2) + 5 * x;
      // float y = 100 * std::log10(x);
      points_.emplace_back(static_cast<float>(screen_width / 2.0f) + x, static_cast<float>(screen_height / 2.0f) - y);
  }

  // Main Loop
  while (!glfwWindowShouldClose(g_window.get())) {
    glfwPollEvents();
    if (glfwGetKey(g_window.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(g_window.get(), 1);
        continue;
    }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::Begin("Render Window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

    // Render as graph
    {
      ImDrawList* draw_list{ImGui::GetWindowDrawList()};
      {
        const ImVec4 color{ImVec4(1.0f, 1.0f, 0.4f, 1.0f)};
        constexpr float thickness{1.0f};
        for(size_t i = 1; i < std::size(points_); i++) {
          // https://github.com/ocornut/imgui/blob/master/imgui_draw.cpp
          ImVec2 p1{points_[i - 1].first, points_[i - 1].second};
          ImVec2 p2{points_[i].first, points_[i].second};
          draw_list->AddLine(p1, p2, ImColor(color), thickness);
        }
      }
    }

    ImGui::End();
    ImGui::PopStyleVar(1);
    ImGui::Render();
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(g_window.get());
  } // End of While

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  g_window.reset();
  glfwTerminate();
  return EXIT_SUCCESS;
} // End of Main
