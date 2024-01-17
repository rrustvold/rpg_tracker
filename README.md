This project documents all of the hardware and code needed to build and run my RPG initiative tracker system. It leverages Home Assistant, ESP home, and the existing tag reader repo github://adonno/tagreader/tagreader.yaml.

I will be adding photos/videos to demonstrate what it actually does in the future, but for now here is what is needed:

1. A running Home Assistant instance with ESP home
2. An NFC tag reader built from:
  a. ESP8266 D1 Mini
  b. PN532 NFC Reader
  c. Optional: TM1637 display to show turn length, monster HP, and other stats to the players at the table.
3. A 3d printed NFC holder. This part houses the tag reader and hangs onto a standard GM screen.
4. 3d printer GM screen initiative tracker miniatures for the GM and each player. HeroForge provides such models, but other models can be found online to 3d print.
5. Blank NFC tags
6. Optional: NeoPixel LED light strip to line the underside of the table at which you play your RPG games. With the light strip you can activate a "hot seat" effect for each player when it is there turn.

