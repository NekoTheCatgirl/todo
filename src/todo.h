#pragma once

typedef __SIZE_TYPE__ size_t;

typedef struct TodoEntry todo_entry_t;
typedef struct TodoList todo_list_t;

todo_list_t* todo_list_ctor(void);
size_t todo_list_size(const todo_list_t *list);
todo_entry_t* todo_list_index(const todo_list_t *list, size_t index);
void todo_list_add(todo_list_t *list, const char *name, const char *description);
void todo_list_remove(todo_list_t *list, size_t index);
void todo_list_clear(todo_list_t *list);
void todo_list_toggle(const todo_list_t *list, size_t index);
void todo_list_dtor(todo_list_t *list);

const char* todo_entry_name(const todo_entry_t* entry);
const char* todo_entry_description(const todo_entry_t* entry);
bool todo_entry_completed(const todo_entry_t* entry);