#include <string>
#include <wayland_display.h>

#include <iostream>


// Wayland display constructor
WaylandDisplay::WaylandDisplay()
    : display(nullptr),
      surface(nullptr),
      compositor(nullptr),
      m_xdg_wm_base(nullptr),
      m_xdg_surface(nullptr) {
  display = wl_display_connect(nullptr);

  if (!display) {
    std::cout << "Failed to connect display" << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "Connected to display" << std::endl;
  }
}

WaylandDisplay::~WaylandDisplay() {
  wl_display_disconnect(display);

  std::cout << "Disconnected from display" << std::endl;
}

void WaylandDisplay::dispatchRegistryListener() {
  struct wl_registry *registry = wl_display_get_registry(display);
  wl_registry_add_listener(registry, &registry_listener, this);

  // Sync with backend to get registry information & compositor
  wl_display_dispatch(display);
  // Block until all pending request are processed by the server
  wl_display_roundtrip(display);
}

void WaylandDisplay::createSurface() {
  surface = wl_compositor_create_surface(compositor);

  if (surface == nullptr) {
    std::cout << "Failed to create a surface" << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "Created surface" << std::endl;
  }
}

void WaylandDisplay::createXdgSurface() {
  m_xdg_surface = xdg_wm_base_get_xdg_surface(m_xdg_wm_base, surface);

  if (m_xdg_surface == nullptr) {
    std::cout << "Failed to create XDG surface" << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "Created XDG surface" << std::endl;
  }

  xdg_surface_add_listener(m_xdg_surface, &xdgSurfaceListener, nullptr);

  xdg_toplevel_set_title(xdg_surface_get_toplevel(m_xdg_surface), "EGL Sample");
}

