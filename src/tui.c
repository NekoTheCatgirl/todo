#include "tui.h"
#include <string.h>
#include "todo.h"

#ifdef _WIN32
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#define STATUS_BAR "  [k/Up] Up  [j/Down] Down  [enter] Toggle  [a] Add  [d] Delete  [q] Quit"

static void draw(const todo_list_t *list, const size_t selected) {
	clear();
	box(stdscr, 0, 0);

	const int inner_width = COLS - 2;
	const char *title = "Todo List";
	const int title_col = 1 + (inner_width - (int)strlen(title)) / 2;
	mvprintw(1, title_col, "%s", title);

	mvhline(2, 1, ACS_HLINE, COLS - 2);
	mvaddch(2, 0,        ACS_LTEE);
	mvaddch(2, COLS - 1, ACS_RTEE);

	for (size_t i = 0; i < todo_list_size(list); ++i) {
		const auto entry = todo_list_index(list, i);

		if (i == selected) attron(A_REVERSE);

		mvprintw((int)i + 3, 2, "[%c] %s - %s",
			todo_entry_completed(entry) ? 'x' : ' ',
			todo_entry_name(entry),
			todo_entry_description(entry)
		);

		if (i == selected) attroff(A_REVERSE);
	}

	mvprintw(LINES - 2, 2, STATUS_BAR);
	refresh();
}

static bool read_string(const int row, const int col, const char *prompt, char *buf, const size_t max_len) {
	echo();
	curs_set(1);

	mvprintw(row, col, "%s", prompt);
	clrtoeol();
	refresh();

	size_t i = 0;
	while (i < max_len - 1) {
		int ch = getch();
		if (ch == 27) {
			noecho();
			curs_set(0);
			return false;
		}
		if (ch == '\n') break;
		if ((ch == KEY_BACKSPACE || ch == 127) && i > 0) {
			buf[--i] = '\0';
			mvprintw(row, col + (int)strlen(prompt), "%s ", buf);
			move(row, col + (int)strlen(prompt) + (int)i);
			continue;
		}
		if (ch >= 32 && ch <= 126) {
			buf[i++] = (char)ch;
			buf[i] = '\0';
		}
	}

	noecho();
	curs_set(0);
	return true;
}

static void add_entry(todo_list_t *list) {
	char name[256]        = {0};
	char description[256] = {0};

	clear();
	mvprintw(0, 0, "=== New Entry === (ESC to cancel)");

	if (!read_string(2, 0, "Name:        ", name, sizeof(name)))        return;
	if (!read_string(3, 0, "Description: ", description, sizeof(description))) return;

	todo_list_add(list, name, description);
}

void tui_run(todo_list_t* list) {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	size_t selected = 0;

	while (true) {
		draw(list, selected);

		switch (getch()) {
			case 'k': case KEY_UP:
				if (selected > 0) selected--;
				break;

			case 'j': case KEY_DOWN:
				if (selected < todo_list_size(list) - 1) selected++;
				break;

			case '\n':
				todo_list_toggle(list, selected);
				break;

			case 'd':
				todo_list_remove(list, selected);
				if (selected > 0 && selected >= todo_list_size(list)) selected--;
				break;

			case 'a':
				add_entry(list);

				if (todo_list_size(list) > 0 && selected >= todo_list_size(list))
					selected = todo_list_size(list) - 1;
				break;

			case 'q':
				endwin();
				return;

			default:
				break;
		}
	}
}
