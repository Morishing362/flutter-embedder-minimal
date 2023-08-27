#include <constants.h>
#include <egl_shell.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <wayland_display.h>

#include <array>
#include <cassert>
#include <iostream>
#include <memory>

EglShell::EglShell(wl_display *display)
    : m_egl_display(nullptr),
      m_egl_surface(nullptr),
      m_egl_config(nullptr),
      m_egl_context(nullptr),
      m_egl_resource_context(nullptr),
      m_wl_egl_window(nullptr) {
  // EGL initialization

  m_egl_display = eglGetDisplay((EGLNativeDisplayType)display);
  if (m_egl_display == EGL_NO_DISPLAY) {
    std::cout << "Failed to create egl display" << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "Created egl display" << std::endl;
  }

  EGLint major, minor;
  if (eglInitialize(m_egl_display, &major, &minor) != EGL_TRUE) {
    std::cout << "Failed to initialize egl display" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto ret_ = eglBindAPI(EGL_OPENGL_ES_API);

  EGLint count;
  eglGetConfigs(m_egl_display, nullptr, 0, &count);

  auto configs =
      reinterpret_cast<EGLConfig *>(calloc(count, sizeof(EGLConfig)));

  EGLint n;
  eglChooseConfig(m_egl_display, k_egl_config_attribs.data(), configs, count,
                  &n);

  EGLint size;
  for (int i = 0; i < n; i++) {
    eglGetConfigAttrib(m_egl_display, configs[i], EGL_BUFFER_SIZE, &size);
    // kBufferSize = 24
    if (size >= 24) {
      m_egl_config = configs[i];
      break;
    }
  }
  free(configs);

  EGLContext egl_context =
      eglCreateContext(m_egl_display, m_egl_config, EGL_NO_CONTEXT,
                       k_egl_context_attribs.data());
  assert(egl_context);
  EGLContext egl_resource_context = eglCreateContext(
      m_egl_display, m_egl_config, egl_context, k_egl_context_attribs.data());
  assert(egl_resource_context);

  m_egl_context = egl_context;
  m_egl_resource_context = egl_resource_context;

  // clear current
  eglMakeCurrent(m_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

EglShell::~EglShell() {}

void EglShell::createEglWindow(wl_surface *surface) {
  m_wl_egl_window = wl_egl_window_create(surface, K_DEFAULT_WINDOW_WIDTH,
                                         K_DEFAULT_WINDOW_HIGHT);
  if (m_wl_egl_window == EGL_NO_SURFACE) {
    std::cout << "Failed to create egl window" << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "Created egl window" << std::endl;
  }

  m_egl_surface =
      eglCreateWindowSurface(m_egl_display, m_egl_config,
                             (EGLNativeWindowType)m_wl_egl_window, nullptr);
  if (m_egl_surface == EGL_NO_SURFACE) {
    std::cout << "Failed to create egl surface" << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "Created egl surface" << std::endl;
  }
}
