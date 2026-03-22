/**
 * @file IDBConnector.h
 * @brief Database connector interface
 */

#pragma once
#include <any>
#include <functional>
#include <string>
#include <variant>
#include <vector>

/**
 * @brief Database operation result with variant (Error/any)
 *
 * @details Holds either an error string or success data of any type.
 *          Use IsSuccess() to check result, GetData<T>() to retrieve data.
 */
class DBResult {
public:
    /// @brief Construct empty success result (for void operations)
    DBResult() : result_(std::monostate{}) {}

    /// @brief Construct error result
    explicit DBResult(std::string err) : result_(std::move(err)) {}

    /// @brief Construct success result with data
    explicit DBResult(std::any data) : result_(std::move(data)) {}

    /// @brief Retrieve data of specified type
    /// @tparam T Expected data type (e.g., int64_t, std::vector<Row>)
    /// @return Data cast to type T
    template<typename T>
    T GetData() const {
        return std::any_cast<T>(std::get<std::any>(result_));
    }

    /// @brief Check if operation succeeded
    /// @return true if no error, false if error occurred
    [[nodiscard]] bool IsSuccess() const {
        return !std::holds_alternative<std::string>(result_);
    }

    /// @brief Get error message
    /// @return Error string or empty string if no error
    [[nodiscard]] std::string GetError() const {
        if (std::holds_alternative<std::string>(result_)) {
            return std::get<std::string>(result_);
        }
        return "";
    }

    /// @brief Check if result contains data
    /// @return true if success with data, false otherwise
    [[nodiscard]] bool HasData() const {
        return std::holds_alternative<std::any>(result_);
    }

private:
    std::variant<std::monostate, std::string, std::any> result_;
};

using DBCallback = std::function<void(const DBResult&)>;

/**
 * @brief Asynchronous database connector interface
 *
 * @details Provides async operations with queue management.
 *
 */
class IDBConnector {
public:
    virtual ~IDBConnector() = default;

    /**
     * @brief Connect to database
     * @param conn Connection string (e.g., "host=localhost dbname=mydb user=postgres")
     * @return true on success, false on failure
     */
    virtual bool Connect(const std::string& conn) = 0;

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
     * @brief Execute INSERT, UPDATE, DELETE query asynchronously
     * @param sql SQL query with placeholders ($1, $2, ...)
     * @param params Parameters to substitute into placeholders
     * @param callback Called with DBResult
     *
     * @example
     * db->Execute("UPDATE orders SET status = $1 WHERE id = $2",
     *             {"CANCELLED", "12345"},
     *             [](const DBResult& result) {
     *                 if (result.IsSuccess()) {
     *                     auto rows = result.GetData<int64_t>();
     *                 }
     *             });
     */
    virtual void Execute(const std::string& sql,
                         const std::vector<std::string>& params,
                         DBCallback callback) = 0;

    /**
     * @brief Execute SELECT query asynchronously
     * @param sql SQL query with placeholders ($1, $2, ...)
     * @param params Parameters to substitute into placeholders
     * @param callback Called with DBResult containing std::vector<Row> on success
     *
     * @example
     * db->Query("SELECT id, price FROM orders WHERE client_id = $1",
     *           {"100"},
     *           [](const DBResult& result) {
     *               if (result.IsSuccess()) {
     *                   auto rows = result.GetData<std::vector<Row>>();
     *               }
     *           });
     */
    virtual void Query(const std::string& sql,
                       const std::vector<std::string>& params,
                       DBCallback callback) = 0;

    /**
     * @brief Prepare a statement for later execution
     * @param name Unique name for this prepared statement
     * @param sql SQL query with placeholders ($1, $2, ...)
     *
     * @details Call this once at startup. Prepared statements are faster
     *          and prevent SQL injection.
     *
     * @example
     * db->Prepare("find_order", "SELECT * FROM orders WHERE id = $1");
     */
    virtual void Prepare(const std::string& name, const std::string& sql) = 0;

    /**
     * @brief Execute a prepared statement
     * @param name Name of previously prepared statement
     * @param params Parameters for placeholders
     * @param callback Called with DBResult containing result data
     *
     * @example
     * db->ExecutePrepared("find_order", {"12345"},
     *                     [](const DBResult& result) {
     *                         if (result.IsSuccess()) {
     *                             auto rows = result.GetData<std::vector<Row>>();
     *                         }
     *                     });
     */
    virtual void ExecutePrepared(const std::string& name,
                                 const std::vector<std::string>& params,
                                 DBCallback callback) = 0;


    /**
     * @brief Wait for all pending operations to complete
     * @param timeoutMs Maximum wait time in milliseconds (0 = infinite)
     * @return true if all operations completed, false on timeout
     *
     * @details Call this before shutdown to ensure all data is saved.
     */
    virtual bool Flush(int timeoutMs = 0) = 0;

    /**
     * @brief Get number of pending operations
     * @return Count of operations waiting to be executed
     *
     * @details Useful for monitoring queue size and detecting overload.
     */
    [[nodiscard]] virtual size_t PendingCount() const = 0;
};