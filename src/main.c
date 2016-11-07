#include <stdio.h>
#include "config.h"

int main(int argc, char **argv){
    printf("%s: Version %d.%d\n", GBEmu_NAME, GBEmu_VERSION_MAJOR, GBEmu_VERSION_MINOR);
    return 0;
}