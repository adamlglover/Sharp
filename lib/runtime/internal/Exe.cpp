//
// Created by bknun on 2/18/2017.
//

#include "Exe.h"
#include "../../util/file.h"
#include "../startup.h"
#include "Environment.h"
#include "../oo/Field.h"
#include "../interp/vm.h"
#include "../oo/Object.h"
#include "../interp/Opcode.h"

Manifest manifest;
Meta meta;

uint64_t n = 0, jobIndx=0;

bool checkFile(file::stream& exe);

nString getstring(file::stream& exe);

int64_t getlong(file::stream& exe);

void getField(file::stream& exe, list <MetaField>& mFields, Field* field);

void getMethod(file::stream& exe, ClassObject *parent, Method* method);

ClassObject *findClass(int64_t superClass);

bool overflowOp(int op) ;

int64_t getmi64(file::stream& exe) ;

int Process_Exe(std::string exe)
{
    file::stream _fStream;
    int __bitFlag, hdr_cnt=0;

    jobIndx++;
    if(!file::exists(exe.c_str())){
        error("file `" + exe + "` doesnt exist!");
    }

    manifest.executable.init();
    manifest.executable = exe;
    file::read_alltext(exe.c_str(), _fStream);
    if(_fStream.empty())
        return 1;

    try {
        if(!checkFile(_fStream)) {
            error("file `" + exe + "` could not be ran");
        }

        jobIndx++;
        bool manifestFlag = false;
        for (;;) {

            hdr_cnt++;
            __bitFlag = _fStream.at(n++);
            switch (__bitFlag) {
                case 0x0:
                case 0x0a:
                case 0x0d:
                case eoh:
                    hdr_cnt--;
                    break;

                case manif:
                    hdr_cnt--;
                    manifestFlag = true;
                    break;

                case 0x2:
                    manifest.application.init();
                    manifest.application =getstring(_fStream);
                    break;
                case 0x4:
                    manifest.version.init();
                    manifest.version =getstring(_fStream);
                    break;
                case 0x5:
                    manifest.debug = _fStream.at(n++) != nil;
                    break;
                case 0x6:
                    manifest.entry =getmi64(_fStream);
                    break;
                case 0x7:
                    manifest.addresses =getmi64(_fStream);
                    break;
                case 0x8:
                    manifest.classes =getmi64(_fStream);
                    break;
                case 0x9:
                    manifest.fvers =getlong(_fStream);
                    break;
                case 0x0c:
                    manifest.strings =getmi64(_fStream);
                    break;
                case 0x0e:
                    manifest.target =getlong(_fStream);
                    break;
                default:
                    throw std::runtime_error("file `" + exe + "` may be corrupt");
            }

            if(__bitFlag == eoh) {
                if(!manifestFlag)
                    throw std::runtime_error("missing manifest flag");
                if(manifest.fvers != 1)
                    throw std::runtime_error("unknown file version");

                if(hdr_cnt != hsz || manifest.target > mvers)
                    return 1;

                break;
            }
        }

        if(_fStream.at(n++) != sdata)
            throw std::runtime_error("file `" + exe + "` may be corrupt");

        /* Data section */
        list<MetaClass> mClasses;
        list<MetaField> mFields;
        int64_t classRefptr=0, macroRefptr=0;

        env->classes =(ClassObject*)malloc(sizeof(ClassObject)*manifest.classes);
        env->__address_spaces = (sh_asp*)malloc(sizeof(sh_asp)*manifest.addresses);
        env->strings = (String*)malloc(sizeof(String)*manifest.strings);
        env->global_heap = (Sh_object*)malloc(sizeof(Sh_object)*manifest.classes);

        if(env->classes == NULL || env->__address_spaces == NULL || env->global_heap == NULL
                || env->strings == NULL) {
            throw Exception("Failed to allocate memory for program,"
                                    " try reducing program size!");
        }

        for (;;) {

            __bitFlag = _fStream.at(n++);
            switch (__bitFlag) {
                case 0x0:
                case 0x0a:
                case 0x0d:
                    break;

                case data_class: {
                    int64_t fieldPtr=0, functionPtr=0;
                    ClassObject* klass = &env->classes[classRefptr++];
                    mClasses.push_back(MetaClass(klass, getlong(_fStream)));

                    klass->id = getmi64(_fStream);
                    klass->name.init();
                    klass->name = getstring(_fStream);
                    klass->fieldCount = getlong(_fStream);
                    klass->methodCount = getlong(_fStream);

                    if(klass->fieldCount != 0) {
                        klass->flds = (Field*)malloc(sizeof(Field)*klass->fieldCount);
                    } else
                        klass->flds = NULL;
                    if(klass->methodCount != 0) {
                        klass->methods = (int64_t *)malloc(sizeof(int64_t)*klass->methodCount);
                    } else
                        klass->methods = NULL;
                    klass->super = NULL;

                    if(klass->fieldCount != 0) {
                        for( ;; ) {
                            if(_fStream.at(n) == data_field) {
                                n++;
                                getField(_fStream, mFields, &klass->flds[fieldPtr++]);
                            } else if(_fStream.at(n) == 0x0a || _fStream.at(n) == 0x0d) {
                                n++;
                            } else
                                break;
                        }

                        if(fieldPtr != klass->fieldCount) {
                            throw std::runtime_error("invalid field size");
                        }
                    }

                    if(klass->methodCount != 0) {
                        for( ;; ) {
                            if(_fStream.at(n) == data_method) {
                                n++;
                                klass->methods[functionPtr++] = getmi64(_fStream);
                                n++;
                            } else if(_fStream.at(n) == 0x0a || _fStream.at(n) == 0x0d){
                                n++;
                            } else
                                break;
                        }

                        if(functionPtr != klass->methodCount) {
                            throw std::runtime_error("invalid method size");
                        }
                    }
                    break;
                }
                case eos:
                    break;
                default:
                    throw std::runtime_error("file `" + exe + "` may be corrupt");
            }

            if(__bitFlag == eos) {
                break;
            }
        }

        /* Resolve classes */
        for(MetaClass& metaClass : mClasses) {
            if(metaClass.super != -1)
                metaClass.c->super = findClass(metaClass.super);
        }

        for(MetaField& metaField : mFields) {
            metaField.field->owner = findClass(metaField.owner);
        }

        /* String section */
        int64_t stringPtr=0, stringCnt=0;

        for (;;) {

            __bitFlag = _fStream.at(n++);
            switch (__bitFlag) {
                case 0x0:
                case 0x0a:
                case 0x0d:
                    break;

                case data_string: {
                    env->strings[stringPtr].id = getmi64(_fStream); n++;
                    env->strings[stringPtr].value.init();
                    env->strings[stringPtr].value = getstring(_fStream);

                    stringPtr++, stringCnt++;
                    break;
                }

                case eos:
                    break;

                default:
                    throw std::runtime_error("file `" + exe + "` may be corrupt");
            }

            if(__bitFlag == eos) {
                if(stringCnt != manifest.strings)
                    throw std::runtime_error("invalid string size");
                break;
            }
        }

        if(_fStream.at(n++) != stext)
            throw std::runtime_error("file `" + exe + "` may be corrupt");

        /* Text section */
        uint64_t aspRef=0;

        for (;;) {

            __bitFlag = _fStream.at(n++);
            switch (__bitFlag) {
                case 0x0:
                case 0x0a:
                case 0x0d:
                    break;

                case data_method: {
                    if(aspRef >= manifest.addresses)
                        throw std::runtime_error("text section may be corrupt");

                    sh_asp* adsp = &env->__address_spaces[aspRef++];
                    adsp->init();

                    adsp->id = getmi64(_fStream);
                    adsp->name = getstring(_fStream);
                    adsp->owner = findClass(getmi64(_fStream));
                    adsp->param_size = getmi64(_fStream);
                    adsp->frame_init = getmi64(_fStream);
                    adsp->cache_size = getmi64(_fStream);
                    break;
                }

                case data_byte:
                    break;
                default:
                    throw std::runtime_error("file `" + exe + "` may be corrupt");
            }

            if(__bitFlag == data_byte) {
                if(aspRef != manifest.addresses)
                    throw std::runtime_error("text section may be corrupt");
                n--;
                break;
            }
        }

        aspRef=0;

        for (;;) {

            __bitFlag = _fStream.at(n++);
            switch (__bitFlag) {
                case 0x0:
                case 0x0a:
                case 0x0d:
                    break;

                case data_byte: {
                    sh_asp* adsp = &env->__address_spaces[aspRef++];

                    if(adsp->param_size > 0) {
                        adsp->params = (int64_t*)malloc(sizeof(int64_t)*adsp->param_size);
                        adsp->arrayFlag = (bool*)malloc(sizeof(bool)*adsp->param_size);
                        for(unsigned int i = 0; i < adsp->param_size; i++) {
                            adsp->params[i] = getlong(_fStream);
                            adsp->arrayFlag[i] = (bool)getlong(_fStream);
                        }
                    }

                    if(adsp->cache_size > 0) {
                        adsp->bytecode = (int64_t*)malloc(sizeof(int64_t)*adsp->cache_size);
                        for(int64_t i = 0; i < adsp->cache_size; i++) {
                            adsp->bytecode[i] = getmi64(_fStream);
                        }
                    }
                    break;
                }

                case eos:
                    break;
                default:
                    throw std::runtime_error("file `" + exe + "` may be corrupt");
            }

            if(__bitFlag == eos) {
                break;
            }
        }
        jobIndx-=2;
    } catch(std::exception &e) {
        return 1;
    }

    return 0;
}

