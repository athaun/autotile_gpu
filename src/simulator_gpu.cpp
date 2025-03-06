// #include "common.h"
// #include "shader/my_shader.hpp"
// #include "simulator.h"
// #include <thread>
// #include <unordered_set>
// #include <vulkan/vulkan.hpp>

// namespace Simulator {

// // Add these new members to the namespace
// kp::Manager kompute_manager;
// std::shared_ptr<kp::Sequence> compute_sequence;
// std::shared_ptr<kp::Algorithm> attachment_algorithm;
// std::shared_ptr<kp::Algorithm> transition_algorithm;

// // Tensor buffers for GPU computation
// std::shared_ptr<kp::TensorT<uint32_t>> grid_tensor;
// std::shared_ptr<kp::TensorT<uint32_t>> grid_info_tensor;
// std::shared_ptr<kp::TensorT<uint32_t>> h_affinity_keys_tensor;
// std::shared_ptr<kp::TensorT<uint32_t>> h_affinity_results_tensor;
// std::shared_ptr<kp::TensorT<uint32_t>> v_affinity_keys_tensor;
// std::shared_ptr<kp::TensorT<uint32_t>> v_affinity_results_tensor;
// std::shared_ptr<kp::TensorT<uint32_t>> h_transition_keys_tensor;
// std::shared_ptr<kp::TensorT<uint32_t>> h_transition_results_tensor;
// std::shared_ptr<kp::TensorT<uint32_t>> v_transition_keys_tensor;
// std::shared_ptr<kp::TensorT<uint32_t>> v_transition_results_tensor;
// std::shared_ptr<kp::TensorT<uint32_t>> possible_deltas_tensor;

// // Initialize Kompute and related resources
// void initialize_gpu() {
//     // Initialize Kompute manager
//     kompute_manager = kp::Manager();
    
//     // Create tensors for grid data initially (will be resized later)
//     grid_tensor = kompute_manager.tensorT<uint32_t>(std::vector<uint32_t>(grid.width * grid.height, 0));
    
//     // Create tensor for grid dimensions
//     grid_info_tensor = kompute_manager.tensorT<uint32_t>({
//         static_cast<uint32_t>(grid.width),
//         static_cast<uint32_t>(grid.height),
//         0, 0  // Padding
//     });
    
//     // Create tensors for rules
//     // For simplicity, we'll initialize these with placeholder values and update them later
//     h_affinity_keys_tensor = kompute_manager.tensorT<uint32_t>(std::vector<uint32_t>(100, 0));
//     h_affinity_results_tensor = kompute_manager.tensorT<uint32_t>(std::vector<uint32_t>(50, 0));
//     v_affinity_keys_tensor = kompute_manager.tensorT<uint32_t>(std::vector<uint32_t>(100, 0));
//     v_affinity_results_tensor = kompute_manager.tensorT<uint32_t>(std::vector<uint32_t>(50, 0));
    
//     h_transition_keys_tensor = kompute_manager.tensorT<uint32_t>(std::vector<uint32_t>(100, 0));
//     h_transition_results_tensor = kompute_manager.tensorT<uint32_t>(std::vector<uint32_t>(50, 0));
//     v_transition_keys_tensor = kompute_manager.tensorT<uint32_t>(std::vector<uint32_t>(100, 0));
//     v_transition_results_tensor = kompute_manager.tensorT<uint32_t>(std::vector<uint32_t>(50, 0));
    
//     // Create tensor for possible deltas (with a max count of 1000)
//     // First element is the count, followed by the delta structures
//     // Each delta uses multiple uint32_t values (9 per delta: 8 fields + 1 type)
//     possible_deltas_tensor = kompute_manager.tensorT<uint32_t>(std::vector<uint32_t>(1 + 9 * 1000, 0));
    
