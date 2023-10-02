#pragma once

#define TMA_MSG "too many arguments"
#define EXTERN_HELP_MSG "external command or application"
#define EXIT_HELP_MSG "exit the shell"
#define PWD_HELP_MSG "print the current directory"
#define CD_HELP_MSG "change the current directory"
#define HISTORY_HELP_MSG "print the history of commands"
#define HISTORY_INVALID_MSG "command invalid"
#define HISTORY_NO_LAST_MSG "no command entered"

/*
 * Concatenate a command and a message.
 *
 * Example:
 * const char *msg = FORMAT_MSG("cd", CD_HELP_MSG);
 *
 * Then you can use `msg` as a string for `write()`.
 */
#define FORMAT_MSG(cmd, msg) cmd ": " msg "\n"

/*
 * Concatenate a number and a command for `history`.
 *
 * Example:
 * const char *msg = FORMAT_HISTORY("0", "cd");
 *
 * Then you can use `msg` as a string for `write()`.
 */
#define FORMAT_HISTORY(num, cmd) num "\t" cmd "\n"
