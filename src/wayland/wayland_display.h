#ifndef WAYLAND_DISPLAY_H
#define WAYLAND_DISPLAY_H

#include <string.h>
#include <wayland-client.h>
#include <xdg-shell-client-protocol.h>

#include <iostream>

class WaylandDisplay {
 public:
  struct wl_compositor *m_wl_compositor;
  struct wl_display *m_wl_display;
  struct wl_surface *m_wl_surface;
  struct wl_shell *m_wl_shell;
  struct wl_shell_surface *m_wl_shell_suerface;

  WaylandDisplay();

  ~WaylandDisplay();

  void dispatchRegistryListener();

  void createSurface();

  void setSurface();
};

static void globalRegistryHandler(void *data, struct wl_registry *registry,
                                  uint32_t id, const char *interface,
                                  uint32_t version) {
  std::cout << "Got a registry event for " << interface << " id " << id
            << std::endl;
  auto wayland_display = static_cast<WaylandDisplay *>(data);

  if (strcmp(interface, "wl_compositor") == 0) {
    wayland_display->m_wl_compositor = (wl_compositor *)wl_registry_bind(
        registry, id, &wl_compositor_interface, version);
  }

  if (strcmp(interface, "wl_shell") == 0) {
    wayland_display->m_wl_shell = static_cast<struct wl_shell *>(
        wl_registry_bind(registry, id, &xdg_wm_base_interface, version));
  }
}

static void globalRegistryRemover(void *data, struct wl_registry *registry,
                                  uint32_t id) {
  std::cout << "Got a registry losing event for " << id << std::endl;
}

static const struct wl_registry_listener registry_listener = {
    globalRegistryHandler,
    globalRegistryRemover,
};

#endif  // WAYLAND_DISPLAY_H
