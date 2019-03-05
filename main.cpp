#include <cstdio>
#include <wiringPi.h>

#define IN1_PIN 1
#define IN2_PIN 4
#define IN3_PIN 5
#define IN4_PIN 6

void init_dc_motor();
void go_backward();
void go_forward();
void go_left();
void go_right();
void stop_dc_motor();

int main() {

    if ( wiringPiSetup() == -1 )
        return 0;
    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    pinMode(IN3_PIN, OUTPUT);
    pinMode(IN4_PIN, OUTPUT);

    init_dc_motor();

    int counter = 0;
    go_forward();
    while (counter != 4)
    {
        delay(1000);
        printf("forward\n");
        counter++;
    }

    counter = 0;
    go_left();
    while (counter != 4)
    {
        delay(1000);
        printf("forward\n");
        counter++;
    }

    counter = 0;
    go_right();
    while (counter != 4)
    {
        delay(1000);
        printf("forward\n");
        counter++;
    }

    counter = 0;
    go_backward();
    while (counter != 4)
    {
        delay(1000);
        printf("forward\n");
        counter++;
    }

    stop_dc_motor();

    return 0;
}


void init_dc_motor()
{
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, HIGH);
    digitalWrite(IN3_PIN, HIGH);
    digitalWrite(IN4_PIN, HIGH);
}

void go_backward()
{
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    digitalWrite(IN3_PIN, LOW);
    digitalWrite(IN4_PIN, HIGH);
}

void go_forward()
{
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    digitalWrite(IN3_PIN, HIGH);
    digitalWrite(IN4_PIN, LOW);
}

void go_left()
{
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    digitalWrite(IN3_PIN, HIGH);
    digitalWrite(IN4_PIN, LOW);
}

void go_right()
{
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    digitalWrite(IN3_PIN, LOW);
    digitalWrite(IN4_PIN, HIGH);
}

void stop_dc_motor()
{
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);
    digitalWrite(IN3_PIN, LOW);
    digitalWrite(IN4_PIN, LOW);
}