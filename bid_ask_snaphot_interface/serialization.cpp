#include "serialization.h"

std::vector<char> SerializeSnapshot::Serialize(const common::Snapshot& snapshot) {
    constexpr size_t size = sizeof(common::Snapshot);
    try {
        std::vector<char> buffer(size);
        std::memcpy(buffer.data(), &snapshot, size);
        return buffer;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Serialization error: " + std::string(e.what()));
    }
}

common::Snapshot SerializeSnapshot::Deserialize(const std::vector<char>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Empty data");
    }

    try{
        common::Snapshot result;
        std::memcpy(&result, data.data(), sizeof(common::Snapshot));
        return result;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Deserialization error: " + std::string(e.what()));
    }


}
