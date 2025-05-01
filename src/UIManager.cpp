#include "UIManager.h"
#include "TrainController360x240.h"
#include <PageManager.h>
#include <MenuPage.h>
#include <ThreadSafeTFT.h>
#include "Config.h"
#include "MatrixKeyboard.h"

UIManager::UIManager() : tft(), uiTaskHandle(nullptr), wifiManager(nullptr) {}

UIManager::~UIManager() {
    if (uiTaskHandle) {
        vTaskDelete(uiTaskHandle);
    }
    if (wifiManager) {
        delete wifiManager;
    }
}

void UIManager::begin() {

    //create an instance of keyboard
    static constexpr uint8_t rowPins[] = {D15, D16};
    static constexpr uint8_t colPins[] = {D17, D18, D19};
    keyboard = new MatrixKeyboard(rowPins, colPins, D22);
    
    // Create an instance of AnalogSwitch
    analogSwitch = new AnalogSwitch(D14, D15, D16);
    
    // Create WiFiConfigManager
    wifiManager = new WiFiConfigManager("/wifi_config.json");

    // Initialize the TFT display
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // Initialize ThreadSafeTFT
    ThreadSafeTFT::init(&tft);

    // Setup the menus, this is the root node
    setupMenus();
    // Show splash screen
    PageManager::showSplash(trainControllerImage, false, trainControllerPalette, 360, 240, 3000);
}

void UIManager::startTask() {
    // Create the UI task
    xTaskCreate(
        uiTask,         // Task function
        "UITask",       // Task name
        4096,           // Stack size (in bytes)
        this,           // Task parameter (pass the UIManager instance)
        1,              // Task priority
        &uiTaskHandle   // Task handle
    );
}

void UIManager::uiTask(void* param) {
    UIManager* self = static_cast<UIManager*>(param);

    while (true) {
        // Handle input and draw the current page
        self->analogSwitch->switchTo(0); // Switch to channel 0
        PageManager::handleInput(self->keyboard);
    }
}

