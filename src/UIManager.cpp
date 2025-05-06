#include "UIManager.h"
#include "TrainController360x240.h"
#include <PageManager.h>
#include <MenuPage.h>
#include <ThreadSafeTFT.h>
#include "Config.h"
#include "MatrixKeyboard.h"
#include "LocoDriverPage.h"
#include "LocoCommandManagerFactory.h" 

UIManager::UIManager() : tft(), uiTaskHandle(nullptr) {}

UIManager::~UIManager() {
    if (uiTaskHandle) {
        vTaskDelete(uiTaskHandle);
    }
    // No need to delete wifiManager as it's now a singleton
}

void UIManager::begin() {

    //create an instance of keyboard
    static constexpr uint8_t rowPins[] = {D17, D18};
    static constexpr uint8_t colPins[] = {D14, D15, D16};
    keyboard = new MatrixKeyboard(rowPins, colPins, D19);
    
    // Create an instance of AnalogSwitch
    analogSwitch = new AnalogSwitch(D21, D22);
    
    // Initialize the WiFiConfigManager singleton with config path
    WiFiConfigManager::getInstance("/wifi_config.json");
    
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
    auto controlSystemMenu = std::make_unique<MenuPage>(mainMenu.get());

    // Setup WiFi submenu
    wifiSubMenu->addItem("Scan for Networks", nullptr, [this]() {
        // Show loading during scan
        PageManager::showLoading("Scanning for WiFi networks...");
        
        // Vector to store discovered SSIDs
        std::vector<ListItem> networks;
        
        // Start scanning for SSIDs using singleton
        WiFiConfigManager::getInstance().startSSIDScan([&networks](const String& ssid) {
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
        WiFiConfigManager::getInstance().stopSSIDScan();
        PageManager::hideLoading();
        
        // Show list dialog with discovered networks
        PageManager::showListDialog("Select WiFi Network", networks,
            [](bool accepted, ListItem selected) {
                if (accepted) {
                    // Load current properties to update just the SSID
                    WiFiConfigManager::NetworkProperties properties = WiFiConfigManager::getInstance().loadNetworkProperties();
                    properties.ssid = selected.label;
                    
                    // Save the updated properties
                    WiFiConfigManager::getInstance().saveNetworkProperties(properties);
                    
                    // Ask for password
                    PageManager::showInput("Enter password for " + selected.label + ":", 
                        ALPHANUMERIC, [properties](String input, bool ok) {
                            if (ok) {
                                // Update properties with password
                                WiFiConfigManager::NetworkProperties updatedProps = properties;
                                updatedProps.password = input;
                                WiFiConfigManager::getInstance().saveNetworkProperties(updatedProps);
                                PageManager::showPopup("WiFi credentials saved!");
                            }
                        });
                }
            });
    });

    wifiSubMenu->addItem("Network Settings", nullptr, []() {
        auto networkSettingsMenu = std::make_unique<MenuPage>();
        WiFiConfigManager::NetworkProperties props = WiFiConfigManager::getInstance().loadNetworkProperties();
        
        // DHCP toggle option
        networkSettingsMenu->addItem(props.dhcp ? "DHCP: On" : "DHCP: Off", nullptr, []() {
            WiFiConfigManager::NetworkProperties props = WiFiConfigManager::getInstance().loadNetworkProperties();
            props.dhcp = !props.dhcp;
            WiFiConfigManager::getInstance().saveNetworkProperties(props);
            PageManager::showPopup(props.dhcp ? "DHCP enabled" : "DHCP disabled");
            PageManager::popPage(); // Return to previous menu
            // Need different approach to refresh menu
            // Previously: setupMenus(); // Refresh menu to show updated state
        });
        
        // IP Address setting
        networkSettingsMenu->addItem("IP Address: " + props.ip, nullptr, []() {
            PageManager::showInput("Enter IP Address:", NUMERIC_IP, [](String input, bool ok) {
                if (ok) {
                    WiFiConfigManager::NetworkProperties props = WiFiConfigManager::getInstance().loadNetworkProperties();
                    props.ip = input;
                    WiFiConfigManager::getInstance().saveNetworkProperties(props);
                    PageManager::showPopup("IP Address saved");
                }
            });
        });
        
        // Subnet Mask
        networkSettingsMenu->addItem("Subnet Mask: " + props.mask, nullptr, []() {
            PageManager::showInput("Enter Subnet Mask:", NUMERIC_IP, [](String input, bool ok) {
                if (ok) {
                    WiFiConfigManager::NetworkProperties props = WiFiConfigManager::getInstance().loadNetworkProperties();
                    props.mask = input;
                    WiFiConfigManager::getInstance().saveNetworkProperties(props);
                    PageManager::showPopup("Subnet Mask saved");
                }
            });
        });
        
        // Gateway/Router
        networkSettingsMenu->addItem("Gateway: " + props.router, nullptr, []() {
            PageManager::showInput("Enter Gateway Address:", NUMERIC_IP, [](String input, bool ok) {
                if (ok) {
                    WiFiConfigManager::NetworkProperties props = WiFiConfigManager::getInstance().loadNetworkProperties();
                    props.router = input;
                    WiFiConfigManager::getInstance().saveNetworkProperties(props);
                    PageManager::showPopup("Gateway saved");
                }
            });
        });
        
        // DNS Server
        networkSettingsMenu->addItem("DNS Server: " + props.dns, nullptr, []() {
            PageManager::showInput("Enter DNS Server:", NUMERIC_IP, [](String input, bool ok) {
                if (ok) {
                    WiFiConfigManager::NetworkProperties props = WiFiConfigManager::getInstance().loadNetworkProperties();
                    props.dns = input;
                    WiFiConfigManager::getInstance().saveNetworkProperties(props);
                    PageManager::showPopup("DNS Server saved");
                }
            });
        });
        
        PageManager::pushPage(std::move(networkSettingsMenu));
    });
    
    // Connect to network option
    wifiSubMenu->addItem("Connect", nullptr, []() {
        PageManager::showLoading("Connecting to WiFi...");
        WiFiConfigManager::getInstance().startNetwork();
        
        // Wait for connection (you might want to implement a timeout)
        for (int i = 0; i < 20; i++) {
            if (WiFiConfigManager::getInstance().isConnected()) {
                break;
            }
            delay(500);
        }
        
        PageManager::hideLoading();
        
        if (WiFiConfigManager::getInstance().isConnected()) {
            // Get current connection information
            WiFiConfigManager::ConnectionInfo info = WiFiConfigManager::getInstance().getConnectionInfo();

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
    wifiSubMenu->addItem("Disconnect", nullptr, []() {
        WiFiConfigManager::getInstance().stopNetwork();
        PageManager::showPopup("Disconnected from WiFi");
    });

    wifiSubMenu->addItem("Show Current Config", nullptr, []() {
        WiFiConfigManager::NetworkProperties props = WiFiConfigManager::getInstance().loadNetworkProperties();
        String configInfo = "SSID: " + props.ssid + "\n" +
                           "DHCP: " + String(props.dhcp ? "Yes" : "No") + "\n";
        
        if (!props.dhcp) {
            configInfo += "IP: " + props.ip + "\n" +
                         "Mask: " + props.mask + "\n" +
                         "Gateway: " + props.router + "\n" +
                         "DNS: " + props.dns;
        }

        // Get current connection information
        WiFiConfigManager::ConnectionInfo info = WiFiConfigManager::getInstance().getConnectionInfo();

        // Display in a popup or status screen
        configInfo +=  "Connection info: \n";
        configInfo +=  "IP: " + info.ip + "\n";
        configInfo +=  "Subnet: " + info.subnet + "\n";
        configInfo +=  "Gateway: " + info.gateway + "\n";
        configInfo +=  "Signal: " + String(info.rssi) + " dBm\n";
        configInfo +=  "MAC: " + info.macAddress;
        
        PageManager::showPopup(configInfo.c_str());
    });

    // Setup Control System configuration submenu
    controlSystemMenu->addItem("System Type", nullptr, []() {
        // Create vector for system type options
        std::vector<ListItem> systemTypes = {
            {"DCC-Ex", static_cast<int>(LocoCommandManagerFactory::ManagerType::DccEx)},
            {"JMRI", static_cast<int>(LocoCommandManagerFactory::ManagerType::JMRI)}
        };
        
        // Get current manager type
        auto& factory = LocoCommandManagerFactory::getInstance();
        auto currentType = factory.getManagerType();
        
        // Pre-select current type
        int selectedIndex = (currentType == LocoCommandManagerFactory::ManagerType::JMRI) ? 1 : 0;
        
        // Show list dialog with system types
        PageManager::showListDialog("Select System Type", systemTypes, selectedIndex,
            [](bool accepted, ListItem selected) {
                if (accepted) {
                    auto& factory = LocoCommandManagerFactory::getInstance();
                    auto newType = static_cast<LocoCommandManagerFactory::ManagerType>(selected.value);
                    factory.setManagerType(newType);
                    PageManager::showPopup("System Type updated to " + selected.label);
                }
            });
    });
    
    controlSystemMenu->addItem("Connection URL", nullptr, []() {
        auto& factory = LocoCommandManagerFactory::getInstance();
        String currentUrl = factory.getConnectionUrl();
        
        PageManager::showInput("Enter Connection URL:", 
            ALPHANUMERIC, 
            currentUrl,
            [](String input, bool ok) {
                if (ok) {
                    auto& factory = LocoCommandManagerFactory::getInstance();
                    factory.setConnectionUrl(input);
                    PageManager::showPopup("Connection URL saved");
                }
            });
    });
    
    controlSystemMenu->addItem("Show Current Config", nullptr, []() {
        auto& factory = LocoCommandManagerFactory::getInstance();
        auto managerType = factory.getManagerType();
        String systemType = (managerType == LocoCommandManagerFactory::ManagerType::JMRI) ? "JMRI" : "DCC-Ex";
        String url = factory.getConnectionUrl();
        if (url.isEmpty()) {
            url = "<Not Set>";
        }
        
        String configInfo = "System Type: " + systemType + "\n" +
                           "Connection URL: " + url;
                           
        PageManager::showPopup(configInfo.c_str());
    });

    // Setup main menu
    mainMenu->addItem("Configure WiFi", std::move(wifiSubMenu));
    mainMenu->addItem("Control System", std::move(controlSystemMenu));
    

    // Push the main menu to the PageManager
    PageManager::pushPage(std::move(mainMenu));
}

void UIManager::setupLocoDriverPage() {
    PageManager::pushPage(std::make_unique<LocoDriverPage>());
}