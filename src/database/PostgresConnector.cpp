#include "PostgresConnector.h"

PostgresConnector::PostgresConnector() = default;

PostgresConnector::~PostgresConnector() = default;

bool PostgresConnector::Connect(const DBConfig &cfg) {
    return true;
}

bool PostgresConnector::IsConnected() const {
    return false;
}

bool PostgresConnector::Disconnect() {
    return true;
}

void PostgresConnector::SaveBid(const Order &order, DBCallback callback) {
    if (callback) {
        callback({});
    }
}

void PostgresConnector::SaveAsk(const Order &order, DBCallback callback) {
    if (callback) {
        callback({});
    }
}

void PostgresConnector::SaveSnapshot(const Snapshot &snapshot, DBCallback callback) {
    if (callback) {
        callback({});
    }
}

bool PostgresConnector::Flush(int timeoutMs) {
    return true;
}

size_t PostgresConnector::PendingCount() const {
    return 0;
}
