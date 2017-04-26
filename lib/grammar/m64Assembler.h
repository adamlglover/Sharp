//
// Created by BraxtonN on 4/4/2017.
//

#ifndef SHARP_M64ASSEMBLER_H
#define SHARP_M64ASSEMBLER_H


#include <cstdint>
#include "../../stdimports.h"
#include "../util/keypair.h"
#include "../util/List2.h"

class m64Assembler {
public:
    m64Assembler() {
        __asm64.init();
        injectors.key.init();
        injectors.value.init();
    }

    void init() {
        injectors.key.init();
        injectors.value.init();
    }

    bool addinjector(string key) {
        if(!has_injector(key)) {
            injectors.key.push_back(key);
            injectors.value.push_back();
            injectors.value.get(injectors.value.size()-1).init();
            return true;
        }
        return false;
    }

    void addinjector_unsafe(std::string key) {
        injectors.key.push_back(key);
        injectors.value.push_back();
        injectors.value.last().init();
    }

    bool add(string injector, int64_t i64) {
        if(has_injector(injector)) {
            get_injector(injector).__asm64.push_back(i64);
            return true;
        }
        return false;
    }

    void push_i64(int64_t i64){
        __asm64.push_back(i64);
    }
    void push_i64(int64_t i64, int64_t xtra){
        __asm64.push_back(i64);
        __asm64.push_back(xtra);
    }

    List<int64_t> __asm64;

    void free() {
        __asm64.free();
        injectors.key.free();

        for(unsigned int i = 0; i < injectors.value.size(); i++) {
            injectors.value.at(i).free();
        }
        injectors.value.free();
    }


    keypair<List<string>, List<m64Assembler>> injectors;

    void inject(int64_t &i, m64Assembler &assembler) {
        if(i < 0) return;

        int64_t start = i, iter = 0;
        for(;;) {
            if(iter >= assembler.__asm64.size())
                break;

            __asm64.insert(start, assembler.__asm64.get(iter++));
            start++;
        }
    }

private:


    m64Assembler& get_injector(string key) {
        for(unsigned int i = 0; i < injectors.key.size(); i++) {
            if(injectors.key.at(i) == key)
                return injectors.value.get(i);
        }
        return injectors.value.get(0);
    }

    bool has_injector(string key) {
        for(unsigned int i = 0; i < injectors.key.size(); i++) {
            if(injectors.key.at(i) == key)
                return true;
        }
        return false;
    }
};


#endif //SHARP_M64ASSEMBLER_H
