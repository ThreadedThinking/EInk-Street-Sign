# E-Ink-Street-Sign - Econotag
SEPTA Smart City Street Sign
By: Alexander Roscoe, Drew Taylor and Colin Weir

This github repository includes eink drivers for a Redwire Econotag.  This project was for a demo during the Bits-N-Bytes at the IETF conference in Toronto, Canada.  It demonstrates the use of low-power wireless communication in an urban environment.  It uses 6lowPAN and CoAP to communicate transit information from a municipal transit location API.

#Hardware
An electrophoretic display was chosen as it can hold an image without a power and it requires very little energy to update. The screen is based on an LB060S01-RD02.  It is powered right from the econotag.

The radio uses the erbium client firmware from the Contiki OS.  This reaches out to a CoAP server to retrieve transit information. We used the econotag from Redwire as it had the most pins available to drive the screen.  The econotag has the ability to fall asleep in a lower power mode and wake up to update the display.  Because the screen and radio require very little energy, energy harvesting would be an ideal power source for perpetual operation.  The one limitition with the econotag was the flash size.  High quality fonts take up a lot of space.

The sign connects through an IPv6 internet connection based around a Raspberry Pi running 6lbr.  This border gateway passes CoAP requests from the sign to a CoAP server that proxied the requests to SEPTA's transit API.


![E-Ink Display](https://raw.githubusercontent.com/ThreadedThinking/EInk-Street-Sign/master/SEPTA_E-Ink.jpg)
![Adaptor Board Back](https://raw.githubusercontent.com/ThreadedThinking/EInk-Street-Sign/master/adaptor_board_back.jpg)

# Credits and Links
Petteri Aimonen wrote a very good writeup on how to drive the screen.
http://essentialscrap.com/eink/software.html

All based on the Contiki OS
https://github.com/contiki-os/contiki/tree/master/examples/er-rest-example  

Border Gateway http://cetic.github.io/6lbr/
