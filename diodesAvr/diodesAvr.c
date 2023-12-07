#include <avr/io.h>
#include <util/delay.h>




void Init(){
DDRD = 0xFF; // ���� D �� �����
DDRB = 0x00; // ���� � �� ����
PORTB = 0xFF;  // �������� ������������� ��������� ����� �
PORTD = 0xFF;
}

#define BUTTON_MODE_OFF   0b00000001
#define BUTTON_MODE_MANUAL  0b00000010
#define BUTTON_MODE_NIGHT   0b00000100
#define BUTTON_MODE_MANUAL_CONTROL  0b00001000

#define MODE_NORMAL     0
#define MODE_NIGHT      1
#define MODE_MANUAL     2
#define MODE_OFF        3

#define NORMAL_MODE_DIODE_COUNT    3

// ����� ������ ������� ������� ������� ������������� ��������, �������, �������� �������� ���������)
char diodes_normal_mode[] = { 0b11111100, 0b11110011, 0b00001111 };



char detect_buttons_in_delay(int delay)
{
	static char previous_port = 0xFF; // ������������� ��� ������� ���������
	int quant_count = delay / 10; /// ������������ ���������� 10�� ������� 
	// ����������� ����� �� �������
	for (int quant_number = 0; quant_number < quant_count; ++quant_number)
		{
		// ������� ������ ����������� ������� ������
		char current_port = PINB;	
		char buttons =  ~current_port & previous_port;
		previous_port = current_port;
		if (buttons) // ���������� ��������� ���������� �������,
		return buttons;// ���� ���������������� ������� ������
		_delay_ms(10); // ����� 10 �� �����
}
// ���� ��������, ���� ���� ���������� �� �����,

return 0x00; // �.�. �� ���� ������� ������
}


char normal_mode(char buttons, char state) {
    // ���� ������� ����� MODE_NORMAL, ��� ������� ���������� ��� � �������
    // ��������� ������� - ����� ���������

    if (buttons & BUTTON_MODE_OFF) {
        state = MODE_OFF;
    }
    if (buttons & BUTTON_MODE_MANUAL) {
        state = MODE_MANUAL;
    }
    if (buttons & BUTTON_MODE_NIGHT) {
        state = MODE_NIGHT;
    }
    // ����������� ������� ���������, ���� ��������� � ���� ������
    static char diodes_index = 0;
    PORTD = diodes_normal_mode[diodes_index]; 
    diodes_index = (diodes_index + 1) % NORMAL_MODE_DIODE_COUNT;
    return state;
}


char off_mode(char buttons, char state) {
    // ���� ������� ����� MODE_OFF, ��� ������� ���������� ��� � �������
    if (buttons & BUTTON_MODE_OFF) {
        state = MODE_NORMAL; // ������� � ���������
    }
	if (buttons & BUTTON_MODE_MANUAL) {
        state = MODE_MANUAL;
    }
    if (buttons & BUTTON_MODE_NIGHT) {
        state = MODE_NIGHT;
    }
    PORTD = 0xFF; // ����� ��� �����, ���� ��������� � ���� ���������
    return state;
}

char night_mode(char buttons, char state) 
{
	if (buttons & BUTTON_MODE_OFF) {
        state = MODE_NORMAL;
    }
    if (buttons & BUTTON_MODE_MANUAL) {
        state = MODE_MANUAL;
    }
	PORTD = diodes_normal_mode[1];
	_delay_ms(1000);
	PORTD = 0xFF;
	
	return state;
}


char manual_mode(char buttons, char state) 
{
	if (buttons & BUTTON_MODE_OFF) {
        state = MODE_OFF;
    }
    if (buttons & BUTTON_MODE_NIGHT) {
        state = MODE_NIGHT;
    }
	
	static char diodes_index = 0;
	if (buttons & BUTTON_MODE_MANUAL_CONTROL) {
		PORTD = diodes_normal_mode[diodes_index];
		diodes_index = (diodes_index + 1) % NORMAL_MODE_DIODE_COUNT;
		state = MODE_MANUAL;
	}
	return state;

}




int main() 
{
	Init();
	
	char state = MODE_OFF;
    while (1) {
        char buttons = detect_buttons_in_delay(300);
        switch (state)
        {
        case MODE_NORMAL:
            state = normal_mode(buttons, state);
            break;
        case MODE_NIGHT:
            state = night_mode(buttons, state);
            break;
        case MODE_MANUAL:
            state = manual_mode(buttons, state);
            break;
        case MODE_OFF:
            state = off_mode(buttons, state);
            break;
        }
    }
}
