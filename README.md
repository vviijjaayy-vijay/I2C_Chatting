# A chatting program made for Arduino

  This is an Arduino program that allows chatting between multiple Arduino devices. This program communicates through the I2C protocol, using pins `SCL` and `SDA`, the pins you find on the corner of your board right next to an usb port.

&nbsp;

## How to run this program?

  It’s simple. Download the files, upload it to your Arduino via Arduino IDE *(or other IDEs you have for Arduino programming)*, and you’re good to go!

&nbsp;

## Features

  When you start the device, you’ll see the following message pop up. It shows which channel(or address) you are on, and the message limit shown as bytes.
```
***************************
** I2C Chatting Platform **
** login on address 64   **
** message limit is 100  **
***************************
```
  After you see this message, you’re now ready to chat! Type whatever into the serial input, and it’ll broadcast through the entire I2C network.

  There will be some tags assigned to each message. Here’s an example of one:
  `[R#64] 3723ms > ...`
  Each tag shows whether the message has been sent or received. If the letter before the hashtag is an `R`, then the message is a message that has been received. If it’s an `S`, that message has been sent. The numbers next to the tag is the timestamp, where it’s measured as the time between the startup of the device and message receive/send as milliseconds.

&nbsp;

### Commands
  Here are some commands you can use over the chat.

  1. `/login <address>`
     * changes the chatting channel to `<address>`
     * after execution, it’ll reshow the initialize message as shown at [Features](#features)
  2. `/resend [number]` or `/r [number]`
	 * sends out previous message again `[number]` times
     * ***Must be used at your own risk***
     * this action will result the network fully occupied
     * while the message is being resent, others on the channel might be unable to chat or use commands
     * yes, it’s spamming