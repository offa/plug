# Technicalities (Archived)

1. Connecting
1. Setting amp
1. Setting and clearing effects
1. Saving settings on amplifier
1. Choosing memory bank
1. Captured data 


## 1. Overview

It has five DSPs. One for each effect family and one for amplifier emulation. DSPs are addressed from 5 to 9. I don't know why first five addresses are omitted and if this means anything.
DSPs with their functionality are as follows:

- 0x05 - amplifier emulation
- 0x06 - stompbox effects
- 0x07 - modulation effects
- 0x08 - delay effects
- 0x09 - reverb effects

I also have found packets with device address 0x0a but don't know what this is.

## 2. Connecting

All the transmission between program and amplifier is using USB interrupt transfer. Endpoint to which you want to send data is 0x01 and USB interface which you want to claim is 0x00. If you want to receive data from amplifier use endpoint 0x81.
Each packet carries 64 bytes of data.
When connecting you should send two packets to the amp and get response for each of them. First should have value "0xc3" set on the first position (counting from zero) and second values "0x1a" and "0x03" on zeroth and first position.
After that a packet asking for amp's settings is send. The packet is of form "0xff" on zeroth and "0xc1" on first position. Amplifier responds to that sending:

- names of all presets in the form:
    - packet with name in a form: values "0x1c 0x01 0x04" as first three bytes, then slot number on forth byte and name encoded in ASCII on 32 bytes starting from sixteenth
    - packet with two first bytes and forth (slot of the preset) the same as in name packet 
- current state of the amp in the form:
    - name of the current preset in the form as described above
    - amplifiers setting in the form the same as when setting amplifier's settings (below) except that first byte is not "0x03" but "0x01" and preset number encoded on the forth byte
    - settings of four effects as described below with the same change to the first byte as in amplifier's settings and preset number encoded on the forth byte
    - setting of some mysterious device with address "0x0a"
    - confirmation packet same as in preset names 
- names with settings of all presets on the Mod and Dly/Rev knobs (first all settings for Mod knob then all settings for Dly/Rev knob) in the form:
    - name of the preset in the form: values "0x1c 0x01 0x04" as first three bytes, value "0x01" for Mod or "0x02" for Dly/Rev knob on third position, slot number on forth position (counted from zero), name encoded in ASCII on 24 bytes starting from sixteenth byte
    - settings of the effect in the same form as for setting the effect (below) except that first byte is "0x01" not "0x03" and preset number encoded on the forth byte
    - This packet is set only when sending Dly/Rev presets Dly/Rev knob hold settings for two effects so the next packet carries info about second effect, if effect is not set the packet have the same form as packet for clearing effect except that first byte is "0x01" not "0x03" and preset number encoded on the forth byte
    - confirmation packet with values "0x1c 0x01" on zeroth and first byte, "0x01" or "0x02" depending on the knob on third byte and preset number encoded on the forth byte

Wireshark file with whole initialization communication can be found here.

## 3. Setting amp

Format of data for setting amp is as follow:

```
     0                                       1
     0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
00 | 1c| 03|DSP|     00    | 01| 01|               00              |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
16 |mod|                         00                                |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
32 |vol|gai|ga2|mvl|tre|mid|bas|prs|?? |dep|bis|?? |   number  |ng |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
48 |ths|cab|?? |sag|bri| 01|?? |               00                  |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

DSP - for amplifier it is always 5
mod - amplifier model; different for each emulated amp
vol - volume
gai - gain
ga2 - gain 2
mvl - master volume
tre - treble
mid - middle
bas - bass
prs - presence
dep - depth
bis - bias
number - don't know what this is; different for each emulated amp
ng - noise gate; value 0x00 - 0x05
ths - threshold; value 0x00 - 0x09
cab - cabinet; value 0x00 - 0x0c
sag - sag; value 0x00 - 0x02
bri - brightness; value 0x00-0x01
?? - values which I haven't decoded yet; different for each emulated amp
```

After packet with data described above you have to send a packet which will tell the amp to actually apply the settings. This packet have zeroth byte set to "0x1c", first to "0x03" and all other to "0x00".


## 4. Setting and clearing effects

Data format for setting an effect is:

