//
// Created by Lucas Watkins on 7/9/25.
//

#ifndef DNSRESPONSE_HPP
#define DNSRESPONSE_HPP

#include "DNSPacket.hpp"
#include <sstream>
#include "DNSRecord_t.hpp"


class DNSResponse : public DNSPacket {

    std::uint16_t _ans_count;
    ssize_t _ans_size;
    std::vector<DNSRecord_t> _answers;

public:
    DNSResponse(const DNSPacket &, const RawDNSResponse &, ssize_t);
};

#endif //DNSRESPONSE_HPP
