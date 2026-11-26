#ifndef SIGTRAP_PROCESS_H
#define SIGTRAP_PROCESS_H

void run_program(const char *path);
void debug_program(const char *path);
void continue_program(void);
void step_program(void);
void close_debug_session(void);

#endif
