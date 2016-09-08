#pragma once


#define FORBID_ASSIGN(ClassName) \
    ClassName& operator = (const ClassName&);

#define FORBID_COPY(ClassName) \
    ClassName(const ClassName&);

#define FORBID_COPY_ASSIGN(ClassName) FORBID_COPY(ClassName) FORBID_ASSIGN(ClassName)
