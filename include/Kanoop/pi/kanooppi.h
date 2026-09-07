#ifndef KANOOPPI_H
#define KANOOPPI_H

#include <QtGlobal>

#if defined(KANOOPPI_LIBRARY)
#  define KANOOPPI_EXPORT Q_DECL_EXPORT
#else
#  define KANOOPPI_EXPORT Q_DECL_IMPORT
#endif

#endif // KANOOPPI_H
