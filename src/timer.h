#pragma once

#include <chrono>
#include <iostream>
#include <string>

struct Timer {
    using clock_t = std::chrono::high_resolution_clock;
    using time_point_t = std::chrono::time_point<clock_t>;
    
    std::unordered_map<std::string, double> total_times;
    std::unordered_map<std::string, time_point_t> start_times;
    
    void start(const std::string& name) {
        start_times[name] = clock_t::now();
    }
    
    void end(const std::string& name) {
        auto end_time = clock_t::now();
        auto start_time = start_times[name];
        double duration = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        total_times[name] += duration;
    }
    
    void report() {
        std::cout << "\n=== Performance Report ===" << std::endl;
        double total_duration = 0.0;
        for (const auto& [name, duration] : total_times) {
            total_duration += duration;
        }
        for (const auto& [name, duration] : total_times) {
            double percentage = (duration / total_duration) * 100.0;
            fmt::print("{}: {:.6f} ms ({:.6f}%)\n", name, duration, percentage);
        }
    }

    void reset() {
        total_times.clear();
        start_times.clear();
    }
};