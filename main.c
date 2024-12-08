
#include "clock.h"
#include "math3d.h"
#include "bumpmain.h"

int main()
{
    //TODO Process command line
    compute_lookup_tables();
    clock_init();

    do_bump_mapping();

    clock_shutdown();

    return 0;
}
