//
// Created by Lucas Watkins on 7/9/25.
//

#ifndef DNSRECORD_HPP
#define DNSRECORD_HPP

#include <vector>
#include <cstdint>

using RawDNSResponse = std::vector<std::uint8_t>;
struct BaseDNSRecord;

#define IP_CLASS_TEMPLATE(NAME) struct NAME final : public BaseDNSRecord {\
                                const std::string ip_addr;\
                                const std::uint32_t ttl;\
                                \
                                NAME(RawDNSResponse data, std::string ip_addr, const std::uint32_t ttl) :\
                                BaseDNSRecord{std::move(data)},\
                                ip_addr{std::move(ip_addr)},\
                                ttl{ttl} {}\
                            };

#include <string>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <utility>
#include <netinet/in.h>

struct BaseDNSRecord {
    const RawDNSResponse _data; // Raw data as chars

    explicit BaseDNSRecord(RawDNSResponse data) : _data(std::move(data)) {}
    virtual ~BaseDNSRecord() = default;
};

IP_CLASS_TEMPLATE(ARecord)

// AAAA Record has a 128-bit ipv6 address
IP_CLASS_TEMPLATE(AAAARecord)

enum class DNSRecord : std::uint16_t {
    A = 1,
    AAAA = 28,
    SOA = 6,
};

inline std::ostream& operator<<(std::ostream& os, const DNSRecord query_type) {
    switch (query_type) {
        case DNSRecord::A:
            return os << "A";
        case DNSRecord::AAAA:
            return os << "AAAA";
        case DNSRecord::SOA:
            return os << "SOA";
    }
    return os;
}

#endif //DNSRECORD_HPP
