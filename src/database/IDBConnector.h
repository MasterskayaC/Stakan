/**
 * @file IDBConnector.h
 * @brief Database connector interface
 */

#pragma once
#include <functional>
#include <string>
#include <optional>

struct Order;
struct Snapshot;

/**
 * @brief Database configuration structure
 */
struct DBConfig {
    std::string connection_string;
    size_t max_queue_size = 10000;
    int flush_timeout_ms = 5000;
};

/**
 * @brief Database operation result
 */
class DBResult {
public:
    DBResult() = default;

    explicit DBResult(std::string err) : error_(std::move(err)) {
    }

    /// @return true if operation succeeded
    [[nodiscard]] bool IsSuccess() const { return !error_.has_value(); }

    /// @return error message or empty string
    [[nodiscard]] std::string GetError() const { return error_.value_or(""); }

private:
    std::optional<std::string> error_;
};

using DBCallback = std::function<void(const DBResult &)>;

/**
 * @brief Asynchronous database connector interface
 */
class IDBConnector {
public:
    virtual ~IDBConnector() = default;

    /**
     * @brief Connect to database
     * @param cfg Database configuration
     * @return true on success, false on failure
     */
    virtual bool Connect(const DBConfig &cfg) = 0;

    /**
     * @brief Check connection status
     * @return true if connected to database
     */
    [[nodiscard]] virtual bool IsConnected() const = 0;

    /**
     * @brief Close database connection
     * @return true on success
     */
    virtual bool Disconnect() = 0;

    /**
     * @brief Save bid order asynchronously
     * @param order Order to save
     * @param callback Called when operation completes
     */
    virtual void SaveBid(const Order &order, DBCallback callback) = 0;

    /**
     * @brief Save ask order asynchronously
     * @param order Order to save
     * @param callback Called when operation completes
     */
    virtual void SaveAsk(const Order &order, DBCallback callback) = 0;

    /**
     * @brief Save order book snapshot asynchronously
     * @param snapshot Snapshot to save
     * @param callback Called when operation completes
     */
    virtual void SaveSnapshot(const Snapshot &snapshot, DBCallback callback) = 0;

    /**
     * @brief Wait for all pending operations to complete
     * @param timeoutMs Maximum wait time in milliseconds (0 = infinite)
     * @return true if all operations completed, false on timeout
     */
    virtual bool Flush(int timeoutMs) = 0;

    /**
     * @brief Get number of pending operations
     * @return Count of operations waiting to be executed
     */
    [[nodiscard]] virtual size_t PendingCount() const = 0;
};
