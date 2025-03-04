#include "thread_communication.h"

template <size_t Capacity>
LockFreeQueue<Capacity>::LockFreeQueue() : buffer(Capacity), head(0), tail(0) {}

template <size_t Capacity>
bool LockFreeQueue<Capacity>::push(const Message& value) {
    size_t current_tail = tail.load(std::memory_order_relaxed);
    size_t next_tail = (current_tail + 1) % Capacity;

    if (next_tail == head.load(std::memory_order_acquire)) {
        return false; // Queue full, drop message
    }

    buffer[current_tail] = value;
    tail.store(next_tail, std::memory_order_release);
    return true;
}

template <size_t Capacity>
std::optional<Message> LockFreeQueue<Capacity>::try_pop() {
    size_t current_head = head.load(std::memory_order_relaxed);
    if (current_head == tail.load(std::memory_order_acquire)) {
        return std::nullopt; // Queue empty
    }

    Message value = buffer[current_head];
    head.store((current_head + 1) % Capacity, std::memory_order_release);
    return value;
}

// Explicit instantiation for global queue
template class LockFreeQueue<256>;
LockFreeQueue<256> simulator_message_queue;
LockFreeQueue<256> frontend_message_queue;