//     // Create attachment algorithm
//     std::vector<std::shared_ptr<kp::Memory>> attachment_params = {
//         grid_tensor, 
//         grid_info_tensor,
//         h_affinity_keys_tensor,
//         h_affinity_results_tensor,
//         v_affinity_keys_tensor,
//         v_affinity_results_tensor,
//         possible_deltas_tensor
//     };
    
//     kp::Workgroup attachment_workgroup({
//         static_cast<uint32_t>((grid.width + 15) / 16),
//         static_cast<uint32_t>((grid.height + 15) / 16),
//         1
//     });
    
//     // Set up algorithm with shaders from my_shader.hpp
//     attachment_algorithm = kompute_manager.algorithm(
//         attachment_params, 
//         shader::CHECK_ATTACHMENT_COMP_SPV,
//         attachment_workgroup
//     );
    
//     // Create transition algorithm
//     std::vector<std::shared_ptr<kp::Memory>> transition_params = {
//         grid_tensor, 
//         grid_info_tensor,
//         h_transition_keys_tensor,
//         h_transition_results_tensor,
//         v_transition_keys_tensor,
//         v_transition_results_tensor,
//         possible_deltas_tensor
//     };
    
//     kp::Workgroup transition_workgroup({
//         static_cast<uint32_t>((grid.width + 15) / 16),
//         static_cast<uint32_t>((grid.height + 15) / 16),
//         1
//     });
    
//     transition_algorithm = kompute_manager.algorithm(
//         transition_params, 
//         shader::CHECK_TRANSITIONS_COMP_SPV,
//         transition_workgroup
//     );
    
//     // Create compute sequence
//     compute_sequence = kompute_manager.sequence();
    
//     // Update rule data tensors
//     update_rule_tensors();
// }

// // Update rule data in the GPU tensors
// void update_rule_tensors() {
//     // Convert horizontal affinities to tensor format
//     std::vector<uint32_t> h_aff_keys;
//     std::vector<uint32_t> h_aff_results;
    
//     for (const auto& affinity_pair : horizontal_affinities.rules) {
//         h_aff_keys.push_back(affinity_pair.first.tile_a);
//         h_aff_keys.push_back(affinity_pair.first.tile_b);
        
//         // Pack the result tiles into a single uint32_t
//         uint32_t result = (affinity_pair.second.tile_a << 16) | affinity_pair.second.tile_b;
//         h_aff_results.push_back(result);
//     }
    
//     // Convert vertical affinities to tensor format
//     std::vector<uint32_t> v_aff_keys;
//     std::vector<uint32_t> v_aff_results;
    
//     for (const auto& affinity_pair : vertical_affinities.rules) {
//         v_aff_keys.push_back(affinity_pair.first.tile_a);
//         v_aff_keys.push_back(affinity_pair.first.tile_b);
        
//         uint32_t result = (affinity_pair.second.tile_a << 16) | affinity_pair.second.tile_b;
//         v_aff_results.push_back(result);
//     }
    
//     // Convert horizontal transitions to tensor format
//     std::vector<uint32_t> h_trans_keys;
//     std::vector<uint32_t> h_trans_results;
    
//     for (const auto& transition_pair : horizontal_transitions.rules) {
//         h_trans_keys.push_back(transition_pair.first.tile_a);
//         h_trans_keys.push_back(transition_pair.first.tile_b);
        
//         uint32_t result = (transition_pair.second.to_a << 16) | transition_pair.second.to_b;
//         h_trans_results.push_back(result);
//     }
    
//     // Convert vertical transitions to tensor format
//     std::vector<uint32_t> v_trans_keys;
//     std::vector<uint32_t> v_trans_results;
    
//     for (const auto& transition_pair : vertical_transitions.rules) {
//         v_trans_keys.push_back(transition_pair.first.tile_a);
//         v_trans_keys.push_back(transition_pair.first.tile_b);
        
//         uint32_t result = (transition_pair.second.to_a << 16) | transition_pair.second.to_b;
//         v_trans_results.push_back(result);
//     }
    
