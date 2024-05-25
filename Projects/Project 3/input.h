// Starting code version 1.0

#ifndef INPUT_H
#define INPUT_H

/*
 * Public Interface:
 */

#define MAX_GETLINE_CHARS 128 // max number of bytes to read from stdin line of input

int Input_GetLine(char** line);
int Input_NextInstruction(char* line);

#endif // INPUT_H