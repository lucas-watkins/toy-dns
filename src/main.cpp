#include <cassert>
#include <iostream>
#include "BaseDNSRecord.hpp"
#include "DNSQuery.hpp"

int main() {

    constexpr std::string name_to_lookup { "lucaspowered.com" };

    const DNSQuery packet{56130, name_to_lookup, DNSRecord::A};

    const DNSResponse resp { packet.send("8.8.8.8") };

    for (const auto &record : resp.get_records<ARecord>()) {
        std::cout << record.ip_addr << '\n';
    }
    std::cout << '\n';

    std::cout << resp << '\n';

    return 0;
}