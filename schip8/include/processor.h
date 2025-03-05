#ifndef PROCESSOR_HEADER_GUARD
#define PROCESSOR_HEADER_GUARD

#include <stdint.h>
#include "ram.h"
#include "lib/display/display.h"
#include "lib/keyboard/keyboard.h"
#include "lib/speaker/speaker.h"

// Structure du processeur
struct Proc {
    uint8_t Vx[16];           // 16 General-purpose 8-bit registers (V0 to VF)
    uint16_t I;               // Index register
    uint16_t PC;              // Program counter
    struct Ram *ram;          // Pointer to RAM
    struct Display *display;  // Pointer to Display
    struct Keyboard *keyboard; // Pointer vers clavier
    struct Speaker *speaker; // Pointer vers speaker
    uint16_t stack[16];       // Call stack (16 levels of nested subroutines)
    uint8_t SP;
    uint8_t t_delay;
    uint8_t t_sound;
    uint32_t t_draw;
};


// Prototypes des fonctions
void initProc(struct Proc *cp, struct Ram *ram, struct Display *display,struct Keyboard *keyboard,struct Speaker *speaker);
void fde_Proc(struct Proc *cp);

#endif
