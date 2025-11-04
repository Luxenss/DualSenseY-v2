# DualSenseY - Project Explanation

## Overview

**DualSenseY** is a feature-rich C++17 desktop application that enables PlayStation DualSense and DualShock 4 controller emulation and configuration on Windows (primary), macOS, and Linux. It provides audio passthrough, adaptive trigger customization, online controller sharing, gyro/touchpad support, and remote control capabilities through an ENet-powered networking layer.

---

## Project Architecture

### Tech Stack

- **Language**: C++17
- **Build System**: CMake 3.10+
- **Graphics/Windowing**: GLFW + GLAD + OpenGL 3.3+
- **UI Framework**: ImGui (with GLFW + OpenGL3 backends)
- **Controller I/O**: duaLib (PlayStation controller library)
- **Audio**: miniaudio (cross-platform audio I/O)
- **Controller Emulation**: ViGEmClient (Windows-only, emulates Xbox 360/DualShock 4)
- **Networking**: 
  - ENet (peer-to-peer room-based communication)
  - ASIO (UDP socket handling)
- **Serialization**: nlohmann/json
- **Additional Libraries**: 
  - stb_image (image loading)
  - nativefiledialog (file dialogs)
  - PlatformFolders (cross-platform settings storage)
  - miniupnpc (UPnP port mapping)
  - Updater helper (automatic updates)

### Runtime Platforms

- **Windows** (primary): Full feature support including ViGEm emulation
- **Linux**: No ViGEm support
- **macOS**: No ViGEm support
- Platform detection at compile-time via `CMakeLists.txt` definitions

---

## Core Directory Structure

```
/home/engine/project/
├── CMakeLists.txt              # Main build configuration
├── CMakePresets.json           # CMake presets for development
├── README.md                   # User-facing readme
├── icon.ico / icon.rc          # Windows application icon
├── .gitignore                  # Git ignore rules
│
├── include/                    # Public headers
│   ├── application.hpp         # Application lifecycle & GLFW setup
│   ├── mainWindow.hpp          # ImGui UI rendering
│   ├── scePadSettings.hpp      # Controller configuration structure
│   ├── controllerEmulation.hpp # ViGEm emulation wrapper
│   ├── audioPassthrough.hpp    # Audio capture/routing
│   ├── udp.hpp                 # UDP command protocol
│   ├── client.hpp              # ENet networking layer
│   ├── appSettings.hpp         # Application-wide settings (serializable)
│   ├── inputBridge.hpp         # Input state abstraction
│   ├── keyboardMouseMapper.hpp # Input remapping
│   ├── controllerHotkey.hpp    # Global hotkey capture
│   ├── led.hpp                 # LED control
│   ├── upnp.hpp                # UPnP port mapping
│   ├── utils.hpp               # Utility functions
│   ├── strings.hpp             # Localization strings
│   ├── log.hpp                 # Logging utilities
│   └── scePadHandle.hpp        # Controller handle typedef
│
├── source/                     # Implementation files
│   ├── main.cpp                # Entry point (platform-specific)
│   ├── application.cpp         # Application lifecycle & ImGui loop
│   ├── mainWindow.cpp          # UI implementation (~44KB - most complex)
│   ├── scePadSettings.cpp      # Settings serialization
│   ├── controllerEmulation.cpp # ViGEm implementation
│   ├── audioPassthrough.cpp    # Audio device management
│   ├── client.cpp              # ENet communication (~28KB)
│   ├── udp.cpp                 # UDP packet handling
│   ├── keyboardMouseMapper.cpp # Input mapping logic
│   ├── inputBridge.cpp         # Input state management
│   ├── scePadCustomTriggers.cpp # Adaptive trigger profiles
│   ├── controllerHotkey.cpp    # Hotkey capture
│   ├── led.cpp                 # LED color handling
│   ├── upnp.cpp                # UPnP implementation
│   ├── utils.cpp               # Utility implementations
│   ├── appSettings.cpp         # Settings persistence
│   └── strings.cpp             # String/translation management
│
├── resources/                  # UI and asset files
│   └── [UI fonts, icons, etc.]
│
└── thirdparty/                 # Third-party dependencies
    ├── glfw/                   # Window creation & input
    ├── glad/                   # OpenGL loader
    ├── imgui/                  # Immediate-mode UI
    ├── duaLib/                 # PlayStation controller driver
    ├── json/                   # JSON serialization
    ├── miniaudio/              # Audio I/O
    ├── ViGEmClient/            # Xbox/DualShock emulation (Windows only)
    ├── asio-cmake/             # Networking (UDP sockets)
    ├── stb_image/              # Image loading
    ├── nativefiledialog/       # File dialogs
    ├── PlatformFolders/        # Platform-specific folders
    ├── miniupnp/               # UPnP port mapping
    └── Updater/                # Auto-update helper
```

