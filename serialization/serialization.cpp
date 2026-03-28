#include "serialization.h"

std::vector<char> SerializeSnapshot::Serialize(const common::Snapshot& snapshot) {
    try {
        std::ostringstream out(std::ios::binary);
        {
            boost::archive::binary_oarchive oa(out);
            oa << snapshot;
        }
                std::string res = out.str();
        return std::vector<char>(res.begin(), res.end());
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

        std::string buffer(data.data(), data.size());
        std::istringstream iss(buffer, std::ios::binary);

        common::Snapshot snapshot;

        boost::archive::binary_iarchive ia(iss);
        ia >> snapshot;

        return snapshot;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Deserialization error: " + std::string(e.what()));
    }


}
