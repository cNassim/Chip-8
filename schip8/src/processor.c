/*
 * Pour info chaque case est expliqué sur la documentation de cowgod (j'ai malgré ça laissé des commentaires
 * à coté de chaque case qui explique en une phrase le fonctionnement ) voici le lien au cas où tu trouves
 * pas le lien dans le sujet.pdf :
    - http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.0
 * 
 * Pour l'affichage je me suis basé sur le main.c ( fonction go()) pour afficher.
 * 
 * Tout le code est fonctionnel au cas où t'as un problème contact moi sur wtsp.
 * 
 * Pour l'instant : 
 *  - ROM 1 ✔️
 *  - ROM 2 ✔️
 *  - ROM 3 ✔️
 *  - ROM 4 ✔️
 *  - ROM 5 ✔️(Display issue)
 *  - ROM 6 ✔️
 *  - ROM 7 ✔️
*/

#include "include/processor.h"
#include "lib/display/display.h"
#include "lib/keyboard/keyboard.h"
#include "stdbool.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


void initProc(struct Proc *cp, struct Ram *ram, struct Display *display,struct Keyboard *keyboard,struct Speaker *speaker) {
    for (int i = 0; i < 16; i++) {
        cp->Vx[i] = 0;
        cp->stack[i] = 0;
    }
    cp->I = 0;
    cp->PC = Dbt_Adrs;
    cp->ram = ram;
    cp->display = display;
    cp->keyboard = keyboard;
    cp->speaker = speaker;
    cp->SP = 0;
    cp->t_delay = 0;
    cp->t_sound = 0;
    cp->t_draw = 0;

}

