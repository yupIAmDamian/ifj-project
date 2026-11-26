// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)
#ifndef TRANSLATOR_TEST_H
#define TRANSLATOR_TEST_H

/**
 * @brief Redirects standard input and standard output to the specified files.
 * @param input_file Path to the input source file to read from.
 * @param output_file Path to the output file to write generated code into.
 * @return 0 on success, or 1 if either file fails to open.
 */
int redirect_streams(char *input_file, char *output_file);

/**
 * @brief Executes the full end-to-end compilation (parsing, semantic analysis,
 * and code generation) from an input file to an output file.
 * @param input_file Path to the IFJ26 source file to compile.
 * @param output_file Path to the destination file for generated IFJcode26
 * instructions.
 * @return 0 on successful translation, 1 on I/O or semantic failure, 99 on
 * internal symbol table allocation failure, or the parser error code on syntax
 * failure.
 */
int translator_test(char *input_file, char *output_file);

/**
 * @brief Entry point that validates command-line arguments and runs the
 * end-to-end translator test.
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings expecting input and output
 * file paths.
 * @return 0 on success, 2 on invalid argument count, or the error code returned
 * by translator_test.
 */
int main(int argc, char *argv[]);

#endif