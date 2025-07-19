//
// Created by Lucas Watkins on 7/1/25.
//

#ifndef DNSQUERY_HPP
#define DNSQUERY_HPP

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
#include "BaseDNSRecord.hpp"
#include <type_traits>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "DNSPacket.hpp"
#include "DNSResponse.hpp"

class DNSQuery : public DNSPacket {
    [[nodiscard]] std::string build_query() const;

public:
    DNSQuery(const std::uint16_t id, const std::string& name, const DNSRecord query_type) : DNSPacket{
        id, name, query_type
    } {
        recursive(true);
    }

    DNSQuery& recursive(bool recursive);

    //bool recursive() const {return };

    DNSQuery& query_type(const DNSRecord r) {
        _query_type = r;
        return *this;
    }

    // cppcheck-suppress duplInheritedMember
    [[nodiscard]] DNSRecord query_type() const { return DNSPacket::query_type(); }

    DNSQuery& name(const std::string& name) {
        _name = name;
        return *this;
    }

    // cppcheck-suppress duplInheritedMember
    [[nodiscard]] const std::string& name() const { return DNSPacket::name(); }

    [[nodiscard]] DNSResponse send(std::string_view addr) const;
};

#endif // DNSQUERY_HPP
