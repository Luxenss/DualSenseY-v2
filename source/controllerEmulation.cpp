#include "controllerEmulation.hpp"
#include "log.hpp"
#include "scePadSettings.hpp"
#include "controllerHotkey.hpp"
#include <cmath>
#include "inputBridge.hpp"

int convertRange(int value, int oldMin, int oldMax, int newMin, int newMax) {
	if (oldMin == oldMax) {
		throw std::invalid_argument("Old minimum and maximum cannot be equal.");
	}
	float ratio = static_cast<float>(newMax - newMin) / static_cast<float>(oldMax - oldMin);
	float scaledValue = (value - oldMin) * ratio + newMin;
	return std::clamp(static_cast<int>(scaledValue), newMin, newMax);
}

#ifdef WINDOWS
PVIGEM_CLIENT Vigem::m_vigemClient;
#endif

#ifdef WINDOWS
void Vigem::update360ByTarget(PVIGEM_TARGET Target, s_ScePadData& state) {
	XUSB_REPORT report = {};

	report.wButtons |= (state.bitmask_buttons & SCE_BM_CROSS) ? XINPUT_GAMEPAD_A : 0;
	report.wButtons |= (state.bitmask_buttons & SCE_BM_CIRCLE) ? XINPUT_GAMEPAD_B : 0;
	report.wButtons |= (state.bitmask_buttons & SCE_BM_SQUARE) ? XINPUT_GAMEPAD_X : 0;
	report.wButtons |= (state.bitmask_buttons & SCE_BM_TRIANGLE) ? XINPUT_GAMEPAD_Y : 0;

	report.wButtons |= (state.bitmask_buttons & SCE_BM_N_DPAD) ? XINPUT_GAMEPAD_DPAD_UP : 0;
	report.wButtons |= (state.bitmask_buttons & SCE_BM_E_DPAD) ? XINPUT_GAMEPAD_DPAD_RIGHT : 0;
	report.wButtons |= (state.bitmask_buttons & SCE_BM_S_DPAD) ? XINPUT_GAMEPAD_DPAD_DOWN : 0;
	report.wButtons |= (state.bitmask_buttons & SCE_BM_W_DPAD) ? XINPUT_GAMEPAD_DPAD_LEFT : 0;

	report.wButtons |= (state.bitmask_buttons & SCE_BM_OPTIONS) ? XINPUT_GAMEPAD_START : 0;
	report.wButtons |= (state.bitmask_buttons & SCE_BM_TOUCH) ? XINPUT_GAMEPAD_BACK : 0;

	report.wButtons |= (state.bitmask_buttons & SCE_BM_L1) ? XINPUT_GAMEPAD_LEFT_SHOULDER : 0;
	report.wButtons |= (state.bitmask_buttons & SCE_BM_R1) ? XINPUT_GAMEPAD_RIGHT_SHOULDER : 0;

	report.wButtons |= (state.bitmask_buttons & SCE_BM_L3) ? XINPUT_GAMEPAD_LEFT_THUMB : 0;
	report.wButtons |= (state.bitmask_buttons & SCE_BM_R3) ? XINPUT_GAMEPAD_RIGHT_THUMB : 0;

	report.bLeftTrigger = state.L2_Analog;
	report.bRightTrigger = state.R2_Analog;

	report.sThumbLX = convertRange(state.LeftStick.X, 0, 255, -32767, 32766);
	report.sThumbLY = convertRange(state.LeftStick.Y, 255, 0, -32767, 32766);
	report.sThumbRX = convertRange(state.RightStick.X, 0, 255, -32767, 32766);
	report.sThumbRY = convertRange(state.RightStick.Y, 255, 0, -32767, 32766);

	vigem_target_x360_update(m_vigemClient, Target, report);
}

