#include <stdlib.h>
#include <time.h>

#include "low.h"
#include "clock.h"
#include "math3d.h"

#include "bumpmain.h"

int main(void)
{
    unsigned char* buffer;
    
    if ((buffer = (unsigned char *)malloc(64000)) == NULL) {
        return 1;
    }

    srand(time(NULL));

    compute_lookup_tables();
    set_mode13h();

    clock_init();

    do_bump_mapping(buffer);

    unset_mode13h();
    clock_shutdown();

    free(buffer);

    return 0;
}
