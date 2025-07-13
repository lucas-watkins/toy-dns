//
// Created by Lucas Watkins on 7/8/25.
//

#ifndef DNSPACKET_HPP
#define DNSPACKET_HPP

#include <cstdint>
#include <utility>
#include <vector>
#include "DNSRecord_t.hpp"

/*
 * Base class for a DNSQuery or DNSResponse. If left unchanged in either class,
 * the class auto initializes an A Record, no names, standard query,
 * and an id of 0
 */
struct DNSPacket {
protected:
    std::uint16_t _id{};
    std::uint16_t _flags{};
    std::string _name{};
    DNSRecord _query_type{};

    DNSPacket(const std::uint16_t id, std::string name, const DNSRecord query_type) : _id{id},
        _name{std::move(name)}, _query_type{query_type} {
    }

    DNSPacket(const std::uint16_t id, std::string name, const DNSRecord query_type, const std::uint16_t flags) :
              _id{id}, _flags{flags}, _name{std::move(name)}, _query_type{query_type} {

    }

    DNSPacket() = default;

public:
    [[nodiscard]] const std::string& name() const { return _name; }
    [[nodiscard]] std::uint16_t id() const { return _id; }
    [[nodiscard]] std::uint16_t flags() const { return _flags; }
    [[nodiscard]] DNSRecord query_type() const { return _query_type; }
};

using RawDNSResponse = std::vector<std::uint8_t>;

inline std::ostream& operator<<(std::ostream& os, const DNSPacket& r) {
    os << "ID: " << r.id() << '\n';
    os << "Query Type: " << r.query_type() << '\n';
    os << "Name: " << r.name() << '\n';
    os << "Flags: " << r.flags();
    return os;
}


#endif //DNSPACKET_HPP
