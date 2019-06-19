#ifndef V2_MSQLITECPP_H
#define V2_MSQLITECPP_H

#ifdef _WIN32
#ifdef BUILD_MSQLITECPP
#define EXPORT_MSQLITEV2 __declspec(dllexport)
#else
#ifdef EMBED_MSQLITECPP
#define EXPORT_MSQLITEV2
#else
#define EXPORT_MSQLITEV2 __declspec(dllimport)
#endif
#endif
#else
#define EXPORT_MSQLITEV2
#endif

#endif // V2_MSQLITECPP_H
