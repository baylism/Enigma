# Enigma
A customisable Enigma cipher machine. 

Usage:

```bash
$ make
$ ./enigma PLUGBOARD_CONFIGURATION REFLECTOR_CONFIGURATION ROTOR1_POSITIONS ROTOR2_POSITIONS ...
```

For example, a possible configuration using the plugboards, reflectors and rotors provided in this repository is:

```bash
$ ./enigma plugboards/IV.pb reflectors/I.rf rotors/II.rot rotors/I.pos < input.txt > output.txt
```