void Vigem::updateDs4ByTarget(PVIGEM_TARGET Target, s_ScePadData& state) {
	// Default as Index 
	auto st = InputBridge::instance().getState(0);

	DS4_REPORT_EX report{};
	// --- Analog sticks ---
	report.Report.bThumbLX = static_cast<uint8_t>(st.analog["left_stick_x"] * 255);
	report.Report.bThumbLY = static_cast<uint8_t>(st.analog["left_stick_y"] * 255);
	report.Report.bThumbRX = static_cast<uint8_t>(st.analog["right_stick_x"] * 255);
	report.Report.bThumbRY = static_cast<uint8_t>(st.analog["right_stick_y"] * 255);

	// --- Digital Buttons ---
	USHORT buttons = 0;
	if (st.digital["r3"])        buttons |= 1 << 15;
	if (st.digital["l3"])        buttons |= 1 << 14;
	if (st.digital["options"])   buttons |= 1 << 13;
	if (st.digital["share"])     buttons |= 1 << 12;
	if (st.digital["r2_btn"])    buttons |= 1 << 11;
	if (st.digital["l2_btn"])    buttons |= 1 << 10;
	if (st.digital["r1"])        buttons |= 1 << 9;
	if (st.digital["l1"])        buttons |= 1 << 8;
	if (st.digital["triangle"])  buttons |= 1 << 7;
	if (st.digital["circle"])    buttons |= 1 << 6;
	if (st.digital["cross"])     buttons |= 1 << 5;
	if (st.digital["square"])    buttons |= 1 << 4;

	// --- D-pad ---
	buttons &= ~0xF;
	bool up = st.digital["dpad_up"];
	bool down = st.digital["dpad_down"];
	bool left = st.digital["dpad_left"];
	bool right = st.digital["dpad_right"];

	if (!up && !down && !left && !right)
		buttons |= 0x8; // nötr
	else if (down && left)  buttons |= (USHORT)DS4_BUTTON_DPAD_SOUTHWEST;
	else if (down && right) buttons |= (USHORT)DS4_BUTTON_DPAD_SOUTHEAST;
	else if (up && right)   buttons |= (USHORT)DS4_BUTTON_DPAD_NORTHEAST;
	else if (up && left)    buttons |= (USHORT)DS4_BUTTON_DPAD_NORTHWEST;
	else if (up)            buttons |= (USHORT)DS4_BUTTON_DPAD_NORTH;
	else if (right)         buttons |= (USHORT)DS4_BUTTON_DPAD_EAST;
	else if (down)          buttons |= (USHORT)DS4_BUTTON_DPAD_SOUTH;
	else if (left)          buttons |= (USHORT)DS4_BUTTON_DPAD_WEST;

	report.Report.wButtons = buttons;

	// --- Specials ---
	USHORT specialbuttons = 0;
	if (st.digital["ps"])             specialbuttons |= 1 << 0;
	if (st.digital["touchpad_click"]) specialbuttons |= 1 << 1;
	report.Report.bSpecial = specialbuttons;

	// --- Trigger Analogs ---
	report.Report.bTriggerL = static_cast<uint8_t>(st.analog["left_trigger"] * 255);
	report.Report.bTriggerR = static_cast<uint8_t>(st.analog["right_trigger"] * 255);
	report.Report.bBatteryLvl = 100;

	// --- Touchpad ---
	static int packetNum = 0;
	packetNum = (packetNum + 1) % 256;

	DS4_TOUCH touch{};
	touch.bPacketCounter = packetNum;

	touch.bIsUpTrackingNum1 = 0; // isUp no need
	touch.bTouchData1[0] = static_cast<uint16_t>(st.analog["touchpad1_x"] * 1920) & 0xFF;
	touch.bTouchData1[1] = (static_cast<uint16_t>(st.analog["touchpad1_x"] * 1920) >> 8 & 0x0F) |
		((static_cast<uint16_t>(st.analog["touchpad1_y"] * 1080) << 4) & 0xF0);
	touch.bTouchData1[2] = static_cast<uint16_t>(st.analog["touchpad1_y"] * 1080) >> 4;

	touch.bIsUpTrackingNum2 = 0;
	touch.bTouchData2[0] = static_cast<uint16_t>(st.analog["touchpad2_x"] * 1920) & 0xFF;
	touch.bTouchData2[1] = (static_cast<uint16_t>(st.analog["touchpad2_x"] * 1920) >> 8 & 0x0F) |
		((static_cast<uint16_t>(st.analog["touchpad2_y"] * 1080) << 4) & 0xF0);
	touch.bTouchData2[2] = static_cast<uint16_t>(st.analog["touchpad2_y"] * 1080) >> 4;

	report.Report.sCurrentTouch = touch;

	// --- Gyroscope and accelerometer---
	report.Report.wAccelX = static_cast<SHORT>(st.analog["accel_x"] * 10000);
	report.Report.wAccelY = static_cast<SHORT>(st.analog["accel_y"] * 10000);
	report.Report.wAccelZ = static_cast<SHORT>(st.analog["accel_z"] * 10000);
	report.Report.wGyroX = static_cast<SHORT>(st.analog["gyro_x"] * 1000);
	report.Report.wGyroY = static_cast<SHORT>(st.analog["gyro_z"] * 1000); // swap
	report.Report.wGyroZ = static_cast<SHORT>(st.analog["gyro_y"] * 1000);
	report.Report.wTimestamp = state.timestamp / 16;

	// --- Send ---
	vigem_target_ds4_update_ex(m_vigemClient, Target, report);
}
#endif