```
     0                                       1
     0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
00 | 1c| 03|DSP|     00    | 01| 01|               00              |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
16 |fxm| 00|slt| ??| ??| ??|                   00                  |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
32 |kb1|kb2|kb3|kb4|kb5|kb6|                   00                  |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
48 |                               00                              |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

DSP - can be either 6, 7, 8 or 9; depends on effect familly
fxm - effect model; independent for each effect
slt - slot; before amp have numbers from 0 to 3, after from 4 to 7
kb1, kb2, kb3, kb4, kb5, kb6 - knobs values; not every time all are used; maximum
  value of the knob depends on the effect
?? - some strange numbers specific for each effect
```

If you want to remove the effect send normal effect-setting packet but set "effect model" and knobs fields to zeros.
I haven't tried what happens if you send such packet to DSP 0x05.
"Execute" command for both setting and clearing the effect is the same as for the amp setting.


## 5. Saving settings on amplifier

Saving settings is very easy since you don't have to transmit all the settings which you want to store. You only send a command containing slot number and name for a preset. Data are taken directly from DSPs.
Packet format is:

```
     0                                       1
     0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
00 | 1c| 01| 03| 00|SLT| 00| 01| 01|               00              |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
16 |                              name                             |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
32 |                              name                             |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
48 |                               00                              |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

SLT - memory bank to which to save settings; value 0x00 - 0x17
name - name of the preset ended with "\0"; if not all fields are used used the rest
  is set to 0x00
```

Fender FUSE after saving settings chooses memory bank it just saved. PLUG also does this.


## 6. Choosing memory bank

"Choose memory bank" command looks like this:

```
     0                                       1
     0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
00 | 1c| 01| 01| 00|SLT| 00| 01|                 00                |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
16 |                               00                              |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
32 |                               00                              |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
48 |                               00                              |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

SLT - memory bank to choose
```

After choosing memory bank amplifier send you settings of that bank.


## 7. Captured data

Wireshark file with whole initialization communication can be found here: http://piorekf.org/plug/files/initialization.log
And here are data from captured packets for setting all the amps and effects (only hex part, the rest are my scribbles):

AMPLIFIERS:

