#include "config.h"
#include "path_helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json-c/json.h"

struct [[maybe_unused]] ConfigData {
	const char *todo_list_path;
};

config_t *config_ctor(void) {
	config_t *config = malloc(sizeof(config_t));
	if (!config) return nullptr;
	config->todo_list_path = get_default_todo_path();
	return config;
}

config_load_result_t config_load(const char* path) {
	json_object *root = json_object_from_file(path);
	if (!root) {
		return (config_load_result_t){
			.error_message = "failed to parse config file",
			.config = nullptr,
		};
	}

	config_t *config = config_ctor();
	if (!config) {
		json_object_put(root);
		return (config_load_result_t){
			.error_message = "failed to create config object",
			.config = nullptr,
		};
	}

	json_object *todo_path_obj;
	if (json_object_object_get_ex(root, "todo_list_path", &todo_path_obj)) {
		config->todo_list_path = strdup(json_object_get_string(todo_path_obj));
	}

	json_object_put(root);
	return (config_load_result_t){
		.error_message = nullptr,
		.config = config,
	};
}

bool config_save(const config_t *config, const char *path) {
	ensure_parent_dirs(path);
	json_object *root = json_object_new_object();

	json_object_object_add(root, "todo_list_path",
		json_object_new_string(config->todo_list_path)
	);

	const int rc = json_object_to_file_ext(path, root, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE);
	json_object_put(root);

	return rc == 0;
}

void config_set_todo_path(config_t* config, const char* path) {
	free((char*)config->todo_list_path);
	config->todo_list_path = strdup(path);
}

const char* config_get_todo_path(const config_t* config) {
	return config->todo_list_path;
}

void config_print(const config_t* config) {
    printf("todo_list_path: %s\n", config->todo_list_path ? config->todo_list_path : "(not set)");
}

void config_dtor(config_t* config) {
	if (!config) return;
	free((char*)config->todo_list_path);
	free(config);
}

