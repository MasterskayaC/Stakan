#pragma once

#include "../bid_ask_snaphot_interface/bid_ask_interface.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>

/**
 * @brief This is a base serialization class for further inheritance from it.
 */

template <typename Obj>
class SerializeObject {
public:
    virtual ~SerializeObject() = default;

    /**
     * @brief Template serialize function
     * @param Template object
     * @return std::vector<char>
     */
    virtual std::vector<char> Serialize(const Obj& obj) = 0;

    /**
     * @brief Template deserialize function
     * @param std::vector<char>
     * @return Template object
     */
    virtual Obj Deserialize(const std::vector<char>& data) = 0;
};

/**
 * @brief Class to de/serialize Snapshot object
 */

class SerializeSnapshot :public SerializeObject<common::Snapshot> {
public:

    /**
     * @brief Serialize Snapshot object
     * @param snapshot
     * @return std::vector<char>
     */
    std::vector<char> Serialize(const common::Snapshot& snapshot) override;

    /**
     * @brief Deserialize Snapshot object
     * @param std::vector<char>
     * @return snapshot
     */
    common::Snapshot Deserialize(const std::vector<char>& data) override;
};

