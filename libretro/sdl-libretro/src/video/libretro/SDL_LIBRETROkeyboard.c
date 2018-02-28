/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2010 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"
#include "SDL_events.h"

//#include "../../events/SDL_keyboard_c.h"
#include "../SDL_sysvideo.h"
#include "../../events/SDL_sysevents.h"
#include "../../events/SDL_events_c.h"

#include "libretro.h"

#include "SDL_LIBRETROkeyboard_c.h"
extern short int libretro_input_state_cb(unsigned port,unsigned device,unsigned index,unsigned id);
/*
extern retro_input_state_t libretro_input_state_cb;
#define libretro_input_state_cb  input_state_cb
*/

Uint16 SDL_RetroToUnicodeTable[256]={
	/* Standard ASCII characters from 0x00 to 0x7e */
	/* Unicode stuff from 0x7f to 0xff */

	0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,
	0x0008,0x0009,0x000A,0x000B,0x000C,0x000D,0x000E,0x000F,
	0x0010,0x0011,0x0012,0x0013,0x0014,0x0015,0x0016,0x0017,
	0x0018,0x0019,0x001A,0x001B,0x001C,0x001D,0x001E,0x001F,
	0x0020,0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,
	0x0028,0x0029,0x002A,0x002B,0x002C,0x002D,0x002E,0x002F,
	0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,
	0x0038,0x0039,0x003A,0x003B,0x003C,0x003D,0x003E,0x003F,

	0x0040,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047,
	0x0048,0x0049,0x004A,0x004B,0x004C,0x004D,0x004E,0x004F,
	0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,
	0x0058,0x0059,0x005A,0x005B,0x005C,0x005D,0x005E,0x005F,
	0x0060,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,
	0x0068,0x0069,0x006A,0x006B,0x006C,0x006D,0x006E,0x006F,
	0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,
	0x0078,0x0079,0x007A,0x007B,0x007C,0x007D,0x007E,0x0394,

	0x00C7,0x00FC,0x00E9,0x00E2,0x00E4,0x00E0,0x00E5,0x00E7,
	0x00EA,0x00EB,0x00E8,0x00EF,0x00EE,0x00EC,0x00C4,0x00C5,
	0x00C9,0x00E6,0x00C6,0x00F4,0x00F6,0x00F2,0x00FB,0x00F9,
	0x00FF,0x00D6,0x00DC,0x00A2,0x00A3,0x00A5,0x00DF,0x0192,
	0x00E1,0x00ED,0x00F3,0x00FA,0x00F1,0x00D1,0x00AA,0x00BA,
	0x00BF,0x2310,0x00AC,0x00BD,0x00BC,0x00A1,0x00AB,0x00BB,
	0x00C3,0x00F5,0x00D8,0x00F8,0x0153,0x0152,0x00C0,0x00C3,
	0x00D5,0x00A8,0x00B4,0x2020,0x00B6,0x00A9,0x00AE,0x2122,

	0x0133,0x0132,0x05D0,0x05D1,0x05D2,0x05D3,0x05D4,0x05D5,
	0x05D6,0x05D7,0x05D8,0x05D9,0x05DB,0x05DC,0x05DE,0x05E0,
	0x05E1,0x05E2,0x05E4,0x05E6,0x05E7,0x05E8,0x05E9,0x05EA,
	0x05DF,0x05DA,0x05DD,0x05E3,0x05E5,0x00A7,0x2038,0x221E,
	0x03B1,0x03B2,0x0393,0x03C0,0x03A3,0x03C3,0x00B5,0x03C4,
	0x03A6,0x0398,0x03A9,0x03B4,0x222E,0x03C6,0x2208,0x2229,
	0x2261,0x00B1,0x2265,0x2264,0x2320,0x2321,0x00F7,0x2248,
	0x00B0,0x2022,0x00B7,0x221A,0x207F,0x00B2,0x00B3,0x00AF
};

