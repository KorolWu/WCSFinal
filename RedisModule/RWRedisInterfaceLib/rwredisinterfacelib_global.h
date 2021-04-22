#ifndef RWREDISINTERFACELIB_GLOBAL_H
#define RWREDISINTERFACELIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RWREDISINTERFACELIB_LIBRARY)
#  define RWREDISINTERFACELIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define RWREDISINTERFACELIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // RWREDISINTERFACELIB_GLOBAL_H