---

## Core Components

### 1. **Application Class** (`application.hpp / .cpp`)

**Role**: Main application lifecycle manager

**Key Responsibilities**:
- Initializes GLFW window with OpenGL context
- Sets up ImGui rendering backend
- Manages the main event loop (polls GLFW, renders ImGui, updates subsystems)
- Loads persisted `AppSettings` from user documents folder
- Detects platform (Windows/Linux/macOS)
- Handles window minimization state to disable controller input when app is not in focus

**Key Methods**:
- `run()`: Main loop - processes events, updates all subsystems, renders ImGui
- `createWindow()`: GLFW window setup with OpenGL context
- `setStyleAndColors()`: ImGui theme configuration
- `disableControllerInputIfMinimized()`: Pauses input processing when minimized
- `~Application()`: Cleanup (ViGEm, audio devices, networking threads)

**Data Members**:
```cpp
std::unique_ptr<GLFWwindow, glfwDeleter> m_glfwWindow;
s_scePadSettings m_scePadSettings[4];  // Settings for 4 controllers
AppSettings m_appSettings;              // Global app config
```

---

### 2. **MainWindow Class** (`mainWindow.hpp / .cpp`)

**Role**: ImGui UI layer rendering and user interaction

**Key Responsibilities**:
- Renders all UI panels in ImGui
- Handles user input from UI elements
- Displays controller status, settings, diagnostics
- Shows error messages and server communication status

**UI Sections** (each is a method returning bool):
- `menuBar()`: Main menu options
- `controllers()`: Controller selection and status
- `led()`: RGB lightbar color picker
- `audio()`: Audio passthrough settings and peak level display
- `emulation()`: Xbox360/DualShock4 emulation mode selection
- `adaptiveTriggers()`: Trigger effect selection and customization
- `keyboardAndMouseMapping()`: Button-to-keyboard/mouse remapping
- `touchpad()`: Touchpad diagnostics and mouse emulation settings
- `treeElement_*()`: Collapsible tree sections for analog sticks, vibration, motion, etc.
- `online()`: Online room management and peer connection UI
- `messageFromServer()`: Display network messages
- `screenBlock()`: Modal dialogs (e.g., waiting for hotkey input)

**Dependencies** (injected via constructor):
```cpp
Strings& m_strings;              // Localization
AudioPassthrough& m_audio;       // Audio subsystem
Vigem& m_vigem;                  // Controller emulation
UDP& m_udp;                      // UDP command protocol
AppSettings& m_appSettings;      // App config
Client& m_client;                // Networking
```

**Data Members**:
```cpp
int m_selectedController;        // Currently selected controller (0-3)
bool m_isAdminWindows;           // Admin status for privilege operations
```

---

### 3. **scePadSettings Struct** (`scePadSettings.hpp`)

**Role**: Centralized controller configuration storage and serialization

**Key Configuration Areas**:

