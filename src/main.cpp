#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <map>
#include "debug/hexdump.hpp"

#define MEMORY_SIZE 160
#define BITS 8
#define MASK(hex) ((opCode & hex))

typedef void(*instructionHandle)();

std::string filename = "resources/test.bin";

uint8_t memory[MEMORY_SIZE];
uint16_t opCode;
int memoryPosition = 0;

std::map<uint8_t, instructionHandle> instructions;

/****** Instruction Handlers ******/

void
push_integer()
{
  std::cout << "Running PUSH_INTEGER" << " :: " << hexdump(opCode) << std::endl;

  int parsedValue = 0;

  //
  // shift 8 bits to the right
  //  0x0104
  //    0x04
  //
  uint8_t intSize = (MASK(0xFF00) >> 8);

  std::cout << "Integer Byte Size: " << (int)(intSize) << std::endl;

  int j = 0;
  while (j < intSize)
  {
    //
    // This preserves the endianness written by C++
    // It essentially does the following (BigEndian) in reverse:
    //  int i = (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | (b[0]);
    //
    parsedValue = parsedValue | memory[memoryPosition] << (j * 8);
    std::cout << "Byte Position: " << (j * 8) << " - " << hexdump( parsedValue ) << " :: " << (int)parsedValue << std::endl;
    memoryPosition++;
    j++;
  }
}

void
load_instruction_handlers()
{
  instructions[0x01] = &push_integer;
}

void
run_instruction(uint8_t instruction)
{
  if ( instructions.count(instruction) )
  {
    (instructions[instruction])();
  }
  else
  {
    std::cout << "Unimplemented Instruction: " << hexdump(instruction) << std::endl;
    exit(1);
  }
}

/****** Memory / Loading ******/

void
increment_position(int amount = 1)
{
  memoryPosition += amount;
}

void reset_memory()
{
  for (int i = 0; i < MEMORY_SIZE; ++i)
  {
    memory[i] = 0;
  }
}

void
write_instructions()
{
  std::ofstream file(filename, std::ios::binary);

  //
  // instruction:
  //    01 - PUSH_INTEGER
  //    04 - Number of Bytes
  //
  uint16_t pushInteger4Bytes = 0x0104;

  //
  // End of Program Instruction
  //
  uint16_t endOfProgram = 0xFFFF;

  file.write( reinterpret_cast<const char *>(&pushInteger4Bytes), sizeof(pushInteger4Bytes) );

  //
  // Value to write into the program
  //  A 4-byte integer
  //  Note that we use BigEndian to represent our integer values
  //
  int value = 47856;
  file.write( reinterpret_cast<const char *>(&value), sizeof(value) );

  // EOP filler
  file.write( reinterpret_cast<const char *>(&endOfProgram), sizeof(endOfProgram) );

  //
  // At this point, memory should look similar to:
  //  04 01 f0 ba 00 00 ff ff
  //
  file.close();
}

void
load_memory()
{
  char byte;
  int i = 0;
  std::ifstream file(filename, std::ios::binary);
  while( file.get(byte) )
  {
    memory[i] = (uint8_t)byte;
    i++;
  }
}

void
run_program()
{
  // get the opcode
  //  we did push_integer:
  //    0x0104
  //
  opCode = memory[memoryPosition] << BITS | memory[memoryPosition + 1];
  increment_position(2);

  // extract the instruction to execute
  /*
    Instruction Example:
      0x6a02

    Big Endian:
      low ---> high
        02   6a

    Mask:
      0x026a & 0xF000
          ^      ^
          |------|
      0x026a & 0xF000
        ^          ^
        |----------|  
  */
  uint8_t instructionOpCode = MASK(0x00FF);
  run_instruction(instructionOpCode);
}

int
main( const int argc, const char **argv )
{
  reset_memory();
  write_instructions();
  load_memory();
  load_instruction_handlers();

  // print out memory contents
  std::cout << hexdump(memory) << std::endl;

  run_program();

  return 0;
}

