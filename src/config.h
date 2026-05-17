#pragma once

typedef struct ConfigData config_t;

/**
 * If config is nullptr, then it's an unsuccessful result. Otherwise, it's a successful result.
 */
typedef struct {
	const char *error_message;
	config_t *config;
} config_load_result_t;

config_t *config_ctor(void);
config_load_result_t config_load(const char *path);
bool config_save(const config_t *config, const char *path);
void config_set_todo_path(config_t *config, const char *path);
const char *config_get_todo_path(const config_t *config);
void config_print(const config_t *config);
void config_dtor(config_t *config);