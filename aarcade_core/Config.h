#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>

class ArcadeConfig {
private:
    std::string databasePath_;

    void debugOutput(const std::string& message) {
        std::string debugMsg = "[ArcadeConfig] " + message + "\n";
        OutputDebugStringA(debugMsg.c_str());
    }

    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(' ');
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

public:
    ArcadeConfig() : databasePath_("database.db") {} // Default value

    bool loadFromFile(const std::string& filename = "config.ini") {
        // Get the full path to help with debugging
        char fullPath[MAX_PATH];
        GetFullPathNameA(filename.c_str(), MAX_PATH, fullPath, NULL);

        std::string fullPathMsg = "Looking for config file at: " + std::string(fullPath);
        debugOutput(fullPathMsg);

        std::ifstream file(filename);
        if (!file.is_open()) {
            debugOutput("Config file '" + filename + "' not found. Using default values.");
            createDefaultConfig(filename);
            return false;
        }

        debugOutput("Loading config from: " + filename);

        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);

            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }

            // Find the equals sign
            size_t equalPos = line.find('=');
            if (equalPos == std::string::npos) {
                continue;
            }

            std::string key = trim(line.substr(0, equalPos));
            std::string value = trim(line.substr(equalPos + 1));

            // Remove quotes if present
            if (value.length() >= 2 && value[0] == '"' && value[value.length() - 1] == '"') {
                value = value.substr(1, value.length() - 2);
            }

            // Process known keys
            if (key == "database_path") {
                databasePath_ = value;
                debugOutput("Set database_path = " + databasePath_);
            }
        }

        file.close();
        debugOutput("Config loaded successfully.");
        return true;
    }

    void createDefaultConfig(const std::string& filename = "config.ini") {
        std::ofstream file(filename);
        if (!file.is_open()) {
            debugOutput("Failed to create default config file: " + filename);
            return;
        }

        file << "# Arcade Core Configuration File\n";
        file << "# This file is automatically created with default values\n";
        file << "\n";
        file << "# Database Configuration\n";
        file << "# Specify the path to the SQLite database file\n";
        file << "# Can be relative to the executable or an absolute path\n";
        file << "database_path = database.db\n";
        file << "\n";
        file << "# Additional configuration options will be added here in the future\n";

        file.close();
        debugOutput("Created default config file: " + filename);
    }

    // Getters
    const std::string& getDatabasePath() const {
        return databasePath_;
    }

    // Setters (for future use)
    void setDatabasePath(const std::string& path) {
        databasePath_ = path;
    }
};

#endif