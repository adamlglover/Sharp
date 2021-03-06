//
// Created by BraxtonN on 2/2/2018.
//

#ifndef SHARP_ACCESSMODIFIER_H
#define SHARP_ACCESSMODIFIER_H

enum AccessModifier
{
    /*
     * Class, Interface, Field, and Method access modifiers
     */
    PUBLIC=0,
    PRIVATE=1,
    PROTECTED=2,

    /*
     * Field access modifiers
     */
    mCONST=3,
    STATIC=4, /* Methods can use this modifier */

    mUNDEFINED=6
};

#endif //SHARP_ACCESSMODIFIER_H
