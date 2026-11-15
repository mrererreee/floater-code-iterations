11/14/26

right now, this program:

1. in state 1 (connected), sends sensor info correctly over every 5s via bluetooth
2. in state 0, uploads samples to file samplelog.txt (in littleFS)
    - sends garbage values, appearing to be esp32 debug statements and random characters

to-do:
1. identify the issue so that clean data is saved to a file
2. build a task that sends the txt file over bluetooth and dumps it (cannot run on same program)
3. make sure that the time count represents a sample every 5s