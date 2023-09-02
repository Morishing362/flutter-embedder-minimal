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
  struct xdg_wm_base *m_xdg_wm_base;
  struct xdg_surface *m_xdg_surface;
  // struct wl_shell *m_wl_shell;
  // struct wl_shell_surface *m_wl_shell_suerface;

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

  if (strcmp(interface, "xdg_wm_base") == 0) {
    wayland_display->m_xdg_wm_base = static_cast<struct xdg_wm_base *>(
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

static void xdgSurfaceConfigure(void *data, struct xdg_surface *xdg_surface,
                                uint32_t serial) {
  xdg_surface_ack_configure(xdg_surface, serial);
}

static const struct xdg_surface_listener xdgSurfaceListener = {
    .configure = xdgSurfaceConfigure,
};

#endif // WAYLAND_DISPLAY_H
