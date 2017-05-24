//
// Created by BraxtonN on 2/24/2017.
//

#ifndef SHARP_MARK_H
#define SHARP_MARK_H

enum gc_mark {
    gc_green=0x0, // allive and well
    gc_orange=0x1, // null but alive
    gc_blue=0x2 // free these
};

#endif //SHARP_MARK_H
