//
// Created by BraxtonN on 3/27/2017.
//

#ifndef SHARP_LIST_H
#define SHARP_LIST_H

#include <algorithm>
#include "../runtime/oo/Exception.h"

template <class T>
class List {
public:
    List(){
        init();
    }

    void init(){
        _Data=NULL;
        len=0;
    }

    void push_back(T& data) {
        __expand();
        _Data[len-1]=data;
    }

    void add(T data) {
        __expand();
        _Data[len-1]=data;
    }

    void push_back() {
        __expand();
    }


    /*
     * Programmer must be responsible
     * for freeing that data himself
     */
    void remove(unsigned long long _X) {
        if(_X>=len || len==0){
            stringstream ss;
            ss << "index out of bounds list::remove() _X: " << _X
               << " length: " << len << endl;
            throw std::runtime_error(ss.str());
        }

        if(len==1){
            free();
        }
        else if(len==2) {
            if(_X==0) {
                _Data[0]=_Data[1];
            }
            __shrink();
        } else {
            if(_X!=(len-1)) {
                for(unsigned long i = _X; i < len-1; i++) {
                    _Data[i] = _Data[i+1];
                }
            }
            __shrink();
        }


    }

    /*
     * Programmer must be responsible
     * for freeing that data himself
     */
    void remove(T& _X) {
        for(unsigned long i = 0; i < len; i++) {
            if(_X == _Data[i]) {
                remove(i);
                return;
            }
        }
    }

    T at(unsigned long long _X) {
        if(_X>=len){
            stringstream ss;
            ss << "index out of bounds list::get() _X: " << _X
               << " length: " << len << endl;
            throw std::runtime_error(ss.str());
        }
        return _Data[_X];
    }

    T& get(long long _X) {
        if(_X>=len || _X < 0){
            stringstream ss;
            ss << "index out of bounds list::get() _X: " << _X
               << " length: " << len << endl;
            throw std::runtime_error(ss.str());
        }
        return _Data[_X];
    }

    void free() {
        if(_Data != NULL)
            delete[] (_Data);
        _Data = NULL;
        len=0;
    }

    CXX11_INLINE
    unsigned long size() { return len; }

    void pop_back() {
        __shrink();
    }

    void addif(T _V) {
        for(unsigned int i = 0; i < len; i++) {
            if(_V == _Data[i])
                return;
        }
        push_back(_V);
    }

    bool check(T _V) {
        for(unsigned int i = 0; i < len; i++) {
            if(_V == _Data[i])
                return true;
        }
        return false;
    }

    void removefirst(T _V) {
        unsigned int iter = -1;
        for(unsigned int i = 0; i < len; i++) {
            if(_V == _Data[i]){
                iter = i;
                break;
            }
        }

        if(iter != -1) {
            remove(iter);
        }
    }

    void addAll(List<T>& list) {
        free();
        for(unsigned int i = 0; i < list.size(); i++) {
            push_back(list.get(i));
        }
    }

    void addAll(list<T> &list) {
        free();
        for(T& t : list) {
            push_back(t);
        }
    }

private:
    CXX11_INLINE
    void __expand() {
        try{
            T* newbuf = new T[len+1];
            std::copy_n(_Data, std::min(len, len+1), newbuf);
            delete[] _Data;
            _Data = newbuf;
            len++;
        } catch(std::exception &e){
            len--;
            throw e;
        }
    }

    CXX11_INLINE
    void __shrink(){
        try {
            if(len-1 <=0) {
                free();
                return;
            }

            T* newbuf = new T[len-1];
            std::copy_n(_Data, std::min(len-1, len-1), newbuf);
            delete[] _Data;
            _Data = newbuf;
            len--;
        } catch(std::exception &e) {
            len++;
            throw e;
        }
    }

    unsigned  long len;
    T* _Data;
};


#endif //SHARP_LIST_H
