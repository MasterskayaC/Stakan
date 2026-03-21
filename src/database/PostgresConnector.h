#pragma once

#include <memory>
#include <pqxx/pqxx>

#include "IDBConnector.h"

class PostgresConnector : public IDBConnector {
public:
    PostgresConnector();

    PostgresConnector(const PostgresConnector &other) = delete;

    PostgresConnector &operator =(const PostgresConnector &other) = delete;

    PostgresConnector(PostgresConnector &&other) noexcept;

    PostgresConnector &operator =(PostgresConnector &&other) noexcept;

    ~PostgresConnector() override;

public:
    bool Connect(const DBConfig &cfg) override;

    [[nodiscard]] bool IsConnected() const override;

    bool Disconnect() override;

    void SaveBid(const Order &order, DBCallback callback) override;

    void SaveAsk(const Order &order, DBCallback callback) override;

    void SaveSnapshot(const Snapshot &order, DBCallback callback) override;

    bool Flush(int timeoutMs) override;

    [[nodiscard]] size_t PendingCount() const override;

private:
    std::unique_ptr<pqxx::connection> conn_;
    size_t maxQueueSize_;
    int flushTimeoutMs_;
};