int64_t getmi64(file::stream& exe) {
    int64_t i64 =GET_mi64(
            SET_mi32(exe.at(n), exe.at(n+1),
                     exe.at(n+2), exe.at(n+3)
            ), SET_mi32(exe.at(n+4), exe.at(n+5),
                        exe.at(n+6), exe.at(n+7)
    )
    ); n+=MI_BYTES;

    return i64;
}

bool overflowOp(int op) {
    return op == op_MOVI ||
           op == op_MOVBI;
}

ClassObject *findClass(int64_t superClass) {
    for(uint64_t i = 0; i < manifest.classes; i++) {
        if(env->classes[i].id == superClass)
            return &env->classes[i];
    }

    return NULL;
}

void getField(file::stream& exe, list <MetaField>& mFields, Field* field) {
    field->name.init();
    field->name = getstring(exe);
    field->id = getlong(exe);
    field->type = (int)getlong(exe);
    field->isstatic = (bool)getlong(exe);
    field->array = (bool)getlong(exe);
    field->owner = NULL;
    mFields.push_back(MetaField(field, getlong(exe)));
}

nString getstring(file::stream& exe) {
    nString s;
    char c = exe.at(n);
    while(exe.at(n++) != nil) {
        s+=exe.at(n-1);
    }

    return s;
}

int64_t getlong(file::stream& exe) {
    return strtoll(getstring(exe).str().c_str(), NULL, 0);
}

nString string_forward(file::stream& str, size_t begin, size_t end) {
    if(begin >= str.size() || end >= str.size())
        throw std::invalid_argument("unexpected end of stream");

    size_t it=0;
    nString s;
    for(size_t i = begin; it++ < end; i++)
        s +=str.at(i);

    return s;
}

bool checkFile(file::stream& exe) {
    if(exe.at(n++) == file_sig && string_forward(exe, n, 3) == "SEF") {
        n +=3 + offset;

        /* Check file's digital signature */
        if(exe.at(n++) == digi_sig1 && exe.at(n++) == digi_sig2
           && exe.at(n++) == digi_sig3) {
            return true;
        }
    }
    return false;
}