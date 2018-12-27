#include "gcrt.h"
#include "sceneselector.h"

/**
 * [WinMain description]
 * @param  hInstance     Process handle
 * @param  hPrevInstance Dummy handle.
 * @param  lpCmdLine     Command line parameters.
 * @param  nShowCmd      How to show the window (maximized, minimized, ...).
 * @return               Error code.
 */

int main(int argc, char** argv)
{
    Launch(argc, argv, GetModuleHandle(NULL));
    return 0;
}