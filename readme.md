# ASSMTRL

## Description
ASSMTRL stands for Arbitrary Struct Serialiser and Multiplexed Transceiver Library. It is a library capable of serialising and deserialising arbitrary structs and sending them onto a file descriptor as well as receiving and handling these structures from multiple file descriptors.

PLEASE NOTE THAT IN ITS CURRENT STATE ASSMTRL IS NON-FUNCTIONAL AND IN DEVELOPMENT! DO NOT ATTEMPT TO FOLLOW THE INSTRUCTIONS BELOW!

## Usage
### 1. The arbitrary struct serialiser
### 1.1 Packages
This is the first part of this library. It handles correctly structuring the information in packages. They are structured in the following way:

1. The first eight bytes hold an unsigned 64-bit integer which notates the complete length of the structure.
2. The following bytes are a null-terminated string holding a unique type name for this package.
3. The bytes following this are another null-terminated string holding a description of the information included in this package.
3. The following bytes are serialised information ranging from chars to strings to unsigned 32-bit integers. Read more on what is possible in the wiki.

These packages can be created using the `create_package(PackageType package_type, void* src_ptr, Package* dest_ptr)` function. It allocates the required space to the buffer at dest_ptr and stores a package created using the package type and the information located at src_ptr.

### 1.2 Package Types
The package type is a struct holding a few informations on the kind of data that is stored in the package.

1. It holds a null terminated string containing a name of this specific package type.
2. It holds a null terminated string that notates the exact datatypes of the package and the order they are stored in.

A PackageType may be created using the `create_package_type(char* package_type_name, char* type_descriptor)` function. It acts like a simple constructor.

### 1.3 Type Descriptors
The type_descriptor (later saved as a TYPE* but it is essentially a char*) is a string that contains a series of characters that notate the information saved inside. Read about syntax and limitations in the wiki.

|Name|Character|Corresponding Type|
|-|-|-|
|`STRUCT`|`'T'`|None - Control Character
|`S_LIMITER`|`'('`|None - Control Character
|`S_DELIMITER`|`')'`|None - Control Character
|`A_DELIMITER`|`'['`|None - Control Character
|`A_LIMITER`|`']'`|None - Control Character
|`STRING`|`'S'`|Series of characters. Size is evaluated during packaging.
|`CHAR`|`'C'`|`char`
|`UINT64`|`'U'`|`uint64_t`
|`UINT32`|`'u'`|`uint32_t`
|`INT64`|`'I'`|`int64_t`
|`INT32`|`'i'`|`int32_t`
|`RATIO`|`'R'`|`Ratio`
|`EOTS`|`'\0'`|End-Of-TypeString. Litterally the `NULL` Character.

#### Comment on the Ratio Type
This type was specifically made for this library to safely send floats. Use the functions `Ratio to_ratio(float src)` and `double from_ratio(Ratio src)` to use. Essentially just a `uint64_t` and an `int64_t` which make up numerator and denominator. 

### 2. Multiplexed Transceiver
To be done