11/14/26

right now, this program:

1. in state 1 (connected), sends sensor info correctly over every 5s via bluetooth
2. in state 0, uploads samples to file samplelog.txt (in littleFS)
    - sends garbage values, appearing to be esp32 debug statements and random characters

to-do:
~~1. identify the issue so that clean data is saved to a file~~
2. build a task that sends the txt file over bluetooth and dumps it (cannot run on same program)
~~3. make sure that the time count represents a sample every 5s~~
4. for testing purposes, wait for a char from serial input before running the program so samplelog isnt auto-updating lol
5. possible to upload to csv file?

errors debugged:
1. this line: "String line = String(millis() + " " + String(voltageMeasure) +"\t" + String(pressure) + "\t" + String(temperature) + "\t" + String(depthMBAR));"
   - the space is a pointer to a character that made some of the values appear wonky. when this was patched to match the tabulation
     following the other measurements, it came out properly
2. new sketch named: **nov14_success**

5:01pm updates:
1. state 1 successfully sends sensor info every 5s over bluetooth
2. state 0 uploads clean samples to samplelog.txt
