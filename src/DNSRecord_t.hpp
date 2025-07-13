//
// Created by Lucas Watkins on 7/9/25.
//

#ifndef DNSRECORD_HPP
#define DNSRECORD_HPP

#include <string>
#include <cstdint>
#include <iostream>

using RawDNSResponse = std::vector<std::uint8_t>;

struct DNSRecord_t {
    const RawDNSResponse data; // Raw data as chars
};

struct CNAMERecord final : public DNSRecord_t {
    const std::string name;
};

struct ARecord final : public DNSRecord_t {
    const std::string ip_addr;
    const std::uint32_t ttl; // How long is response valid?
};

typedef ARecord AAAARecord; // AAAA Record has a 128-bit ipv6 address

enum class DNSRecord : std::uint16_t {
    A = 1,
    AAAA = 28,
    CNAME = 5,
};

inline std::ostream& operator<<(std::ostream& os, const DNSRecord query_type) {
    switch (query_type) {
        case DNSRecord::A:
            return os << "A";
        case DNSRecord::AAAA:
            return os << "AAAA";
        case DNSRecord::CNAME:
            return os << "CNAME";
    }
    return os;
}

#endif //DNSRECORD_HPP
