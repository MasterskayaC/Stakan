#include "client_session_manager.h"

void ClientSessionManager::add_session(ClientId id, SessionPtr session) {
    sessions_[id] = session;
}

void ClientSessionManager::remove_session(ClientId id) {
    sessions_.erase(id);
}

SessionPtr ClientSessionManager::get_session(ClientId id) const {
    auto it = sessions_.find(id);
    if (it == sessions_.end()) {
        return SessionPtr{};
    }
    return it->second;
}

size_t ClientSessionManager::size() const {
    return sessions_.size();
}

std::vector<SessionPtr> ClientSessionManager::get_all_sessions() const {
    std::vector<SessionPtr> result;
    result.reserve(sessions_.size());
    for (const auto& pair : sessions_) {
        result.push_back(pair.second);
    }
    return result;
}