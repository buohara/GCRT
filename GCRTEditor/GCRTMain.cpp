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

int CALLBACK WinMain(
    __in HINSTANCE hInstance,
    __in HINSTANCE hPrevInstance, 
    __in LPSTR lpCmdLine,
    __in int nShowCmd
)
{
    LPWSTR *argv;
    int argc;
    argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    char name[256];
    wcstombs(name, argv[1], 256);
    Launch(string(name), hInstance);

    return 0;
}