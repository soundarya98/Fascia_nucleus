#ifndef PACKAGE
#define PACKAGE

#define PACKAGE_LENGTH_BYTES 63 // rude estimation: 1(package num) + 3(24 bit integer) * 16(num channels) + 8(timestamp) + 6 accel
// TODO: GENERATE 24 BIT INTEGERS FOR EEG/EMG INSTEAD RANDOM BYTES

class Package {
public:
    static void get_package (unsigned char *package);
    static unsigned char package_num;
};

#endif
