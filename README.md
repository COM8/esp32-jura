# esp32-jura
ESP32 powered Jura coffee maker

## Protocol

### Notation
```
Original Binary -> dec1	hex1	char1	|	dec2	hex2	char2	|	dec3  hex3  char3
```

`dec1, hex1, char1` use the original data.  
`dec2, hex2, char2` use the reverse_1 data (10101010 -> 01010101).
`dec3, hex3, char3` use the reverse_2 data (11110000 -> 00001111).

### General
All messages end with:
```
[... MESSAGE ...]
0 1 1 1 0 0 0 0 -> 112	70	p	|	14	0e	SO	|	07  07  '\a'
1 0 1 0 0 0 0 0 -> 160	a0	�	|	5	05	ENQ	|	10	0a	'\n'
```
Which should correspond to "\r\n" of the original protocol.

### Initial Message
The initial message of the dongle is always the same:
```
0 0 0 1 0 1 0 1 -> 21	15	NAK	|	168	a8	�	|	81	51	Q
0 1 1 0 0 1 0 1 -> 101	65	e	|	166	a6	�	|	86	56	V
1 0 1 0 1 1 0 0 -> 172	ac	�	|	53	35	5	|	202	ca	�
0 1 1 1 0 0 0 0 -> 112	70	p	|	14	0e	SO	|	07  07  '\a'
1 0 1 0 0 0 0 0 -> 160	a0	�	|	5	05	ENQ	|	10	0a	'\n'
```