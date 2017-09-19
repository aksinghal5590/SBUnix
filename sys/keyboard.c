#include <sys/string.h>
#include <sys/interrupt.h>
#include <sys/kprintf.h>


extern uint8_t scancodes[256][2];

void initKeyboard() {

	initScanCodeTable();
}

void printKeyboardChar(uint8_t printChar)
{
	char s[2] = {};
	s[0] = (char)printChar;
	s[1] ='\0';
	kprintf("Input character: %s\n", s);
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
				else {
					uint8_t printChar = 0;
					switch((char)c)
					{
						case '`':
							printChar = 126;
							printKeyboardChar(printChar);
							break;
						case '1':
							printChar = 33;
							printKeyboardChar(printChar);
							break;
						case '2':
							printChar = 64;
							printKeyboardChar(printChar);
							break;
						case '3':	
							printChar = 35;
							printKeyboardChar(printChar);
							break;
						case '4':
							printChar = 36;
							printKeyboardChar(printChar);
							break;
						case '5':
							printChar = 37;
							printKeyboardChar(printChar);
							break;
						case '6':
							printChar = 94;
							printKeyboardChar(printChar);
							break;
						case '7':
							printChar = 38;
							printKeyboardChar(printChar);
							break;
						case '8':
							printChar = 42;
							printKeyboardChar(printChar);
							break;
						case '9':
							printChar = 40;
							printKeyboardChar(printChar);
							break;
						case '0':
							printChar = 41;
							printKeyboardChar(printChar);
							break;
						case '-':
							printChar = 95;
							printKeyboardChar(printChar);
							break;
						case '=':
							printChar = 43;
							printKeyboardChar(printChar);
							break;
						case '[':
							printChar = 123;
							printKeyboardChar(printChar);
							break;
						case ']':
							printChar = 125;
							printKeyboardChar(printChar);
							break;
						case '\\':
							printChar = 124;
							printKeyboardChar(printChar);
							break;
						case ';':
							printChar = 58;
							printKeyboardChar(printChar);
							break;
						case '\'':
							printChar = 34;
							printKeyboardChar(printChar);
							break;
						case ',':
							printChar = 60;
							printKeyboardChar(printChar);
							break;
						case '.':
							printChar = 62;
							printKeyboardChar(printChar);
							break;
						case '/':
							printChar = 63;
							printKeyboardChar(printChar);
							break;
						default:
							printChar = c;
							printKeyboardChar(printChar);
							break;
					}
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
