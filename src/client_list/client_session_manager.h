#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

using ClientId = uint64_t;
class Session;
using SessionPtr = std::shared_ptr<Session>;

/**
 * @brief Interface for managing client TCP sessions
 *
 * Provides methods to add, remove, and retrieve session objects associated with clients
 */
class IClientSessionManager {
public:
    virtual ~IClientSessionManager() = default;

    /**
     * @brief Associates a session with a client
     * @param id Client identifier
     * @param session Shared pointer to the session object
     */
    virtual void add_session(ClientId id, SessionPtr session) = 0;

    /**
     * @brief Removes the session associated with a client
     * @param id Client identifier
     */
    virtual void remove_session(ClientId id) = 0;

    /**
     * @brief Returns the total number of active sessions
     * @return Number of sessions currently managed
     */
    virtual size_t size() const = 0;

    /**
     * @brief Retrieves the session for a given client
     * @param id Client identifier
     * @return Shared pointer to the session, or nullptr if not found
     */
    virtual SessionPtr get_session(ClientId id) const = 0;

    /**
     * @brief Retrieves all active sessions
     * @return Vector of shared pointers to all managed sessions
     */
    virtual std::vector<SessionPtr> get_all_sessions() const = 0;
};

/**
 * @brief Concrete implementation of IClientSessionManager using an unordered_map
 *
 * Stores client sessions in a hash map keyed by client ID
 */
class ClientSessionManager : public IClientSessionManager {
private:
    std::unordered_map<ClientId, SessionPtr> sessions_;

public:
    void add_session(ClientId id, SessionPtr session) override;
    void remove_session(ClientId id) override;

    SessionPtr get_session(ClientId id) const override;
    size_t size() const override;
    std::vector<SessionPtr> get_all_sessions() const override;
};
