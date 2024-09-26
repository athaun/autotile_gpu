file(REMOVE_RECURSE
  "ShaderLogisticRegression.hpp"
  "ShaderOpMult.hpp"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/kp_shader.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