void UIManager::setupMenus() {
    auto mainMenu = std::make_unique<MenuPage>();
    auto wifiSubMenu = std::make_unique<MenuPage>(mainMenu.get());

    // Setup WiFi submenu
    wifiSubMenu->addItem("Scan for Networks", nullptr, [this]() {
        // Show loading during scan
        PageManager::showLoading("Scanning for WiFi networks...");
        
        // Vector to store discovered SSIDs
        std::vector<ListItem> networks;
        
        // Start scanning for SSIDs
        wifiManager->startSSIDScan([&networks](const String& ssid) {
            // Check if SSID is already in the list to avoid duplicates
            bool isDuplicate = false;
            for (const auto& network : networks) {
                if (network.label == ssid) {
                    isDuplicate = true;
                    break;
                }
            }
            
            if (!isDuplicate) {
                networks.push_back({ssid, 0}); // Add to list with dummy value
            }
        });
        
        // Wait a bit for scan results
        delay(5000);
        
        // Stop scanning
        wifiManager->stopSSIDScan();
        PageManager::hideLoading();
        
        // Show list dialog with discovered networks
        PageManager::showListDialog("Select WiFi Network", networks,
            [this](bool accepted, ListItem selected) {
                if (accepted) {
                    // Load current properties to update just the SSID
                    WiFiConfigManager::NetworkProperties properties = wifiManager->loadNetworkProperties();
                    properties.ssid = selected.label;
                    
                    // Save the updated properties
                    wifiManager->saveNetworkProperties(properties);
                    
                    // Ask for password
                    PageManager::showInput("Enter password for " + selected.label + ":", 
                        ALPHANUMERIC, [this, properties](String input, bool ok) {
                            if (ok) {
                                // Update properties with password
                                WiFiConfigManager::NetworkProperties updatedProps = properties;
                                updatedProps.password = input;
                                wifiManager->saveNetworkProperties(updatedProps);
                                PageManager::showPopup("WiFi credentials saved!");
                            }
                        });
                }
            });
    });

    wifiSubMenu->addItem("Network Settings", nullptr, [this]() {
        auto networkSettingsMenu = std::make_unique<MenuPage>();
        WiFiConfigManager::NetworkProperties props = wifiManager->loadNetworkProperties();
        
        // DHCP toggle option
        networkSettingsMenu->addItem(props.dhcp ? "DHCP: On" : "DHCP: Off", nullptr, [this]() {
            WiFiConfigManager::NetworkProperties props = wifiManager->loadNetworkProperties();
            props.dhcp = !props.dhcp;
            wifiManager->saveNetworkProperties(props);
            PageManager::showPopup(props.dhcp ? "DHCP enabled" : "DHCP disabled");
            PageManager::popPage(); // Return to previous menu
            setupMenus();  // Refresh menu to show updated state
        });
        
        // IP Address setting
        networkSettingsMenu->addItem("IP Address: " + props.ip, nullptr, [this]() {
            PageManager::showInput("Enter IP Address:", NUMERIC_IP, [this](String input, bool ok) {
                if (ok) {
                    WiFiConfigManager::NetworkProperties props = wifiManager->loadNetworkProperties();
                    props.ip = input;
                    wifiManager->saveNetworkProperties(props);
                    PageManager::showPopup("IP Address saved");
                }
            });
        });
        
        // Subnet Mask
        networkSettingsMenu->addItem("Subnet Mask: " + props.mask, nullptr, [this]() {
            PageManager::showInput("Enter Subnet Mask:", NUMERIC_IP, [this](String input, bool ok) {
                if (ok) {
                    WiFiConfigManager::NetworkProperties props = wifiManager->loadNetworkProperties();
                    props.mask = input;
                    wifiManager->saveNetworkProperties(props);
                    PageManager::showPopup("Subnet Mask saved");
                }
            });
        });
        
        // Gateway/Router
        networkSettingsMenu->addItem("Gateway: " + props.router, nullptr, [this]() {
            PageManager::showInput("Enter Gateway Address:", NUMERIC_IP, [this](String input, bool ok) {
                if (ok) {
                    WiFiConfigManager::NetworkProperties props = wifiManager->loadNetworkProperties();
                    props.router = input;
                    wifiManager->saveNetworkProperties(props);
                    PageManager::showPopup("Gateway saved");
                }
            });
        });
        
        // DNS Server
        networkSettingsMenu->addItem("DNS Server: " + props.dns, nullptr, [this]() {
            PageManager::showInput("Enter DNS Server:", NUMERIC_IP, [this](String input, bool ok) {
                if (ok) {
                    WiFiConfigManager::NetworkProperties props = wifiManager->loadNetworkProperties();
                    props.dns = input;
                    wifiManager->saveNetworkProperties(props);
                    PageManager::showPopup("DNS Server saved");
                }
            });
        });
        
        PageManager::pushPage(std::move(networkSettingsMenu));
    });
    
    // Connect to network option
    wifiSubMenu->addItem("Connect", nullptr, [this]() {
        PageManager::showLoading("Connecting to WiFi...");
        wifiManager->startNetwork();
        
        // Wait for connection (you might want to implement a timeout)
        for (int i = 0; i < 20; i++) {
            if (wifiManager->isConnected()) {
                break;
            }
            delay(500);
        }
        
        PageManager::hideLoading();
        
        if (wifiManager->isConnected()) {
            // Get current connection information
            WiFiConfigManager::ConnectionInfo info = wifiManager->getConnectionInfo();

            // Display in a popup or status screen
            String statusMessage = "SSID: " + info.ssid + "\n" +
                                "IP: " + info.ip + "\n" +
                                "Subnet: " + info.subnet + "\n" +
                                "Gateway: " + info.gateway + "\n" +
                                "Signal: " + String(info.rssi) + " dBm\n" +
                                "MAC: " + info.macAddress;

            PageManager::showPopup(statusMessage.c_str());
        } else {
            PageManager::showPopup("Failed to connect");
        }
    });
    
    // Disconnect from network option
    wifiSubMenu->addItem("Disconnect", nullptr, [this]() {
        wifiManager->stopNetwork();
        PageManager::showPopup("Disconnected from WiFi");
    });

    wifiSubMenu->addItem("Show Current Config", nullptr, [this]() {
        WiFiConfigManager::NetworkProperties props = wifiManager->loadNetworkProperties();
        String configInfo = "SSID: " + props.ssid + "\n" +
                           "DHCP: " + String(props.dhcp ? "Yes" : "No") + "\n";
        
        if (!props.dhcp) {
            configInfo += "IP: " + props.ip + "\n" +
                         "Mask: " + props.mask + "\n" +
                         "Gateway: " + props.router + "\n" +
                         "DNS: " + props.dns;
        }

        // Get current connection information
        WiFiConfigManager::ConnectionInfo info = wifiManager->getConnectionInfo();

        // Display in a popup or status screen
        configInfo +=  "Connection info: \n";
        configInfo +=  "IP: " + info.ip + "\n";
        configInfo +=  "Subnet: " + info.subnet + "\n";
        configInfo +=  "Gateway: " + info.gateway + "\n";
        configInfo +=  "Signal: " + String(info.rssi) + " dBm\n";
        configInfo +=  "MAC: " + info.macAddress;
        
        PageManager::showPopup(configInfo.c_str());
    });

    // Setup main menu
    mainMenu->addItem("Configure WiFi", std::move(wifiSubMenu));
    
    // Add other main menu items
    mainMenu->addItem("Option 1", nullptr, []() {
        PageManager::showPopup("Option 1 selected");
    });
    
    mainMenu->addItem("Option 3", nullptr, []() {
        PageManager::showInput("Enter value:", ALPHANUMERIC, [](String input, bool ok) {
            if (ok) {
                Serial1.printf("Input: %s\n", input.c_str());
            } else {
                Serial1.println("Input cancelled");
            }
        });
    });
    
    mainMenu->addItem("Loading example", nullptr, []() {
        PageManager::showLoading("Loading...");
        delay(5000);
        PageManager::hideLoading();
    });
    
    mainMenu->addItem("Option 5");
    mainMenu->addItem("Option 6");

    // Push the main menu to the PageManager
    PageManager::pushPage(std::move(mainMenu));
}