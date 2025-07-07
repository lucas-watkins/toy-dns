#include <iostream>
#include "DNSQuery.hpp"

int main() {
    DNSQuery packet{56130, Opcode::StandardQuery};

    packet.recursive(true);
    packet.record_type(RecordType::A);
    packet.name("lucaspowered.com");

    packet.send("1.1.1.1");
}