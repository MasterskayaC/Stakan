#include "client_list.h"

#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "bid_ask_interface.h"
#include "command_queue.h"

// клиенты, их сессии, заявки bid/ask и подписка на рассылку.

class ClientList : public IClientList {
private:
    // Заявка в списке клиента: указатель и позиция id в списке порядка.
    struct OrderEntry {
        std::shared_ptr<common::Order> order;
        std::list<common::ID>::iterator it;
    };

    // Состояние одного клиента: сессия, заявки и флаг подписки.
    struct ClientContext {
        SessionPtr session;

        std::unordered_map<common::ID, OrderEntry> bids;
        std::unordered_map<common::ID, OrderEntry> asks;
        std::list<common::ID> bids_in_order;
        std::list<common::ID> asks_in_order;
        bool subscribed = false;
    };

    std::unordered_map<ClientId, ClientContext> clients_;
    std::unordered_map<const Session*, ClientId> session_to_client_;
    mutable std::mutex mutex_;

    // Добавляет заявку на выбранную сторону (bids или asks): карта по id и список порядка.
    static void add_order_to_side(std::unordered_map<common::ID, OrderEntry>& by_id,
                                  std::list<common::ID>& in_order,
                                  std::shared_ptr<common::Order> order) {
        in_order.push_back(order->id);
        auto last = std::prev(in_order.end());
        by_id[order->id] = {std::move(order), last};
    }

