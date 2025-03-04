#pragma once

#include <vector>
#include <atomic>
#include <optional>
#include "common.h"

struct Message {
    enum MessageType { PAUSE, RUN, STEP, TILE_UPDATE, EXIT, CUSTOM } type;
    std::string content;
    Simulator::loc_t location;
    tile_t value;
};

template <size_t Capacity>
class LockFreeQueue {
public:
    LockFreeQueue();

    bool push(const Message& value);
    std::optional<Message> try_pop();

private:
    std::vector<Message> buffer;
    std::atomic<size_t> head, tail;
};
 
extern LockFreeQueue<256> simulator_message_queue;
extern LockFreeQueue<256> frontend_message_queue;

