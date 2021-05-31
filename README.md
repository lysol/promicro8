# promicro8
_A simple, quantizing CV sequencer based on an Arduino Pro Micro_

See `promicro.ino` for constants that should be changed to your liking, including the set of notes the sequencer will quantize against.

The voltage divider in the schematic near the TL072 is to adjust the level at which incoming voltages are normalized to either VCC or ground. You'll want to use a proper voltage regulator rather than USB power if you want true 5V levels. I measured as low as 4V on VCC with USB power from a laptop. Your other hardware may not care for these gate levels logically or you may not get as much gain as you want, or it might work fine, I don't really know because I just started building synthesizer circuits.

I haven't set mine up to use a regulator yet, but I don't know for sure how the digital levels will behave once I modify my sequencer to use it, hence the note about adjusting the voltage divider as needed. If your digital levels are truly 0 and 5 volts, you should be able to wire the digital outputs directly to the output jacks, though personally I think I'd rather buffer them still, I think. I don't really know what I'm doing and I don't know if they're already buffered from the Arduino.

# panel

![image](https://user-images.githubusercontent.com/55573/120209393-417faf00-c1f4-11eb-9779-f85efa5d57de.png)


# schematic

![image](https://user-images.githubusercontent.com/55573/120211250-53fae800-c1f6-11eb-9477-fc228bd5ab3f.png)