Vigem::Vigem(s_scePadSettings* scePadSettings, UDP& udp) : m_scePadSettings(scePadSettings), m_udp(udp) {
#ifdef WINDOWS
	if (!m_vigemClientInitalized) {
		m_vigemClient = vigem_alloc();

		VIGEM_ERROR retval = vigem_connect(m_vigemClient);
		if (!VIGEM_SUCCESS(retval)) {
			LOGE("Failed to connect to ViGEm client");
			return;
		}

		m_vigemThread = std::thread(&Vigem::emulatedControllerUpdate, this);
		m_vigemThread.detach();

		LOGI("ViGEm Client initialized");
		m_vigemClientInitalized = true;
	}

	for (uint32_t i = 0; i < 4; i++) {
		m_360[i] = vigem_target_x360_alloc();
		m_ds4[i] = vigem_target_ds4_alloc();
	}
#endif
}

Vigem::~Vigem() {
#ifdef WINDOWS
	m_vigemThreadRunning = false;
	if (m_vigemThread.joinable()) {
		m_vigemThread.join();
	}

	for (uint32_t i = 0; i < 4; i++) {
		vigem_target_x360_unregister_notification(m_360[i]);
		vigem_target_remove(m_vigemClient, m_360[i]);
		vigem_target_ds4_unregister_notification(m_ds4[i]);
		vigem_target_remove(m_vigemClient, m_ds4[i]);
		vigem_target_free(m_360[i]);
		vigem_target_free(m_ds4[i]);
	}

	vigem_disconnect(m_vigemClient);
	vigem_free(m_vigemClient);
#endif
}

void Vigem::plugControllerByIndex(uint32_t index, uint32_t controllerType) {
#ifdef WINDOWS
	static uint32_t lastEmulatedController[4] = {};

	if ((EmulatedController)controllerType == EmulatedController::NONE && (EmulatedController)lastEmulatedController[index] != EmulatedController::NONE) {
		vigem_target_remove(m_vigemClient, m_360[index]);
		vigem_target_remove(m_vigemClient, m_ds4[index]);
		vigem_target_x360_unregister_notification(m_360[index]);
		vigem_target_ds4_unregister_notification(m_ds4[index]);
		lastEmulatedController[index] = (uint32_t)EmulatedController::NONE;
	}
	else if ((EmulatedController)controllerType == EmulatedController::XBOX360 && (EmulatedController)lastEmulatedController[index] != EmulatedController::XBOX360) {
		vigem_target_remove(m_vigemClient, m_ds4[index]);
		vigem_target_ds4_unregister_notification(m_ds4[index]);
		vigem_target_add(m_vigemClient, m_360[index]);
		vigem_target_x360_register_notification(m_vigemClient, m_360[index], xbox360Notification, &m_userData[index]);
		lastEmulatedController[index] = (uint32_t)EmulatedController::XBOX360;
	}
	else if ((EmulatedController)controllerType == EmulatedController::DUALSHOCK4 && (EmulatedController)lastEmulatedController[index] != EmulatedController::DUALSHOCK4) {
		vigem_target_remove(m_vigemClient, m_360[index]);
		vigem_target_x360_unregister_notification(m_360[index]);
		vigem_target_add(m_vigemClient, m_ds4[index]);
		vigem_target_ds4_register_notification(m_vigemClient, m_ds4[index], ds4Notification, &m_userData[index]);
		lastEmulatedController[index] = (uint32_t)EmulatedController::DUALSHOCK4;
	}
#endif
}

void Vigem::setSelectedController(uint32_t selectedController) {
	m_selectedController = selectedController;
}

void Vigem::SetPeerControllerDataPointer(std::shared_ptr<std::unordered_map<uint32_t, PeerControllerData>> Pointer) {
#ifdef WINDOWS
	m_PeerControllers = Pointer;
#endif
}

bool Vigem::isVigemConnected() {
#if !defined(__linux__) && !defined(__MACOS__)
	return m_vigemClientInitalized;
#endif;
	return false;
}

