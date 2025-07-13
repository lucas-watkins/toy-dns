//
// Created by Lucas Watkins on 7/9/25.
//

#include "DNSResponse.hpp"

/*
 * Gets 16-bit integer from a DNSResponse provided a start index and DNSResponse
 */
static std::uint16_t get_16_t_from_resp(const RawDNSResponse &r, const std::size_t start_idx) {
    if (start_idx >= r.size() - 1) {
        throw std::invalid_argument("get_16_t_from_str() start_idx >= last index of str");
    }

    return r[start_idx] << 8 | r[start_idx + 1];
}

/*
 * Gets 32-bit integer from DNSResponse provided a start index and a DNSResponse
 */
static std::uint32_t get_32_t_from_resp(const RawDNSResponse &r, const std::size_t start_idx) {
    if (start_idx >= r.size() - 4) {
        throw std::invalid_argument("get_32_from_str() start_idx >= last index of str");
    }

    return get_16_t_from_resp(r, start_idx) << 16 | get_16_t_from_resp(r, start_idx + 2);
}

static std::string get_ip_from_32_t(const std::uint32_t ip) {
    std::stringstream ss;

    for (int shift {24}; shift >= 0; shift -= 8) {
        ss << (ip >> shift & 0xFF);

        if (shift > 0)
            ss << '.';
    }

    return ss.str();
}

/*
 * Parses DNS request from raw data into an object
 * Bytes 1-2 are the ID
 * Bytes 2-3 are the flags
 * Bytes 6-8 are the answer count
 * Name starts on byte 12
 * Query type is two bytes after end of name
 */

DNSResponse::DNSResponse(const DNSPacket &p, const RawDNSResponse &data, const ssize_t ans_size) :
             DNSPacket{p}, _ans_count{get_16_t_from_resp(data, 6)}, _ans_size {ans_size} {

    _flags = get_16_t_from_resp(data, 2); // update flags since they change in response

    // marker for where first response starts (and updated to count each response)
    std::size_t offset { 13 + _name.size() + 5 };

   while (offset < _ans_size) {
       const DNSRecord record_type { get_16_t_from_resp(data, offset + 2) };

       if (record_type == DNSRecord::A) {
           _answers.push_back(
               ARecord {
                   DNSRecord_t {
                       .data = RawDNSResponse(
                            data.begin() + static_cast<ssize_t>(offset),
                            data.begin() + static_cast<ssize_t>(offset) + 16
                           )
                   },
                   get_ip_from_32_t(get_32_t_from_resp(data, offset + 12)),
                   get_32_t_from_resp(data, offset + 6),
               }
           );
           offset += 16; // An A record is 16 bytes
       } else if (record_type == DNSRecord::AAAA) {

       }
   }

}
