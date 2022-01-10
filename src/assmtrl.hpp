#ifndef ASSMTRL_H
#define ASSMTRL_H

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <vector>
#include <utility>
#include <iostream>

typedef enum : char {
  STRUCT = 'T',
  S_LIMITER = '(',
  S_DELIMITER = ')',
  A_DELIMITER = '[',
  A_LIMITER = ']',
  STRING = 'S',
  CHAR = 'C',
  UINT64 = 'U',
  UINT32 = 'u',
  INT64 = 'I',
  INT32 = 'i',
  RATIO = 'R',
  EOTS = '\0',
} Type;

typedef enum : int {
  ALL_GOOD = -1,
  RECEIVER_GENERAL_ERROR = -100,
  RECEIVER_RESERVED_ERROR = -101,

  SOCKET_GENERAL_ERROR = -200,
  SOCKET_RESERVED_ERROR = -201,

  PARSER_GENERAL_ERROR = -300,
  PARSER_INVALID_ELEMENT = -301,
  PARSER_MAXSIZE_EXCEEDED = 302,
  PARSER_MISSING_STRUCT_LIMITER = -303,
  PARSER_MISSING_STRUCT_DELIMITER = -304,
  PARSER_MISSING_ARRAY_LIMITER = -305,
  PARSER_MISSING_ARRAY_DELIMITER = -306,
} statuscode;

typedef int socket_t;
typedef Type* TypeDescriptor;

// PackageType Class
class PackageType {
public:
  char* name;
  TypeDescriptor descriptor;

  // Copy Constructor
  PackageType(const PackageType &type);

  // Constructor - Read the documention on what a TypeDescriptor is.
  PackageType(const char* name, const TypeDescriptor descriptor);

  ~PackageType();
};

// Package Class
class Package {
public:
  int id;
  static int count;
  void* data;
  PackageType type;

  // Constructor
  Package(PackageType type, void* SRC);
  ~Package();
};

// Connection Class
class Connection {
  friend class Server;
  socket_t self, other;
  statuscode STATUS;

  void* handler;
  void* handshake;
  Connection(bool trigger);

public:
  // Constructor
  Connection();
  ~Connection();

  int current_status();
  int close();
  int send_package(socket_t socket_fd, Package package);
  int retrieve_package(char* package_type_name, void* buffer);
};

// Server Class
class Server {
  std::vector<Connection> clients;
public:
  Server();
  ~Server();
};

// Returns cstring on the heap with contents from SRC
char* stralcpy(const char* SRC);

// Returns pointer offset by 'offset' from ptr
void* ptr_offset(const void* ptr, const size_t offset);

// Returns theoretical size of a field described by a Type t
size_t sizeof_type(const Type t, void* ptr);

// Returns offset to next aligned pointer
size_t get_aligned_offset(const Type t, void* ptr);

// Returns how many Type elements are in the descriptor.
size_t descriptorlen(const TypeDescriptor type_descriptor);

// Copies contents of TypeDescriptor DEST to TypeDescriptor SRC
TypeDescriptor descriptorcpy(TypeDescriptor DEST, const TypeDescriptor SRC);

// Returns TypeDescriptor on the heap with contents from SRC
TypeDescriptor descralcpy(const TypeDescriptor SRC);

// Returns TypeDescriptor on the heap with contents from SRC
TypeDescriptor descralcpy(const char* SRC);

// Returns cstring with the name of the type
const char* type_symbol(const Type t);

// Returns the theoretical length of the package described by the PackageType
size_t descriptor_calc_size(const PackageType descirptor, void* ptr);

#endif //ASSMTRL_H