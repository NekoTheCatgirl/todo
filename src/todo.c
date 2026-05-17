#include "todo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "path_helper.h"

#define INITIAL_CAPACITY 4

struct [[maybe_unused]] TodoEntry {
	const char *name;
	const char *description;
	bool completed;
};

struct [[maybe_unused]] TodoList {
	todo_entry_t **entries;
	size_t capacity;
	size_t size;
};

static size_t next_power_of_two(size_t n) {
	if (n == 0) return 2;
	size_t capacity = 2;
	while (capacity < n) capacity <<= 1;
	return capacity;
}

static todo_list_t* todo_list_with_capacity(size_t capacity) {
	todo_list_t *list = malloc(sizeof(todo_list_t));
	if (!list) return nullptr;
	const size_t real_cap = next_power_of_two(capacity);
	list->entries = malloc(real_cap * sizeof(todo_entry_t *));
	if (!list->entries) {
		free(list);
		return nullptr;
	}
	list->capacity = real_cap;
	list->size = 0;
	return list;
}

todo_list_t* todo_list_ctor(void) {
	return todo_list_with_capacity(INITIAL_CAPACITY);
}

todo_list_t* todo_list_load(const char* path) {
	json_object *root = json_object_from_file(path);
	if (!root) {
		fprintf(stderr, "error: failed to parse todolist: %s\n", path);
		return nullptr;
	}

	const size_t count = json_object_array_length(root);
	todo_list_t *list = todo_list_with_capacity(count);
	if (!list) return nullptr;

	for (size_t i = 0; i < count; i++) {
		const json_object *obj = json_object_array_get_idx(root, i);

		json_object *name_obj, *desc_obj, *completed_obj;
		json_object_object_get_ex(obj, "name", &name_obj);
		json_object_object_get_ex(obj, "description", &desc_obj);
		json_object_object_get_ex(obj, "completed", &completed_obj);

		todo_list_add(list, json_object_get_string(name_obj), json_object_get_string(desc_obj));
		list->entries[list->size - 1]->completed = json_object_get_boolean(completed_obj);
	}

	json_object_put(root);
	return list;
}

bool todo_list_save(const todo_list_t* list, const char* path) {
	ensure_parent_dirs(path);
	json_object *root = json_object_new_array();

	for (size_t i = 0; i < list->size; i++) {
		const todo_entry_t *entry = list->entries[i];
		json_object *obj = json_object_new_object();
		{
			json_object_object_add(obj, "name", json_object_new_string(entry->name));
			json_object_object_add(obj, "description", json_object_new_string(entry->description));
			json_object_object_add(obj, "completed", json_object_new_boolean(entry->completed));
		}
		json_object_array_add(root, obj);
	}

	const int rc = json_object_to_file_ext(path, root, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE);
	json_object_put(root);
	return rc == 0;
}

size_t todo_list_size(const todo_list_t* list) {
	return list->size;
}

todo_entry_t* todo_list_index(const todo_list_t* list, const size_t index) {
	if (index >= list->size) return nullptr;
	return list->entries[index];
}

void todo_list_add(todo_list_t* list, const char* name, const char* description) {
	if (list->size == list->capacity) {
		const size_t new_cap = list->capacity ? list->capacity * 2 : INITIAL_CAPACITY;
		const auto new_list = realloc(list->entries, new_cap * sizeof(todo_entry_t *));
		if (new_list == nullptr) return;
		list->entries = new_list;
		list->capacity = new_cap;
	}
	todo_entry_t *entry = malloc(sizeof(todo_entry_t));
	if (entry == nullptr) return;
	entry->name = strdup(name);
	entry->description = strdup(description);
	if (!entry->name || !entry->description) {
		free((char *)entry->name);
		free((char *)entry->description);
		free(entry);
		return;
	}

	entry->completed = false;
	list->entries[list->size++] = entry;
}

void todo_list_remove(todo_list_t* list, const size_t index) {
	if (index >= list->size) return;

	free((char *)list->entries[index]->name);
	free((char *)list->entries[index]->description);
	free(list->entries[index]);

	memmove(
		&list->entries[index],
		&list->entries[index + 1],
		(list->size - index - 1) * sizeof(todo_entry_t *)
	);
	list->size--;

	if (list->size > 0 && list->size <= list->capacity / 4 && list->capacity > INITIAL_CAPACITY) {
		const size_t new_cap = list->capacity / 2;
		const auto new_list = realloc(list->entries, new_cap * sizeof(todo_entry_t *));
		if (new_list == nullptr) return;
		list->entries = new_list;
		list->capacity = new_cap;
	}
}

void todo_list_clear(todo_list_t* list) {
	for (size_t i = 0; i < list->size; i++) {
		if (!list->entries[i]) continue;
		free((char *)list->entries[i]->name);
		free((char *)list->entries[i]->description);
		free(list->entries[i]);
	}
	list->size = 0;
}

void todo_list_toggle(const todo_list_t* list, const size_t index) {
	if (index >= list->size) return;
	list->entries[index]->completed = !list->entries[index]->completed;
}

void todo_list_dtor(todo_list_t* list) {
	if (!list) return;
	todo_list_clear(list);
	free(list->entries);
	free(list);
}

const char* todo_entry_name(const todo_entry_t* entry) {
	return entry->name;
}

const char* todo_entry_description(const todo_entry_t* entry) {
	return entry->description;
}

bool todo_entry_completed(const todo_entry_t* entry) {
	return entry->completed;
}