    // Удаляет заявку по order_id или последнюю в списке порядка; при отсутствии id — сообщение в консоль.
    static void remove_order_from_side(ClientId id,
                                       std::unordered_map<common::ID, OrderEntry>& by_id,
                                       std::list<common::ID>& in_order,
                                       std::optional<common::ID> order_id) {
        if (by_id.empty()) {
            return;
        }
        if (order_id) {
            auto order_it = by_id.find(order_id.value());
            if (order_it == by_id.end()) {
                std::cout << "Client with id: " << id << " doesn't have and order with id: " << order_id.value()
                          << '\n';
                return;
            }
            in_order.erase(order_it->second.it);
            by_id.erase(order_it);
        } else {
            const common::ID last_id = in_order.back();
            in_order.pop_back();
            by_id.erase(last_id);
        }
    }

public:
    // Привязывает сессию к клиенту; при повторном id старая сессия снимается из session_to_client_.
    void add_session(ClientId id, SessionPtr session) override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.find(id);
        if (it != clients_.end() && it->second.session) {
            session_to_client_.erase(it->second.session.get());
            std::cout << "Client with id: " << id << " was recreated\n";
            // Можно заменить на общий логгер.
        }
        clients_[id].session = std::move(session);
        if (clients_[id].session) {
            session_to_client_[clients_[id].session.get()] = id;
        }
    }

    // Удаляет клиента и его сессию из обоих отображений.
    void remove_session(ClientId id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.find(id);
        if (it != clients_.end() && it->second.session) {
            session_to_client_.erase(it->second.session.get());
            std::cout << "Session with id: " << id << " was deleted\n";
        }
        clients_.erase(id);
        std::cout << "Client with id: " << id << " was deleted\n";
    }

    // Возвращает сессию клиента (под мьютексом после проверки существования).
    SessionPtr get_session(ClientId id) const override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.at(id);
        return it.session;
    }

    // Число записей в clients_ (не обязательно все с ненулевой сессией).
    size_t size() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return clients_.size();
    }

    // Все ненулевые сессии из контекстов клиентов.
    std::vector<SessionPtr> get_all_sessions() const override {
        std::vector<SessionPtr> tmp;
        std::lock_guard<std::mutex> lock(mutex_);
        tmp.reserve(clients_.size());
        for (const auto& [id, ctx] : clients_) {
            (void)id;
            if (ctx.session) {
                tmp.push_back(ctx.session);
            }
        }
        return tmp;
    }

    // Проверка наличия клиента по id.
    bool has_client(ClientId id) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return clients_.find(id) != clients_.end();
    }


    // Добавляет bid в карту и хвост списка порядка.
    void add_bid(ClientId id, std::shared_ptr<common::Order> bid) override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        ClientContext& ctx = clients_.at(id);
        add_order_to_side(ctx.bids, ctx.bids_in_order, std::move(bid));
    }

    // Добавляет ask в карту и хвост списка порядка.
    void add_ask(ClientId id, std::shared_ptr<common::Order> ask) override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        ClientContext& ctx = clients_.at(id);
        add_order_to_side(ctx.asks, ctx.asks_in_order, std::move(ask));
    }

    // order_id задан — снять конкретную заявку; иначе — последнюю в порядке поступления.
    void remove_bid(ClientId id, std::optional<common::ID> order_id = std::nullopt) override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        ClientContext& ctx = clients_.at(id);
        remove_order_from_side(id, ctx.bids, ctx.bids_in_order, order_id);
    }

    // Аналогично remove_bid для стороны ask.
    void remove_ask(ClientId id, std::optional<common::ID> order_id = std::nullopt) override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        ClientContext& ctx = clients_.at(id);
        remove_order_from_side(id, ctx.asks, ctx.asks_in_order, order_id);
    }

    // Заявки в порядке bids_in_order / asks_in_order.
    std::vector<std::shared_ptr<common::Order>> get_bids(ClientId id) const override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::vector<std::shared_ptr<common::Order>> tmp;
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.at(id);
        for (common::ID order_id : it.bids_in_order) {
            tmp.push_back(it.bids.at(order_id).order);
        }
        return tmp;
    }

    std::vector<std::shared_ptr<common::Order>> get_asks(ClientId id) const override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::vector<std::shared_ptr<common::Order>> tmp;
        std::lock_guard<std::mutex> lock(mutex_);
        const ClientContext& it = clients_.at(id);
        for (common::ID order_id : it.asks_in_order) {
            tmp.push_back(it.asks.at(order_id).order);
        }
        return tmp;
    }

    // Флаг клиента на обновления стакана.
    bool is_subscribed(ClientId id) const override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        const ClientContext& it = clients_.at(id);
        return it.subscribed;
    }

    void subscribe(ClientId id) override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        ClientContext& it = clients_.at(id);
        it.subscribed = true;
    }

    void unsubscribe(ClientId id) override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = clients_.at(id);
        it.subscribed = false;
    }

    // Идентификаторы клиентов с subscribed == true.
    std::vector<ClientId> get_subscribed_clients() const override {
        std::vector<ClientId> tmp;
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& [id, ctx] : clients_) {
            if (ctx.subscribed) {
                tmp.push_back(id);
            }
        }
        return tmp;
    }

    // Сессии клиентов (может быть nullptr, если сессии нет).
    std::vector<SessionPtr> get_subscribed_sessions() const override {
        std::lock_guard lck(mutex_);
        std::vector<SessionPtr> res;
        res.reserve(clients_.size());
        for (auto [client_id, client_context] : clients_) {
            if (client_context.subscribed) {
                res.push_back(client_context.session);
            }
        }
        return res;
    }

    // Отправляет сообщение всем подписанным сессиям.
    void broadcast_to_subscribed(const std::vector<char>& message) override {
        std::vector<SessionPtr> sub_sessions = get_subscribed_sessions();
        for (SessionPtr s : sub_sessions) {
            s->SendMsg(message);
        }
    }

    // Отправляет сообщение одному клиенту по id (через его сессию).
    void broadcast_to_certain(ClientId id, const std::vector<char>& message) override {
        SessionPtr s = get_session(id);
        s->SendMsg(message);
    }

    // Обратное отображение: указатель сессии -> id клиента.
    std::optional<ClientId> find_client_id_by_session(const Session* session) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = session_to_client_.find(session);
        if (it == session_to_client_.end()) {
            return std::nullopt;
        }
        return it->second;
    }
};

// Фабрика: единственная точка создания реализации списка клиентов.
std::unique_ptr<IClientList> makeClientList() {
    return std::make_unique<ClientList>();
}
