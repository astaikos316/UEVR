#pragma once

// XRTV: env-gated per-frame AFR eye trace (XRTV_UEVR_EYE_TRACE=1). Off by default; capped at
// 40000 lines. Used to prove that the eye each frame was RENDERED with (CalculateStereoViewOffset)
// matches the eye swapchain PRESENT copies it into (cargame extreme compat, UE 5.8).

#include <atomic>
#include <cstdlib>

#include <spdlog/spdlog.h>

namespace xrtv {
inline bool eye_trace_on() {
    static const bool on = std::getenv("XRTV_UEVR_EYE_TRACE") != nullptr;
    return on;
}

// XRTV_UEVR_EYE_TAG=1 (debug only): eye 0 renders pitched +85 deg, eye 1 -85 deg, and present logs
// the centre pixel of the frame it copies ("K" lines), to prove which rendered eye lands in which
// swapchain. Makes the image unusable in a headset.
inline bool eye_tag_on() {
    static const bool on = std::getenv("XRTV_UEVR_EYE_TAG") != nullptr;
    return on;
}

// Patch 3 (pose update from present) mode. Default: update_hmd_state() (re-publishes the render
// frame count; CloudXR pose misses ~130/150 s). XRTV_UEVR_P3_SLOT=1: locate into the submit slot
// only, without touching the count (measured WORSE: 800-1100 misses/150 s) - experiment only.
inline bool p3_enqueue_on() {
    static const bool on = std::getenv("XRTV_UEVR_P3_SLOT") == nullptr;
    return on;
}

// Set by engine_tick_hook when UEVR's AFR eye is driven from UGameEngine::Tick (no
// UGameViewportClient::Draw hook). Enables the present-parity eye assignment in VR::on_present.
extern std::atomic<bool> g_afr_from_tick;

// XRTV_UEVR_PROJ_TAG=1 (debug only): add a large horizontal off-centre term to eye 1's projection,
// so a screenshot shows whether the engine actually renders with UEVR's per-eye projection.
// The value picks the tagged eye index ("0" or "1"); anything else tags eye 1.
inline bool proj_tag_on() {
    static const bool on = std::getenv("XRTV_UEVR_PROJ_TAG") != nullptr;
    return on;
}

inline int proj_tag_eye() {
    static const int eye = [] { const auto e = std::getenv("XRTV_UEVR_PROJ_TAG"); return (e != nullptr && e[0] == '0') ? 0 : 1; }();
    return eye;
}

// Image-based AFR eye detection (D3D11 extreme-compat present path). XRTV_UEVR_EYE_DETECT=0 disables it.
inline bool eye_detect_on() {
    static const bool on = [] { const auto e = std::getenv("XRTV_UEVR_EYE_DETECT"); return e == nullptr || e[0] != '0'; }();
    return on;
}

// Present index shared by VR::on_present (writer) and the D3D11 present path (reader); same thread.
extern std::atomic<uint64_t> g_present_index;

// One observation: the image presented at `present` was the left eye (true) / right eye (false).
void img_vote_push(uint64_t present, bool left);

// -1 = not enough recent image evidence; else the phase (0: left on even presents, 1: left on odd).
int img_vote_phase(uint64_t now);

inline bool eye_trace_take() {
    static std::atomic<int> budget{40000};
    return eye_trace_on() && budget.fetch_sub(1) > 0;
}
}

#define XRTV_EYE_TRACE(...) do { if (xrtv::eye_trace_take()) { spdlog::info(__VA_ARGS__); } } while (0)

// Always on, low volume: the first 40 calls per site, then every 2000th (~1/min at 36 fps), so a
// normal headset session records the real per-eye geometry. Full rate when XRTV_UEVR_EYE_TRACE is set.
#define XRTV_GEOM_LOG(...) do { static std::atomic<uint32_t> xrtv_n_{0}; const auto xrtv_i_ = xrtv_n_.fetch_add(1);     if (xrtv::eye_trace_take() || xrtv_i_ < 40 || (xrtv_i_ % 2000) == 0) { spdlog::info(__VA_ARGS__); } } while (0)
