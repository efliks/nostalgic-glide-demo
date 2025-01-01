
#include "sys.h"
#include "math3d.h"
#include "bumpmain.h"

int main()
{
    int retval;

    //TODO Process command line
    compute_lookup_tables();
    clock_init();

    retval = do_bump_mapping();

    clock_shutdown();

    return (retval == 1) ? 0 : 1;
}
