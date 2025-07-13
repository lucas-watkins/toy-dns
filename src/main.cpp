#include <iostream>
#include "DNSRecord_t.hpp"
#include "DNSQuery.hpp"

int main() {
    constexpr std::string name_to_lookup {"google.com"};

    const DNSQuery packet{56130, name_to_lookup, DNSRecord::A};
    packet.send("8.8.8.8");
}