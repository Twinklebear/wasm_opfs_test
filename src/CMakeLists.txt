add_executable(wgpu_app main.cpp)

set_target_properties(wgpu_app PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED
                                                          ON)

if(EMSCRIPTEN)
  target_link_options(
    wgpu_app
    PRIVATE
    "SHELL: -sENVIRONMENT='web,worker'"
    -sEXPORT_ES6
    -sEXPORT_NAME=WGPUApp
    -sUSE_WEBGPU=1
    -sALLOW_MEMORY_GROWTH=1
    -sINVOKE_RUN=0
    -sWASMFS
    -pthread
    -sPTHREAD_POOL_SIZE=2
    -sEXIT_RUNTIME=0
    # RelWithDebInfo build flags, enable separate dwarf to reduce wasm size
    $<$<CONFIG:RELWITHDEBINFO>:-gseparate-dwarf=${CMAKE_CURRENT_BINARY_DIR}/wgpu_app.dwarf>
    $<$<CONFIG:RELWITHDEBINFO>:-g>
    $<$<CONFIG:RELWITHDEBINFO>:-O2>
    # Debug build flags
    $<$<CONFIG:DEBUG>:-fwasm-exceptions>
    $<$<CONFIG:DEBUG>:-g>
    $<$<CONFIG:DEBUG>:-O0>
    # Exported Emscripten runtime methods
    "SHELL:-sEXPORTED_RUNTIME_METHODS='[\"callMain\"]'")

  option(ENABLE_MEMORY_PROFILER "Enable Emscripten's memory profiler" OFF)
  if(ENABLE_MEMORY_PROFILER)
    target_link_options(wgpu_app PRIVATE --memoryprofiler)
  endif()

  option(ENABLE_CPU_PROFILER "Enable Emscripten's CPU profiler" OFF)
  if(ENABLE_CPU_PROFILER)
    target_link_options(wgpu_app PRIVATE --cpuprofiler)
  endif()

  option(ENABLE_THREAD_PROFILER "Enable Emscripten's thread profiler" OFF)
  if(ENABLE_THREAD_PROFILER)
    target_link_options(wgpu_app PRIVATE --threadprofiler)
  endif()

  if(ENABLE_MEMORY_PROFILER
     OR ENABLE_CPU_PROFILER
     OR ENABLE_THREAD_PROFILER)
    target_compile_options(wgpu_app PRIVATE $<$<CONFIG:RELEASE>:--profiling>)
    target_link_options(wgpu_app PRIVATE $<$<CONFIG:RELEASE>:--profiling>)
  endif()

  set(WEB_OUT_DIR ${PROJECT_SOURCE_DIR}/web/src/cpp)

  add_custom_command(
    DEPENDS wgpu_app
    OUTPUT ${WEB_OUT_DIR}/wgpu_app.js ${WEB_OUT_DIR}/wgpu_app.wasm
    COMMAND cmake -E make_directory ${WEB_OUT_DIR}
    COMMAND cmake -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/wgpu_app.js
            ${CMAKE_CURRENT_BINARY_DIR}/wgpu_app.wasm ${WEB_OUT_DIR})

  add_custom_target(
    copy_wasm_to_app ALL
    DEPENDS ${WEB_OUT_DIR}/wgpu_app.js ${WEB_OUT_DIR}/wgpu_app.wasm
    COMMENT "Copying wasm build to ${WEB_OUT_DIR}")
endif()
