#include <egl_shell.h>
#include <flutter_runner.h>
#include <wayland_display.h>

#include <iostream>
#include <memory>

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    std::cerr << "Error: No Flutter data path provided." << std::endl;
    std::cerr << "Usage: " << argv[0]
              << " <bundle path>" << std::endl;
    exit(EXIT_SUCCESS);
  }

  auto wayland_display = std::make_shared<WaylandDisplay>();

  wayland_display->dispatchRegistryListener();
  wayland_display->createSurface();
  wayland_display->setSurface();

  auto egl_shell = EglShell(wayland_display->m_wl_display);

  egl_shell.createEglWindow(wayland_display->m_wl_surface);

  runFlutter(&egl_shell, argv[1]);

  while (true) {
    // thead running...
  }

  return EXIT_SUCCESS;
}
