//
// Created by Lucas Watkins on 7/9/25.
//

#ifndef DNSRESPONSE_HPP
#define DNSRESPONSE_HPP

#include "DNSPacket.hpp"
#include <sstream>
#include "DNSRecord_t.hpp"
#include <memory>


class DNSResponse : public DNSPacket {

    std::uint16_t _ans_count;
    ssize_t _ans_size;
    std::vector<std::shared_ptr<DNSRecord_t>> _answers {};


public:
    DNSResponse(const DNSPacket &, const RawDNSResponse &, ssize_t);

    template <typename T> requires std::is_base_of_v<DNSRecord_t, T>
    [[nodiscard]] std::vector<T> get_records() const {
        std::vector<T> records;
        for (const auto &r : _answers) {
            if (T* record = dynamic_cast<T*>(r.get())) {
                records.push_back(*record);
            }
        }
        return records;
    }
};

#endif //DNSRESPONSE_HPP
