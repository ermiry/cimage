
#if defined(_WIN32)
    #define PLATAFORM_WINDOWS
#elif defined(__APPLE__)
    #define PLATAFORM_MAC
#else
    #define LINUX
#endif