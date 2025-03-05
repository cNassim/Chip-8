#ifndef RAM_HEADER_GUARD
#define RAM_HEADER_GUARD

#include <stdint.h>
#include <stdlib.h>
/////////////Structure de la ram///////////////////
#define RAM_Ta 4096
#define Dbt_Adrs 0x200
#define FONTSET_SIZE 80
struct Ram {
  uint8_t Dta[RAM_Ta];  
};

////////////////Prototype de fct///////////////
void Dta_init(struct Ram *ptr_ram);
uint8_t LirefromRam(struct Ram *ptr_ram, uint16_t adrs);
void Wrt_ram(struct Ram *ptr_ram, uint16_t adrs ,uint8_t vl);
void ld_rom(const char *fch_path, struct Ram *ptr_ram);




#endif