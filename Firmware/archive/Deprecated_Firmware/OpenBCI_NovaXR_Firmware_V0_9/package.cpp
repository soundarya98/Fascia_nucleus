#include "package.h"
#include "Arduino.h"


unsigned char Package::package_num = 0;

void Package::get_package (unsigned char *package)
{
    package[0] = package_num++;
    int i;
    for (i = 1; i < PACKAGE_LENGTH_BYTES; i++)
    {
        package[i + 1] = (unsigned char) random (0, 255);
    } 
}
