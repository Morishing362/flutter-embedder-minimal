#include <flutter_runner.h>
#include <wayland_display.h>

#include <iostream>
#include <memory>

int main(int argc, char const *argv[]) {
  if (argc < 3) {
    std::cerr << "Error: No Flutter data path provided." << std::endl;
    std::cerr << "Usage: " << argv[0]
              << " [flutter_assets path] [icudtl.dat path]" << std::endl;
    exit(EXIT_SUCCESS);
  }

  auto wayland_display = std::make_shared<WaylandDisplay>();

  wayland_display->dispatchRegistryListener();
  wayland_display->createSurface();
  wayland_display->createXdgSurface();

  auto egl_shell = EglShell(wayland_display->display);
  egl_shell.createEglWindow(wayland_display->surface);

  runFlutter(&egl_shell, argv[1], argv[2]);

  while (true) {
    // thead running...
  }

  return EXIT_SUCCESS;
}
