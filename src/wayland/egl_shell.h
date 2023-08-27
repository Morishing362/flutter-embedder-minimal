#ifndef EGL_SHELL_H
#define EGL_SHELL_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <wayland-client.h>

#include <array>
#include <memory>

class EglShell {
 public:
  EGLDisplay m_egl_display;
  EGLSurface m_egl_surface;
  EGLContext m_egl_context;
  EGLContext m_egl_resource_context;
  EGLConfig m_egl_config;
  struct wl_egl_window *m_wl_egl_window;

  EglShell(wl_display *display);

  ~EglShell();

  void createEglWindow(wl_surface *surface);
};

static constexpr std::array<EGLint, 15> k_egl_config_attribs = {{
    // clang-format off
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
#if defined(BUILD_EGL_ENABLE_TRANSPARENCY)
    EGL_ALPHA_SIZE, 8,
#else
    EGL_ALPHA_SIZE, 0,
#endif
    EGL_NONE // termination sentinel
    // clang-format on
}};

static constexpr std::array<EGLint, 5> k_egl_context_attribs = {{
    // clang-format off
    EGL_CONTEXT_MAJOR_VERSION, 3,
    EGL_CONTEXT_MAJOR_VERSION, 2,
    EGL_NONE
    // clang-format on
}};

#endif  // EGL_SHELL_H