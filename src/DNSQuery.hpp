//
// Created by Lucas Watkins on 7/1/25.
//

#ifndef DNSPACKET_HPP
#define DNSPACKET_HPP

#include <cstdint>
#include <format>
#include <cassert>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "Opcode.hpp"
#include "RecordType.hpp"
#include <type_traits>
#include <fcntl.h>
#include <sys/ioctl.h>

class DNSQuery {
    std::uint16_t _id {0};
    std::uint16_t _flags {0};
    Opcode _opcode;
    std::string _name {};
    RecordType _record_type {};


public:
    DNSQuery(const std::uint16_t id, const Opcode opcode) : _id{id}, _opcode{opcode} {
        _flags = static_cast<std::uint16_t>(opcode);
    }

    [[nodiscard]] std::uint8_t flags() const;
    [[nodiscard]] Opcode opcode() const;
    void recursive(bool recursive);
    void record_type(RecordType);
    void name(std::string);
    void send(std::string_view addr);

    friend void send_packet(const DNSQuery &);
};

void push_16_t(std::stringstream &, std::uint16_t num);

#endif //DNSPACKET_HPP