void Vigem::applyInputSettingsToScePadState(s_scePadSettings& settings, s_ScePadData& state) {
#pragma region Trigger threshold
	state.L2_Analog = state.L2_Analog >= settings.leftTriggerThreshold ? state.L2_Analog : 0;
	state.R2_Analog = state.R2_Analog >= settings.rightTriggerThreshold ? state.R2_Analog : 0;
#pragma endregion

#pragma region Analog deadzone
	auto applyDeadzone = [&](int deadzone, s_SceStickData& stick) {
		if (deadzone <= 0)
			return;

		float centerX = (stick.X - 128);
		float centerY = (stick.Y - 128);
		float magnitude = sqrt(centerX * centerX + centerY * centerY);

		float deadzoneNorm = deadzone / 128;

		stick.X = magnitude > deadzone ? stick.X : 128;
		stick.Y = magnitude > deadzone ? stick.Y : 128;
		};
	applyDeadzone(settings.leftStickDeadzone, state.LeftStick);
	applyDeadzone(settings.rightStickDeadzone, state.RightStick);

#pragma endregion

#pragma region Gyro to right stick
	if (settings.gyroToRightStick && IsHotkeyActive(settings.gyroToRightStickActivationButton, state.bitmask_buttons)) {
		if (abs(state.RightStick.X - 128) <= 80 &&
		    abs(state.RightStick.Y - 128) <= 80) {

			const float maxVelValue = 1000.0f;

			float normalizedVelX = state.angularVelocity.x / maxVelValue;
			float normalizedVelY = state.angularVelocity.z / maxVelValue;

			float adjustedX = -normalizedVelX * settings.gyroToRightStickSensitivity;
			float adjustedY = -normalizedVelY * settings.gyroToRightStickSensitivity;

			state.RightStick.X = static_cast<int>((adjustedY) - 127);
			state.RightStick.Y = static_cast<int>((adjustedX) - 127);
			applyDeadzone(settings.gyroToRightStickDeadzone, state.RightStick);
		}
	}
#pragma endregion
}

#ifdef WINDOWS
void Vigem::emulatedControllerUpdate() {
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	timeBeginPeriod(1);

	EXECUTION_STATE prevState = SetThreadExecutionState(
		ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED
	);

	HANDLE hTimer = CreateWaitableTimerEx(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
	LARGE_INTEGER liDueTime;
	liDueTime.QuadPart = -5000LL;

	while (m_vigemThreadRunning) {
		for (uint32_t i = 0; i < 4; i++) {

			if ((EmulatedController)m_scePadSettings[i].emulatedController != EmulatedController::NONE) {
				s_ScePadData scePadState = {};
				uint32_t result = scePadReadState(g_scePad[i], &scePadState);
				InputBridge::instance().updateFromPs5(scePadState, i);


				s_scePadSettings settingsToUse = (m_selectedController == i && m_udp.isActive()) ? m_udp.getSettings() : m_scePadSettings[i];
				applyInputSettingsToScePadState(settingsToUse, scePadState);

				if (result == SCE_OK) {

					if ((EmulatedController)m_scePadSettings[i].emulatedController == EmulatedController::XBOX360) {
						update360ByTarget(m_360[i], scePadState);
					}
					else if ((EmulatedController)m_scePadSettings[i].emulatedController == EmulatedController::DUALSHOCK4) {
						updateDs4ByTarget(m_ds4[i], scePadState);
					}
				}
			}
		}

		if (auto peerControllers = m_PeerControllers.lock()) {
			for (auto it = peerControllers->begin(); it != peerControllers->end(); ) {
				auto& peer = it->second;
				auto targetIter = m_PeerControllerTargets.find(it->first);

				if (peer.AllowedToReceive && targetIter == m_PeerControllerTargets.end()) {
					PVIGEM_TARGET target = peer.Controller == CONTROLLER::XBOX360 ? vigem_target_x360_alloc() : vigem_target_ds4_alloc();
					VIGEM_ERROR error = vigem_target_add(m_vigemClient, target);
					if (peer.Controller == CONTROLLER::XBOX360) vigem_target_x360_register_notification(m_vigemClient, target, x360PeerNotification, &peer);
					if (peer.Controller == CONTROLLER::DUALSHOCK4) vigem_target_ds4_register_notification(m_vigemClient, target, ds4PeerNotification, &peer);
					if (error == VIGEM_ERROR_NONE) {
						m_PeerControllerTargets[it->first] = target;
						it->second.Disconnected = false;
						LOGI("[PEER CONTROLLER] Controller created");
					}
					else {
						LOGE("[PEER CONTROLLER] Failed to allocate peer controller");
					}
				}

				if (targetIter != m_PeerControllerTargets.end() && peer.Disconnected) {
					if (!targetIter->second) continue;
					VIGEM_ERROR error = vigem_target_remove(m_vigemClient, targetIter->second);
					if (error == VIGEM_ERROR_NONE) {
						if (peer.Controller == CONTROLLER::XBOX360) vigem_target_x360_unregister_notification(targetIter->second);
						else if (peer.Controller == CONTROLLER::DUALSHOCK4) vigem_target_ds4_unregister_notification(targetIter->second);
						vigem_target_free(targetIter->second);
						m_PeerControllerTargets.erase(targetIter);
						it->second.AllowedToReceive = false;
						it->second.AllowedToSend = false;
						it->second.Disconnected = false;
						it = peerControllers->erase(it);
						LOGI("[PEER CONTROLLER] Controller destroyed");
					}
					else {
						LOGE("[PEER CONTROLLER] Controller failed to be removed, ERROR: %d", (int)error);
					}
					continue;
				}

				s_ScePadData inputState = {  };
				inputState.LeftStick.X = 128; inputState.LeftStick.Y = 128;
				inputState.RightStick.X = 128; inputState.RightStick.Y = 128;
				{
					std::lock_guard<std::mutex> inputGuard(peer.Lock);
					inputState = peer.InputState;
				}
				applyInputSettingsToScePadState(peer.Settings, inputState);

				targetIter = m_PeerControllerTargets.find(it->first);
				if (targetIter != m_PeerControllerTargets.end() && targetIter->second) {
					if(peer.Controller == CONTROLLER::XBOX360)
						update360ByTarget(targetIter->second, inputState);
					else if (peer.Controller == CONTROLLER::DUALSHOCK4)
						updateDs4ByTarget(targetIter->second, inputState);
				}

				++it;
			}
		}

		SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0);
		WaitForSingleObject(hTimer, INFINITE);
	}

	CloseHandle(hTimer);
}
#endif

