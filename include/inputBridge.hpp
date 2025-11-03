#pragma once
#include "scePadSettings.hpp"
#include <unordered_map>
#include <string>
#include <mutex>

struct InputBridgeState {
    std::unordered_map<std::string, float> analog;
    std::unordered_map<std::string, bool>  digital;
};

class InputBridge {
public:
    static InputBridge& instance();

    void updateFromPs5(const s_ScePadData& state, int index);
    InputBridgeState getState(int index);

private:
    InputBridge() = default;
    InputBridge(const InputBridge&) = delete;
    InputBridge& operator=(const InputBridge&) = delete;

    std::mutex m_mutex;
    InputBridgeState m_states[4];
};
