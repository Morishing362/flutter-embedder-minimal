#include <constants.h>
#include <egl_shell.h>
#include <embedder.h>
#include <flutter_runner.h>

#include <cassert>
#include <iostream>
#include <list>
#include <pthread.h>
#include <string>
#include <unordered_map>

// Keeps track of the most recent frame damages so that existing damage can
// be easily computed.
std::list<FlutterRect> damage_history;

// Keeps track of the existing damage associated with each FBO ID
std::unordered_map<intptr_t, FlutterRect *> existing_damage_map;

// Auxiliary function to union the damage regions comprised by two FlutterRect
// element. It saves the result of this join in the rect variable.
static void _JoinFlutterRect(FlutterRect *rect, FlutterRect additional_rect) {
  rect->left = std::min(rect->left, additional_rect.left);
  rect->top = std::min(rect->top, additional_rect.top);
  rect->right = std::max(rect->right, additional_rect.right);
  rect->bottom = std::max(rect->bottom, additional_rect.bottom);
}

void runFlutter(EglShell *egl_shell, std::string flutter_assets_path,
                std::string icudtl_path) {
  FlutterRendererConfig flutter_config = {};
  FlutterProjectArgs args;
  FlutterEngineResult result;
  FlutterEngine engine;
  FlutterWindowMetricsEvent fwme = {.struct_size = sizeof(fwme),
                                    .width = K_DEFAULT_WINDOW_WIDTH,
                                    .height = K_DEFAULT_WINDOW_HIGHT,
                                    .pixel_ratio = K_DEFAULT_PIXEL_RATIO};

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

  flutter_config.open_gl.make_resource_current = [](void *userdata) -> bool {
    auto _egl = static_cast<EglShell *>(userdata);
    auto ret = eglMakeCurrent(_egl->m_egl_display, EGL_NO_SURFACE,
                              EGL_NO_SURFACE, _egl->m_egl_resource_context);

    assert(_egl->m_egl_resource_context == eglGetCurrentContext());
    return ret == EGL_TRUE;
  },

  flutter_config.open_gl.clear_current = [](void *userdata) -> bool {
    auto _egl = static_cast<EglShell *>(userdata);
    auto ret = eglMakeCurrent(_egl->m_egl_display, EGL_NO_SURFACE,
                              EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (ret != EGL_TRUE) {
      std::cout << "eglMakeCurrent (clear) failed" << std::endl;
      return false;
    }
    std::cout << "eglMakeCurrent (clear) called" << std::endl;
    return true;
  };

  flutter_config.open_gl.present = [](void *userdata) -> bool {
    auto _egl = static_cast<EglShell *>(userdata);
    auto ret = eglSwapBuffers(_egl->m_egl_display, _egl->m_egl_surface);
    if (ret != EGL_TRUE) {
      std::cout << "eglSwapBuffers failed" << std::endl;
      return false;
    }
    std::cout << "eglSwapBuffers called" << std::endl;
    return true;
  };

  flutter_config.open_gl.fbo_reset_after_present = false,

  flutter_config.open_gl.fbo_callback = [](void *userdata) -> uint32_t {
    auto _egl = static_cast<EglShell *>(userdata);
    return 0; // FBO0
  };

  flutter_config.open_gl.gl_proc_resolver = [](void *userdata,
                                               const char *name) -> void * {
    auto address = reinterpret_cast<void *>(eglGetProcAddress(name));
    if (address == nullptr) {
      std::cout << "eglProcAddress is null" << std::endl;
      return nullptr;
    }
    return address;
  };

  flutter_config.open_gl.populate_existing_damage =
      [](void *userdata, intptr_t fbo_id,
         FlutterDamage *existing_damage) -> void {
    auto _egl = static_cast<EglShell *>(userdata);

    // Given the FBO age, create existing damage region by joining all frame
    // damages since FBO was last used
    EGLint age;
    eglQuerySurface(_egl->m_egl_display, _egl->m_egl_surface,
                    EGL_BUFFER_AGE_EXT, &age);

    existing_damage->num_rects = 1;

    // Allocate the array of rectangles for the existing damage.
    existing_damage_map[fbo_id] = static_cast<FlutterRect *>(
        malloc(sizeof(FlutterRect) * existing_damage->num_rects));
    existing_damage_map[fbo_id][0] =
        FlutterRect{0, 0, K_DEFAULT_WINDOW_WIDTH, K_DEFAULT_WINDOW_HIGHT};
    existing_damage->damage = existing_damage_map[fbo_id];

    if (age > 1) {
      --age;
      // join up to (age - 1) last rects from damage history
      for (auto i = damage_history.rbegin();
           i != damage_history.rend() && age > 0; ++i, --age) {
        if (i == damage_history.rbegin()) {
          if (i != damage_history.rend()) {
            existing_damage->damage[0] = {i->left, i->top, i->right, i->bottom};
          }
        } else {
          _JoinFlutterRect(&(existing_damage->damage[0]), *i);
        }
      }
    }
  };

  args = {
      .struct_size = sizeof(FlutterProjectArgs),
      .assets_path = flutter_assets_path.c_str(),
      .icu_data_path =
          icudtl_path.c_str(), // Find this in your bin/cache directory.
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