#ifdef WINDOWS
VOID Vigem::xbox360Notification(PVIGEM_CLIENT Client, PVIGEM_TARGET Target, UCHAR LargeMotor, UCHAR SmallMotor, UCHAR LedNumber, LPVOID UserData) {
	auto* data = static_cast<VigemUserData*>(UserData);
	if (!data || !data->instance) return;
	if (!data->instance->m_scePadSettings) return;

	data->instance->m_scePadSettings[data->index].rumbleFromEmulatedController = { LargeMotor, SmallMotor };
}

VOID Vigem::ds4Notification(PVIGEM_CLIENT Client, PVIGEM_TARGET Target, UCHAR LargeMotor, UCHAR SmallMotor, DS4_LIGHTBAR_COLOR LightbarColor, LPVOID UserData) {
	auto* data = static_cast<VigemUserData*>(UserData);
	if (!data || !data->instance) return;
	if (!data->instance->m_scePadSettings) return;

	data->instance->m_scePadSettings[data->index].lightbarFromEmulatedController = { LightbarColor.Red, LightbarColor.Green, LightbarColor.Blue };	
	data->instance->m_scePadSettings[data->index].rumbleFromEmulatedController = { LargeMotor, SmallMotor };
}
VOID Vigem::x360PeerNotification(PVIGEM_CLIENT Client, PVIGEM_TARGET Target, UCHAR LargeMotor, UCHAR SmallMotor, UCHAR LedNumber, LPVOID UserData) {
	auto* data = static_cast<PeerControllerData*>(UserData);
	if (!data) return;

	data->Vibration = { LargeMotor, SmallMotor };
}
VOID Vigem::ds4PeerNotification(PVIGEM_CLIENT Client, PVIGEM_TARGET Target, UCHAR LargeMotor, UCHAR SmallMotor, DS4_LIGHTBAR_COLOR LightbarColor, LPVOID UserData) {
	auto* data = static_cast<PeerControllerData*>(UserData);
	if (!data) return;

	data->Vibration = { LargeMotor, SmallMotor };
	data->Lightbar = {LightbarColor.Red, LightbarColor.Green, LightbarColor.Blue};
}
#endif

