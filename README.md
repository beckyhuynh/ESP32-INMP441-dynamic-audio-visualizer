# INMP441-dynamic-music-visualizer
Music visualizer using esp32 and inmp441 mic that takes live music/ audio and displays it on ili9341 tft lcd screen, via a bar graph/ circular rotating display

Inspired by youtube visualizers whenever i looked up songs: I like music and wanted to make my own visualizer
<img width="600" height="338" alt="image" src="https://github.com/user-attachments/assets/80d987a9-5d6b-4a62-965f-78e155c0d62f" />


Wiring guide:
From esp32 to tft lcd
<img width="975" height="878" alt="image" src="https://github.com/user-attachments/assets/0c9e6e78-e8b9-406a-a6df-08e9d42102d8" />

From esp32 to mic

<img width="724" height="444" alt="image" src="https://github.com/user-attachments/assets/9a7b7f42-4fab-4f4d-bd72-12599431b6f3" />

Product:
<img width="1600" height="1200" alt="image" src="https://github.com/user-attachments/assets/7b151cea-e7f3-47e1-9e80-303d04a33fc7" />
<img width="1200" height="1600" alt="image" src="https://github.com/user-attachments/assets/ae7de784-8e72-4134-ba54-cb60573890e1" />


Demonstration:!!!!!
- please click on this GOOGLE DRIVE link for the different visualizers to different songs. suggestions for new songs? :>
https://drive.google.com/drive/folders/12u9cZiUxfz3R1oFt0d3_5P34Rd-_aWLp?usp=sharing


Sources for some of the research I did that allowed me to dive deeper into the project: special thanks to scott marley for making the fft band logic clearer to understand:

https://github.com/atomic14/esp32-i2s-mic-test
https://www.youtube.com/watch?v=UkJIMCtsypo&ab_channel=OwenO%27Brien
https://www.youtube.com/watch?v=m8LwPNXqK9o&t=98s&ab_channel=ThatProject
https://www.youtube.com/watch?v=Mgh2WblO5_c&ab_channel=ScottMarley
https://doc-tft-espi.readthedocs.io/graphics/
https://www.youtube.com/watch?v=9CI4Y2V4sxE&ab_channel=AkashMurthy

Important notice:!!
This project works best with some kind of noise amplifier or LOUD music playing. I used my fender amp to make the music more alive for the mic. The mic is not very sensitive with regular microphone or laptop speaker, the bars will barely spike. This might be due to the manufacturing of the mic, since it is not a professional recording mic, or could also be due to the root mean square method which better detects PEAKS in loud sounds.


