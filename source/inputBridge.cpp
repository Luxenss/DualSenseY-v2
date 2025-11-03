#include "inputBridge.hpp"

InputBridge& InputBridge::instance() {
    static InputBridge bridge;
    return bridge;
}

void InputBridge::updateFromPs5(const s_ScePadData& state, int index) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto& st = m_states[index];

    // --- Analog datas ---
    st.analog["left_stick_x"] = state.LeftStick.X / 255.0f;
    st.analog["left_stick_y"] = state.LeftStick.Y / 255.0f;
    st.analog["right_stick_x"] = state.RightStick.X / 255.0f;
    st.analog["right_stick_y"] = state.RightStick.Y / 255.0f;

    st.analog["left_trigger"] = state.L2_Analog / 255.0f;
    st.analog["right_trigger"] = state.R2_Analog / 255.0f;

    // Gyroscope
    st.analog["gyro_x"] = state.angularVelocity.x / 1000.0f;
    st.analog["gyro_y"] = state.angularVelocity.y / 1000.0f;
    st.analog["gyro_z"] = state.angularVelocity.z / 1000.0f;

    // Accelerometer
    st.analog["accel_x"] = state.acceleration.x / 10000.0f;
    st.analog["accel_y"] = state.acceleration.y / 10000.0f;
    st.analog["accel_z"] = state.acceleration.z / 10000.0f;

    // Touchpad (two fingers)
    st.analog["touchpad1_x"] = state.touchData.touch[0].x / 1920.0f;
    st.analog["touchpad1_y"] = state.touchData.touch[0].y / 1080.0f;
    st.analog["touchpad2_x"] = state.touchData.touch[1].x / 1920.0f;
    st.analog["touchpad2_y"] = state.touchData.touch[1].y / 1080.0f;

    // --- Dijital datas ---
    auto bm = state.bitmask_buttons;

    st.digital["cross"] = bm & SCE_BM_CROSS;
    st.digital["square"] = bm & SCE_BM_SQUARE;
    st.digital["triangle"] = bm & SCE_BM_TRIANGLE;
    st.digital["circle"] = bm & (SCE_BM_CIRCLE | SCE_BM_TRIANGLE); // when triangle pressed also circle active

    st.digital["dpad_up"] = state.bitmask_buttons & SCE_BM_N_DPAD;
    st.digital["dpad_down"] = state.bitmask_buttons & SCE_BM_S_DPAD;
    st.digital["dpad_left"] = state.bitmask_buttons & SCE_BM_W_DPAD;
    st.digital["dpad_right"] = state.bitmask_buttons & SCE_BM_E_DPAD;

    st.digital["l1"] = state.bitmask_buttons & SCE_BM_L1;
    st.digital["r1"] = state.bitmask_buttons & SCE_BM_R1;
    st.digital["l2_btn"] = state.bitmask_buttons & SCE_BM_L2;
    st.digital["r2_btn"] = state.bitmask_buttons & SCE_BM_R2;
    st.digital["l3"] = state.bitmask_buttons & SCE_BM_L3;
    st.digital["r3"] = state.bitmask_buttons & SCE_BM_R3;

    st.digital["options"] = state.bitmask_buttons & SCE_BM_OPTIONS;
    st.digital["share"] = state.bitmask_buttons & SCE_BM_SHARE;
    st.digital["ps"] = state.bitmask_buttons & SCE_BM_PSBTN;
    st.digital["touchpad_click"] = state.bitmask_buttons & SCE_BM_TOUCH;
}

InputBridgeState InputBridge::getState(int index) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_states[index];
}
