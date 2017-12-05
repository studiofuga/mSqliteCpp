#ifndef MSQLITECPP_H
#define MSQLITECPP_H

#ifdef _WIN32
#ifdef BUILD_MSQLITECPP
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif
#else
#define EXPORT
#endif

#endif // MSQLITECPP_H
