#include<iostream>
#include<vector>
#include<cctype>
#include<cstring>

using namespace std;

#include "enigma.h"
#include "errors.h"


//./enigma plugboards/m1.pb reflectors/m1.rf rotors/shift_up.rot rotors/II.rot rotors/m1.pos
int main(int argc, char** argv)
{
  char c;
  int position;
  int num_rotors = 0;

  PlugBoard p;
  Reflector r;
  vector<Rotor> rotors;

  // Check command line arguments
  if ((argc < 3) || (argc == 4)) {
    cerr <<  "usage: enigma plugboard-file reflector-file (<rotor-file>* rotor-positions)?" << endl;
    return INSUFFICIENT_NUMBER_OF_PARAMETERS;
  }
  else if (argc >= 5) {
    num_rotors = argc - 4;
  }

  int positions[num_rotors];

  // Build Plugboard and Reflector
  try {
    p.build_map(argv[1]);
    r.build_map(argv[2]);
  } catch (int error) {
    return error;
  }

  // Build rotors and set starting positions
  if (num_rotors > 0) {
    for (int i = 3; i < argc - 1; i++) {
      try {
        rotors.push_back(Rotor(argv[i]));
      } catch (int error) {
        return error;
      }
    }

    try {
      read_rotor_positions(argv[argc-1], positions, num_rotors);
    } catch (int error) {
      return error;
    }

    for (int i = 0; i < num_rotors; i++) {
      rotors[i].set_position(positions[i]);
    }
  }

  /* Use the Enigma machine */
  cin >> ws;
  cin >> c;
  while (true) {
    // Validate character input
    if (cin.eof())
      return NO_ERROR;
    else if (!isupper(c)) {
      cerr << c << " is not a valid input character (input characters must be upper case letters A-Z)!" << endl;
      return INVALID_INPUT_CHARACTER;
    }

    // Encode character
    position = p.encode(c);                               // Plugboard
    position = rotors_encode_backward(position, rotors);  // Rotors right-left
    position = r.encode(position);                        // Reflector
    position = rotors_encode_forward(position, rotors);   // Rotors left-right
    c = p.decode(position);                               // Plugboard

    cout << c;
    cin >> ws;
    cin >> c;
  }

  return 0;
}
