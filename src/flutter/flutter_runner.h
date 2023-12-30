#ifndef FLUTTER_RUNNER_H
#define FLUTTER_RUNNER_H

#include <constants.h>
#include <egl_shell.h>
#include <embedder.h>

#include <string>

void runFlutter(EglShell *egl_shell, std::string bundle_path);

#endif // FLUTTER_RUNNER_H
