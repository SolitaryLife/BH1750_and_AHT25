bool checkWiFiConnection() {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi Connected...");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        Serial.println("Connecting to WiFi...");
        delay(5000);
        return false;
    }
}

bool reconnectTCP() {
    if (espClient.connect(_serverIP, _serverPort)) {
        Serial.println("Connected to Node-RED server");
        // String message = "Node-RED Connected";
        // espClient.println(message);
        return true;
    } else {
        Serial.println("Node-RED Connection failed!");
        delay(2000);
        return false;
    }
}

bool initializeBH1750(int retryLimit = 3, int retryDelay = 2000) {
    int attempt = 0;
    while (attempt < retryLimit) {
        Serial.printf("Initializing BH1750... Attempt %d of %d\n", attempt + 1, retryLimit);
        if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire)) {
            Serial.println(F("BH1750 initialized successfully."));
            return true;  // successful start
        } else {
            Serial.println(F("Error initializing BH1750! Retrying..."));
            attempt++;
            delay(retryDelay);  // Wait some time before trying again.
        }
    }
    Serial.println(F("Failed to initialize BH1750 after retries."));
    return false;  // Initialization failed after the required number of attempts.
}

String generateJson() {

    // WiFi connection status
    jsonDoc["statusConnectWIFI"] = (WiFi.status() == WL_CONNECTED) ? "connected" : "disconnected";
    jsonDoc["IPAddRessWIFI"] = WiFi.localIP().toString();

    // Node-RED connection status
    jsonDoc["statusConnectNode"] = espClient.connected() ? "connected" : "disconnected";
    if (espClient.connected()) {
        jsonDoc["IPAddressNode"] = _serverIP;
        jsonDoc["Port"] = _serverPort;
    } else {
        jsonDoc["IPAddressNode"] = "N/A";
        jsonDoc["Port"] = 0;
    }

    // Create Jsonarray 
    JsonArray sensors = jsonDoc.createNestedArray("sensors");

    //Sensor (BH1750)
    JsonObject sensorBH1750 = sensors.createNestedObject();
    // Configure sensor name
    sensorBH1750["name"] = "BH1750";
    // Set working status
    if (_light) {
        sensorBH1750["status"] = "running";
        sensorBH1750["LightLevel"] = isnan(_lux) ? "Failed to read" :  String(_lux); // lx
    } else {
        sensorBH1750["status"] = "stopped";
        sensorBH1750["LightLevel"] = "Sensor not initialized";
    }

    // Sensor (AHT25)
    JsonObject sensorAHT25 = sensors.createNestedObject();
    // Configure sensor name
    sensorAHT25["name"] = "AHT25";
    // Set working status
    sensorAHT25["status"] = "N/A";
    sensorAHT25["temperature"] = "N/A";  // °C
    sensorAHT25["humidity"] = "N/A"; // %

    // Convert JSON Document to String
    String output;
    serializeJson(jsonDoc, output);
    return output;
}