#define RETROMOD(a,i) {\
        SDL_keysym sym;\
	    memset(&sym, 0, sizeof(SDL_keysym));\
        sym.scancode=i;\
        sym.sym=i;\
        sym.unicode = 0;\
    	if (SDL_TranslateUNICODE && a) {\
    		if (i<256)sym.unicode = SDL_RetroToUnicodeTable[i];\
    	}\
	    SDL_PrivateKeyboard(a ? SDL_PRESSED : SDL_RELEASED, &sym);\
}  

 void Process_key(_THIS)
{
	int i;

	for(i=0;i<322;i++)
        	this->hidden->Key_Sate[i]=libretro_input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0,i) ? 0x80: 0;
   
	if(memcmp( this->hidden->Key_Sate,this->hidden->old_Key_Sate , sizeof(this->hidden->Key_Sate) ) )
	 	for(i=0;i<322;i++)
			if(this->hidden->Key_Sate[i] && this->hidden->Key_Sate[i]!=this->hidden->old_Key_Sate[i]  )
        	{

				if(i==RETROK_RCTRL){
					//CTRLON=-CTRLON;
					//printf("Modifier crtl pressed %d \n",CTRLON); 
                    RETROMOD(1,i);
					continue;
				}
				if(i==RETROK_RSHIFT){
					//SHITFON=-SHITFON;
					//printf("Modifier shift pressed %d \n",SHIFTON); 
                    RETROMOD(1,i);
					continue;
				}

				if(i==RETROK_LCTRL){
					//CTRLON=-CTRLON;
					//printf("Modifier crtl pressed %d \n",CTRLON); 
                    RETROMOD(1,i);
					continue;
				}
				if(i==RETROK_LSHIFT){
					//SHITFON=-SHITFON;
					//printf("Modifier shift pressed %d \n",SHIFTON); 
                    RETROMOD(1,i);
					continue;
				}

				if(i==RETROK_LALT){
					//KBMOD=-KBMOD;
					//printf("Modifier alt pressed %d \n",KBMOD); 
                    RETROMOD(1,i);
					continue;
				}
				if(i==RETROK_RALT){
					//KBMOD=-KBMOD;
					//printf("Modifier alt pressed %d \n",KBMOD); 
                    RETROMOD(1,i);
					continue;
				}

				if(i==RETROK_LMETA){
					//KBMOD=-KBMOD;
					//printf("Modifier alt pressed %d \n",KBMOD);
                    RETROMOD(1,i); 
					continue;
				}
				if(i==RETROK_RMETA){
					//KBMOD=-KBMOD;
					//printf("Modifier alt pressed %d \n",KBMOD); 
                    RETROMOD(1,i);
					continue;
				}

				//printf("press: %d \n",i);
                    RETROMOD(1,i);
				//retro_key_down(i);
	
        	}	
        	else if ( !this->hidden->Key_Sate[i] && this->hidden->Key_Sate[i]!=this->hidden->old_Key_Sate[i]  )
        	{

				if(i==RETROK_RCTRL){
					//CTRLON=-CTRLON;
					//printf("Modifier crtl released %d \n",CTRLON); 
                    RETROMOD(0,i);
					continue;
				}
				if(i==RETROK_RSHIFT){
					//SHIFTON=-SHIFTON;
					//printf("Modifier shift released %d \n",SHIFTON); 
                    RETROMOD(0,i);
					continue;
				}

				if(i==RETROK_LCTRL){
					//CTRLON=-CTRLON;
					//printf("Modifier crtl pressed %d \n",CTRLON); 
                    RETROMOD(0,i);
					continue;
				}
				if(i==RETROK_LSHIFT){
					//SHITFON=-SHITFON;
					//printf("Modifier shift pressed %d \n",SHIFTON); 
                    RETROMOD(0,i);
					continue;
				}

				if(i==RETROK_LALT){
//					KBMOD=-KBMOD;
					//printf("Modifier alt released %d \n",KBMOD); 
                    RETROMOD(0,i);
					continue;
				}
				if(i==RETROK_RALT){
					//KBMOD=-KBMOD;
					//printf("Modifier alt pressed %d \n",KBMOD); 
                    RETROMOD(0,i);
					continue;
				}

				if(i==RETROK_LMETA){
					//KBMOD=-KBMOD;
					//printf("Modifier alt pressed %d \n",KBMOD); 
                    RETROMOD(0,i);
					continue;
				}
				if(i==RETROK_RMETA){
					//KBMOD=-KBMOD;
					//printf("Modifier alt pressed %d \n",KBMOD); 
                    RETROMOD(0,i);
					continue;
				}

				//printf("release: %d \n",i);
                    RETROMOD(0,i);
				//retro_key_up(i);
	
        	}	

	memcpy(this->hidden->old_Key_Sate,this->hidden->Key_Sate , sizeof(this->hidden->Key_Sate) );

}


void LIBRETRO_PumpKeyboard(_THIS)
{    
        Process_key(this);
}

void
LIBRETRO_InitKeyboard(_THIS)
{

}

void
LIBRETRO_QuitKeyboard(_THIS)
{

}


/* vi: set ts=4 sw=4 expandtab: */