```cpp
struct s_scePadSettings {
    bool udpConfig = false;
    
    // LED Settings
    std::array<float, 3> led;           // RGB values
    bool audioToLed = false;            // Drive LED from audio peak
    int brightness = 0;                 // LED brightness
    
    // Audio Passthrough
    bool audioPassthroughActive = false;
    float hapticIntensity = 1.0f;       // Vibration intensity multiplier
    uint32_t selectedAudioDevice = 0;
    
    // Emulation Mode
    EmulatedController emulatedController = EmulatedController::NONE;
    
    // Vibration
    s_ScePadVibrationParam vibrationParam;
    
    // Adaptive Triggers
    bool enableAdaptiveTriggers = true;
    TriggerMode triggerModeL2, triggerModeR2;
    std::vector<uint8_t> triggerParamL2, triggerParamR2;
    
    // Input Mapping
    std::unordered_map<uint8_t, std::string> buttonKeyMapping;
    std::unordered_map<uint8_t, std::string> buttonMouseMapping;
    
    // Gyro / Touchpad
    bool gyroToMouse = false;
    bool touchpadToMouse = false;
    float gyroSensitivity = 1.0f;
    float touchpadSensitivity = 1.0f;
    
    // Network Streaming
    bool streamGyroToServer = false;
    bool streamTouchpadToServer = false;
};
```

**JSON Serialization**: Settings persist to `{UserDocumentsFolder}/DualSenseY/settings_{controller_index}.json`

---

### 4. **Vigem (ControllerEmulation)** (`controllerEmulation.hpp / .cpp`)

**Role**: Windows-only ViGEm abstraction layer for controller emulation

**Key Responsibilities**:
- Bridges DualSense/DualShock4 input to Xbox 360 or DualShock 4 emulation targets
- Manages up to 8 emulated controllers (4 local user + 4 remote peers)
- Handles vibration feedback from games back to controller
- Runs background thread for ViGEm updates

**Key Methods**:
- `plugControllerByIndex(index, type)`: Create emulation target (XBOX360 or DS4)
- `setSelectedController(index)`: Set active controller for input routing
- `isVigemConnected()`: Check if ViGEm service is running
- `SetPeerControllerDataPointer()`: Link peer controller data for network sharing

**Platform Support**:
- **Windows**: Full ViGEm implementation
- **Linux/macOS**: Stub implementations (disabled)

**Data Members**:
```cpp
PVIGEM_TARGET m_360[VIGEM_CONTROLLER_MAX];   // Xbox 360 targets
PVIGEM_TARGET m_ds4[VIGEM_CONTROLLER_MAX];   // DS4 targets
std::thread m_vigemThread;                   // Background update thread
std::atomic<bool> m_vigemThreadRunning;
```

---

### 5. **AudioPassthrough Class** (`audioPassthrough.hpp / .cpp`)

**Role**: Cross-platform audio capture and playback routing

**Key Responsibilities**:
- Captures system audio via miniaudio
- Routes captured audio to controller haptic drivers (if supported)
- Monitors audio peak level for LED visualization
- Maintains separate audio buffers per controller

**Key Methods**:
- `startByUserId(id)`: Begin audio capture for controller
- `stopByUserId(id)`: Stop audio capture
- `setHapticIntensityByUserId(id, intensity)`: Haptic feedback volume control
- `getCurrentCapturePeak()`: Get real-time audio peak for visualization
- `validate()`: Periodically check for audio device disconnections

**Data Members**:
```cpp
std::vector<float> m_audioBuffer[4];         // Per-controller buffers
ma_device m_controller[4];                   // Playback devices
ma_device m_captureDevice;                   // Capture device
std::atomic<float> m_hapticIntensity[4];     // Per-controller intensity
std::atomic<float> m_currentCapturePeak;     // Real-time peak
```

---

### 6. **UDP Class** (`udp.hpp / .cpp`)

**Role**: DSX-compatible UDP command protocol handler

**Purpose**: Allows external applications (mods, games) to send controller configuration commands via UDP on port 6969

**Supported Commands** (InstructionType):
- `GetDSXStatus`: Query current state
- `TriggerUpdate`: Update adaptive trigger effect
- `RGBUpdate`: Change LED color
- `PlayerLED`: Set player number LED
- `TriggerThreshold`: Set trigger activation threshold
- `MicLED`: Control microphone LED
- `PlayerLEDNewRevision`: New player LED format
- `ResetToUserSettings`: Revert to saved config

