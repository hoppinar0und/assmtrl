#include "assmtrl.hpp"

size_t ASSMTRL_PACKAGE_MAXSIZE = 1024;
size_t ASSMTRL_MAXDEPTH = 64;

size_t stralcpy(char* dest, const char* src) {
    size_t size = strlen(src) + sizeof(char);
    dest = (char*)malloc(size);
    strcpy(dest, src);
    return size;
}

Error error(Error errcode) {
    std::cout << "Encountered error with code " << errcode << '\n';
    return errcode;
}

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

size_t get_aligned_offset(Type t, void* ptr) {
    size_t size = sizeof_type(t);
    std::cout << "  Evaluating sizeof type t: " << size << '\n';
    size_t overturn = (uint64_t)ptr % size;
    std::cout << "  Evaluating overturn: " << overturn << '\n';
    size_t _return;

    if(overturn == 0)
        _return = 0;
    else
        _return = size - overturn;

    return _return;
}

size_t descriptorlen(Type* type_descriptor) {
    return strlen((char*)type_descriptor);
}

PackageType create_package_type(const char* type_name, const char* type_descriptor) {
    PackageType _return;
    std::cout << stralcpy(_return.type_name, type_name) << '\n';
    std::cout << stralcpy((char*)_return.type_descriptor, type_descriptor) << '\n';
    return _return;
}

Error create_package(PackageType package_type, void* src, Package dest) {
    void* buffer = malloc(ASSMTRL_PACKAGE_MAXSIZE);

    size_t size = sizeof(uint64_t)                              // Initial eight bytes of package which holds the total size
                + strlen(package_type.type_name)        // Cstring that holds the name of the package type
                + descriptorlen(package_type.type_descriptor)   // Descriptor string that describes the data inside this specific package_type
                + sizeof(char);                                 // The null-termination character of the Cstring.
    
    size_t  writehead = size;   // Offset bytes from dest to where we will write next.
    size_t  readhead = 0;       // Offset bytes from src to where we have to read next.

    // Control flow variables
    size_t struct_depth = 0;
    size_t array_depth = 0;
    bool eval_array = false;
    Type arrytype;

    for(int i = 0; i < descriptorlen(package_type.type_descriptor); i++) {
        switch(package_type.type_descriptor[i]) {
            case STRUCT:
                struct_depth++;
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
            {
                void* dest_offset = ptr_offset(dest, writehead);
                void* src_offset = ptr_offset(dest, readhead);

                size_t typesize = strlen((char*)dest_offset) + sizeof(char);
                size += typesize;
                if(size > ASSMTRL_PACKAGE_MAXSIZE) {
                    return error(PARSER_MAXSIZE_EXCEEDED);
                }

                memcpy(dest_offset, src_offset, typesize);
                readhead += typesize;
                writehead += typesize;
            }
            break;

            case RATIO:
            case CHAR:
            case UINT64:
            case UINT32:
            case INT64:
            case INT32:
            {
                // Array Computation
                if(package_type.type_descriptor[i+1] == A_LIMITER) {
                    array_depth++;
                    // Preprocess
                    Type type = package_type.type_descriptor[i];
                    size_t typesize = sizeof_type(type);
                    size_t alignment = get_aligned_offset(type, ptr_offset(src, readhead));
                    readhead += alignment;

                    // how many elements?
                    i +=2;
                    size_t arrsize = atoi((char*)ptr_offset(package_type.type_descriptor, i));

                    // Sizecheck
                    size += typesize * arrsize;
                    if(size > ASSMTRL_PACKAGE_MAXSIZE) {
                        return error(PARSER_MAXSIZE_EXCEEDED);
                    }

                    // This section actually does the computing!
                    for(int j = 0; j < arrsize; j++) {
                        void* dest_offset = ptr_offset(dest, writehead);
                        void* src_offset = ptr_offset(dest, readhead);
                        memcpy(dest_offset, src_offset, typesize);
                        readhead += typesize;
                        writehead += typesize;
                    }
                }

                // Regular Computation
                else {
                    Type type = package_type.type_descriptor[i];
                    size_t typesize = sizeof_type(type);
                    size_t alignment = get_aligned_offset(type, ptr_offset(src, readhead));
                    readhead += alignment;

                    size += typesize;
                    if(size > ASSMTRL_PACKAGE_MAXSIZE) {
                        return error(PARSER_MAXSIZE_EXCEEDED);
                    }

                    void* dest_offset = ptr_offset(dest, writehead);
                    void* src_offset = ptr_offset(dest, readhead);
                    memcpy(dest_offset, src_offset, typesize);
                    readhead += typesize;
                    writehead += typesize;
                }
            }
            break;

            case A_LIMITER:
            case S_LIMITER:
            break;
            case A_DELIMITER:
                array_depth--;
            break;
            case S_DELIMITER:
                struct_depth--;
            break;

            default:
                std::cout << "AT I = " << i << " AND ELEMENT = " << (short)package_type.type_descriptor[i] << ": ";
                return error(PARSER_INVALID_ELEMENT);
                break;
        }
    }

    size = writehead;

    memcpy(dest, &size, sizeof(uint64_t));
    writehead = sizeof(uint64_t);
    strcpy((char*)ptr_offset(dest, writehead), package_type.type_name);
    writehead = strlen(package_type.type_name);
    strcpy((char*)ptr_offset(dest, writehead), (char*)package_type.type_descriptor);

    free(buffer);
    return ALL_GOOD;
}

Error create_package(char* type_name, Type* type_descriptor, void* src_ptr, Package* dest_ptr) {
    return ALL_GOOD;
}

Error send_package(socket_t socket_fd, Package package) {
    return ALL_GOOD;
}