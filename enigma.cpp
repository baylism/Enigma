#include<iostream>
#include<fstream>
#include<cctype>
#include<vector>
#include<cstring>

using namespace std;

#include "enigma.h"
#include "errors.h"

/* Forward declarations */

class Rotor;

/* ------------------- Error checking helper functions --------------------- */

/* Return true if number is outside of range 0-15 */
bool index_error(int index) {
  return ((index < 0) || (index > 25));
}

/* Return true if number in already provided list */
bool already_provided(int index, vector<int>& provided) {
  vector<int>::iterator i;
  for (i = provided.begin(); i !=provided.end(); ++i) {
    if (index == *i)
      return true;
  }
  return false;
}

/* Get index for already mapped character */
int get_index(int output, vector<int>& provided) {
  vector<int>::iterator i;
  for (i = provided.begin(); i !=provided.end(); ++i) {
    if (output == *i)
      return i - provided.begin();
  }
  return -1;
}

/* Throw error if character <0 or >26, or has already been provided. */
void test_valid_input(int index, vector<int>& provided, const char input_file[], char type) {
  int error_code;
  string name;

  if (type == 'P') {
    name = "plugboard";
    error_code = IMPOSSIBLE_PLUGBOARD_CONFIGURATION;
  }
  else if (type == 'R') {
    name = "reflector";
    error_code = INVALID_REFLECTOR_MAPPING;
  }
  else if (type == 'T') {
    name = "rotor";
    error_code = INVALID_ROTOR_MAPPING;
  }

  if (index_error(index)) {
    cerr << "Index error in " << name << " file " << input_file << endl;
    throw INVALID_INDEX;
  }
  else if (already_provided(index, provided)) {
    cerr << "Invalid mapping of input " << provided.size() << " to output "
         << index << " (output " << index << " is already mapped to from input "
         << get_index(index, provided) << ") " << endl;
    throw error_code;
  }
}

/* Throw error if unepcted eof encountered in stream */
void test_params(ifstream &stream, const char input_file[], char type) {
  string message;
  int error_code;

  if (type == 'P') {
    message = "number of parameters in plugboard file ";
    error_code = INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS;
  }
  else if (type == 'R') {
    message = "(odd) number of parameters in reflector file ";
    error_code = INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS;
  }

  if (stream.eof()) {
    cerr << "Incorrect " << message << input_file << endl;
    throw error_code;
  }
}

/* Throw error if character of incorrect type encountered in stream */
void test_numerics(ifstream &stream, const char input_file[], char type) {
  string name;

  if (type == 'P')
    name = "plugboard";
  else if (type == 'R')
    name = "reflector";
  else if (type == 'T')
    name = "rotor";
  else if (type == 'S')
    name = "rotor positions";

  if (type == 'T') {
    if ((!stream.eof()) && (stream.fail())) {
      cerr << "Non-numeric character for mapping in " << name << " file " << input_file << endl;
      throw NON_NUMERIC_CHARACTER;
    }
  }

  else if ((!stream.eof()) && (stream.fail())) {
    cerr << "Non-numeric character in " << name << " file " << input_file << endl;
    throw NON_NUMERIC_CHARACTER;
  }
}


/* ----------------------- General helper functions ------------------------ */

/* Modify positions of array to contain provided rotor positions */

void read_rotor_positions(const char input_file[], int positions[], int num_rotors) {
  ifstream stream;
  stream.open(input_file);

  int i;
  int index = 0;
  stream >> i;
  test_numerics(stream, input_file, 'S');

  while ((!stream.eof())) {
    test_numerics(stream, input_file, 'S');
    positions[index] = i;
    index ++;
    stream >> i;
  }

  if (index < num_rotors) {
    cerr << "No starting position for rotor " << num_rotors - 1 << " in rotor position file: " << input_file << endl;
    throw NO_ROTOR_STARTING_POSITION;
  }
}

/* Return final output position after first pass (right-left) through rotors */
int rotors_encode_backward(int position, vector<Rotor> &rotors) {
  vector<Rotor>::reverse_iterator i;
  int position_temp = position;

  i = rotors.rbegin();

  // break early if we have no rotors
  if (i == rotors.rend())
    return position;

  // rotate the first rotor
  (*i).rotate();

  for (; i != rotors.rend(); ++i) {
    // Encoding with last rotor
    if (next(i) == rotors.rend())
      return (*i).encode_backward(position_temp);

    // Encoding if there is another rotor
    else
      position_temp = (*i).encode_backward(position_temp, *next(i));
  }
  return position_temp;
}

/* Return final output position after second pass (left-right) through rotors */
int rotors_encode_forward(int position, vector<Rotor> rotors) {
  vector<Rotor>::iterator i;
  for (i = rotors.begin(); i != rotors.end(); ++i) {
    position = (*i).encode_forward(position);
  }
  return position;
}

/* --------------------------- Component classes --------------------------- */

/* ------------------------------ Plugboard -------------------------------- */

/* Initialise plugboard without building map */
PlugBoard::PlugBoard() {
  for (int i = 0; i < 26; i++)
    char_map[i] = i;
}

/* Initialise plugboard and build map */
PlugBoard::PlugBoard(const char input_file[]) {
  for (int i = 0; i < 26; i++)
    char_map[i] = i;
  build_map(input_file);
}