**Protocol**:
```
UDP Packet Format:
{
  "instructions": [
    {
      "type": 1,           // InstructionType enum value
      "parameters": [...]  // Instruction-specific parameters
    },
    ...
  ]
}
```

**Key Methods**:
- `listen()`: Background thread listening on UDP socket
- `isActive()`: Returns true if receiving commands
- `getSettings()`: Retrieve current settings modified by UDP
- `setVibrationToUdpConfig()`: Pass vibration params from controller

**Data Members**:
```cpp
asio::io_context m_ioContext;
asio::ip::udp::socket m_socket;              // Bound to port 6969
std::thread m_listenThread;
s_scePadSettings m_settings;                 // UDP-modified settings
```

---

### 7. **Client Class** (`client.hpp / .cpp`)

**Role**: ENet-based networking for online room management and remote controller streaming

**Features**:
- Connect to central server (maluch.mikr.us:30151)
- Create/join rooms for controller sharing
- Stream local controller input to room peers
- Receive remote controller input and stream via ViGEm
- Peer-to-peer gyro/touchpad data streaming

**Protocol** (ENet Channels):
- **Channel 0** (Reliable): Request/Response, room management
- **Channel 1** (Unreliable): Input state streaming
- **Channel 2** (Unreliable): Gimmick/special data (gyro, touchpad)
- **Channel 3** (Reliable): Settings updates

**Key Commands** (CMD enum):
```
CMD_OPEN_ROOM
CMD_JOIN_ROOM
CMD_LEAVE_ROOM
CMD_PEER_REQUEST_VIGEM      // Request to stream controller to peer
CMD_PEER_INPUT_STATE        // Stream input data
CMD_PEER_GIMMICK_STATE      // Stream gyro/touchpad
CMD_PEER_SETTINGS_STATE     // Sync settings
CMD_PING / CMD_PONG         // Keep-alive
CMD_CHANGE_NICKNAME         // Set display name
CMD_GET_PEER_INFO           // Query peer details
```

**Response Codes** (RESPONSE_CODE):
- `E_SUCCESS`
- `E_PEER_ALREADY_IN_ROOM`, `E_PEER_UNAVAILABLE`, `E_PEER_CANT_EMULATE`
- `E_ROOM_FULL`, `E_ROOM_DOESNT_EXIST`, `E_ROOM_ALREADY_EXISTS`
- `E_SERVER_ERROR`

**Key Methods**:
- `connectToServer()`: Connect to central server
- `createRoom(name)`: Create online room
- `joinRoom(roomId)`: Join existing room
- `leaveRoom()`: Leave current room
- `sendInput(state)`: Stream controller input to peers
- `requestPeerViGem(peerId)`: Request permission to control peer's console
- `handleIncomingPeerInput()`: Process remote controller data

**Data Members**:
```cpp
ENetHost* m_client;          // ENet host
ENetPeer* m_serverPeer;      // Connection to central server
std::unordered_map<uint32_t, PeerControllerData> m_peerControllers;
std::thread m_networkThread; // Background receive thread
std::mutex m_peerLock;       // Thread-safe peer access
```

---

### 8. **Adaptive Triggers** (`scePadCustomTriggers.hpp / .cpp`)

**Role**: Pre-configured and custom adaptive trigger effect profiles

**Built-in Effects**:
- `OFF`: No resistance
- `FEEDBACK`: Simple resistance feedback
- `WEAPON`: Gun trigger feel
- `VIBRATION`: Haptic feedback
- `SLOPE_FEEDBACK`: Progressive resistance
- `MULTIPLE_POSITION_FEEDBACK`: Multi-stage resistance points
- `MULTIPLE_POSITION_VIBRATION`: Multi-stage haptic effects
- `RIGID`: Full trigger lock
- `RIGID_A`, `RIGID_B`, `RIGID_AB`: Segmented lock modes
- `PULSE`, `PULSE_A`, `PULSE_B`, `PULSE_AB`: Pulsing effects
- `VIBRATE_TRIGGER`: Continuous vibration
- `VIBRATE_TRIGGER_PULSE`: Pulsing vibration
- `CHOPPY`: Stuttering effect
- `GAMEUCBE`: GameCube controller feel
- `HARD`, `VERY_HARD`, `HARDEST`: Different resistance levels
- Custom trigger effects: `BOW`, `GALLOPING`, `SEMI_AUTOMATIC_GUN`, `AUTOMATIC_GUN`, `MACHINE`, `RESISTANCE`

