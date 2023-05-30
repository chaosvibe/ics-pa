#include <isa.h>
#include <stdlib.h>
#include <time.h>
#include "local-include/reg.h"

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  word_t sample[8];
  word_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

#define PRINT_REG_4(reg_param)  printf("%s: [%d], [%X]\n", reg_name(reg_param, 4), reg_l(reg_param), reg_l(reg_param))
#define PRINT_REG_2(reg_param)  printf("%s: [%d], [%X]\n", reg_name(reg_param, 2), reg_w(reg_param), reg_w(reg_param))
#define PRINT_REG_1(reg_param)  printf("%s: [%d], [%X]\n", reg_name(reg_param, 1), reg_b(reg_param), reg_b(reg_param))


void isa_reg_display() {
  PRINT_REG_4(R_EAX);
  PRINT_REG_4(R_ECX);
  PRINT_REG_4(R_EDX);
  PRINT_REG_4(R_EBX);
  PRINT_REG_4(R_ESP);
  PRINT_REG_4(R_EBP);
  PRINT_REG_4(R_ESI);
  PRINT_REG_4(R_EDI);

  PRINT_REG_2(R_AX);
  PRINT_REG_2(R_CX);
  PRINT_REG_2(R_DX);
  PRINT_REG_2(R_BX);
  PRINT_REG_2(R_SP);
  PRINT_REG_2(R_BP);
  PRINT_REG_2(R_SI);
  PRINT_REG_2(R_DI);

  PRINT_REG_1(R_AL);
  PRINT_REG_1(R_CL);
  PRINT_REG_1(R_DL);
  PRINT_REG_1(R_BL);
  PRINT_REG_1(R_AH);
  PRINT_REG_1(R_CH);
  PRINT_REG_1(R_DH);
  PRINT_REG_1(R_BH);
}

word_t isa_reg_str2val(const char *s, bool *success) {
  return 0;
}
