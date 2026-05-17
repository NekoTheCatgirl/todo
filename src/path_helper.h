#pragma once

const char* get_home_path(void);
const char* get_config_path(void);
const char* get_default_todo_path(void);

void ensure_parent_dirs(const char *path);