**Effect Parameters**:
Each effect is encoded as an 11-byte FFB (Force Feedback) command:
```cpp
void customTriggerXxx(std::vector<uint8_t> param, uint8_t ffb[11]);
// param: User-defined parameters (intensity, frequency, etc.)
// ffb[11]: Output buffer for DualSense haptic command
```

---

### 9. **Input Bridge** (`inputBridge.hpp / .cpp`)

**Role**: State-based abstraction over controller input

**Purpose**: Decouples controller state from immediate UI/emulation consumption, allowing state snapshots and analysis

**Supported Inputs**:
- Analog stick values (left_x, left_y, right_x, right_y)
- Trigger values (l2, r2)
- Digital buttons (dpad_up, dpad_down, square, triangle, etc.)
- Motion data (gyro_x, gyro_y, gyro_z, accel_x, accel_y, accel_z)
- Touchpad position and pressure

**Key Methods**:
- `updateFromPs5(state, index)`: Update state from controller
- `getState(index)`: Retrieve current state snapshot

---

### 10. **AppSettings Class** (`appSettings.hpp / .cpp`)

**Role**: Application-wide settings persistence

**Persisted Settings**:
```cpp
struct AppSettings {
    bool DisableAllBluetoothControllersOnExit;
    bool DontConnectToServerOnStart;
    std::string SelectedLanguage;  // "en", "pl", "es", "tr"
};
```

**Storage**: JSON file at `{PlatformFolders::documents}/DualSenseY/appSettings.json`

**Key Functions**:
- `saveAppSettings()`: Serialize to JSON and persist
- `loadAppSettings()`: Deserialize from JSON

---

### 11. **Utility Classes**

#### **KeyboardMouseMapper** (`keyboardMouseMapper.hpp / .cpp`)
- Maps controller buttons to keyboard keys or mouse actions
- Supports modifier keys (Shift, Ctrl, Alt)
- Provides remapping UI

#### **ControllerHotkey** (`controllerHotkey.hpp / .cpp`)
- Captures specific controller button sequences for global hotkey actions
- Allows users to define hotkeys directly from controller input

#### **LED** (`led.hpp / .cpp`)
- Manages RGB color conversions for lightbar
- Supports color presets and audio-reactive mode

#### **UPnP** (`upnp.hpp / .cpp`)
- Automatic port mapping for network connectivity
- Registers UDP port 6969 and ENet ports on local router

#### **Strings** (`strings.hpp / .cpp`)
- Localization system managing translated strings
- Supports multiple languages: English, Polish, Spanish, Turkish

#### **Utils** (`utils.hpp / .cpp`)
- Cross-platform utility functions:
  - `isRunningAsAdministratorWindows()`: Check admin privileges
  - File/folder operations
  - String utilities
  - DPI scaling helpers for high-resolution displays

#### **Log** (`log.hpp`)
- Simple logging macros (DEBUG, INFO, WARN, ERROR)
- Platform-specific output (console on Windows, syslog on Linux)

---

## Data Flow

### Typical Input Processing Loop

```
User connects DualSense controller
    ↓
duaLib detects controller via USB/Bluetooth
    ↓
Application reads s_ScePadData from controller each frame
    ↓
InputBridge.updateFromPs5() creates abstracted state
    ↓
scePadSettings filters/remaps input:
  - Button → Keyboard/Mouse mapping
  - Gyro/Touchpad → Mouse movement
  - Audio peak → LED color
    ↓
Vigem.emulatedControllerUpdate() converts to:
  - Xbox 360 or DualShock 4 virtual controller state
    ↓
Windows receives input from virtual controller
    ↓
[Optionally] Client.sendInput() streams to remote peers
```

