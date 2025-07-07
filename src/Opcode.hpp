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
    StandardQuery = 0b0000000000000000,
    IQuery        = 0b0000100000000000,
    Status        = 0b0001000000000000,
    Unassigned    = 0b0001100000000000,
    Notify        = 0b0010000000000000,
    Update        = 0b0010100000000000,
    DSO           = 0b0011000000000000,
};

#endif //DNSOPCODES_HPP
