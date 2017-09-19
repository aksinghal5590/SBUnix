#include <sys/interrupt.h>

//1st row: glyph
//2nd row: key press(0) or release(1)
uint8_t scancodes[256][2];

void addScanCode(uint8_t pos, uint8_t val, uint8_t state) {

        scancodes[pos][0] = val;
        scancodes[pos][1] = state;
}

void initScanCodeTable() {
	uint8_t i = 0;
	for(i = 0; i < 255; i++)
		scancodes[i][1] = 1;
	//add numbers
	for(i = 2; i <= 11; i++) {
		scancodes[i][0] = (i-1)%10 + 48;
		scancodes[i][1] = 0;
	}
	for(i = 0x82; i <= 0x8B; i++) {
                scancodes[i][0] = (i-0x81)%10 + 48;
                scancodes[i][1] = 1;
        }
	
	//add alphabets
	i = 0x10;
	addScanCode(i++, 'q', 0);
	addScanCode(i++, 'w', 0);
	addScanCode(i++, 'e', 0);
	addScanCode(i++, 'r', 0);
	addScanCode(i++, 't', 0);
	addScanCode(i++, 'y', 0);
	addScanCode(i++, 'u', 0);
	addScanCode(i++, 'i', 0);
	addScanCode(i++, 'o', 0);
	addScanCode(i++, 'p', 0);
	i = 0x1E;
	addScanCode(i++, 'a', 0);
	addScanCode(i++, 's', 0);
	addScanCode(i++, 'd', 0);
	addScanCode(i++, 'f', 0);
	addScanCode(i++, 'g', 0);
	addScanCode(i++, 'h', 0);
	addScanCode(i++, 'j', 0);
	addScanCode(i++, 'k', 0);
	addScanCode(i++, 'l', 0);
	i = 0x2C;
	addScanCode(i++, 'z', 0);
	addScanCode(i++, 'x', 0);
	addScanCode(i++, 'c', 0);
	addScanCode(i++, 'v', 0);
	addScanCode(i++, 'b', 0);
	addScanCode(i++, 'n', 0);
	addScanCode(i++, 'm', 0);
	i = 0x90;
        addScanCode(i++, 'q', 1);
        addScanCode(i++, 'w', 1);
        addScanCode(i++, 'e', 1);
        addScanCode(i++, 'r', 1);
        addScanCode(i++, 't', 1);
        addScanCode(i++, 'y', 1);
        addScanCode(i++, 'u', 1);
        addScanCode(i++, 'i', 1);
        addScanCode(i++, 'o', 1);
        addScanCode(i++, 'p', 1);
        i = 0x9E;
        addScanCode(i++, 'a', 1);
        addScanCode(i++, 's', 1);
        addScanCode(i++, 'd', 1);
        addScanCode(i++, 'f', 1);
        addScanCode(i++, 'g', 1);
        addScanCode(i++, 'h', 1);
        addScanCode(i++, 'j', 1);
        addScanCode(i++, 'k', 1);
        addScanCode(i++, 'l', 1);
        i = 0xAC;
        addScanCode(i++, 'z', 1);
        addScanCode(i++, 'x', 1);
        addScanCode(i++, 'c', 1);
        addScanCode(i++, 'v', 1);
        addScanCode(i++, 'b', 1);
        addScanCode(i++, 'n', 1);
        addScanCode(i++, 'm', 1);
	
	//Shift and Ctrl
	addScanCode(0x2A, 128, 0);
	addScanCode(0x36, 128, 0);
	addScanCode(0xAA, 128, 1);
        addScanCode(0xB6, 128, 1);

        addScanCode(0x1D, 129, 0);
        addScanCode(0x9D, 129, 1);

	//Special Keys pressed
	addScanCode(0x01, 27, 0);//escape
	addScanCode(0x0C, 45, 0);//-
	addScanCode(0x0D, 61, 0);//=
	addScanCode(0x0E, 8, 0);//backspace
	addScanCode(0x0F, 9, 0);//tab
	addScanCode(0x1A, 91, 0);//[
	addScanCode(0x1B, 93, 0);//]
	addScanCode(0x1C, 13, 0);//enter
	addScanCode(0x27, 59, 0);//;
	addScanCode(0x28, 39, 0);//'
	addScanCode(0x29, 96, 0);//`
	addScanCode(0x2B, 92, 0);//backslash
	addScanCode(0x33, 44, 0);//,
	addScanCode(0x34, 46, 0);//.
	addScanCode(0x35, 47, 0);///
	addScanCode(0x37, 42, 0);//*
	addScanCode(0x39, 32, 0);//space
	addScanCode(0x3A, 130, 0);//Caps
	addScanCode(0x3B, 131, 0);//f1
	addScanCode(0x3C, 132, 0);//f2
	addScanCode(0x3D, 133, 0);//f3
	addScanCode(0x3E, 134, 0);//f4
	addScanCode(0x3F, 135, 0);//f5
	addScanCode(0x40, 136, 0);//f6
	addScanCode(0x41, 137, 0);//f7
	addScanCode(0x42, 138, 0);//f8
	addScanCode(0x43, 139, 0);//f9
	addScanCode(0x44, 140, 0);//f10
	addScanCode(0x45, 141, 0);//number lock
	addScanCode(0x46, 142, 0);//scrolllock
	addScanCode(0x47, 55, 0);//keypad 7
	addScanCode(0x48, 56, 0);//keypad 8
	addScanCode(0x49, 57, 0);//keypad 9
	addScanCode(0x4A, 45, 0);//keypad -
	addScanCode(0x4B, 52, 0);//keypad 4
	addScanCode(0x4C, 53, 0);//keypad 5
	addScanCode(0x4D, 54, 0);//keypad 6
	addScanCode(0x4E, 43, 0);//keypad +
	addScanCode(0x4F, 49, 0);//keypad 1
	addScanCode(0x50, 50, 0);//keypad 2
	addScanCode(0x51, 51, 0);//keypad 3
	addScanCode(0x52, 48, 0);//keypad 0
	addScanCode(0x53, 46, 0);//keypad .
	addScanCode(0x57, 143, 0);//f11
	addScanCode(0x58, 144, 0);//f12
	
	//special characters released
	addScanCode(0x81, 27, 1);//escape
	addScanCode(0x8C, 45, 1);//-
	addScanCode(0x8D, 61, 1);//=
	addScanCode(0x8E, 8, 1);//backspace
	addScanCode(0x8F, 9, 1);//tab
	addScanCode(0x9A, 91, 1);//[
	addScanCode(0x9B, 93, 1);//]
	addScanCode(0x9C, 13, 1);//enter
	addScanCode(0xA7, 59, 1);//;
	addScanCode(0xA8, 39, 1);//'
	addScanCode(0xA9, 96, 1);//`
	addScanCode(0xAB, 92, 1);//backslash
	addScanCode(0xB3, 44, 1);//,
	addScanCode(0xB4, 46, 1);//.
	addScanCode(0xB5, 47, 1);///
	addScanCode(0xB7, 42, 1);//*
	addScanCode(0xB9, 32, 1);//space
	addScanCode(0xBA, 130, 1);//capslock
	addScanCode(0xBB, 131, 1);//f1
	addScanCode(0xBC, 132, 1);//f2
	addScanCode(0xBD, 133, 1);//f3
	addScanCode(0xBE, 134, 1);//f4
	addScanCode(0xBF, 135, 1);//f5
	addScanCode(0xC0, 136, 1);//f6
	addScanCode(0xC1, 137, 1);//f7
	addScanCode(0xC2, 138, 1);//f8
	addScanCode(0xC3, 139, 1);//f9
	addScanCode(0xC4, 140, 1);//f10
	addScanCode(0xC5, 141, 1);//number lock
	addScanCode(0xC6, 142, 1);//scroll lock
	addScanCode(0xC7, 55, 1);//keypad 7
	addScanCode(0xC8, 56, 1);//keypad 8
	addScanCode(0xC9, 57, 1);//keypad 9
	addScanCode(0xCA, 45, 1);//keypad -
	addScanCode(0xCB, 52, 1);//keypad 4
	addScanCode(0xCC, 53, 1);//keypad 5
	addScanCode(0xCD, 54, 1);//keypad 6
	addScanCode(0xCE, 43, 1);//keypad +
	addScanCode(0xCF, 49, 1);//keypad 1
	addScanCode(0xD0, 50, 1);//keypad 2
	addScanCode(0xD1, 51, 1);//keypad 3
	addScanCode(0xD2, 48, 1);//keypad 0
	addScanCode(0xD3, 46, 1);//keypad .
	addScanCode(0xD7, 143, 1);//f11
	addScanCode(0xD8, 144, 1);//f12
}
