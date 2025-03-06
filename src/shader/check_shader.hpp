// my_shader.hpp - Define the compute shader for tile operations

#ifndef MY_GPU_SHADER_HPP
#define MY_GPU_SHADER_HPP

#include <vector>
#include <cstdint>

namespace shader {

// Shader data structures that match our C++ code
struct alignas(16) TileData {
    uint32_t tile_type;
    uint32_t is_locked;
    uint32_t padding1;
    uint32_t padding2;
};

struct alignas(16) GridInfo {
    uint32_t width;
    uint32_t height;
    uint32_t padding1;
    uint32_t padding2;
};

struct alignas(16) Delta {
    uint32_t before_tile_a;
    uint32_t before_tile_b;
    uint32_t after_tile_a;
    uint32_t after_tile_b;
    uint32_t location_a_x;
    uint32_t location_a_y;
    uint32_t location_b_x;
    uint32_t location_b_y;
    uint32_t type; // 0 for attachment, 1 for transition
};

// GLSL compute shader for check_attachment
constexpr const char* CHECK_ATTACHMENT_SHADER = R"(
    Attachment shader code
)";

// GLSL compute shader for check_transitions
constexpr const char* CHECK_TRANSITIONS_SHADER = R"(
    Transition shader code
)";

// Exported SPIR-V binary data
// These would be the compiled versions of the above GLSL shaders
// In a real implementation, you would compile these shaders to SPIR-V
extern const std::vector<uint32_t> CHECK_ATTACHMENT_COMP_SPV;
extern const std::vector<uint32_t> CHECK_TRANSITIONS_COMP_SPV;

}  // namespace shader

#endif // MY_GPU_SHADER_HPP