### Output Processing Loop

```
Game requests vibration/LED via ViGEm
    ↓
Vigem notification callback triggered
    ↓
Convert to s_ScePadVibrationParam
    ↓
duaLib sends haptic command to physical controller
    ↓
[Optionally] Send to UDP for mod commands
    ↓
[Optionally] AudioPassthrough overlays audio haptics
```

### Network Flow

```
User creates/joins online room
    ↓
Client.connectToServer() establishes ENet connection
    ↓
User selects peer to share controller with
    ↓
Client.requestPeerViGem() sends permission request
    ↓
Peer accepts → Client creates ViGEm target for peer
    ↓
Each frame:
  Client.sendInput() → Peer's Vigem → Peer's emulated controller
  Peer's ViGEm vibration → Client receives → Physical controller
```

---

## Build and Compilation

### CMake Configuration

**Platform-Specific Defines**:
```cmake
# Windows
target_compile_definitions(DualSenseY PUBLIC WINDOWS=1)

# Linux
target_compile_definitions(DualSenseY PUBLIC LINUX=1)

# macOS
target_compile_definitions(DualSenseY PUBLIC APPLE=1)
```

**Build Types**:
- **Development Build** (default): Resources loaded from source directory
- **Production Build** (opt-in): Resources bundled with executable

**Compiler Flags**:
- MSVC: `/utf-8` (UTF-8 source encoding)
- GCC/Clang: `-finput-charset=UTF-8 -fexec-charset=UTF-8`

**Linked Libraries**:
```
glfw, glad, imgui, duaLib, nlohmann_json, miniaudio, ViGEmClient,
asio, stb_image, nativefiledialog, sago::platform_folders,
libminiupnpc-static, tiny-process-library
```

---

## Key Features

### 1. **Controller Emulation**
- Emulate Xbox 360 or DualShock 4 controllers
- Support for up to 4 local + 4 remote controllers
- Full haptic feedback support
- Motion and touchpad data transmission

### 2. **Adaptive Trigger Customization**
- 20+ pre-configured trigger effects
- Custom trigger profiles with configurable parameters
- Real-time effect preview
- DSX-compatible UDP protocol support

### 3. **Audio Passthrough**
- Capture system audio and route to controller haptics
- Per-controller haptic intensity control
- Real-time audio peak visualization for LED feedback
- Supports all audio devices (speakers, headphones, virtual audio)

### 4. **Input Remapping**
- Map any button to keyboard key or mouse action
- Modifier key support (Shift, Ctrl, Alt)
- Gyro to mouse movement
- Touchpad to mouse movement
- Hotkey definition

### 5. **Online Features**
- Create and join online rooms
- Share controller with remote peers
- Gyro/touchpad streaming to server
- Real-time synchronization of settings
- Room-based access control

### 6. **RGB Lightbar Control**
- Color picker UI
- Preset colors
- Audio-reactive LED mode
- Brightness adjustment
- Per-device LED customization

### 7. **Multi-language Support**
- English, Polish, Spanish, Turkish
- Font support for special characters (Cyrillic, CJK, Thai)
- Crowdin translation platform integration

### 8. **Cross-Platform**
- Windows (full features)
- Linux (no ViGEm emulation)
- macOS (no ViGEm emulation)

---

## User Workflow

### Basic Setup

1. **Launch Application**
   - Application initializes GLFW window
   - Loads AppSettings and controller configurations
   - Connects to online server (optional)

2. **Connect Controller**
   - DualSense/DualShock4 connected via USB or Bluetooth
   - duaLib detects and initializes controller
   - Controller status shown in MainWindow

3. **Select Emulation Mode**
   - User selects Xbox 360 or DualShock 4 emulation
   - Vigem creates virtual controller target
   - Games recognize virtual controller

4. **Configure Settings**
   - LED color adjustment
   - Audio passthrough activation
   - Adaptive trigger effect selection
   - Button remapping
   - Gyro/touchpad options

5. **Save Configuration**
   - Settings auto-saved to JSON
   - Persisted across sessions

### Online Sharing

