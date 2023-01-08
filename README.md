## LoRa-Node-Module
Using an Arduino controller, create a LoRa transceiver for temperature, moisture, and relay circuits. The temperature sensor (ds18b20) 
is connected to the Arduino through a single wire, whilst the capacitive moisture sensor and relay circuit are connected via digital pins. 
This project is a consolidated version of these components, the following four repositories must be set up in order to test this application.
1. LoRa Node Module  https://github.com/sarbbjeet/LoRa-Node-Module 
2. LoRa Gateway Module https://github.com/sarbbjeet/LoRa-Gateway-Module
3. Cloud Server https://github.com/sarbbjeet/Agro-server
4. Mobile App https://github.com/sarbbjeet/Agro-monitoring-app

## Architecture of the project 
The image below depicts the overall architecture of the project, which includes sensor units, a gateway module, a cloud server, and a front user interface application.
This project features LoRa communication, MQTT client/broker interaction, and database queries communicating with JSON API.


<img width="716" alt="Screenshot 2023-01-04 at 10 40 32" src="https://user-images.githubusercontent.com/9445093/211218105-fccd1078-afa1-4744-a16c-c1dd53dca03f.png">

## Circuit diagram 
Circuit diagram shows the connectivity of LoRA transceiver and sensors with arduino
<img width="442" alt="Screenshot 2022-12-15 at 00 26 24" src="https://user-images.githubusercontent.com/9445093/211222904-3b0ecbcc-78ca-4c83-8012-907968df4d48.png">
