# Spresense Arduino Package with libjpeg for MainCore

How to install Spresense Arduino Package with libjpeg. Please note that this package is baseed on Spresense SDK Ver 2.4.0

## Step 1.
Open "Preferences" on Arduino IDE, and set the below URL on "Additional Boards Manager URLs"
https://raw.githubusercontent.com/YoshinoTaro/spresense-arduino-libjpeg/main/package_spresense_tensorflow_index.json

<img src="https://user-images.githubusercontent.com/18510684/153807548-c0450aba-40bf-47c4-b30e-3c618f4f1f94.png" width="400"/>

## Step 2.
Open "Board Manager" on Arduino IDE, and install Spresense libjpeg Board

<img src="https://user-images.githubusercontent.com/18510684/153807989-2ae09d02-3dd6-4312-9540-52ec183dbfa2.png" width="400" height="255"/>


## Step 3.
Select "Spresense libjpeg" on the board package selection.

<img src="https://user-images.githubusercontent.com/18510684/153808924-89fdb30d-c98c-4c4e-9565-b7be07c8e2d3.png" width="400"/>

## Step 4.
Compile and run the examples on this github repository. 

|example name | application |
| ---- | ---- |
|spresense_libjpeg_example | simple jpeg file decode example|
|spresense_libjpeg_decode_example | decode jpeg data captured by spresense camera to RGB data |
|spresense_libjpeg_decode_clip_example | partly decode jpeg data captured by spresense camera to RGB data |
|spresense_encode_example | encode RGB565 data captured by spresense camera to jpeg data |

