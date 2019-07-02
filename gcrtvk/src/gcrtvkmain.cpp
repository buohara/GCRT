#include "gcrtvk.h"
#include "scenes.h"

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
    LaunchBasicScene(GetModuleHandle(NULL));
    return 0;
}