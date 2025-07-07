//
// Created by Lucas Watkins on 7/1/25.
//

#include "DNSQuery.hpp"

/*
 * |QR|OPCODE|0|TC|RD|0|000|0000|
 * QR (Query) -> 1 bit
 * OPCODE -> 4 bits
 * TC (Truncated) -> 1 bit (just retry over tcp)
 * RD (Recursion Request) -> 1 bit
 *
 * https://www.iana.org/assignments/dns-parameters/dns-parameters.xhtml
 */

Opcode DNSQuery::opcode() const {
    return _opcode;
}

std::uint8_t DNSQuery::flags() const {
    return _flags;
}

void DNSQuery::recursive(const bool recursive) {
    // Magic numbers to set recursion bit in dns request
    // https://courses.cs.duke.edu/fall16/compsci356/DNS/DNS-primer.pdf
    if (recursive) {
        _flags |= 256;
    } else {
        _flags &= 254;
    }
}

void DNSQuery::record_type(const RecordType r) {
    _record_type = r;
}

void DNSQuery::name(const std::string &name) {
    _name = name;
}

void push_16_t(std::stringstream &os, const std::uint16_t num) {
    static_assert(std::is_same<std::uint8_t, unsigned char>());

    const std::uint8_t high {static_cast<std::uint8_t>((num & 0xFF00) >> 8)};
    const std::uint8_t low {static_cast<std::uint8_t>(num & 0x00FF)};
    os << high << low;
}

std::string DNSQuery::build_query() const {
    std::stringstream stream;

    push_16_t(stream, _id);
    push_16_t(stream, _flags);
    push_16_t(stream, 1); // Question Count
    push_16_t(stream, 0); // ANCOUNT
    push_16_t(stream, 0); // NSCOUNT
    push_16_t(stream, 0); // ARCOUNT

    std::vector<std::string> tokens;
    std::string token;

    for (auto iter = _name.begin(); iter != _name.end(); std::advance(iter, 1)) {
        if (*iter == '.') {
            tokens.push_back(token);
            token.clear();
        } else if (_name.end() - iter == 1) { // last item
            token += *iter;
            tokens.push_back(token);
        }
        else {
            token += *iter;
        }
    }

    for (const auto &t : tokens) {
        stream << static_cast<char>(t.size()) << t;
    }

    stream << static_cast<char>(0);  // Null terminating character
    push_16_t(stream, static_cast<std::uint16_t>(_record_type));
    push_16_t(stream, 1); //Query is class in

    return stream.str();
}

void DNSQuery::send(const std::string_view addr) const {
    const int sock = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_port = htons(53);
    address.sin_addr.s_addr = inet_addr(addr.data());

    if (connect(sock, reinterpret_cast<sockaddr *>(&address), sizeof(address))) {
        throw std::runtime_error("DNSQuery::send [connect failed]");
    }

    const std::string data {build_query()};
    const char *pbuf = data.c_str();

    size_t buflen {data.size()};

    while (buflen > 0) {
        ssize_t num_bytes = ::send(sock, pbuf, buflen, 0);
        std::cout << "Sent: " << num_bytes << std::endl;
        pbuf += num_bytes;
        buflen -= num_bytes;
    }


    std::vector<unsigned char> buffer(BUFSIZ); // Max packet size
    const ssize_t packet_size {recv(sock, &buffer[0], buffer.size(), MSG_PEEK)};

    std::cout << "To Read: " << packet_size << std::endl;

    // recv consumes one datagram regardless if it is truncated
    const ssize_t num_bytes = recv(sock, &buffer[0], buffer.size(), MSG_PEEK);
    std::cout << "Received: " << num_bytes << std::endl;


    std::cout << "DONE" << std::endl;

    // raw packet
    std::cout << std::hex;
    for (auto it {buffer.begin()}; it != buffer.begin() + packet_size; ++it) {
        std::cout << static_cast<int>(*it);
    }
    close(sock);
}