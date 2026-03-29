#include "serialization.h"

std::vector<char> SerializeSnapshot::Serialize(const common::Snapshot& snapshot) {

    try {
        std::vector<char> buffer;
        {
            boost::iostreams::back_insert_device<std::vector<char>> device(buffer);
            boost::iostreams::stream<boost::iostreams::back_insert_device<std::vector<char>>> os(device);

            boost::archive::binary_oarchive oa(os);
            oa << snapshot;
        }            
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
        boost::iostreams::basic_array_source<char> source(data.data(), data.size());
        boost::iostreams::stream<boost::iostreams::basic_array_source<char>> is(source);

        common::Snapshot snapshot;

        boost::archive::binary_iarchive ia(is);
        ia >> snapshot;

        return snapshot;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Deserialization error: " + std::string(e.what()));
    }


}
