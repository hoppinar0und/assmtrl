#include "assmtrl.hpp"

size_t SHTP_PACKAGE_MAXSIZE = 1024;
size_t SHTP_MAXDEPTH = 64;

void* ptr_offset(void* ptr, size_t offset) {
    return (void*)((uint64_t)ptr + offset);
}

size_t sizeof_type(Type t) {
    switch(t) {
        case 'C':
            return sizeof(char);
            break;

        case 'U':
            return sizeof(uint64_t);
            break;

        case 'u':
            return sizeof(uint32_t);
            break;

        case 'I':
            return sizeof(int64_t);
            break;

        case 'i':
            return sizeof(int32_t);
            break;

        case 'R':
            return sizeof(int64_t) + sizeof(uint64_t);
            break;

        default:
            return 0;
            break;
    }

    return 0;
}

bool is_aligned(Type t, void* ptr) {
    size_t eval = (uint64_t)ptr % sizeof_type(t);
    if(eval != 0)
        return false;
    else
        return true;
}

size_t descriptorlen(Type* type_descriptor) {
    return strlen((char*)type_descriptor);
}

Error create_package(PackageType package_type, void* src, Package* dest) {
    size_t _return;
    Type* buffer = (Type*)malloc(MAXDEPTH);

    // Control flow variables
    size_t struct_depth = 0;
    size_t array_depth = 0;
    size_t limiter_depth = 0;
    bool eval_array = false;
    Type arrytype;

    for(int i = 0; i < descriptorlen(package_type.type_descriptor); i++) {

        switch(package_type.type_descriptor[i]) {
            case STRUCT:
                struct_depth++;
                buffer[struct_depth + array_depth - 1] = STRUCT;
                break;

            case EOTS:

            break;

            case STRING:

            break;

            case RATIO:

            break;

            case CHAR:
            case UINT64:
            case UINT32:
            case INT64:
            case INT32:
            if(package_type.type_descriptor[i+1] == A_LIMITER) {
                /* DO ARRAY STUFF */
            } else {
                /* DO NORMAL STUFF */
            }
            break;

            default:
                return error(PARSER_INVALID_ELEMENT);
                break;
        }

        if(package_type.type_descriptor[i + 1] == '(' || '[') {
            
        }
    }

    free(buffer);
    return ALL_GOOD;
}

Error create_package(char* package_type_name, Type* type_descriptor, void* src_ptr, Package* dest_ptr) {
    return ALL_GOOD;
}

Error send_package(socket_t socket_fd, Package package) {
    return ALL_GOOD;
}