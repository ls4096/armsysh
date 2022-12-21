#ifndef _COMMAND_H_
#define _COMMAND_H_

#define PC_RC_RESET (-1)
#define PC_RC_STOP (-2)

int command_process(unsigned char* cmd_str);
const char* command_tab_complete(const char* cmd, unsigned int cmd_len, unsigned int* match_count);

#endif // _COMMAND_H_
