#ifndef ENIGMA_H
#define ENIGMA_H

#include<vector>

using namespace std;


/* Forward declarations */

class Rotor;

/* General helper functions */
void read_rotor_positions(const char input_file[], int positions[], int num_rotors);
int rotors_encode_backward(int position, vector<Rotor> &rotors);
int rotors_encode_forward(int position, vector<Rotor> rotors);

/* Error checking helper functions */
bool index_error(int index);
void print_error_message(int error_code);


/* Component classes */
class PlugBoard {
private:
  int char_map[26];

public:
  /* Constructors*/
  PlugBoard();
  PlugBoard(const char input_file[]);


  /* Production functions */
  void build_map(const char input_file[]);
  void print_map();
  int encode(const char letter);
  char decode(const int position);
};

class Reflector {
private:
  int char_map[26];

public:
  /* Constructors*/
  Reflector();
  Reflector(const char input_file[]);


  /* Production functions */
  void build_map(const char input_file[]);
  void print_map();
  int encode(int position);

};

class Rotor {
private:
  int char_map[26];
  vector<int> notches;
  int abs_rotation;
  void normalise(int& position);

public:
  /* Constructors*/
  Rotor(const char input_file[]);

  /* Production functions */
  void build_map(const char input_file[]);
  void print_map();
  int encode_forward(const int position);
  int encode_backward(const int position, Rotor &next_rotor);
  int encode_backward(const int position);
  void rotate();
  void set_position(int target_position);
  bool check_notches();
};

#endif
