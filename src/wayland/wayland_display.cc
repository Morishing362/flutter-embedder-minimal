#include "xdg-shell-client-protocol.h"
#include <wayland_display.h>

#include <iostream>
#include <string>

// Wayland display constructor
WaylandDisplay::WaylandDisplay()
    : m_wl_display(nullptr), m_wl_surface(nullptr), m_wl_compositor(nullptr),
      m_xdg_wm_base(nullptr), m_xdg_surface(nullptr) {
  m_wl_display = wl_display_connect(nullptr);

  if (!m_wl_display) {
    std::cout << "Failed to connect display" << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "Connected to display" << std::endl;
  }
}

WaylandDisplay::~WaylandDisplay() {
  wl_display_disconnect(m_wl_display);

  std::cout << "Disconnected from display" << std::endl;
}

void WaylandDisplay::dispatchRegistryListener() {
  struct wl_registry *registry = wl_display_get_registry(m_wl_display);
  wl_registry_add_listener(registry, &registry_listener, this);

  // Sync with backend to get registry information & compositor
  wl_display_dispatch(m_wl_display);
  // Block until all pending request are processed by the server
  wl_display_roundtrip(m_wl_display);
}

void WaylandDisplay::createSurface() {
  m_wl_surface = wl_compositor_create_surface(m_wl_compositor);

  if (m_wl_surface == nullptr) {
    std::cout << "Failed to create a surface" << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "Created surface" << std::endl;
  }
}

void WaylandDisplay::setSurface() {
  m_xdg_surface= xdg_wm_base_get_xdg_surface(m_xdg_wm_base, m_wl_surface);
  if (m_xdg_surface == nullptr) {
    std::cout << "Failed to create xdg surface" << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "Created xdg surface" << std::endl;
  }

  xdg_surface_add_listener(m_xdg_surface, &xdgSurfaceListener, nullptr);

  xdg_toplevel_set_title(xdg_surface_get_toplevel(m_xdg_surface), "XDG Sample");
}
