#include "pd_main.h"
#include <string.h>

int main(int ac, char **av)
{
    if (!ac && av)
        return (0);
    csv_read("test.csv", 0);
}