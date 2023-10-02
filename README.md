# Assignment 9: Simple Shell

## Overview

In this assignment, you will develop a simple shell. A typical shell provides a command line prompt
at which user commands are entered. It executes each command in a separate process. It also runs a
command in the background if the ampersand (`&`) is used at the end of the command. You will use the
system calls you have learned so far to implement such a shell.

There are three mandatory features that you will need to implement:

* Execute user commands in separate processes, either in the foreground or in the background if
  `&` is used.
* Implement some internal commands.
* Implement a history feature.

Before you start the assignment, make sure you `record`.

## Task 0: Basic Command Support

The first feature you need to implement is executing programs that a user enters at the prompt. You
need to use the necessary system calls appropriately to implement this feature. Of course, you need
to handle a user command as input and pass all arguments correctly for execution. There are two
modes of execution for external programs.

* Foreground execution: In this mode, you need to execute a program in a child process and wait
  until the child process terminates.
* Background execution: In this mode, you need to execute a program in a child process but should
  not wait for the termination of the child process.
* `&` at the end of a user-entered command means that the command should run in the background.

There are a few things to keep in mind.

* You need to show a simple prompt using this string: `"$ "`. It should be printed out to the
  standard output stream.
* For foreground execution, you need to wait for the exact process you fork and not for any other
  processes.
* Since we do not wait for a background process to terminate, it can become a zombie process. Thus,
  your shell should still wait on it *at some point* to clean it up. There are many ways to achieve
  this, but for this assignment, you can make an *additional* `waitpid()` call every time a user
  enters a command. The following are some hints.
    * This `waitpid()` call should wait on *any child process* since you're cleaning up zombie
      processes. Use an appropriate `pid` for that.
    * This `waitpid()` call should *return immediately if no child has exited*. Use an appropriate
      option for that.
    * Consult the man page of `waitpid()` for the above two points.
    * Remember that there can be multiple background processes launched previously. Thus, a single
      call will not clean up all of them. You need to use a loop to clean up all zombie processes.
* For input/output, use `read()` and `write()` since we need to handle a signal in the next task.
  Other functions like `printf()` do not play nicely with signals.

## Task 1: Shell Prompt, Internal Commands, and SIGINT

The second task is to implement a shell prompt and some internal commands.

* Your shell prompt should always show the current working directory. For example, if the user is in
  the `/home/cmpt201` folder, the prompt should be:

  ```bash
  /home/cmpt201$
  ```

* Your shell should implement a few internal commands described below. Internal commands are
  built-in commands of the shell itself, as opposed to a separate program that is executed. You
  should not fork new processes for internal commands as they are built-in. All the commands should
  be *case-sensitive*. For all error messages, *use the macros defined in `msgs.h` for correct
  formatting* and print out to the *standard error* stream.
    * `exit`: Exit the shell program. If the user provides any argument, abort the operation and
      print out this error message: `exit: too many arguments`.
    * `pwd`: Display the current working directory. Use the `getcwd()` function. If `getcwd()`
      returns an error, display the error message using `perror("pwd")`. If the user provides any
      argument, abort the operation and print out this error message: `pwd: too many arguments`.
    * `cd`: Change the current working directory. Use the `chdir()` function. If `chdir()` returns
      an error, display the error message using `perror("cd")`. If the user passed in more than one
      argument, abort the operation and print out this error message: `cd: too many arguments`.
      Additionally, implement the following features:
        * Change to the home directory if no argument is provided.
        * Support `~` for the home folder. For example, `cd ~/cmpt201` should change to the
          "cmpt201" directory under the current user's home directory. Issuing `cd ~` will switch to
          the home directory.
        * Support `-` for changing back to the previous directory. For example, suppose that the
          current working directory is `/home` and you issued `cd /` to change to the root
          directory. Then, `cd -` will switch back to the `/home` directory.
        * You may find the `getuid()` and `getpwuid()` functions useful. They allow you to gather
          useful information about the current user.
    * `help`: Display help information on internal commands.
        * If the first argument is one of the internal commands, use the macros in `msgs.h` to print
          out the command and the help message to the standard output stream. For example, if the
          command is `cd`, print out `cd: change the current directory`.
        * If the first argument is not an internal command, use the macros in `msgs.h` to print out
          the program name and the help message to the standard output stream. For example, if
          argument is `ls`, print out `ls: external command or application`.
        * If there is more than one argument, display this error message: `help: too many
          arguments`.
        * If there is no argument provided, list all the supported internal commands one by one with
          their corresponding help messages.
