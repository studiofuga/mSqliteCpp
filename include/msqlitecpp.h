#ifndef MSQLITECPP_H
#define MSQLITECPP_H

#ifdef _WIN32
    #ifdef BUILD_MSQLITECPP
        #define EXPORT __declspec(dllexport)
    #else
        #ifdef EMBED_MSQLITECPP
            #define EXPORT
        #else
            #define EXPORT __declspec(dllimport)
        #endif
    #endif
#else
    #define EXPORT
#endif

#endif // MSQLITECPP_H
