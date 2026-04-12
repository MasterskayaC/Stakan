#pragma once

#include <atomic>
#include <optional>
#include <shared_mutex>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include "bid_ask_interface.h"
#include "logger.h"

namespace server {
    class OrderBook {
    public:
        /// @brief Добавляет новую заявку в контейнер bid-ордера.
        void NewBid(common::Order order);
        /// @brief Добавляет новую заявку в контейнер ask-ордера.
        void NewAsk(common::Order order);
        /// @brief Удаляет bid-заявку по её идентификатору.
        void CancelBid(common::ID order_id);
        /// @brief Удаляет ask-заявку по её идентификатору.
        void CancelAsk(common::ID order_id);
        /// @brief Обновляет существующую bid-заявку, сохраняя её идентификатор.
        void ReplaceBid(common::Order old_order, common::Order new_order);
        /// @brief Обновляет существующую ask-заявку, сохраняя её идентификатор.
        void ReplaceAsk(common::Order old_order, common::Order new_order);

        /// @brief Возвращает текущий снапшот topN bid/ask заявок, если он еще не был отправлен.
        /// @details Метод возвращает новый снапшот только после успешной операции,
        /// изменившей topN заявок. Повторный вызов без таких изменений вернет std::nullopt.
        [[nodiscard]] std::optional<common::Snapshot> GetTopSnapshot() const;

        /// @brief Возвращает лучшую bid-заявку.
        [[nodiscard]] common::Order BestBid() const;
        /// @brief Возвращает лучшую ask-заявку.
        [[nodiscard]] common::Order BestAsk() const;

    private:
        struct BidComparator {
            bool operator()(const common::Order& a, const common::Order& b) const {
                return a.price > b.price
                    || (a.price == b.price && a.quantity > b.quantity)
                    || (a.price == b.price && a.quantity == b.quantity && a.id < b.id);
            }
        };

        struct AskComparator {
            bool operator()(const common::Order& a, const common::Order& b) const {
                return a.price < b.price
                    || (a.price == b.price && a.quantity > b.quantity)
                    || (a.price == b.price && a.quantity == b.quantity && a.id < b.id);
            }
        };

        template <typename Comparator>
        using PriceIndex = boost::multi_index::ordered_non_unique<
            boost::multi_index::identity<common::Order>,
            Comparator
        >;

        using IdIndex = boost::multi_index::hashed_unique<
            boost::multi_index::member<common::Order, common::ID, &common::Order::id>
        >;

        template <typename Comparator>
        // то же что set (сортировка по цене) + map <Id, it>, только атомарный + надежнее
        using OrderContainer = boost::multi_index::multi_index_container<
            common::Order,
            boost::multi_index::indexed_by<
                PriceIndex<Comparator>,
                IdIndex
            >
        >;

        using BidContainer = OrderContainer<BidComparator>;
        using AskContainer = OrderContainer<AskComparator>;

        /// @brief Проверяет, входит ли заявка в первые @p top_n элементов индекса.
        /// @tparam Index Тип упорядоченного индекса контейнера (так как у нас два контейнера: BidContainer и AskContainer)
        /// @param index Индекс, в котором проверяется принадлежность к topN.
        /// @param order_id Идентификатор заявки.
        /// @param top_n Количество верхних элементов, которые считаются значимыми.
        /// @return true, если заявка присутствует в первых @p top_n элементах.
        template <typename Index>
        [[nodiscard]] static bool IsInTopN(const Index& index, common::ID order_id, size_t top_n);

        /// @brief Строит новый снапшот topN bid/ask заявок на текущем состоянии книги.
        /// @return Новый снапшот, который будет отдан вызывающему коду.
        [[nodiscard]] common::Snapshot BuildNewSnapshot() const;
        void AllowBuildNewSnapshot();

        BidContainer bids_;
        AskContainer asks_;
        
        mutable std::atomic<bool> is_ready_new_snapshot_{true};

        // мьютексы для потокобезопасности
        mutable std::shared_mutex bids_mutex_;
        mutable std::shared_mutex asks_mutex_;
    };
}
