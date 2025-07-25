//
// Created by Lucas Watkins on 7/1/25.
//

#include "DNSQuery.hpp"
#include "DNSResponse.hpp"

/*
 * |QR|OPCODE|0|TC|RD|0|000|0000|
 * QR (Query) -> 1 bit
 * OPCODE -> 4 bits
 * TC (Truncated) -> 1 bit (just retry over tcp)
 * RD (Recursion Request) -> 1 bit
 *
 * https://www.iana.org/assignments/dns-parameters/dns-parameters.xhtml
 */

DNSQuery& DNSQuery::recursive(const bool recursive) {
    // Magic numbers to set recursion bit in dns request
    // https://courses.cs.duke.edu/fall16/compsci356/DNS/DNS-primer.pdf
    if (recursive) {
        _flags |= 256;
    } else {
        _flags &= 254;
    }

    return *this;
}

/*
 * Splits a 16-bit integer into two one byte parts and pushes them into a string stream
 */
static void push_16_t(std::stringstream& os, const std::uint16_t num) {
    static_assert(std::is_same<std::uint8_t, unsigned char>());

    const std::uint8_t high{static_cast<std::uint8_t>((num & 0xFF00) >> 8)};
    const std::uint8_t low{static_cast<std::uint8_t>(num & 0x00FF)};

    os << high << low;
}

/*
 * Responsible for building the entire query which is expressed as a raw string of characters
 */
std::string DNSQuery::build_query() const {
    std::stringstream stream;

    push_16_t(stream, _id);
    push_16_t(stream, _flags);
    push_16_t(stream, 1); // Question Count
    push_16_t(stream, 0); // ANCOUNT
    push_16_t(stream, 0); // NSCOUNT
    push_16_t(stream, 0); // ARCOUNT

    std::string token;
    std::vector<std::string> tokens;

    for (auto iter = _name.begin(); iter != _name.end(); ++iter) {
        if (*iter == '.') {
            tokens.push_back(token);
            token.clear();
        } else if (_name.end() - iter == 1) {
            // last item
            token += *iter;
            tokens.push_back(token);
        } else {
            token += *iter;
        }
    }

    for (const auto& t: tokens) {
        stream << static_cast<char>(t.size()) << t;
    }

    stream << static_cast<char>(0); // Null terminating character
    push_16_t(stream, static_cast<std::uint16_t>(_query_type));
    push_16_t(stream, 1); //Query is class IN (internet address)

    return stream.str();
}

/*
 * Sends the DNS query and captures a response. For now, it prints the raw packet to stdout
 */
DNSResponse DNSQuery::send(const std::string_view addr) const {
    const int sock = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(53);
    address.sin_addr.s_addr = inet_addr(addr.data());

    const std::string data{std::move(build_query())};

    const char* pbuf = data.c_str();

    size_t buflen{data.size()};

    std::vector<std::uint8_t> resp_buffer(BUFSIZ); // Max packet size

    if (connect(sock, reinterpret_cast<sockaddr *>(&address), sizeof(address))) {
        throw std::runtime_error("DNSQuery::send [connect failed]");
    }

    while (buflen > 0) {
        const ssize_t num_bytes = ::send(sock, pbuf, buflen, 0);
        pbuf += num_bytes;
        buflen -= num_bytes;
    }

    const ssize_t packet_size{recv(sock, &resp_buffer[0], resp_buffer.size(), MSG_PEEK)};

    // recv consumes one datagram regardless if it is truncated
    const ssize_t num_bytes{recv(sock, &resp_buffer[0], resp_buffer.size(), MSG_PEEK)};

    if (packet_size != num_bytes || num_bytes <= 0)
        throw std::runtime_error("DNSQuery::send [unable to receive full response]");

    close(sock);

    return DNSResponse {*this, resp_buffer, packet_size};
}
