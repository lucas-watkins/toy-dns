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

    for (int shift {(sizeof(T) - 1) * 8}; shift >= 0; shift -= 8 ) {
        result |= r[start_idx] << shift;
        start_idx++;
    }

    return result;
}

std::string get_ipv4_from_resp(const RawDNSResponse &r, const ssize_t start_idx) {
    std::stringstream ss;

    const auto end { r.begin() + start_idx + 3 };

    for (auto iter { r.begin() + start_idx }; iter <= end; ++iter) {
        ss << static_cast<std::uint16_t>(*iter);

        if (iter != end)
            ss << '.';
    }

    return ss.str();
}

std::string get_ipv6_from_resp(const RawDNSResponse &r, const ssize_t start_idx) {

    std::stringstream ss;

    ss << std::hex;

    const auto end { r.begin() + start_idx + 15 };

    bool skip { false };

    for (auto iter {r.begin() + start_idx}; iter <= end; std::advance(iter, 2)) {

        const auto token { static_cast<std::uint16_t>((*iter << 8) + *std::next(iter)) };

        if (token > 0) {
            ss << token;

            if (iter + 1 < end)
                ss << ':';

            skip = false;
        } else {
            if (!skip && iter + 1 < end)
                ss << ':';

            skip = true;
        }
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
    ssize_t offset { static_cast<ssize_t>(13 + _name.size() + 5) };

   while (offset < _ans_size) {
       const DNSRecord record_type { get_uintn_t_from_resp<std::uint16_t>(data, offset + 2) };
       if (record_type == DNSRecord::SOA)
           throw std::ios_base::failure("IP Address Not Found");

       if (record_type == DNSRecord::A) {
           _answers.push_back(
               std::make_shared<ARecord> (
                   RawDNSResponse(
                       data.begin() + offset,
                       data.begin() + offset + 16
                       ),
                       get_ipv4_from_resp(data, offset + 12),
                    get_uintn_t_from_resp<std::uint32_t>(data, offset + 6)
                )
            );
           offset += 16; // An A record is 16 bytes

       } else if (record_type == DNSRecord::AAAA) {
           _answers.push_back(
              std::make_shared<AAAARecord> (
                  RawDNSResponse(
                      data.begin() + offset,
                      data.begin() + offset + 28
                      ),

               get_ipv6_from_resp(data, offset + 12),
                   get_uintn_t_from_resp<std::uint32_t>(data, offset + 6)
               )
           );
           offset += 28;
       } else {
           const std::uint16_t len { get_uintn_t_from_resp<std::uint16_t>(data,offset + 10) };

           offset += 12 + len;
       }
   }
}
