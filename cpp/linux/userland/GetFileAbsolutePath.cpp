#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// Determine whether the file is a relative path
bool IsAbsolutePath(const char* path) {
  if (path == NULL) {
    return false;
  }
  if (path[0] == '/') {
    return true;
  }
  if (path[0] == '\\') {
    return true;
  }
  if (path[1] == ':') {
    return true;
  }
  return false;
}


// Convert relative path to absolute path in linux
bool GetFileAbsolutePath(const char* relative_path, char* absolute_path) {
  if (relative_path == NULL) {
    return false;
  }
  if (absolute_path == NULL) {
    return false;
  }
  if (IsAbsolutePath(relative_path)) {
    strcpy(absolute_path, relative_path);
    return true;
  }
  char current_path[512];
  if (getcwd(current_path, 512) == NULL) {
    return false;
  }
  snprintf(absolute_path, 512, "%s/%s", current_path, relative_path);
  return true;
}
