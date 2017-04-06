//
// Created by BraxtonN on 1/31/2017.
//

#ifndef SHARP_NATIVEFIELD_H
#define SHARP_NATIVEFIELD_H

enum NativeField
{
    fi8,
    fi16,
    fi32,
    fi64,
    fui8,
    fui16,
    fui32,
    fui64,
    fvar,
    fdynamic, /* Dynamic object type (Not a native type but is treated as one) */
    fvoid, /* only used as a return type for functions */
    fnof /* not a field */
};

#endif //SHARP_NATIVEFIELD_H
