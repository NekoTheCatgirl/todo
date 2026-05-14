#include <tui.h>

int main(void) {
	const auto list = todo_list_ctor();

	tui_run(list);

	todo_list_dtor(list);
	return 0;
}
