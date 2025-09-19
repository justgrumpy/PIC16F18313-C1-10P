# DFPlayer Pro AT Commands Reference

| Command | Function | Description |
|---------|----------|-------------|
| `AT\r\n` | Test Connection | No command and parameter required |
| `AT+VOL=5\r\n` | Set/Query Volume (Volume: 0-30) | param<br/>-n: Volume-n<br/>+n: Volume+n<br/>n: Designate volume to n<br/>?: Query volume |
| `AT+PLAYMODE=1\r\n` | Control playback mode | param<br/>1: repeat one song<br/>2: repeat all<br/>3: play one song and pause<br/>4: Play randomly<br/>5: Repeat all in the folder<br/>?: query the current playback mode |
| `AT+PLAY=NEXT\r\n` | Control playing | param<br/>PP: Play & Pause<br/>NEXT: next<br/>LAST: last |
| `AT+TIME=-5\r\n` | Fast Rewind 5S | param<br/>-n: Fast Rewind n S<br/>+n: Fast Forward n S<br/>n: Start playing from the Nth second |
| `AT+QUERY=1\r\n` | Query the file number of the currently-playing file | param<br/>1: Query the file number of the currently-playing file<br/>2: Query the total number of the files<br/>3: Query the time length the song has played.<br/>4: Query the total time of the currently-playing file.<br/>5: Query the file name of the currently-playing file. |
| `AT+PLAYNUM=5\r\n` | Play the file No.5 | param<br>File number (Play the first file if there is no such file)|
| `AT+PLAYFILE=/test/test.mp3\r\n` | Play the specific file | File path |
| `AT+DEL\r\n` | Delete currently-playing file | No parameter required |
| `AT+AMP=ON\r\n` | Amplifier On/OFF command | param: ON,OFF |
| `AT+REC=SAVE\r\n` | Recording control | param<br/>RP: Record & Pause<br/>SAVE: Save the recorded voice |
| `AT+BAUDRATE=115200\r\n` | Set baud rate (power-down save, valid after re-powering on) | Param<br/>9600,19200,38400,57600,115200 |
| `AT+PROMPT=ON\r\n` | Prompt tone ON/OFF command (Power-down save) | param: ON,OFF |
| `AT+LED=ON\r\n` | LED Prompt ON/OFF command (Power-down save) | param: ON,OFF |

## Notes:
- All commands end with `\r\n` (carriage return + line feed)
- Volume range is 0-30
- Use `?` parameter to query current settings
- Commands are case-sensitive
