#include "include/processor.h"
#include "include/ram.h"
#include "lib/display/display.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
    struct Ram ram = {0};
    struct Display display;
    struct Proc proc;
    struct Speaker spec;
    struct Keyboard keyboard;

    // Initialisation des composants
    Dta_init(&ram);
    ld_rom("Projet_C/ROM/ROM5.ch8", &ram);
    Keyboard_init(&keyboard);
    Speaker_init(&spec);
    Display_init(&display, 15);
    initProc(&proc, &ram, &display, &keyboard, &spec);

    // Boucle principale
    bool running = true; 
    SDL_Event event;
    
    // Variables pour le timer
    uint32_t last_timer_update = SDL_GetTicks();


    while (running) {

        // Handle SDL events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;  // Exit if the user closes the window
            }
        }

        // Execute one instruction cycle
        fde_Proc(&proc);

        // Update display at 60Hz
        Display_update(&display);
        
            // Mise à jour des timers CHIP-8 (t_delay et t_sound) à 60 Hz
        uint32_t current_time = SDL_GetTicks();
        if (current_time - last_timer_update >= 1000 / 60) {
            if (proc.t_delay > 0) --proc.t_delay;
            if (proc.t_sound > 0) --proc.t_sound;
            last_timer_update = current_time;
        }
        SDL_Delay(1);
    }

    // Cleanup
    Keyboard_destroy(&keyboard);
    Speaker_destroy(&spec);
    Display_destroy(&display);
    SDL_Quit();

    return 0;
}
