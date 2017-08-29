#ifndef UV_SSIZE_T_H
#define UV_SSIZE_T_H

#include <stdlib.h> // ssize_t

#if defined(_SSIZE_T_) || defined(_SSIZE_T_DEFINED)
typedef ssize_t uv_ssize_t;
#elif defined(_WIN32)
typedef intptr_t uv_ssize_t;
#else
typedef ssize_t uv_ssize_t;
#endif

#endif // UV_SSIZE_T_H
