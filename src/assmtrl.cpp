#include "assmtrl.hpp"

//////////// PACKAGE TYPE FUNCTIONS ////////////

PackageType::PackageType(const char* name, const TypeDescriptor descriptor) {
    this->name = stralcpy(name);
    this->descriptor = descralcpy(descriptor);
}

PackageType::PackageType(const PackageType &type) {
    this->name = stralcpy(type.name);
    this->descriptor = descralcpy(type.descriptor);
}

PackageType::~PackageType() {
    free(name);
    free(descriptor);
}


//////////// PACKAGE FUNCTIONS ////////////

Package::Package(PackageType type, void* SRC) : type(type) {
    std::cout << "Package constructor invoked" << '\n';
    std::cout << "Initial conditions:" << '\n';
    std::cout << " type {" << '\n';
    std::cout << "  name = \"" << type.name << "\"," << '\n';
    std::cout << "  descriptor = \"" << (char*)type.descriptor << "\"," << '\n';
    std::cout << " }" << '\n';
    std::cout << " SRC is @ " << SRC << '\n' << '\n';

    std::cout << "Calculating size... ";
    size_t fullsize = descriptor_calc_size(type);
    std::cout << fullsize << '\n';
    std::cout << "Allocating \'data\' @ ";
    data = malloc(fullsize);
    std::cout << data << '\n';

    size_t readhead = 0, writehead = 0;
}

Package::~Package() {
    std::cout << "Package destructor invoked" << '\n';

}

//////////// HELPER FUNCTIONS ////////////

void* ptr_offset(void* ptr, size_t offset) {
    return (void*)((uint64_t)ptr + offset);
}

char* stralcpy(const char* SRC) {
    char* _return = (char*)malloc(strlen(SRC) + sizeof(char));
    strcpy(_return, SRC);
    return _return;
}

size_t descriptorlen(TypeDescriptor type_descriptor) {
    return strlen((char*)type_descriptor);
}

TypeDescriptor descriptorcpy(TypeDescriptor DEST, const TypeDescriptor SRC) {
    strcpy((char*)DEST, (char*)SRC);
    return DEST;
}

TypeDescriptor descralcpy(const TypeDescriptor SRC) {
    TypeDescriptor _return = (TypeDescriptor)malloc(descriptorlen(SRC));
    descriptorcpy(_return, SRC);
    return _return;
}

TypeDescriptor descralcpy(const char* SRC) {
    TypeDescriptor _return = (TypeDescriptor)malloc(strlen(SRC) + sizeof(char));
    strcpy((char*)_return, SRC);
    return _return;
}

size_t sizeof_type(Type t) {
    switch(t) {
        STRUCT:
            return 0;
        break;

        S_LIMITER:
            return 0;
        break;

        S_DELIMITER:
            return 0;
        break;

        A_DELIMITER:
            return 0;
        break;

        A_LIMITER:
            return 0;
        break;

        STRING:
            return 0;
        break;

        CHAR:
            return sizeof(char);
        break;

        UINT64:
            return sizeof(uint64_t);
        break;

        UINT32:
            return sizeof(uint32_t);
        break;

        INT64:
            return sizeof(int64_t);
        break;

        INT32:
            return sizeof(int32_t);
        break;

        RATIO:
            return sizeof(int64_t) + sizeof(uint64_t);
        break;

        EOTS:
            return 0;
        break;

        default:
            std::cout << "!!ERROR HERE!!";
    }
    return 0;
}

size_t get_aligned_offset(Type t, void* ptr) {
    size_t size = sizeof_type(t);
    size_t overturn = (uint64_t)ptr % size;
    size_t _return;

    if(overturn == 0)
        _return = 0;
    else
        _return = size - overturn;

    return _return;
}

size_t descriptor_calc_size(PackageType ptype) {
    std::cout << "with " << (char*)ptype.descriptor << ' ';
    size_t sizebuffer = 0;

    sizebuffer += strlen(ptype.name) + sizeof(char);
    std::cout << strlen(ptype.name) + sizeof(char) << " + ";
    sizebuffer += descriptorlen(ptype.descriptor) + sizeof(char);
    std::cout << descriptorlen(ptype.descriptor) + sizeof(char);

    for(int i = 0; i < descriptorlen(ptype.descriptor); i++) {
        std::cout << (char)ptype.descriptor[i];
        sizebuffer += sizeof_type(ptype.descriptor[i]);
        std::cout << " + " << sizeof_type(ptype.descriptor[i]);
    }

    std::cout << " = ";

    return sizebuffer;
}