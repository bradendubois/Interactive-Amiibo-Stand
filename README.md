# arduino-amiibo-stand

A fork of [ModusPwnin](https://github.com/ModusPwnin)'s [Interactive-Amiibo-Stand](https://github.com/ModusPwnin/Interactive-Amiibo-Stand), overhauled to reduce code size, fix bugs, and streamline support for future Amiibo releases.

## Requirements

The following libraries are required:
- [wavehc](https://code.google.com/archive/p/wavehc/)
- [NFC](https://github.com/adafruit/Adafruit_NFCShield_I2C)

## Tradeoffs

The advantage of this re-write is that once this code is flashed to an Arduino, it should *never need an update* to support future Amiibo's when they are released. This is done through imposing an arbitrary naming scheme on the audio clips that the user wants to play on particular Amiibo. As a result, the code size decreases from ~1500 lines to just under 200.

## Formatting Audio Data

There are (at most) two clips that will play on a particular Amiibo. The first song the user wants to play must be formatted as "\<CID\>.wav", where \<CID\> corresponds to the Character ID of the Amiibo (see tables in the ``cid-tables`` directory). After this clip has played, if the Amiibo is still present, it can play (and loop until the Amiibo is removed) a second clip, formatted "\<CID\>_song.wav". 

If an audio clip is not present for a particular Amiibo, it will print out an error message, followed by the name of the file it expected to find. If you get a new Amiibo, that'll be the easiest way to find what the CID/filename should be.

## Where to get audio data?

**No audio is hosted anywhere in this repository.**

You can get various Nintendo sound clips from different websites already, such as [sound-resource.com](https://www.sounds-resource.com/wii_u/supersmashbrosforwiiu/). Intro sounds for characters and whatnot are there for the Wii U characters. I do not plan on hosting any audio in this repository.

## Acknowledgements / Other Resources

- For a neat video showing what this can look like when done, see [Modus](https://github.com/ModusPwnin)'s (the author of the original repository) [video showing it in action](https://www.youtube.com/watch?v=nZ2LU7xCp8U&feature=youtu.be).
- Modus also made an [imgur album](https://imgur.com/a/HaJBy) showing it being built.
- [Willem_Pijl](https://www.instructables.com/member/Willem_Pijl/) made their own [instructable building theirs](https://www.instructables.com/id/Interactive-Arduino-Amiibo-Stand/), and they use Modus's original code on it, so following that guide should also work. I think this one makes a better looking stand than the original.