//     // Update the tensors if they need to be resized
//     if (h_aff_keys.size() > h_affinity_keys_tensor->size()) {
//         h_affinity_keys_tensor = kompute_manager.tensorT<uint32_t>(h_aff_keys);
//     } else {
//         h_affinity_keys_tensor->setData(h_aff_keys);
//     }
    
//     if (h_aff_results.size() > h_affinity_results_tensor->size()) {
//         h_affinity_results_tensor = kompute_manager.tensorT<uint32_t>(h_aff_results);
//     } else {
//         h_affinity_results_tensor->setData(h_aff_results);
//     }
    
//     if (v_aff_keys.size() > v_affinity_keys_tensor->size()) {
//         v_affinity_keys_tensor = kompute_manager.tensorT<uint32_t>(v_aff_keys);
//     } else {
//         v_affinity_keys_tensor->setData(v_aff_keys);
//     }
    
//     if (v_aff_results.size() > v_affinity_results_tensor->size()) {
//         v_affinity_results_tensor = kompute_manager.tensorT<uint32_t>(v_aff_results);
//     } else {
//         v_affinity_results_tensor->setData(v_aff_results);
//     }
    
//     if (h_trans_keys.size() > h_transition_keys_tensor->size()) {
//         h_transition_keys_tensor = kompute_manager.tensorT<uint32_t>(h_trans_keys);
//     } else {
//         h_transition_keys_tensor->setData(h_trans_keys);
//     }
    
//     if (h_trans_results.size() > h_transition_results_tensor->size()) {
//         h_transition_results_tensor = kompute_manager.tensorT<uint32_t>(h_trans_results);
//     } else {
//         h_transition_results_tensor->setData(h_trans_results);
//     }
    
//     if (v_trans_keys.size() > v_transition_keys_tensor->size()) {
//         v_transition_keys_tensor = kompute_manager.tensorT<uint32_t>(v_trans_keys);
//     } else {
//         v_transition_keys_tensor->setData(v_trans_keys);
//     }
    
//     if (v_trans_results.size() > v_transition_results_tensor->size()) {
//         v_transition_results_tensor = kompute_manager.tensorT<uint32_t>(v_trans_results);
//     } else {
//         v_transition_results_tensor->setData(v_trans_results);
//     }
// }

// // Update the grid tensor with current grid data
// void update_grid_tensor() {
//     // Convert grid data to a vector of uint32_t
//     std::vector<uint32_t> grid_data(grid.width * grid.height);
//     std::memcpy(grid_data.data(), grid.tiles, grid.width * grid.height * sizeof(uint32_t));
    
//     // Update grid info tensor
//     grid_info_tensor->setData({
//         static_cast<uint32_t>(grid.width),
//         static_cast<uint32_t>(grid.height),
//         0, 0  // Padding
//     });
    
//     // Resize grid tensor if needed
//     if (grid_data.size() > grid_tensor->size()) {
//         grid_tensor = kompute_manager.tensorT<uint32_t>(grid_data);
//     } else {
//         grid_tensor->setData(grid_data);
//     }
// }

// // Extract deltas from the GPU tensor
// std::vector<delta_t> extract_possible_deltas() {
//     // Get the delta count
//     std::vector<uint32_t> delta_data = possible_deltas_tensor->vector();
//     uint32_t delta_count = delta_data[0];
    
//     // Clamp to maximum allowed deltas
//     if (delta_count > 1000) {
//         delta_count = 1000;
//     }
    
//     std::vector<delta_t> result;
//     result.reserve(delta_count);
    
//     // Extract deltas from the data
//     for (uint32_t i = 0; i < delta_count; i++) {
//         // Each delta is stored as 9 uint32_t values (8 fields + type)
//         uint32_t base_idx = 1 + i * 9;
        
