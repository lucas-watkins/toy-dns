//
// Created by Lucas Watkins on 7/2/25.
//

#ifndef DNSOPCODES_HPP
#define DNSOPCODES_HPP
#include <cstdint>

/*
 * |QR|OPCODE|0|TC|RD|0|000|0000|
 * QR (Query) -> 1 bit
 * OPCODE -> 4 bits
 * TC (Truncated) -> 1 bit (just retry over tcp)
 * RD (Recursion Request) -> 1 bit
 *
 * https://www.iana.org/assignments/dns-parameters/dns-parameters.xhtml
 */
enum class Opcode : std::uint16_t {
    StandardQuery = 0,
    IQuery        = 1,
    Status        = 2,
    Unassigned    = 3,
    Notify        = 4,
    Update        = 5,
    DSO           = 6,
};

#endif //DNSOPCODES_HPP
