#ifndef FLUTTER_RUNNER_H
#define FLUTTER_RUNNER_H

#include <egl_shell.h>

#include <string>

void runFlutter(EglShell *egl_shell, std::string flutter_assets_path,
                std::string icudtl_path);

#endif  // FLUTTER_RUNNER_H