```
fender 57 deluxe:
1c:03:05:00:00:00:01:01 00:00:00:00:00:00:00:00
67:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
aa:99:80:80:be:80:80:80 80:80:80:80:01:01:01:00
00:01:01:01:00:01:53:00 00:00:00:00:00:00:00:00
0-10: gain[6] volume[7] treble[7.5] middle[5] bass[5]
NG: off -> threshold: 0/10 depth: mid sag: 2/3 bias: mid cabinet: 57dlx

fender 59 bassman:
1c:03:05:00:00:00:01:01 00:00:00:00:00:00:00:00
64:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
aa:a2:80:80:80:7a:a2:91 80:80:80:80:02:02:02:00
00:02:02:01:00:01:67:00 00:00:00:00:00:00:00:00
0-10: gain[6.5] volume[7] treble[5] middle[~5] bass[6.5] presence[~6]
blend: mid NG: off -> threshold: 0/10 depth: mid sag: 2/3 bias: mid cabinet: bssmn

fender 57 champ:
1c:03:05:00:00:00:01:01 00:00:00:00:00:00:00:00
7c:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
aa:b3:00:ff:80:80:80:80 80:80:80:80:0c:0c:0c:00
00:05:0c:01:00:01:00:00 00:00:00:00:00:00:00:00
0-10: gain[7] volume[~7] treble[5] middle[5] bass[5]
NG: off -> threshold: 0/10 depth: mid sag: 2/3 bias: mid cabinet: champ

fender 65 deluxe reverb:
1c:03:05:00:00:00:01:01 00:00:00:00:00:00:00:00
53:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
aa:71:00:ff:91:cf:38:00 00:00:80:00:03:03:03:00
00:03:03:01:00:01:6a:00 00:00:00:00:00:00:00:00
1-10: gain[5] volume[7] treble[6.25] middle[8.75] bass[3] -reverb[~1] -speed[7] -intensivity[9]
NG: off -> threshold: 0/10 depth: 0 sag: 2/3 bias: mid cabinet: 65dlx

fender 65 princeton:
1c:03:05:00:00:00:01:01 00:00:00:00:00:00:00:00
6a:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
aa:55:00:ff:99:cc:4c:80 80:80:80:80:04:04:04:00
00:04:04:01:00:01:61:00 00:00:00:00:00:00:00:00
1-10: gain[4] volume[7] treble[6.5] middle[8.5] bass[4] -reverb[5.5] -speed[7] -intensivity[9]
NG: off -> threshold: 0/10 depth: mid sag: 2/3 bias: mid cabinet: 65prn

fender 65 twin reverb:
1c:03:05:00:00:00:01:01 00:00:00:00:00:00:00:00
75:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
aa:55:80:63:b3:bb:aa:80 80:80:80:80:05:05:05:00
00:09:05:01:00:01:72:00 00:00:00:00:00:00:00:00
1-10: +bright gain[4] volume[7] treble[7.5] middle[8] bass[7] -reverb[5.5] -speed[7] -intensivity[9]
NG: off -> threshold: 0/10 depth: mid sag: 2/3 bias: mid cabinet: 65twn

fender super sonic:
1c:03:05:00:00:00:01:01 00:00:00:00:00:00:00:00
72:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
aa:bb:82:55:99:a2:99:80 80:80:80:80:06:06:06:02
00:0c:06:01:00:01:79:00 00:00:00:00:00:00:00:00
1-10: gain[7.5] gain2[5.5] volume[7] treble[6.5] middle[7] bass[6.5] -reverb[5.5]
master vol: 33% NG: mid -> threshold: 0/10 depth: mid sag: 2/3 bias: mid cabinet: ss112

british 60s:
1c:03:05:00:00:00:01:01 00:00:00:00:00:00:00:00
61:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
aa:a2:80:63:99:80:b0:00 80:80:80:80:07:07:07:00
00:07:07:01:00:01:5e:00 00:00:00:00:00:00:00:00
0-10: +bright gain[6.5] volume[7] treble[6] middle[5] bass[7] -speed[7] -depth[8.75] cut[0]
NG: off -> threshold: 0/10 depth: mid sag: 2/3 bias: mid cabinet: 2x12c

british 70s:
1c:03:05:00:00:00:01:01 00:00:00:00:00:00:00:00
79:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
aa:ff:80:7d:aa:5b:c4:80 80:80:80:80:0b:0b:0b:01
00:08:0b:01:00:01:7c:00 00:00:00:00:00:00:00:00
0-10: gain[10] volume[7] treble[7] middle[3] bass[8] presence[5]
blend: mid NG: low -> threshold: 0/10 depth: mid sag: 2/3 bias: mid cabinet: 4x12g

british 80s:
1c:03:05:00:00:00:01:01 00:00:00:00:00:00:00:00
5e:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
aa:ff:80:7d:aa:5b:c4:80 80:80:80:80:09:09:09:01
00:06:09:01:00:01:5d:00 00:00:00:00:00:00:00:00
0-10: gain[10] volume[7] treble[7] middle[3] bass[8] presence[5]
master vol: 50% NG: low -> threshold: 0/10 depth: mid sag: 2/3 bias: mid cabinet: 4x12m

american 90s:
1c:03:05:00:00:00:01:01 00:00:00:00:00:00:00:00
5d:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
aa:8e:80:66:a4:19:c7:71 80:80:80:80:0a:0a:0a:03
00:0a:0a:01:00:01:6d:00 00:00:00:00:00:00:00:00
?1-10?: gain[~1/2] volume[2/3] treble[2/3] middle[~1/10] bass[3/4] presence[~1/2]
master vol: 33% NG: high -> threshold: 0/10 depth: mid sag: 2/3 bias: mid cabinet: 4x12v

metal 2000:
1c:03:05:00:00:00:01:01 00:00:00:00:00:00:00:00
6d:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
aa:a4:80:55:99:4c:91:8e 80:80:80:80:08:08:08:02
00:08:08:01:00:01:75:00 00:00:00:00:00:00:00:00
0-10: gain[6.5] volume[7] treble[6] middle[3] bass[6] presence[5.5]
master vol: 33% NG: mid -> threshold: 0/10 depth: mid sag: 2/3 bias: mid cabinet: 4x12g



EFFECTS:

overdrive:
1c:03:06:00:00:00:01:01 00:00:00:00:00:00:00:00
3c:00:03:00:08:01:00:00 00:00:00:00:00:00:00:00
80:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level gain low mid high

fixed wah:
1c:03:06:00:00:00:01:01 00:00:00:00:00:00:00:00
49:00:03:01:08:01:00:00 00:00:00:00:00:00:00:00
80:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level frequency min frequency max frequency q

touch wah:
1c:03:06:00:00:00:01:01 00:00:00:00:00:00:00:00
4a:00:03:01:08:01:00:00 00:00:00:00:00:00:00:00
80:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level sensivity min frequency max frequency q

fuzz:
1c:03:06:00:00:00:01:01 00:00:00:00:00:00:00:00
1a:00:03:00:08:01:00:00 00:00:00:00:00:00:00:00
80:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level gain octave low high

fuzz touch wah:
1c:03:06:00:00:00:01:01 00:00:00:00:00:00:00:00
1c:00:03:00:08:01:00:00 00:00:00:00:00:00:00:00
80:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level gain sensivity octave peak

simple comp (1 knob):
1c:03:06:00:00:00:01:01 00:00:00:00:00:00:00:00
88:00:03:08:08:01:00:00 00:00:00:00:00:00:00:00
01:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
type[0-3]

compressor:
1c:03:06:00:00:00:01:01 00:00:00:00:00:00:00:00
07:00:03:00:08:01:00:00 00:00:00:00:00:00:00:00
8d:0f:4f:7f:7f:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level threshhold ratio attack release



sine chorus:
1c:03:07:00:00:00:01:01 00:00:00:00:00:00:00:00
12:00:02:01:01:01:00:00 00:00:00:00:00:00:00:00
ff:0e:19:19:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level rate depth average delay lr phase

triangle chorus:
1c:03:07:00:00:00:01:01 00:00:00:00:00:00:00:00
13:00:02:01:01:01:00:00 00:00:00:00:00:00:00:00
5d:0e:19:19:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level rate depth average delay lr phase

sine flanger:
1c:03:07:00:00:00:01:01 00:00:00:00:00:00:00:00
18:00:02:01:01:01:00:00 00:00:00:00:00:00:00:00
ff:0e:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level rate depth feedback lr phase

triangle flanger:
1c:03:07:00:00:00:01:01 00:00:00:00:00:00:00:00
19:00:02:01:01:01:00:00 00:00:00:00:00:00:00:00
ff:00:ff:33:41:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level rate depth feedback lr phase

vibratone:
1c:03:07:00:00:00:01:01 00:00:00:00:00:00:00:00
2d:00:02:01:01:01:00:00 00:00:00:00:00:00:00:00
f4:ff:27:ad:82:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level rotor depth feedback lr phase

vintage tremolo:
1c:03:07:00:00:00:01:01 00:00:00:00:00:00:00:00
40:00:02:01:01:01:00:00 00:00:00:00:00:00:00:00
db:ad:63:f4:f1:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level rate duty cycle attack release

sine tremolo:
1c:03:07:00:00:00:01:01 00:00:00:00:00:00:00:00
41:00:02:01:01:01:00:00 00:00:00:00:00:00:00:00
db:99:7d:00:00:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level rate duty cycle lfo clipping tri shaping

ring modulator:
1c:03:07:00:00:00:01:01 00:00:00:00:00:00:00:00
22:00:02:01:08:01:00:00 00:00:00:00:00:00:00:00
ff:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level frequency depth lfo shape[0-1] lfo phase

step filter:
1c:03:07:00:00:00:01:01 00:00:00:00:00:00:00:00
29:00:02:01:01:01:00:00 00:00:00:00:00:00:00:00
ff:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level rate resonance min frequency max frequency

phaser:
1c:03:07:00:00:00:01:01 00:00:00:00:00:00:00:00
4f:00:02:01:01:01:00:00 00:00:00:00:00:00:00:00
fd:00:fd:b8:00:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level rate depth feedback lfo shape[0-1]

pitch shifter:
1c:03:07:00:00:00:01:01 00:00:00:00:00:00:00:00
1f:00:02:01:08:01:00:00 00:00:00:00:00:00:00:00
c7:3e:80:00:00:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level pitch detune feedback predelay



mono delay:
1c:03:08:00:00:00:01:01 00:00:00:00:00:00:00:00
16:00:02:02:01:01:00:00 00:00:00:00:00:00:00:00
ff:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level delay time feedback brightness attenuation

mono echo filter (6 knobs):
1c:03:08:00:00:00:01:01 00:00:00:00:00:00:00:00
43:00:02:02:01:01:00:00 00:00:00:00:00:00:00:00
ff:80:80:80:80:80:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level delay time feedback frequency ressonance input level

stereo echo filter (6 knobs):
1c:03:08:00:00:00:01:01 00:00:00:00:00:00:00:00
48:00:02:02:01:01:00:00 00:00:00:00:00:00:00:00
80:b3:80:80:80:80:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level delay time feedback frequency ressonance input level

multitap delay:
1c:03:08:00:00:00:01:01 00:00:00:00:00:00:00:00
44:00:02:02:01:01:00:00 00:00:00:00:00:00:00:00
ff:80:66:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level delay time feedback brightness attenuation

ping pong delay:
1c:03:08:00:00:00:01:01 00:00:00:00:00:00:00:00
45:00:02:02:01:01:00:00 00:00:00:00:00:00:00:00
ff:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level delay time feedback brightness attenuation

ducking delay:
1c:03:08:00:00:00:01:01 00:00:00:00:00:00:00:00
15:00:02:02:01:01:00:00 00:00:00:00:00:00:00:00
ff:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level delay time feedback release threshold

reverse delay:
1c:03:08:00:00:00:01:01 00:00:00:00:00:00:00:00
46:00:02:02:01:01:00:00 00:00:00:00:00:00:00:00
ff:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level delay time feedback brightness attenuation

tape delay (6 knobs):
1c:03:08:00:00:00:01:01 00:00:00:00:00:00:00:00
2b:00:02:02:01:01:00:00 00:00:00:00:00:00:00:00
7d:1c:00:63:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level delay time feedback flutter brightness stereo

stereo tape delay (6 knobs):
1c:03:08:00:00:00:01:01 00:00:00:00:00:00:00:00
2a:00:02:02:01:01:00:00 00:00:00:00:00:00:00:00
7d:88:1c:63:ff:80:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level delay time feedback flutter separation brightness



small hall reverb:
1c:03:09:00:00:00:01:01 00:00:00:00:00:00:00:00
24:00:02:00:08:01:00:00 00:00:00:00:00:00:00:00
6e:5d:6e:80:91:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level decay dwell diffusion tone

large hall reverb:
1c:03:09:00:00:00:01:01 00:00:00:00:00:00:00:00
3a:00:02:00:08:01:00:00 00:00:00:00:00:00:00:00
4f:3e:80:05:b0:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level decay dwell diffusion tone

small room reverb:
1c:03:09:00:00:00:01:01 00:00:00:00:00:00:00:00
26:00:02:00:08:01:00:00 00:00:00:00:00:00:00:00
80:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level decay dwell diffusion tone

large room reverb:
1c:03:09:00:00:00:01:01 00:00:00:00:00:00:00:00
3b:00:02:00:08:01:00:00 00:00:00:00:00:00:00:00
80:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level decay dwell diffusion tone

small plate reverb:
1c:03:09:00:00:00:01:01 00:00:00:00:00:00:00:00
4e:00:02:00:08:01:00:00 00:00:00:00:00:00:00:00
80:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level decay dwell diffusion tone

large plate reverb:
1c:03:09:00:00:00:01:01 00:00:00:00:00:00:00:00
4b:00:02:00:08:01:00:00 00:00:00:00:00:00:00:00
38:80:91:80:b6:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level decay dwell diffusion tone

ambient reverb:
1c:03:09:00:00:00:01:01 00:00:00:00:00:00:00:00
4c:00:02:00:08:01:00:00 00:00:00:00:00:00:00:00
ff:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level decay dwell diffusion tone

arena reverb:
1c:03:09:00:00:00:01:01 00:00:00:00:00:00:00:00
4d:00:02:00:08:01:00:00 00:00:00:00:00:00:00:00
ff:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level decay dwell diffusion tone

'63 fender spring reverb:
1c:03:09:00:00:00:01:01 00:00:00:00:00:00:00:00
21:00:02:00:08:01:00:00 00:00:00:00:00:00:00:00
80:80:80:80:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level decay dwell diffusion tone

'65 fender spring reverb:
1c:03:09:00:00:00:01:01 00:00:00:00:00:00:00:00
0b:00:02:00:08:01:00:00 00:00:00:00:00:00:00:00
80:8b:49:ff:80:00:00:00 00:00:00:00:00:00:00:00
00:00:00:00:00:00:00:00 00:00:00:00:00:00:00:00
level decay dwell diffusion tone
```

----------------

*This page and it's content is licensed under CC BY-SA 3.0 Unported license.*
