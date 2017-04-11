//
// Created by BraxtonN on 1/31/2017.
//

#ifndef SHARP_NATIVEFIELD_H
#define SHARP_NATIVEFIELD_H

enum NativeField
{
    fi8=0,
    fi16=1,
    fi32=2,
    fi64=3,
    fui8=4,
    fui16=5,
    fui32=6,
    fui64=7,
    fvar=8,
    fdynamic=9, /* Dynamic object type (Not a native type but is treated as one) */
    fvoid=10, /* only used as a return type for functions */
    fnof=11 /* not a field */
};

#endif //SHARP_NATIVEFIELD_H
