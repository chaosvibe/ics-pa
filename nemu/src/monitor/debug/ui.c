#include <isa.h>
#include "expr.h"
#include "watchpoint.h"
#include <memory/paddr.h>

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_n(char *args) {
  if (args == NULL) {
    cpu_exec(1);
  } else {
    if (atoi(args) == 0) {
      printf("The N step must be integers, and cannot be '%s'\n", args);
      return 0;
    }
    
    cpu_exec(atoi(args));
  }
  return 0;
}

static struct {
  char *subcmd;
  void (*handler) ();
} print_subcmds[] = {
  { "r", isa_reg_display },
  { "w", },
};

#define PRINT_SUBCMD_SIZE (sizeof(print_subcmds) / sizeof(print_subcmds[0]))

static int print_status(char *args) {
  if (args == NULL) 
  {
    printf("sub command cannot be empty\n");
    return 0;
  }
  

  int i;
  for (i = 0; i < PRINT_SUBCMD_SIZE; i++)
  {
    if (strcmp(args, print_subcmds[i].subcmd) == 0) 
    {
      print_subcmds[i].handler();
      break;
    }
  }

  if (i == PRINT_SUBCMD_SIZE) {
    printf("Unknown print sub command '%s'\n", args); 
  }
  
  return 0;
}

static int scan_mem_expr(char *args) {
  if (args == NULL || strlen(args) == 0)
  {
    printf("scan memory only accept two param, empty is not valid\n");
    return 0;
  }
  
  int N = 1;
  char *expr_str = "";
  for (int i = 0; i < 3; i++)
  {
    char *arg = strtok(args, " ");
    if (arg == NULL || strlen(arg) == 0) {
      if (i < 1)
      {
        printf("scan memory only accept two param, empty or only one param is not valid\n");
      }
      
      break;
    }
    
    if (i == 0)
    {
      N = atoi(arg);
    } else if (i == 1) {
      expr_str = arg;
      if (expr_str == NULL || strlen(expr_str) == 0) {
        printf("scan memory only accept two param, the second param expr cannot be empty\n");
        return 0;
      }
    } else if (i == 2) {
      printf("scan memory only accept two param, '[%s]' is not valid\n", arg);
      return 0;
    }
  }
  bool success;
  word_t expr_value = expr(expr_str, &success);
  if (success) {
    print_N_by_paddr(expr_value, N, 4);
  } else {
    printf("expr: [%s] parse failed", expr_str);
  }
    
  return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "Make program run by steps N", cmd_n},
  { "info", "Print the information depend on the SUBCMD, r for registers and w for watchpoints", print_status},
  { "x", "Scan the memory, for example [x N EXPR]", scan_mem_expr}
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
