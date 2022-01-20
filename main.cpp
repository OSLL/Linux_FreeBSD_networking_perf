#include "cli/cli_main.h"
#include "gui/gui_main.h"

int main(int argc, char *argv[]) {

    if (argc > 1) {
        cli_main(argc, argv);
    } else {
        gui_main(argc, argv);
    }
    return 0;
}
