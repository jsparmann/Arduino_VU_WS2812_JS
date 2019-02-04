# Arduino VU WS2812 JS
Fork from [s-marley/Uno_vu_line](https://github.com/s-marley/Uno_vu_line/) -> thanks, nice work ;).
Code wurde für den Arduino UNO & Co. geschrieben. Es wird ein Stereo VU-Meter für die WS2812b LED's realisiert.

## Features
- 10 VU-Meter passend zur Musik.
- 2 Standby Pattern ohne Musik bezug.
- Helligkeitsregelung über Fotowiderstand hinzugefügt.
- Starten mit dem letztn Pattern vor dem Ausschalten.
- Taster zum Umschalten der Pattern, langer druck -> Automodus
- Support für 75 LED's per Channel (150 Total). Much more on Mega etc.
- Completely open source and hackable. Create your own patterns or use it in whatever you like!

## Extra info from s-marley/Uno_vu_line
- The more LEDs you have, the more power you will need to provide. Make sure your power source is able to cope.
- If the controller seems to go weird, I've found a quick reflash of the code usually clears it up quickly.
- This project is for entertainment purposes only and the output is not calibrated in any way.
- Code is written to run on the Nano / Uno or equivalent. Using an AVR with more SRAM will result in being able to use more LEDs.

Code is based on demos from the [FastLED](http://fastled.io/) library and code from [Cinelights](https://www.youtube.com/channel/UCOG6Bi2kvpDa1c8gHWZI5CQ) YouTube channel.
