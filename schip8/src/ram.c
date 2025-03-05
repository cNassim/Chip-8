#include "include/ram.h"
#include "lib/misc/debug.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t fontset[FONTSET_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,       // 0
	0x20, 0x60, 0x20, 0x20, 0x70,       // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,       // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,       // 3
	0x90, 0x90, 0xF0, 0x10, 0x10,       // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,       // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,       // 6
	0xF0, 0x10, 0x20, 0x40, 0x40,       // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,       // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,       // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,       // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,       // B
	0xF0, 0x80, 0x80, 0x80, 0xF0,       // C
	0xE0, 0x90, 0x90, 0x90, 0xE0,       // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,       // E
	0xF0, 0x80, 0xF0, 0x80, 0x80        // F
};
//Noms des fonctions explique what they do no need for more explications
void Dta_init(struct Ram *ptr_ram) {
    uint16_t start_address = 0x50;
    if (RAM_Ta >= start_address + FONTSET_SIZE) {
        for (int i = 0; i < FONTSET_SIZE; i++) {
            Wrt_ram(ptr_ram, start_address + i, fontset[i]);
        }
    }
}

uint8_t LirefromRam(struct Ram *ptr_ram, uint16_t adrs){
    if(adrs < RAM_Ta){
        return ptr_ram->Dta[adrs];
    }else{
        return 0;
    }
}

void Wrt_ram(struct Ram *ptr_ram, uint16_t adrs ,uint8_t vl){
    if(adrs < RAM_Ta){
        ptr_ram->Dta[adrs]=vl;
    }
}

void ld_rom(const char *fch_path, struct Ram *ptr_ram){
    FILE * rom = fopen(fch_path,"rb");
    if(rom!=NULL){
        uint8_t bt1, bt2;
        uint16_t adrs = Dbt_Adrs;
        while(1){
            size_t r1 = fread(&bt1, sizeof(uint8_t), 1, rom);
            size_t r2 = fread(&bt2, sizeof(uint8_t), 1, rom);
            if (r1 != 1 || r2 != 1) {
                break;
            }
            //uint16_t instr = (bt1 << 8) | bt2;
            // Pour debug j'ai print les bytes
            // printf("byte 1 : 0x%X, byte 2 : 0x%X\n",bt1,bt2);
            Wrt_ram(ptr_ram, adrs, bt1);
            Wrt_ram(ptr_ram, adrs + 1, bt2);
            //printf("Address: 0x%03X | Instruction: 0x%04X | %s\n", adrs, instr,instruction_as_str(instr));
            adrs += 2;
        }
        fclose(rom);
    }else{
        printf("Toz ! Erreur.");
    }
}