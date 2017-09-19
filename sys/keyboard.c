#include <sys/interrupt.h>
#include <sys/kprintf.h>

extern uint8_t scancodes[256][2];

void initKeyboard() {

	initScanCodeTable();
}

uint8_t special_char = 0;

void keyboardImpl() {
	
	uint8_t input = inIO(0x60);
	uint8_t c = scancodes[input][0];

	if(scancodes[input][1]) {
		if(c > 127) {
			special_char = 0;
		}
		return;
	}
	if(c <= 127) {
		if(!special_char) {
			char s[] = {(char)c, '\0'};
			kprintf("Input character: %s\n", s);
			return;
		} else {
			if(special_char == 128) {
				if(c >= 97 && c <= 122) {
					c -= 32;
					char s[] = {(char)c, '\0'};
					kprintf("Input character: %s\n", s);
				}
			} else if(special_char == 129) {
				if(c >= 97 && c <= 122) {
                                        char s[] = {'^', (char)(c-32), '\0'};
                                        kprintf("Input character: %s\n", s);
                                }
				if(c >= 1 && c <= 26) {
					char s[] = {'^', (char)(c+65), '\0'};
					kprintf("Input character: %s\n", s);
				}
			}
		}
	}
	if(scancodes[input][0] > 127) {
		special_char = scancodes[input][0];
	}
}
