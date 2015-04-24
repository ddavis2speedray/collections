//
// Created by Donald Lamar Davis II on 4/23/15.
//

#ifndef COLLECTIONS_COMMONDEFINES_H
#define COLLECTIONS_COMMONDEFINES_H

namespace collections {
#if COLLECTIONS_OS_WINDOWS
# define COLLECTIONS_PATH_SEP_ "\\"
# define COLLECTIONS_HAS_ALT_PATH_SEP_ 1
// The biggest signed integer type the compiler supports.
typedef __int64 BiggestInt;
#else
# define COLLECTIONS_PATH_SEP_ "/"
# define COLLECTIONS_HAS_ALT_PATH_SEP_ 0
    typedef long long BiggestInt;  // NOLINT
#endif  // COLLECTIONS_OS_WINDOWS

    // The maximum number a BiggestInt can represent.  This definition
    // works no matter BiggestInt is represented in one's complement or
    // two's complement.
    //
    // We cannot rely on numeric_limits in STL, as __int64 and long long
    // are not part of standard C++ and numeric_limits doesn't need to be
    // defined for them.
    const BiggestInt kMaxBiggestInt =
        ~(static_cast<BiggestInt>(1) << (8*sizeof(BiggestInt) - 1));

    // This template class serves as a compile-time function from size to
    // type.  It maps a size in bytes to a primitive type with that
    // size. e.g.
    //
    //   TypeWithSize<4>::UInt
    //
    // is typedef-ed to be unsigned int (unsigned integer made up of 4
    // bytes).
    //
    // Such functionality should belong to STL, but I cannot find it
    // there.
    //
    // Google Test uses this class in the implementation of floating-point
    // comparison.
    //
    // For now it only handles UInt (unsigned int) as that's all Google Test
    // needs.  Other types can be easily added in the future if need
    // arises.
    template <size_t size>
    class TypeWithSize {
     public:
      // This prevents the user from using TypeWithSize<N> with incorrect
      // values of N.
      typedef void UInt;
    };

    // The specialization for size 4.
    template <>
    class TypeWithSize<4> {
     public:
      // unsigned int has size 4 in both gcc and MSVC.
      //
      // As base/basictypes.h doesn't compile on Windows, we cannot use
      // uint32, uint64, and etc here.
      typedef int Int;
      typedef unsigned int UInt;
    };

    // The specialization for size 8.
    template <>
    class TypeWithSize<8> {
     public:
    #if GTEST_OS_WINDOWS
      typedef __int64 Int;
      typedef unsigned __int64 UInt;
    #else
      typedef long long Int;  // NOLINT
      typedef unsigned long long UInt;  // NOLINT
    #endif  // GTEST_OS_WINDOWS
    };

    // Integer types of known sizes.
    typedef TypeWithSize<4>::Int Int32;
    typedef TypeWithSize<4>::UInt UInt32;
    typedef TypeWithSize<8>::Int Int64;
    typedef TypeWithSize<8>::UInt UInt64;
    typedef TypeWithSize<8>::Int TimeInMillis;  // Represents time in milliseconds.

    // Parses 'str' for a 32-bit signed integer.  If successful, writes
    // the result to *value and returns true; otherwise leaves *value
    // unchanged and returns false.
    bool ParseInt32(const std::string& src_text, const char* str, Int32* value) {
        // Parses the environment variable as a decimal integer.
        char* end = NULL;
        const long long_value = strtol(str, &end, 10);  // NOLINT

        // Has strtol() consumed all characters in the string?
        if (*end != '\0') {
            // No - an invalid character was encountered.
            std::cerr << "WARNING: " << src_text
            << " is expected to be a 32-bit integer, but actually"
            << " has value \"" << str << "\"." << std::endl;
            return false;
        }

        // Is the parsed value in the range of an Int32?
        const Int32 result = static_cast<Int32>(long_value);
        if (long_value == LONG_MAX || long_value == LONG_MIN ||
            // The parsed value overflows as a long.  (strtol() returns
            // LONG_MAX or LONG_MIN when the input overflows.)
            result != long_value
            // The parsed value overflows as an Int32.
                ) {
            std::cerr << "WARNING: " << src_text
            << " is expected to be a 32-bit integer, but actually"
            << " has value " << str << ", which overflows." << std::endl;
            return false;
        }

        *value = result;
        return true;
    }


}

#endif //COLLECTIONS_COMMONDEFINES_H
