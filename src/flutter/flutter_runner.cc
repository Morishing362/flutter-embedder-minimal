#include <egl_shell.h>
#include <embedder.h>
#include <flutter_runner.h>
#include <constants.h>

#include <iostream>
#include <string>

void runFlutter(EglShell *egl_shell, std::string flutter_assets_path,
                std::string icudtl_path) {
  FlutterRendererConfig flutter_config = {};
  FlutterProjectArgs args;
  FlutterEngineResult result;
  FlutterEngine engine;
  FlutterWindowMetricsEvent fwme = {.struct_size = sizeof(fwme),
                                    .width = K_DEFAULT_WINDOW_WIDTH,
                                    .height = K_DEFAULT_WINDOW_HIGHT,
                                    .pixel_ratio = 1.0};

  flutter_config.type = kOpenGL;

  flutter_config.open_gl.struct_size = sizeof(flutter_config.open_gl);

  flutter_config.open_gl.make_current = [](void *userdata) -> bool {
    auto _egl = static_cast<EglShell *>(userdata);
    auto ret = eglMakeCurrent(_egl->m_egl_display, _egl->m_egl_surface,
                              _egl->m_egl_surface, _egl->m_egl_context);
    if (ret != EGL_TRUE) {
      std::cout << "eglMakeCurrent failed" << std::endl;
      return false;
    }
    return true;
  };

  flutter_config.open_gl.clear_current = [](void *userdata) -> bool {
    auto _egl = static_cast<EglShell *>(userdata);
    auto ret = eglMakeCurrent(_egl->m_egl_display, EGL_NO_SURFACE,
                              EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (ret != EGL_TRUE) {
      std::cout << "eglMakeCurrent (clear) failed" << std::endl;
      return false;
    }
    return true;
  };

  flutter_config.open_gl.present = [](void *userdata) -> bool {
    auto _egl = static_cast<EglShell *>(userdata);
    auto ret = eglSwapBuffers(_egl->m_egl_display, _egl->m_egl_surface);
    if (ret != EGL_TRUE) {
      std::cout << "eglSwapBuffers failed" << std::endl;
      return false;
    }
    return true;
  };

  flutter_config.open_gl.fbo_callback = [](void *userdata) -> uint32_t {
    auto _egl = static_cast<EglShell *>(userdata);
    return 0;  // FBO0
  };

  flutter_config.open_gl.gl_proc_resolver = [](void *userdata,
                                               const char *name) -> void * {
    auto address = reinterpret_cast<void *>(eglGetProcAddress(name));
    if (address == nullptr) return nullptr;
    return address;
  };

  args = {
      .struct_size = sizeof(FlutterProjectArgs),
      .assets_path = flutter_assets_path.c_str(),
      .icu_data_path =
          icudtl_path.c_str(),  // Find this in your bin/cache directory.
  };

  result = FlutterEngineRun(FLUTTER_ENGINE_VERSION, &flutter_config, &args,
                            egl_shell, &engine);

  if (result != kSuccess || engine == nullptr) {
    std::cout << "Failed to run Flutter engine." << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "Ran Flutter engine." << std::endl;
  }

  result = FlutterEngineSendWindowMetricsEvent(engine, &fwme);

  if (result != kSuccess) {
    std::cout << "Failed to set flutter window metrics." << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "Set flutter window metrics." << std::endl;
    return;
  }
}