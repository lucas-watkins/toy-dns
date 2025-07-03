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

void DNSQuery::name(std::string name) {
    _name = std::move(name);
}

void push_16_t(std::stringstream &os, const std::uint16_t num) {
    static_assert(std::is_same<std::uint8_t, unsigned char>());

    const std::uint8_t high {static_cast<std::uint8_t>((num & 0xFF00) >> 8)};
    const std::uint8_t low {static_cast<std::uint8_t>(num & 0x00FF)};
    os << high << low;
}

void DNSQuery::send(std::string_view addr) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_port = htons(53);
    address.sin_addr.s_addr = inet_addr(addr.data());

    // No blocking
    //fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK);

    connect(sock, reinterpret_cast<sockaddr *>(&address), sizeof(address));

    std::stringstream stream;
    //stream << std::hex << std::setfill('0') << std::setw(4);
    push_16_t(stream, _id);
    push_16_t(stream, _flags);
    push_16_t(stream, 1); // Question Count
    push_16_t(stream, 0); // ANCOUNT
    push_16_t(stream, 0); // NSCOUNT
    push_16_t(stream, 0); // ARCOUNT

    size_t pos;

    // add input validation here

    // copy to prevent modifying real string
    std::string name {_name + '.'};

    while ((pos = name.find('.')) != std::string::npos) {
        std::string token = name.substr(0, pos);
        assert(token.size() <= 255); // Max token length as length param is 1 byte

        stream << static_cast<char>(token.size());
        stream << token;

        name.erase(0, pos + 1);
    }

    stream << static_cast<char>(0);  // Null terminating character
    push_16_t(stream, static_cast<std::uint16_t>(_record_type));
    push_16_t(stream, 1); //Query is class in

    const std::string data {stream.str()};
    const char *pbuf = data.c_str();

    ssize_t buflen (data.size());

    while (buflen > 0) {
        ssize_t num_bytes = ::send(sock, pbuf, buflen, 0);
        std::cout << "Sent: " << num_bytes << std::endl;
        pbuf += num_bytes;
        buflen -= num_bytes;
    }


    std::vector<unsigned char> buffer(BUFSIZ); // Max packet size
    ssize_t bytes_to_read {recv(sock, &buffer[0], buffer.size(), MSG_PEEK)};
    ssize_t packet_size {bytes_to_read};

    std::cout << "To Read: " << bytes_to_read << std::endl;

    do {
        ssize_t num_bytes = recv(sock, &buffer[0], buffer.size(), MSG_PEEK);
        std::cout << "Received: " << num_bytes << std::endl;
        bytes_to_read -= num_bytes;
    } while (bytes_to_read > 0);

    std::cout << "DONE" << std::endl;

    // raw packet
    std::cout << std::hex;
    for (auto it {buffer.begin()}; it != buffer.begin() + packet_size; ++it) {
        std::cout << static_cast<int>(*it);
    }
    close(sock);
}