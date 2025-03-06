#pragma once

#include <atomic>
#include <memory>
#include <optional>

#include "common.h"

struct Message {
    enum MessageType { PAUSE, RUN, STEP, TILE_UPDATE, EXIT, CUSTOM, RESET, RESIZE_CONFIRMATION } type;
    std::string content;
    Simulator::loc_t location;
    tile_t value;
};

class LockFreeQueue {
private:
    struct Node {
        Message data;
        std::atomic<Node*> next;
        Node(const Message& msg) : data(msg), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    LockFreeQueue() {
        Node* dummy = new Node({Message::CUSTOM});
        head.store(dummy);
        tail.store(dummy);
    }

    ~LockFreeQueue() {
        while (try_pop()) {} // Free all nodes
        delete head.load();
    }

    void push(const Message& msg) {
        Node* new_node = new Node(msg);
        Node* prev_tail;

        do {
            prev_tail = tail.load(std::memory_order_acquire);
        } while (!tail.compare_exchange_weak(prev_tail, new_node, std::memory_order_release));

        prev_tail->next.store(new_node, std::memory_order_release);
    }

    std::optional<Message> try_pop() {
        Node* first;
        Node* next;

        do {
            first = head.load(std::memory_order_acquire);
            next = first->next.load(std::memory_order_acquire);
            if (!next) return std::nullopt; // Empty queue
        } while (!head.compare_exchange_weak(first, next, std::memory_order_release));

        Message result = next->data;
        delete first; // Free old dummy node
        return result;
    }
};

extern LockFreeQueue simulator_message_queue;
extern LockFreeQueue frontend_message_queue;