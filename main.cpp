#include "Live.h"

/**
 * Main function that is called when the program starts.
 * Here an instance of the MyMoblet class is created and
 * the program enters the main event loop.
 */
extern "C" int MAMain() {
    (new Live())->enterEventLoop();
    return 0;
}