* Your shell should not terminate when a user presses `CTRL-C`. Thus, you need to write a signal
  handler for `SIGINT`.
    * Have the signal handler display the help information (same as the `help` command).
    * Then re-display the command-prompt before returning.
    * A user can press `CTRL-C` anytime and interrupt system calls that are being executed. Thus,
      you need to properly check the return value of a system call and also check `errno`. More
      specifically, you need to understand which value `errno` gets when a signal interrupts a
      system call. You need to differentiate signal interrupts from regular errors.

## Task 2: History Feature

The next task is to modify your shell to provide a history feature that allows the user access up to
the 10 most recently entered commands. Start numbering the user's commands at 0 and increment for
each command entered. These numbers will grow past 9. For example, if the user has entered 35
commands, then the most recent 10 will be numbered 15 through 34. For all error messages, *use the
macros defined in `msgs.h` for correct formatting* and print out to the *standard error* stream.

* First, implement an internal command `history` which displays the 10 most recent commands executed
  in the shell. If there are less than 10 commands entered, display all the commands entered so far.
    * Display the commands in descending order (sorted by its command number).
    * The output should include both external application commands and internal commands.
    * Display the command number on the left, and the command (with all its arguments) on the right.
        * Use the macros in `msgs.h`.
        * If the command is run in the background using `&`, it must be added to the history with the
          `&`.
    * A sample output of the history command is shown below:

      ```bash
      /home/cmpt201$ history
      30	history
      29	cd /home/cmpt201
      28	cd /proc
      27	cat uptime &
      26	cd /usr
      25	ls
      24	man pthread_create
      23	ls
      22	echo "Hello World from my shell!"
      21	ls -la
      /home/cmpt201$
      ```

* Next, implement the `!` and related commands which allows users to run commands directly from the
history list:
    * Command `!n` runs command number n. For example, `!22` re-runs the 23rd command in the
      history. In the above example, this will re-run the echo command.
        * If n is not a number, or an invalid value (not one of the previous 10 command numbers)
          then display this error: `history: command invalid`.
    * Command `!!` runs the last command entered.
        * If there is no previous command, display the error message: `history: no command entered`.
          The history should remain empty.
    * When running a command using `!n` or `!!`, display the command from the history to the screen
      so the user can see what command they are actually running.
    * Neither the `!!` nor the `!n` commands are to be added to the history list themselves, but
      rather the command being executed from the history must be added. Here is an example.

      ```bash
      /home/cmpt201$ echo test
      test
      /home/cmpt201$ !!
      echo test
      test
      /home/cmpt201$ history
      15	history
      14	echo test
      13	echo test
      12	ls
      11	man pthread_create
      10	cd /home/cmpt201
      9	    ls
      8	    ls -la
      7	    echo Hello World from my shell!
      6	    history
      /home/cmpt201$
      ```

## Other Requirements and Exceptions

* Your code must not have any memory leaks or memory access errors. Use appropriate sanitizers and
  test to make sure you do not leak any memory.
* Your shell must free all memory before it exits. However, your child processes may exit without
  freeing all their memory. You do not need to handle such cases.
* You do not need to support either `>` (output redirection) or `|` (pipe).

## Grading

* Code that does not compile gets a 0.
* Distribution
    * [30 points] Task 0
    * [30 points] Task 1
    * [30 points] Task 2
    * [10 points] No memory issues

## Acknowledgment

Created by Mohamed Hefeeda, modified by Brian Fraser, Keval Vora, Tianzheng Wang, and Steve Ko.
