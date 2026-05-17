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

	const int width = 50;
	const int height = 8;
	const int start_y = (LINES - height) / 2;
	const int start_x = (COLS - width) / 2;

	WINDOW *win = newwin(height, width, start_y, start_x);
	keypad(win, TRUE);
	box(win, 0, 0);
	mvwprintw(win, 0, (width - 13) / 2, " New Entry ");
	mvwprintw(win, height - 1, (width - 15) / 2, " ESC to cancel ");
	wrefresh(win);

	echo();
	curs_set(1);

	mvwprintw(win, 2, 2, "Name: ");
	wrefresh(win);
	int ch;
	while (true) {
		ch = wgetch(win);
		if (ch == 27) { // ESC
			delwin(win);
			noecho();
			curs_set(0);
			return;
		}
		if (ch == '\n') break;
		if (ch == KEY_BACKSPACE || ch == 127 || ch == KEY_LEFT) {
			if (strlen(name) > 0) {
				name[strlen(name) - 1] = '\0';
				mvwprintw(win, 2, 8, "%s ", name);
				wmove(win, 2, 8 + (int)strlen(name));
			}
		} else if (ch >= 32 && ch <= 126 && strlen(name) < sizeof(name) - 1) {
			size_t len = strlen(name);
			name[len] = (char)ch;
			name[len + 1] = '\0';
			mvwprintw(win, 2, 8, "%s", name);
		}
		wrefresh(win);
	}

	if (strlen(name) == 0) {
		delwin(win);
		noecho();
		curs_set(0);
		return;
	}

	mvwprintw(win, 4, 2, "Description: ");
	wrefresh(win);
	while (true) {
		ch = wgetch(win);
		if (ch == 27) { // ESC
			delwin(win);
			noecho();
			curs_set(0);
			return;
		}
		if (ch == '\n') break;
		if (ch == KEY_BACKSPACE || ch == 127 || ch == KEY_LEFT) {
			if (strlen(description) > 0) {
				description[strlen(description) - 1] = '\0';
				mvwprintw(win, 4, 15, "%s ", description);
				wmove(win, 4, 15 + (int)strlen(description));
			}
		} else if (ch >= 32 && ch <= 126 && strlen(description) < sizeof(description) - 1) {
			size_t len = strlen(description);
			description[len] = (char)ch;
			description[len + 1] = '\0';
			mvwprintw(win, 4, 15, "%s", description);
		}
		wrefresh(win);
	}

	noecho();
	curs_set(0);
	delwin(win);

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

			case 'd': {
				if (todo_list_size(list) == 0) break;
				const auto entry = todo_list_index(list, selected);
				const char *name = todo_entry_name(entry);

				const int width = (int)strlen(name) + 40;
				const int height = 5;
				const int start_y = (LINES - height) / 2;
				const int start_x = (COLS - width) / 2;

				WINDOW *win = newwin(height, width, start_y, start_x);
				box(win, 0, 0);
				mvwprintw(win, 2, 2, "Are you sure you want to delete %s? y/n", name);
				wrefresh(win);

				int confirm = wgetch(win);
				if (confirm == 'y' || confirm == 'Y') {
					todo_list_remove(list, selected);
					if (selected > 0 && selected >= todo_list_size(list)) selected--;
				}

				delwin(win);
				break;
			}

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