//         delta_t delta;
//         delta.before.tile_a = delta_data[base_idx];
//         delta.before.tile_b = delta_data[base_idx + 1];
//         delta.after.tile_a = delta_data[base_idx + 2];
//         delta.after.tile_b = delta_data[base_idx + 3];
//         delta.location_a = {
//             static_cast<int>(delta_data[base_idx + 4]),
//             static_cast<int>(delta_data[base_idx + 5])
//         };
//         delta.location_b = {
//             static_cast<int>(delta_data[base_idx + 6]),
//             static_cast<int>(delta_data[base_idx + 7])
//         };
        
//         delta.type = (delta_data[base_idx + 8] == 0) ? 
//             delta_t::Type::ATTACHMENT : delta_t::Type::TRANSITION;
        
//         result.push_back(delta);
//     }
    
//     return result;
// }

// // Function to replace CPU check_attachment and check_transitions
// void check_gpu(const loc_t& location, std::vector<delta_t>& possible_deltas) {
//     // Update GPU tensors with current data
//     update_grid_tensor();
    
//     // Reset possible deltas count to 0
//     std::vector<uint32_t> reset_count = possible_deltas_tensor->vector();
//     reset_count[0] = 0;
//     possible_deltas_tensor->setData(reset_count);
    
//     // Dispatch both compute shaders to find attachments and transitions
//     compute_sequence->reset()
//         // Sync data to device
//         ->record<kp::OpSyncDevice>({
//             grid_tensor, 
//             grid_info_tensor,
//             h_affinity_keys_tensor,
//             h_affinity_results_tensor,
//             v_affinity_keys_tensor,
//             v_affinity_results_tensor,
//             h_transition_keys_tensor,
//             h_transition_results_tensor,
//             v_transition_keys_tensor,
//             v_transition_results_tensor,
//             possible_deltas_tensor
//         })
//         // Run attachment algorithm
//         ->record<kp::OpAlgoDispatch>(attachment_algorithm)
//         // Run transition algorithm
//         ->record<kp::OpAlgoDispatch>(transition_algorithm)
//         // Sync results back
//         ->record<kp::OpSyncLocal>({possible_deltas_tensor})
//         // Execute the sequence
//         ->eval();
    
//     // Extract GPU results
//     std::vector<delta_t> gpu_deltas = extract_possible_deltas();
    
//     // Add GPU results to possible_deltas
//     possible_deltas.insert(possible_deltas.end(), gpu_deltas.begin(), gpu_deltas.end());
// }

// // Modified run_serial to use GPU-accelerated functions
// void run_serial_gpu() {
//     // Initialize the GPU resources
//     initialize_gpu();
    
//     std::vector<delta_t> possible_deltas = std::vector<delta_t>();
//     srand(time(NULL));

//     int ticks = 0;
//     while (true) {
//         process_control_messages();

//         if (sim_state == SimState::PAUSED) {
//             continue;
//         }
        
//         loc_t location = choose_random_location();
        
//         // Use the GPU to check for attachments and transitions
//         check_gpu(location, possible_deltas);

//         choose_delta(possible_deltas);
//         send_deltas(delta_buffer);        
//         apply_deltas(delta_buffer);
//     }
// }

// // Update init function to set up GPU when initializing the system
// void init(std::string system_name) {
//     system_name_cache = "assets/systems/" + system_name;

//     grid = Seed::load(system_name_cache + ".seed");
//     initialize_available_locations();

//     Rules::load(system_name_cache + ".htrans", horizontal_transitions);
//     Rules::load(system_name_cache + ".vtrans", vertical_transitions);

//     Rules::load(system_name_cache + ".haff", horizontal_affinities);
//     Rules::load(system_name_cache + ".vaff", vertical_affinities);

//     Rules::load_name_keys(system_name_cache + ".key", Tile::name_keys);
    
//     // Initialize GPU resources after loading all data
//     initialize_gpu();
// }

// } // namespace Simulator