1. **Create Room**
   - User clicks "Create Room" in online tab
   - Specifies room name and password
   - Server assigns room ID

2. **Invite Peer**
   - Share room ID with remote peer
   - Peer joins using room ID

3. **Request Control**
   - Peer requests to control local console
   - User approves/denies request
   - If approved, peer's ViGEm receives user's controller input

---

## Threading Model

- **Main Thread**: GLFW event loop, ImGui rendering, UI updates
- **ViGEm Thread**: Updates emulated controller states (Windows only)
- **Network Thread**: ENet receive loop, server communication
- **Audio Capture Thread**: miniaudio callback (driver-managed)
- **UDP Listen Thread**: Listen for UDP commands on port 6969
- **Updater Thread**: Check for application updates

All threads use atomic variables or mutexes for thread-safe state sharing.

---

## Configuration Storage

### Locations

- **Windows**: `C:\Users\{user}\Documents\DualSenseY\`
- **Linux**: `~/.local/share/DualSenseY/`
- **macOS**: `~/Library/Application Support/DualSenseY/`

### Files

- `appSettings.json`: Global application settings
- `settings_0.json` to `settings_3.json`: Per-controller configurations
- `controllerEmulationCache.json`: ViGEm target data (Windows)

---

## External Protocol Support

### UDP Port 6969 (DSX Compatibility)

Games with DSX mods can send JSON commands:

```json
{
  "instructions": [
    {
      "type": 1,  // TriggerUpdate
      "parameters": [0, 1, 100]  // L2 trigger, effect_type, intensity
    },
    {
      "type": 2,  // RGBUpdate
      "parameters": [255, 0, 0]  // R, G, B
    }
  ]
}
```

### ENet Networking

Central server: `maluch.mikr.us:30151`
- Private room management
- Peer discovery
- Connection brokering

---

## Error Handling

- **ViGEm Unavailable**: UI shows warning, disables emulation
- **Audio Device Disconnected**: AudioPassthrough.validate() retries connection
- **Network Disconnection**: Client reconnects on next heartbeat
- **Invalid Configuration**: Revert to defaults, log error
- **Permission Errors**: Prompt for admin elevation (Windows)

---

## Future Extensibility

### Possible Enhancements

1. **Controller Motion Capture**: Stream gyro/accel data to PC applications
2. **Haptic Playback Library**: Pre-recorded haptic patterns for games
3. **Cross-Platform Emulation**: ViGEm alternatives for Linux/macOS
4. **Controller Diagnostics**: Battery, temperature, connection quality monitoring
5. **Macro Recording**: Record and playback button sequences
6. **Plugin System**: Extend functionality via third-party modules
7. **Web Dashboard**: Remote configuration via web browser

---

## Development Guidelines

### Code Organization

- **Headers** (`include/`): Public interfaces and data structures
- **Implementation** (`source/`): All functionality
- **Third-party** (`thirdparty/`): External dependencies (vendored)
- **Resources** (`resources/`): UI assets, translations

### Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/DualSenseY
```

### Testing

- No formal test suite (monolithic application)
- Manual testing on Windows, Linux, macOS
- GitHub Actions CI runs build checks

---

## Summary

**DualSenseY** is a sophisticated, feature-rich controller application combining:
- **Real-time I/O**: DualSense controller input, ViGEm emulation, audio capture
- **Networking**: ENet-based peer-to-peer controller sharing
- **Configuration**: Persistent JSON storage, multiple profiles
- **UI**: ImGui-based cross-platform interface
- **Cross-Platform**: Windows (full), Linux/macOS (limited)

Its architecture separates concerns cleanly:
- **I/O Layer**: duaLib (input), ViGEm (output), miniaudio (audio)
- **Business Logic**: Settings, filtering, remapping
- **Networking**: ENet client with server brokering
- **UI**: ImGui renderer with real-time updates
- **Persistence**: JSON serialization with platform-specific paths

This design allows independent evolution of each subsystem while maintaining tight integration via shared data structures (s_scePadSettings, s_ScePadData) passed by reference through constructor injection.
