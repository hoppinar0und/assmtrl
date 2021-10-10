#include "assmtrl.hpp"

size_t ASSMTRL_PACKAGE_MAXSIZE = 1024;
size_t ASSMTRL_MAXDEPTH = 64;

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
    void* buffer = malloc(ASSMTRL_PACKAGE_MAXSIZE);

    size_t size = sizeof(uint64_t)                              // Initial eight bytes of package which holds the total size
                + strlen(package_type.package_type_name)        // Cstring that holds the name of the package type
                + descriptorlen(package_type.type_descriptor)   // Descriptor string that describes the data inside this specific package_type
                + sizeof(char);                                 // The null-termination character of the Cstring.
    
    size_t  writehead = size,  // Offset bytes from dest to where we will write next.
            readhead = 0;   // Offset bytes from src to where we have to read next.

    Type* typebuffer = (Type*)malloc(ASSMTRL_MAXDEPTH);

    // Control flow variables
    size_t struct_depth = 0;
    size_t array_depth = 0;
    bool eval_array = false;
    Type arrytype;

    for(int i = 0; i < descriptorlen(package_type.type_descriptor); i++) {
        switch(package_type.type_descriptor[i]) {
            case STRUCT:
                struct_depth++;
                typebuffer[struct_depth + array_depth - 1] = STRUCT;
            break;

            case EOTS:
                if(struct_depth != 0) {
                    return error(PARSER_MISSING_STRUCT_CLOSER);
                }
                if(array_depth != 0) {
                    return error(PARSER_MISSING_ARRAY_CLOSER);
                }
            break;

            case STRING:
                size += strlen((char*)ptr_offset(src, readhead)) + sizeof(char);
            break;

            case RATIO:
                size += sizeof(int64_t) + sizeof(uint64_t);
            break;

            case CHAR:
            case UINT64:
            case UINT32:
            case INT64:
            case INT32:
            if(package_type.type_descriptor[i+1] == A_LIMITER) {
                /* DO ARRAY STUFF */
            } else {
                size += sizeof_type(package_type.type_descriptor[i]);
                memcpy(ptr_offset(dest, writehead), ptr_offset(src, readhead), sizeof_type(package_type.type_descriptor[i]));
                readhead += sizeof_type(package_type.type_descriptor[i]);
                writehead += sizeof_type(package_type.type_descriptor[i]);
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