void fde_Proc(struct Proc *cp) {

    uint16_t opc = (LirefromRam(cp->ram, cp->PC) << 8) | LirefromRam(cp->ram, cp->PC + 1);
    cp->PC += 2;
    /*if (cp->t_delay > 0) {
        cp->t_delay--;
    }*/
    if (cp->t_sound >0) {
        //--cp->t_sound ;
        Speaker_on(cp->speaker); 
    }else{
        Speaker_off(cp->speaker);
    }

    switch (opc & 0xF000) {
        case 0x0000:
            if ((opc & 0x00FF) == 0x00E0) { // 00E0: Clear the screen
                Display_CLS(cp->display);
            }else if ((opc & 0x00FF) == 0x00EE) { // 00EE: Return from subroutine
                if (cp->SP == 0) {
                  printf("Stack underflow error\n");
                }else {
                cp->SP--;
                cp->PC = cp->stack[cp->SP];
                //printf("Returning to 0x%X\n", cp->PC);
                }
            }
        break;

        case 0x1000: // 1nnn: Jump to address nnn
            {
               cp->PC = opc & 0x0FFF;
               //printf("prorgramme will jump to 0x%X\n",cp->PC);
            }
        break;
        
        case 0x2000: // 2nnn: Call subroutine at nnn
            cp->stack[cp->SP] = cp->PC; 
            cp->SP++;                   
            cp->PC = opc & 0x0FFF;      
        break;
        
        case 0x3000: // Skip next instruction if Vx = kk
            uint8_t X1 = (opc & 0x0F00) >> 8;
            uint8_t nn1 = opc & 0x00FF;
            if ( cp->Vx[X1] == nn1 ){
                cp->PC += 2;
            }
        break;
        
        case 0x4000: // Skip next instruction if Vx != kk
            uint8_t X2 = (opc & 0x0F00) >> 8;
            uint8_t nn2 = opc & 0x00FF;
            if ( cp->Vx[X2] != nn2 ){
                cp->PC += 2;
            }
        break;
        
        case 0x5000: // Skip next instruction if Vx = Vy
            uint8_t X3 = (opc & 0x0F00) >> 8;
            uint8_t Y = (opc & 0x00F0) >> 4;
            if ( cp->Vx[X3] == cp->Vx[Y]){
                cp->PC += 2;
            }
        break;

        case 0x6000: // 6xnn: Set Vx = nn
            {
                uint8_t x = (opc & 0x0F00) >> 8;
                uint8_t nn = opc & 0x00FF;
                cp->Vx[x] = nn;
            }
        break;
        
        case 0x7000:  // 7xnn - Add immediate value to register Vx
            uint8_t X = (opc & 0x0F00) >> 8;  
            uint8_t nn = opc & 0x00FF;         
            cp->Vx[X] += nn; 
        break;
        
        case 0x8000:
            switch(opc & 0x000F){
                case 0x0000: // 8xy0: Set Vx = Vy
                    uint8_t x_1 =(opc & 0x0F00) >> 8;
                    uint8_t y_1 = (opc & 0x00F0) >> 4;
                    cp->Vx[x_1]=cp->Vx[y_1];
                    //printf("good v1\n");
                break;
                
                case 0x0001: // 8xy1 : Set Vx = Vx OR Vy.
                    uint8_t x = (opc & 0x0F00) >> 8;
                    uint8_t y = (opc & 0x00F0) >> 4;
                    uint8_t vx = cp->Vx[x];         
                    uint8_t vy = cp->Vx[y];        
                    uint8_t result = vx | vy;        
                    cp->Vx[x] = result;
                    cp->Vx[0xF] = 0;
                break;
                
                case 0x0002: // 8xy2 : Set Vx = Vx AND Vy.
                    uint8_t x1 = (opc & 0x0F00) >> 8;
                    uint8_t y1 = (opc & 0x00F0) >> 4;
                    uint8_t vx1 = cp->Vx[x1];         
                    uint8_t vy1 = cp->Vx[y1];
                    uint8_t result1 = vx1 & vy1;
                    cp->Vx[x1] = result1; 
                    cp->Vx[0xF] = 0;
                    
                break;
                            
                case 0x0003: // 8xy3 : Set Vx = Vx XOR Vy.
                    uint8_t x2 = (opc & 0x0F00) >> 8;
                    uint8_t y2 = (opc & 0x00F0) >> 4;
                    uint8_t vx2 = cp->Vx[x2];         
                    uint8_t vy2 = cp->Vx[y2];
                    uint8_t result2 = vx2 ^ vy2;
                    cp->Vx[x2] = result2; 
                    cp->Vx[0xF] = 0;
                break;
                
                
                case 0x0004: // 8xy4 : Set Vx = Vx + Vy, set VF = carry.
                    uint8_t x3 = (opc & 0x0F00) >> 8;
                    uint8_t y3 = (opc & 0x00F0) >> 4;
                    uint16_t sum = cp->Vx[x3] + cp->Vx[y3];
                    cp->Vx[x3]= sum & 0xFF;
                    if(sum>0xFF){
                        cp->Vx[0xF] = 1;
                    }else{
                        cp->Vx[0xF] = 0;
                    }
                break;
                
               case 0x0005: // 8xy5 : Set Vx = Vx - Vy, set VF = NOT borrow.
                    uint8_t x4 = (opc & 0x0F00) >> 8;
                    uint8_t y4 = (opc & 0x00F0) >> 4;
                    uint8_t borrow;
                    if (cp->Vx[x4] < cp->Vx[y4]){
                        borrow = 1;
                    }else{
                        borrow = 0;
                    }
                    cp->Vx[x4] = cp->Vx[x4] - cp->Vx[y4];
                    if (borrow == 0){ 
                        cp->Vx[0xF] = 1;
                    }else {
                        cp->Vx[0xF] = 0;
                    }
                break;

                
                case 0x0006: // 8xy6 : Set Vx = Vy SHR 1.
                    uint8_t x5= (opc & 0x0F00) >> 8;
                    uint8_t y5 = (opc & 0x00F0) >> 4;
                    uint8_t vy5 = cp->Vx[y5];
                    cp->Vx[x5] = vy5 >> 1;
                    cp->Vx[0xF] = vy5 & 0x01;
                break;

                case 0x0007: // 8xy7 : Set Vx = Vy - Vx, set VF = NOT borrow
                    uint8_t xx = (opc & 0x0F00) >> 8;
                    uint8_t yy = (opc & 0x00F0) >> 4;
                    if (cp->Vx[yy] < cp->Vx[xx]) {
                        borrow = 1;
                    } else {
                        borrow = 0;
                    }
                    cp->Vx[xx] = cp->Vx[yy] - cp->Vx[xx];
                    if (borrow == 0){
                        cp->Vx[0xF] = 1;
                    }else {
                        cp->Vx[0xF] = 0;
                    }
                break;

                
                case 0x000E: // Set Vx = Vx SHL 1
                    uint8_t x6 = (opc & 0x0F00) >> 8;
                    uint8_t y6 = (opc & 0x00F0) >> 4;
                    uint8_t vy6 = cp->Vx[y6];
                    cp->Vx[x6] = vy6 << 1;
                    cp->Vx[0xF] = (vy6 & 0x80) >> 7;
                break;

                
                default:
                    printf("Unknown 0x8 : 0x%04X\n", opc);
                break;
            }
        break;
        
        case 0x9000: 
            uint8_t X4 = (opc & 0x0F00) >> 8;
            uint8_t Y2 = (opc & 0x00F0) >> 4;
            if ( cp->Vx[X4] != cp->Vx[Y2]){
                cp->PC += 2;
            }
        break;
        
        case 0xA000: // Annn: Set I = nnn
            cp->I = opc & 0x0FFF;
        break;
        
        case 0xB000: // Instruction Bnnn: JP V0, addr
            cp->PC = (opc & 0x0FFF) + cp->Vx[0];
        break;
        
        case 0xC000: // Set Vx = random byte AND kk
            uint8_t x_c = (opc & 0x0F00) >> 8;
            uint8_t kk_c = opc & 0x00FF;    

            uint8_t random_value = rand() % 256;     
            
            cp->Vx[x_c] = random_value & kk_c;
        break;

        case 0xD000:// Dxyn: Draw sprite
            uint8_t x = cp->Vx[(opc & 0x0F00) >> 8];
            uint8_t y = cp->Vx[(opc & 0x00F0) >> 4];
            uint8_t n = opc & 0x000F;
            uint8_t collision = 0;
            // Drawing logic
            struct Sprite sprite;
            Sprite_init(&sprite, n);

            for (int i = 0; i < n; i++) {
                Sprite_add(&sprite, cp->ram->Dta[cp->I + i]);
            }

            Display_DRW(cp->display, &sprite, x, y, &collision);
            cp->Vx[0xF] = collision;
            Sprite_destroy(&sprite);
        break;
        

        case 0xE000:
            switch(opc & 0x00FF){
                case 0x009E: // Skip next instruction if key with the value of Vx is pressed.
                    int state;
                    uint8_t x_a = (opc & 0x0F00) >> 8;
                    if (Keyboard_get(cp->keyboard, cp->Vx[x_a], &state) == 0 && state == KEY_DOWN) {
                        cp->PC += 2;
                    }
                break;
                
                case 0x00A1: // Skip next instruction if key in Vx is not pressed
                    uint8_t x_b = (opc & 0x0F00) >> 8;
                    if (Keyboard_get(cp->keyboard, cp->Vx[x_b], &state) == 0 && state == KEY_UP) {
                        cp->PC += 2;
                    }
                break;
                
                default:
                        printf("Unknown 0xE : 0x%04X\n", opc);
                break;
            }
        break;
        
        
        case 0xF000: // Fx1E: Set I = I + Vx
                switch(opc & 0x00FF){
                    case 0x000A:// Don't use Keyboard wait  sans me dire ça plant mon pc...
                        uint8_t x_c = (opc & 0x0F00) >> 8;
                        int key_state = 0;
                        bool key_pressed = false;
                        bool key_released = false;

                        while (!key_pressed) {
                            for (int i = 0; i < 16; i++) { // Parcourir toutes les touches
                                if (Keyboard_get(cp->keyboard, i, &key_state) == 0 && key_state == KEY_DOWN) {
                                    cp->Vx[x_c] = i;  // Stocker l'index de la touche appuyée
                                    key_pressed = true;
                                    break;
                                }
                            }
                            if (cp->t_delay > 0) {
                                cp->t_delay--;
                            }
                        }
                        while (!key_released) {
                            int key_state = 0;
                            if (Keyboard_get(cp->keyboard, cp->Vx[x_c], &key_state) == 0 && key_state == KEY_UP) {
                                key_released = true;
                            }
                        }
                    break;
                    
                    case 0x001E: //  Set I = I + Vx.
                        uint8_t x_2 = (opc & 0x0F00) >> 8; 
                        cp->I += cp->Vx[x_2]; 
                    break;
                    
                    case 0x0007: // Set Vx = delay timer value.
                        uint8_t x_5 = (opc & 0x0F00) >> 8;
                        cp->Vx[x_5] = cp->t_delay;
                    break;
                    
                    case 0x0015: // Set delay timer = Vx.
                        uint8_t x_1 = (opc & 0x0F00) >> 8; 
                        cp->t_delay = cp->Vx[x_1];
                    break;
                    
                    case 0x0018: // Set sound timer = Vx.
                        uint8_t px = (opc & 0x0F00) >> 8;
                        cp->t_sound = cp->Vx[px];
                    break;
                    
                    case 0x0029: // Fx29: Set I = location of sprite for character in Vx
                        uint8_t x = (opc & 0x0F00) >> 8;
                        cp->I = 0x050 + (cp->Vx[x] * 5);  // Calculate sprite address based on Vx value
                    break;
                    
                    
                    
                    case 0x0065: // Read registers V0 through Vx from memory starting at location I
                        uint8_t x65 = (opc & 0x0F00) >> 8;
                        for (uint8_t i = 0; i <= x65; i++) {
                            cp->Vx[i] = cp->ram->Dta[cp->I ++];
                        }
                    break;
                        
                    case 0x0055: // Store registers V0 through Vx in memory starting at location I.
                        uint8_t x_3 = (opc & 0x0F00) >> 8;
                        for (uint8_t i = 0; i <= x_3; i++) {
                            //printf("Entry\n");
                            cp->ram->Dta[cp->I ++]=cp->Vx[i];
                            //printf(I'm out\n");
                        }
                    break;
                    
                    case 0x0033: // Store BCD representation of Vx in memory locations I, I+1, and I+2.
                            uint8_t x_4 = (opc & 0x0F00) >> 8;  
                            uint8_t value = cp->Vx[x_4];
                            uint8_t hundreds = value / 100;  
                            uint8_t tens = (value / 10) % 10; 
                            uint8_t ones = value % 10; 
                            cp->ram->Dta[cp->I] = hundreds;
                            cp->ram->Dta[cp->I + 1] = tens;
                            cp->ram->Dta[cp->I + 2] = ones;
                    break;
                    default:
                        printf("Unknown 0xF : 0x%04X\n", opc);
                    break;
                }
        break;
    
        default:
            printf("Unknown : 0x%04X\n", opc);
            break;
    }
}