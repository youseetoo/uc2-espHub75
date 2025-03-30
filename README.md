# ESP32 Hub75 RGB Panel Firmware for Laser protection

This firmware uses [ESP32-HUB75-MatrixPanel-I2S-DMA](https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA) to control an RGB matrix (64x32 in this example) via a simple JSON serial protocol.

This is used to have brightfield while providing a laser cover. Only those leds are turned on which will provide brightfield 


## Project Structure

- **platformio.ini**  
  Defines the PlatformIO environment and dependencies.
- **src/main.cpp**  
  Main firmware code for the ESP32, which:
  1. Initializes and configures the Hub75 panel.
  2. Waits for JSON commands over serial.
  3. Draws filled circles on the matrix based on the commands.

## Dependencies

Look into `platformio.ini` under `lib_deps`.

## Usage

### Connection/Pinout

Configure the pin connections if needed in the `HUB75_I2S_CFG` structure (inside `setup()` in `main.cpp`). Refer to the documentation for the correct pin assignments based on your specific hardware.

### Serial Commands

Send a single line of JSON ending in `\n`. Example:
```
{"task":"/hub_act","x":10,"y":8,"diameter":16,"intensity":255,"r":255,"g":0,"b":0,"qid":101}
```

- **task**: `"/hub_act"`  
- **x**: X-coordinate of the circle center  
- **y**: Y-coordinate of the circle center  
- **diameter**: Circle diameter in pixels  
- **intensity**: Panel brightness (0-255)  
- **r**: Red component (0-255)  
- **g**: Green component (0-255)  
- **b**: Blue component (0-255)  
- **qid** (optional): Request ID to match response with request  

After drawing the circle, the firmware sends a JSON response:
```
{"qid":0,"success":1}
```
Replace `qid` with the incoming value if provided.

## Example

```
{"task":"/hub_act","x":0,"y":0,"diameter":10,"intensity":255,"r":255,"g":0,"b":0,"qid":0}
```
This draws a red filled circle at coordinates (0,0), diameter of 10, maximum brightness. The panel responds:
```
{"qid":0,"success":1}
```

## Compile and Flash

1. Install [PlatformIO](https://platformio.org/).
2. Clone or download this repository.
3. Open the folder in your preferred PlatformIO environment (VSCode, etc.).
4. Modify `platformio.ini` as needed for your board.
5. Upload the firmware:
   ```
   pio run --target upload
   ```
6. Open the serial monitor (115200 baud):
   ```
   pio device monitor
   ```
7. Send JSON commands terminated by newline to draw circles on the panel.