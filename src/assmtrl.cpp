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
    exit(errcode);
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
    size_t overturn = (uint64_t)ptr % size;
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

PackageType create_package_type(const char* package_type_name, const char* type_descriptor) {
    PackageType _return;
    stralcpy(_return.package_type_name, package_type_name);
    stralcpy((char*)_return.type_descriptor, type_descriptor);
    return _return;
}

Package create_package(PackageType package_type, void* src) {
    std::cout << "Creating package..." << '\n';
    std::cout << " dest has capacity of " << ASSMTRL_PACKAGE_MAXSIZE << '\n';
    Package dest = malloc(ASSMTRL_PACKAGE_MAXSIZE);

    size_t size = sizeof(uint64_t)                              // Initial eight bytes of package which holds the total size
                + strlen(package_type.package_type_name)        // Cstring that holds the name of the package type
                + descriptorlen(package_type.type_descriptor)   // Descriptor string that describes the data inside this specific package_type
                + sizeof(char);                                 // The null-termination character of the Cstring.
    std::cout << " size is currently " << size << '\n';

    size_t writehead = size;   // Offset bytes from dest to where we will write next.
    size_t readhead = 0;       // Offset bytes from src to where we have to read next.

    // Control flow variables
    size_t struct_depth = 0; // Unused
    size_t array_depth = 0; // Unused
    bool exit_condition = false;

    for(int i = 0; i < descriptorlen(package_type.type_descriptor) || exit_condition; i++) {
        std::cout << "  readhead " << readhead << '\n';
        std::cout << " writehead " << writehead << '\n';
        std::cout << " ENTERING SWITCH. CURRENT TYPE IS " << package_type.type_descriptor[i] << '\n';
        switch(package_type.type_descriptor[i]) {
            case STRUCT:
                if(!(package_type.type_descriptor[i+1] != S_DELIMITER)) {
                    error(PARSER_MISSING_STRUCT_DELIMITER);
                }

                else {
                    i++;
                }
            break;

            case EOTS:
                exit_condition = true;
            break;

            case STRING:
            {
                void* offset_src = ptr_offset(src, readhead);
                void* offset_dest = ptr_offset(dest, writehead);
                Type typebuf = package_type.type_descriptor[i];

                size_t sizebuf = strlen((char*)offset_src) + sizeof(char);

                memcpy(offset_dest, offset_src, sizebuf);
                writehead += sizebuf;
                readhead += sizebuf;
            }
            break;

            case RATIO:
                size += sizeof(int64_t) + sizeof(uint64_t);
            break;

            case CHAR:
            case UINT64:
            case UINT32:
            case INT64:
            case INT32:
            {
                // Array Computation
                if(package_type.type_descriptor[i+1] == A_LIMITER) {
                    Type typebuf = package_type.type_descriptor[i];
                    size_t sizebuf = sizeof_type(typebuf);
                    size_t readhead_alignment = get_aligned_offset(typebuf, ptr_offset(src, readhead));
                    size_t aligned_readhead = readhead + readhead_alignment;

                    i+=2; // Jump to beginning of WHAT SHOULD BE A STRING THAT DENOTES LENGTH. THIS SIMPLY ASSUMES!
                    size_t arrlen = atoi((char*)&package_type.type_descriptor[i]);
                    i += strlen((char*)&package_type.type_descriptor[i]);

                    for(int j = 0; j < arrlen; j++) {
                        void* offset_src  = ptr_offset(src, aligned_readhead);
                        void* offset_dest = ptr_offset(dest, writehead);

                        memcpy(offset_dest, offset_src, sizebuf);
                        writehead += sizebuf;
                        aligned_readhead += sizebuf;
                    }

                    readhead = aligned_readhead;
                }

                // Regular Computation
                else {
                    Type typebuf = package_type.type_descriptor[i];
                    size_t sizebuf = sizeof_type(typebuf);
                    size_t readhead_alignment = get_aligned_offset(typebuf, ptr_offset(src, readhead));
                    size_t aligned_readhead = readhead + readhead_alignment;
                    void* offset_src  = ptr_offset(src, aligned_readhead);
                    void* offset_dest = ptr_offset(dest, writehead);

                    memcpy(offset_dest, offset_src, sizebuf);
                    writehead += sizebuf;
                    readhead += readhead_alignment + sizebuf;
                }
            }
            break;

            case A_DELIMITER:
            case S_DELIMITER:
            break;

            default:
                error(PARSER_INVALID_ELEMENT);
                break;
        }

        if(package_type.type_descriptor[i + 1] == '(' || '[') {
            
        }
    }

    size += writehead;

    memcpy(dest, &size, sizeof(uint64_t));

    return dest;
}

Error send_package(socket_t socket_fd, Package package) {
    return ALL_GOOD;
}