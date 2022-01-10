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
    size_t fullsize = descriptor_calc_size(type, SRC);
    std::cout << fullsize << '\n';
    std::cout << "Allocating \'data\' @ ";
    data = malloc(fullsize);
    std::cout << data << '\n';
    memcpy(data, &fullsize, sizeof(uint64_t));

    size_t readhead = 0, writehead = 0;
}

Package::~Package() {
    std::cout << "Package destructor invoked" << '\n';

}

//////////// HELPER FUNCTIONS ////////////

void* ptr_offset(const void* ptr, const size_t offset) {
    return (void*)((uint64_t)ptr + offset);
}

char* stralcpy(const char* SRC) {
    char* _return = (char*)malloc(strlen(SRC) + sizeof(char));
    strcpy(_return, SRC);
    return _return;
}

size_t descriptorlen(const TypeDescriptor type_descriptor) {
    return strlen((char*)type_descriptor);
}

TypeDescriptor descriptorcpy(TypeDescriptor DEST, const TypeDescriptor SRC) {
    strcpy((char*)DEST, (char*)SRC);
    return DEST;
}

TypeDescriptor descralcpy(const TypeDescriptor SRC) {
    TypeDescriptor _return = (TypeDescriptor)malloc(descriptorlen(SRC) + sizeof(char));
    descriptorcpy(_return, SRC);
    return _return;
}

TypeDescriptor descralcpy(const char* SRC) {
    TypeDescriptor _return = (TypeDescriptor)malloc(strlen(SRC) + sizeof(char));
    strcpy((char*)_return, SRC);
    return _return;
}

const char* type_symbol(const Type t) {
    switch(t) {
        case STRUCT:
            return "STRUCT";
        break;

        case S_LIMITER:
            return "S_LIMITER";
        break;

        case S_DELIMITER:
            return "S_DELIMITER";
        break;

        case A_DELIMITER:
            return "A_DELIMITER";
        break;

        case A_LIMITER:
            return "A_LIMITER";
        break;

        case STRING:
            return "STRING";
        break;

        case CHAR:
            return "CHAR";
        break;

        case UINT64:
            return "UINT64_T";
        break;

        case UINT32:
            return "UINT32_T";
        break;

        case INT64:
            return "INT64_T";
        break;

        case INT32:
            return "INT32_T";
        break;

        case RATIO:
            return "RATIO";
        break;

        case EOTS:
            return "EOTS";
        break;

        default:
            std::cout << "!!ERROR HERE!! CHAR EXAMINED WAS >>" << (char)t << "<< !!";
    }
    return NULL;
}

size_t sizeof_type(const Type t, void* ptr) {
    switch(t) {
        case STRUCT:
            return 0;
        break;

        case S_LIMITER:
            return 0;
        break;

        case S_DELIMITER:
            return 0;
        break;

        case A_DELIMITER:
            return 0;
        break;

        case A_LIMITER:
            return 0;
        break;

        case STRING:
            std::cout << "ADDITION INFO DUE TO STRING CASE: \n" << "  String contains: \"" << (char*)ptr << "\"\n";
            return strlen((char*)ptr) + sizeof(char);
        break;

        case CHAR:
            return sizeof(char);
        break;

        case UINT64:
            return sizeof(uint64_t);
        break;

        case UINT32:
            return sizeof(uint32_t);
        break;

        case INT64:
            return sizeof(int64_t);
        break;

        case INT32:
            return sizeof(int32_t);
        break;

        case RATIO:
            return sizeof(int64_t) + sizeof(uint64_t);
        break;

        case EOTS:
            return 0;
        break;

        default:
            std::cout << "!!ERROR HERE!! CHAR EXAMINED WAS >>" << (char)t << "<< !!";
    }
    return 0;
}

size_t get_aligned_offset(const Type t, void* ptr) {
    size_t size;
    size_t overturn;
    size_t _return = 0;

    // Strings must be handled differently
    if(t == STRING)
        size = 8;
    else
        size = sizeof_type(t, ptr);

    // Evaluate whether or not we can calculate the overturn.
    if(size < 1)
        _return = 0;
    else
        size_t overturn = (uint64_t)ptr % size;

    // Evaluate the overturn.
    if(overturn == 0)
        _return = 0;
    else
        _return = size - overturn;

    return _return;
}

size_t descriptor_calc_size(const PackageType ptype, void* ptr) {
    std::cout << std::endl;

    size_t size = 0;
    size_t readhead = 0;

    size += strlen(ptype.name) + sizeof(char);
    size += descriptorlen(ptype.descriptor) + sizeof(char);

    for(int i = 0; i < descriptorlen(ptype.descriptor); i++) {
        std::cout << "with " << type_symbol(ptype.descriptor[i]) << '\n';

        if(ptype.descriptor[i] == STRUCT) {
            readhead += get_aligned_offset(UINT64, ptr);
            void* aligned_ptr = ptr_offset(ptr, readhead);
        }

        else {
            readhead += get_aligned_offset(ptype.descriptor[i], ptr);
            void* aligned_ptr = ptr_offset(ptr, readhead);
        }

        size_t sizebuffer = sizeof_type(ptype.descriptor[i], ptr);
        readhead += sizebuffer;
        size += sizebuffer;

        std::cout << "  readhead @ " << readhead << '\n';
        std::cout << "  sizebuffer = " << sizebuffer << '\n';
        std::cout << "  size = " << size << '\n';
    }

    return size;
}