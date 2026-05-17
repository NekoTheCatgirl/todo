#include <argparse.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "path_helper.h"
#include "todo.h"
#include "tui.h"

int main(const int argc, const char** argv) {
	const char *list_path = nullptr;
	bool use_default_config = 0;

	static const char *const usage[] = {
		"todo [options]",
		nullptr,
	};

	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_STRING('l', "list", &list_path, "Path to the todo list file"),
		OPT_BOOLEAN('n', "no-default", &use_default_config, "Disallow default config if config couldnt be loaded."),
		OPT_END()
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usage, 0);
	argparse_describe(&argparse,
		"\nA stupidly simple TUI todolist.",
		"±nIf no list is specified, a default list path from config is used."
	);
	argparse_parse(&argparse, argc, argv);
	const auto config_path = get_config_path();
	const auto result = config_load(config_path);

	auto config = result.config;

	if (!result.config) {
		if (!use_default_config) {
			fprintf(stderr, "warning: config failed to load (%s), falling back to defaults\n", result.error_message);
			config = config_ctor();
			if (!config_save(config, get_config_path())) {
				fprintf(stderr, "warning: could not save default config to '%s'\n", get_config_path());
			}
		} else {
			fprintf(stderr, "error: failed to load config: %s\n", result.error_message);
			fprintf(stderr, "tip: run with -d to fall back to built-in defaults if config fails\n");
			exit(1);
		}
	}

	const char *todo_path = list_path ? list_path : config_get_todo_path(config);
	todo_list_t *list = todo_list_load(todo_path);

	if (!list) {
		fprintf(stderr, "warning: failed to load todo list from '%s', starting fresh\n", todo_path);
		list = todo_list_ctor();
		todo_list_save(list, todo_path);
	}

	tui_run(list);

	todo_list_save(list, todo_path);
	todo_list_dtor(list);
	config_dtor(config);
	return 0;
}
