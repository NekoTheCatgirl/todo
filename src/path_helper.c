#include "path_helper.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define mkdir_single(path) _mkdir(path)
#define PATH_SEP "\\"
#else
#include <sys/stat.h>
#define mkdir_single(path) mkdir(path, 0755)
#define PATH_SEP "/"
#endif


const char *get_home_path(void) {
#ifdef _WIN32
	const char *home = getenv("USERPROFILE");
	if (!home) home = getenv("HOMEPATH");
#else
	const char *home = getenv("HOME");
#endif
	return home;
}

const char *get_config_path(void) {
	static char path[4096];

#ifdef _WIN32
	const char *appdata = getenv("APPDATA");
	if (!appdata) appdata = get_home_path();
	snprintf(path, sizeof(path), "%s%stodo%sconfig.json", appdata, PATH_SEP, PATH_SEP);
#else
	const char *xdg = getenv("XDG_CONFIG_HOME");
	if (xdg) {
		snprintf(path, sizeof(path), "%s%stodo%sconfig.json", xdg, PATH_SEP, PATH_SEP);
	} else {
		const char *home = get_home_path();
		snprintf(path, sizeof(path), "%s%s.config%stodo%sconfig.json", home, PATH_SEP, PATH_SEP, PATH_SEP);
	}
#endif

	return path;
}

const char *get_default_todo_path(void) {
	static char path[4096];

#ifdef _WIN32
	const char *appdata = getenv("APPDATA");
	if (!appdata) appdata = get_home_path();
	snprintf(path, sizeof(path), "%s%stodo%stodo.json", appdata, PATH_SEP, PATH_SEP);
#else
	const char *xdg = getenv("XDG_DATA_HOME");
	if (xdg) {
		snprintf(path, sizeof(path), "%s%stodo%stodo.json", xdg, PATH_SEP, PATH_SEP);
	} else {
		const char *home = get_home_path();
		snprintf(path, sizeof(path), "%s%s.local%sshare%stodo%stodo.json", home, PATH_SEP, PATH_SEP, PATH_SEP, PATH_SEP);
	}
#endif

	return path;
}

void ensure_parent_dirs(const char *path) {
	char tmp[4096];
	strncpy(tmp, path, sizeof(tmp));
	tmp[sizeof(tmp) - 1] = '\0';

	for (char *p = tmp + 1; *p; p++) {
		if (*p == PATH_SEP[0]) {
			*p = '\0';
			mkdir_single(tmp); // ignore errors, dir may already exist
			*p = PATH_SEP[0];
		}
	}
}
