# ESP32-INMP441-dynamic-audio-visualizer
## **FINAL DEMO LINKED NEAR THE END OF THIS README**
## PURPOSE:
- Music visualizer using esp32 and inmp441 mic that takes live music/ audio and displays it on ili9341 tft lcd screen, via a bar graph/ circular rotating display
- Inspired by music visualizers that can be found when you search up audios for certain songs. I wanted to make my own version using hardware.

<p align="center">
  <img width="600" height="338" alt="image" src="https://github.com/user-attachments/assets/80d987a9-5d6b-4a62-965f-78e155c0d62f" />
</p>

## HARDWARE: (setup)
### 1. Main parts used: (all parts can be found on Amazon)
- ESP32
- ILLI9341 TFT LCD display
- INMP441 microphone (**make sure you solder the legs(pins) onto the circular main part before using, most mics come delivered without being soldered**)
- Wires

### 2. Hardware overview:
<p align="center">
  <img width="800" height="500" alt="image" src="https://github.com/user-attachments/assets/7b151cea-e7f3-47e1-9e80-303d04a33fc7" />
</p>

<p align="center">
  <img width="800" height="800" alt="image" src="https://github.com/user-attachments/assets/ae7de784-8e72-4134-ba54-cb60573890e1" />
</p>

### 3. Wiring guide:
#### - From esp32 to tft lcd:

<p align="center">
  <img width="600" height="500" alt="image" src="https://github.com/user-attachments/assets/0c9e6e78-e8b9-406a-a6df-08e9d42102d8" />
</p>

#### - From esp32 to mic:
<p align="center">
  <img width="400" height="300" alt="image" src="https://github.com/user-attachments/assets/9a7b7f42-4fab-4f4d-bd72-12599431b6f3" />
</p>

## SOFTWARE (setup):
- refer to attached source code

**To run this project, simply wire per hardware guide, download needed libraries, copy the code into ArduinoIDE, change the following lines and hit play on any songs or sounds you want :)**

### Libraries to download:
- arduinoFFT by Enrique
- TFT_eSPI by Bodmer
- follow set ups for esp32 board linked below if you haven't already

### Main lines that can be changed:
1. 58, depending on your testing environment, play around with values to filter out low noise that you dont want displayed 
2. 165, change to constrain max bar height (default is set at 90% of the screen's height)
3. 169-221, comment out the different showSpectrum functions depending on what display you want to show

## 🎆🎆FINAL VISUAL DEMO 🎆🎆
- GOOGLE DRIVE link for the different visualizers to different songs. suggestions for new songs? :>
https://drive.google.com/drive/folders/12u9cZiUxfz3R1oFt0d3_5P34Rd-_aWLp?usp=sharing


## Important notices and final notes:
- This project works best with some kind of noise amplifier or LOUD music playing. I used my fender guitar amp to make the music more alive for the mic.
- The mic is not very sensitive with regular microphone or laptop speaker, the bars will spike very little.
- This might be due to the manufacturing of the mic, since it is not a professional recording mic, or could also be due to the root mean square method which better detects PEAKS in loud sounds.

  
## SOURCES
### Sources for the research I did that allowed me to dive deeper into the project, some of the screenshots and code are adapted from here: 

- [How to use ESP32 with the ILLI9341 board (libraries and more)](https://randomnerdtutorials.com/esp32-tft-touchscreen-display-2-8-ili9341-arduino/)
- [Code test to see if your isolated mic works](https://github.com/atomic14/esp32-i2s-mic-test)
- [Overview for INMP441 mic](https://www.youtube.com/watch?v=UkJIMCtsypo&ab_channel=OwenO%27Brien)
- [Amazing video for structuring a visualizer inspiration (only for a led matrix and not lcd display)](https://www.youtube.com/watch?v=Mgh2WblO5_c&ab_channel=ScottMarley)
- [Functions for drawing on ILLI9341](https://doc-tft-espi.readthedocs.io/graphics/)
- [What is root mean square](https://www.youtube.com/watch?v=9CI4Y2V4sxE&ab_channel=AkashMurthy)
