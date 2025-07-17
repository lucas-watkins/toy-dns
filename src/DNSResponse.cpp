//
// Created by Lucas Watkins on 7/9/25.
//

#include "DNSResponse.hpp"

template <typename T> requires std::is_arithmetic_v<T> && std::is_unsigned_v<T>
static T get_uintn_t_from_resp(const RawDNSResponse &r, std::size_t start_idx) {

    if (start_idx >= r.size() - sizeof(T)) {
        throw std::invalid_argument("get_n_bits_from_resp start_idx >= last index of str");
    }

    T result {0};

    for (unsigned num_bits {sizeof(T) * 8}; num_bits >= 8; num_bits -= 8 ) {
        result |= r[start_idx] << (num_bits - 8);
        start_idx++;
    }

    return result;
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
             DNSPacket{p}, _ans_count{get_uintn_t_from_resp<std::uint16_t>(data, 6)}, _ans_size {ans_size} {

    _flags = get_uintn_t_from_resp<std::uint16_t>(data, 2); // update flags since they change in response

    // marker for where first response starts (and updated to count each response)
    std::size_t offset { 13 + _name.size() + 5 };

   while (offset < _ans_size) {
       const DNSRecord record_type { get_uintn_t_from_resp<std::uint16_t>(data, offset + 2) };

       if (record_type == DNSRecord::A) {
           _answers.emplace_back(
               std::make_shared<ARecord> (
                   RawDNSResponse(
                       data.begin() + static_cast<ssize_t>(offset),
                       data.begin() + static_cast<ssize_t>(offset) + 1
                       ),
                       get_ip_from_32_t(get_uintn_t_from_resp<std::uint32_t>(data, offset + 12)),
                    get_uintn_t_from_resp<std::uint32_t>(data, offset + 6)
                )
            );
           offset += 16; // An A record is 16 bytes
       } else if (record_type == DNSRecord::AAAA) {

       }
   }
}