/* Read plugboard configuration file and modify char_map accordingly */
void PlugBoard::build_map(const char input_file[]) {
  int c1;
  int c2;
  vector<int> provided;
  ifstream stream;

  stream.open(input_file);

  stream >> c1;
  if (stream.eof()) return;
  test_numerics(stream, input_file, 'P');

  while (stream.good()) {
    test_valid_input(c1, provided, input_file, 'P');
    provided.push_back(c1);                           // add c1 to history

    stream >> c2;
    test_params(stream, input_file, 'P');
    test_numerics(stream, input_file, 'P');
    test_valid_input(c2, provided, input_file, 'P');
    provided.push_back(c2);                           // add c2 to history

    char_map[c1] = c2;
    char_map[c2] = c1;

    stream >> c1;
    if (stream.eof()) return;

    else if (provided.size() == 26) {
      cerr << "Incorrect number of parameters in plugboard file " << input_file << endl;
      throw INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS;
    }

    test_numerics(stream, input_file, 'P');
  }
  stream.close();
}

/* Return new character after applying switches */
int PlugBoard::encode(const char letter) {
  return char_map[letter - 65];
}

/* Return final character after second pass through plugboard */
char PlugBoard::decode(const int position) {
  return static_cast<char>(char_map[position] + 65);
}

/* ------------------------------ Reflector -------------------------------- */

/* Initialise reflector without building map */
Reflector::Reflector() {
  for (int i = 0; i < 26; i++)
    char_map[i] = i;
}

/* Initialise reflector and build map */
Reflector::Reflector(const char input_file[]) {
  for (int i = 0; i < 26; i++)
    char_map[i] = i;
  build_map(input_file);
}

/* Read reflector configuration file and modify char_map accordingly */
void Reflector::build_map(const char input_file[]) {
  int c1;
  int c2;
  vector<int> provided;
  ifstream stream;

  stream.open(input_file);

  // Check first character
  stream >> c1;
  test_params(stream, input_file, 'R');
  test_numerics(stream, input_file, 'R');

  // Check ramaining characters
  while (stream.good()) {
    test_valid_input(c1, provided, input_file, 'R');
    provided.push_back(c1);                      // add c1 to history

    stream >> c2;
    test_params(stream, input_file, 'R');
    test_numerics(stream, input_file, 'R');
    test_valid_input(c2, provided, input_file, 'R');
    provided.push_back(c2);                      // add c2 to history

    char_map[c1] = c2;
    char_map[c2] = c1;

    stream >> c1;
    if ((!stream.eof()) && (provided.size() == 26)) {
      cerr << "Incorrect (odd) number of parameters in reflector file " << input_file << endl;
      throw INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS;
    }
    test_numerics(stream, input_file, 'R');
  }
  stream.close();

  if (provided.size() < 26) {
    cerr << "Insufficient number of mappings in reflector file: " << input_file << endl;
    throw INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS;
  }
}

/* Return output position when leaving reflector */
int Reflector::encode(const int position) {
  return char_map[position];
}

/* ------------------------------ Rotor -------------------------------- */

Rotor::Rotor(const char input_file[]) {
  build_map(input_file);
  abs_rotation = 0;
}

/* Output rotor configuration stdout */
void Rotor::print_map() {
  for (int i = 0; i < 26; i++)
    cout << char_map[i] << ' ';
    cout << endl;
}

void Rotor::build_map(const char input_file[]) {
  int i, offset_temp;
  ifstream stream;
  stream.open(input_file);
  vector<int> provided;

  int index = 0;
  stream >> i;

  if (stream.eof()) {
    cerr << "Not all inputs mapped in rotor file: " << input_file << endl;
    throw INVALID_ROTOR_MAPPING;
  }

  while ((!stream.eof()) && (index < 26)) {
    test_numerics(stream, input_file, 'T');
    test_valid_input(i, provided, input_file, 'T');
    provided.push_back(i);
    offset_temp = i - index;

    char_map[index] = offset_temp;
    index ++;
    stream >> i;
  }

  if (index != 26) {
    cerr << "Not all inputs mapped in rotor file: " << input_file << endl;
    throw INVALID_ROTOR_MAPPING;
  }

  while (stream.good()) {
    notches.push_back(i);
    stream >> i;
  }

  stream.close();
}

/* Return true if first element of rotor char_map is a designated a notch */
bool Rotor::check_notches() {
  vector<int>::iterator i;

  for (i = notches.begin(); i != notches.end(); ++i) {
    if (abs_rotation == *i)
      return true;
  }
  return false;
}

/* Bring position into proper range */
void Rotor::normalise(int& position) {
  if (position < 0)
    position += 26;
  else if (position > 25)
    position -= 26;
}

/* Return character (int) in char_map at position and rotate next rotor */
int Rotor::encode_backward(int position, Rotor &next_rotor) {

  if (check_notches())
    next_rotor.rotate();

  position = position + char_map[position];
  normalise(position);
  return position;
}

/* Return character (int) in char_map at position */
int Rotor::encode_backward(int position) {
  position = position + char_map[position];
  normalise(position);
  return position;
 }

/* Return absolute position on character in char map */
int Rotor::encode_forward(const int position) {
  for (int i = 0; i < 26; i++) {
    if (encode_backward(i) == position)
      return i;
    }
  return position;
}

/* Rotate rotor one position to the left */
void Rotor::rotate() {
  int position_0 = char_map[0];

  for (int i = 0; i < 25; i++)
    char_map[i] = char_map[i + 1];
  char_map[25] = position_0;

  abs_rotation ++;
}

/* Rotate rotor until configuration position in at top (absolute position 0) */
void Rotor::set_position(int target_position) {
  for (int i = 0; i < target_position; i++)
    rotate();
}
