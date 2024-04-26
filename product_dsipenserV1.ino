#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//PINS
#define prog_pin 30
#define buzz_pin 31
#define coin_acpt_pin 52
byte led_pins[2] = { 44, 42 };
byte pump_btn_pins[2] = { 45, 43 };
LiquidCrystal_I2C lcd(0x27, 20, 4);
//MOVE TO EPROMM
String company = "ECOFRESH";
unsigned int pump_times[2] = { 6000, 3000 };
byte pump_amounts[2] = { 10, 5 };
String pump_names[2] = { "MAS COLOR", "FABULOSO" };
//VARS
boolean prog = false;
byte main_step = 1;
byte credit = 0;
float liters = 0;
byte pump_slct = 0;
boolean prog_edit = false;
byte prog_step;
float pump_time_sec = 0;
//CUSTOM ARRAYS
const String letrs[38] = { " ", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "X", "Y", "Z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ".", "-" };
byte char_heart[8] = { 0b00000, 0b01010, 0b11111, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000 };
//TEMPS
unsigned int temp_pump_times[2];
byte temp_pump_amounts[2];
String temp_pump_names[2];
String temp_company;
byte temp_pump_slct = 0;
float temp_pump_time_sec = 0;
unsigned int temp_pump_time_old = 0;
byte temp_pump_amount_old = 0;
String letr;
byte letr_pointer;
String temp_pump_name_old = "";
byte dsp_pump_name_counter;
String temp_company_old;
byte dsp_company_counter;

void setLeds(boolean value) {
  for (byte i = 0; i < sizeof(led_pins); i++) {
    digitalWrite(led_pins[i], value);
  }
}

void blink(int milis, byte pulses, byte pin) {
  boolean dsp = true;
  setLeds(LOW);

  for (byte i = 0; i < pulses; i++) {
    if (dsp) {
      lcd.display();
      digitalWrite(pin, HIGH);
      digitalWrite(buzz_pin, HIGH);
    } else {
      lcd.noDisplay();
      digitalWrite(pin, LOW);
      digitalWrite(buzz_pin, LOW);
    }

    delay(milis / pulses);
    dsp = !dsp;
  }

  digitalWrite(buzz_pin, LOW);
  digitalWrite(pin, HIGH);
  lcd.display();
}

void progDefVals() {
  pump_slct = 0;
  prog_edit = false;
  setLeds(LOW);
  lcd.clear();
}

String lcdCenterStr(String str) {
  byte adjust_center = (20 - str.length()) / 2;
  String str_temp = "";

  for (byte i = 0; i < adjust_center; i++)
    str_temp += " ";

  return str_temp + str;
}

void buzzHandle(int millis, byte pulses) {
  for (byte i = 0; i < pulses; i++) {
    digitalWrite(buzz_pin, HIGH);
    delay(millis / 2);
    digitalWrite(buzz_pin, LOW);
    delay(millis / 2);
  }
}

void setup() {
  //Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, char_heart);
  pinMode(prog_pin, INPUT_PULLUP);
  pinMode(buzz_pin, OUTPUT);
  pinMode(coin_acpt_pin, INPUT_PULLUP);
  for (byte i = 0; i < sizeof(led_pins); i++)
    pinMode(led_pins[i], OUTPUT);
  for (byte i = 0; i < sizeof(pump_btn_pins); i++)
    pinMode(pump_btn_pins[i], INPUT_PULLUP);

  setLeds(HIGH);
  buzzHandle(100, 4);
}

void loop() {
  if (!prog) {
    switch (main_step) {
      case 1:
        if (credit == 0) {
          lcd.setCursor(0, 0);
          lcd.print("    Bienvenido a");
          lcd.setCursor(0, 1);
          lcd.print(lcdCenterStr(company));
          lcd.setCursor(0, 3);
          lcd.print("  INGRESE  CREDITOS");

          if (digitalRead(prog_pin) == LOW) {
            while (digitalRead(prog_pin) == LOW) {}
            progDefVals();

            for (byte i = 0; i < sizeof(temp_pump_times); i++) {
              temp_pump_times[i] = pump_times[i];
            }

            buzzHandle(100, 1);

            prog_step = 1;
            prog = true;
          }
        } else {
          if (credit >= 1) {
            if (digitalRead(pump_btn_pins[0]) == LOW && credit >= pump_amounts[0]) {
              while (digitalRead(pump_btn_pins[0]) == LOW) {}
              pump_slct = 1;
              main_step++;
            }

            if (digitalRead(pump_btn_pins[1]) == LOW && credit >= pump_amounts[1]) {
              while (digitalRead(pump_btn_pins[1]) == LOW) {}
              pump_slct = 2;
              main_step++;
            }
          }

          //ADD PENDING BOMBS
        }

        //IMPLEMENT COIN ACCEPTOR
        if (digitalRead(coin_acpt_pin) == LOW) {
          while (digitalRead(coin_acpt_pin) == LOW) {}
          credit += 6;

          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("      CREDITOS");
          lcd.setCursor(0, 2);
          lcd.print(lcdCenterStr("$" + (String)credit));
        }

        break;
      case 2:
        liters = credit / (float)pump_amounts[pump_slct - 1];

        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("   COLOQUE ENVASE");
        lcd.setCursor(0, 2);
        lcd.print(lcdCenterStr((String)liters + " L"));
        blink(4000, 17, led_pins[pump_slct - 1]);

        main_step++;
        break;
      case 3:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(lcdCenterStr(pump_names[pump_slct - 1]));
        lcd.setCursor(0, 1);
        lcd.print(lcdCenterStr("SURTIENDO " + (String)liters + " L"));
        lcd.setCursor(0, 3);
        lcd.print("Espere un momento...");

        //PENDING ACTIVATE BOMB
        delay(pump_times[pump_slct - 1] * liters);
        //PENDING DESACTIVATE BOMB

        main_step++;
        break;
      case 4:
        setLeds(LOW);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("       ");
        for (byte i = 0; i < 6; i++)
          lcd.write(0);
        lcd.setCursor(0, 2);
        lcd.print("GRACIAS POR COMPRAR!");
        lcd.setCursor(0, 3);
        lcd.print("   vuelva  pronto");
        buzzHandle(200, 3);
        delay(2800);

        main_step = 1;
        credit = 0;
        liters = 0;
        pump_slct = 0;
        setLeds(HIGH);
        lcd.clear();
        break;
    }
  } else {
    switch (prog_step) {
      case 1:
        if (pump_slct == 0) {
          lcd.setCursor(0, 0);
          lcd.print("------CALIBRAR------");
          lcd.setCursor(0, 3);
          lcd.print("Seleccionar producto");
        } else {
          lcd.setCursor(0, 0);
          lcd.print("------CALIBRAR------");
          lcd.setCursor(0, 1);
          lcd.print("Producto ");
          lcd.print(pump_slct);
          lcd.setCursor(0, 2);
          lcd.print(pump_time_sec);
          lcd.print("s");

          if (temp_pump_times[pump_slct - 1] != pump_times[pump_slct - 1]) {
            lcd.setCursor(0, 3);
            lcd.print(temp_pump_time_sec);
            lcd.print("s");
          }
        }

        if (digitalRead(pump_btn_pins[0]) == LOW || digitalRead(pump_btn_pins[1]) == LOW) {  //ADD PENDING BOMBS
          if (!prog_edit) {
            if (digitalRead(pump_btn_pins[0]) == LOW) {
              temp_pump_slct = 1;
            } else if (digitalRead(pump_btn_pins[1]) == LOW) {
              temp_pump_slct = 2;
            }
            //ADD PENDING BOMBS

            while (digitalRead(pump_btn_pins[temp_pump_slct - 1]) == LOW) {}

            if (temp_pump_slct == pump_slct) {
              prog_edit = true;
              temp_pump_time_old = temp_pump_times[pump_slct - 1];
              temp_pump_times[pump_slct - 1] = 0;
            }

            pump_slct = temp_pump_slct;
            pump_time_sec = (float)pump_times[pump_slct - 1] / 1000;
            temp_pump_slct = 0;
            temp_pump_time_sec = (float)temp_pump_times[pump_slct - 1] / 1000;

            setLeds(LOW);
            digitalWrite(led_pins[pump_slct - 1], HIGH);
            lcd.clear();
          } else {
            if (digitalRead(pump_btn_pins[pump_slct - 1]) == LOW) {
              unsigned long current_time = millis();
              unsigned long pressed_time = 0;

              while (digitalRead(pump_btn_pins[pump_slct - 1]) == LOW) {
                lcd.setCursor(0, 3);
                lcd.print("Surtiendo...");
                pressed_time = millis();
                //PENDING ACTIVATE BOMB
              }
              //PENDING DESACTIVATE BOMB

              temp_pump_times[pump_slct - 1] = pressed_time - current_time;
              temp_pump_time_sec = (float)temp_pump_times[pump_slct - 1] / 1000;

              if (temp_pump_time_sec > 60) {
                temp_pump_times[pump_slct - 1] = 0;
                temp_pump_time_sec = 0;
              }

              lcd.setCursor(0, 3);
              lcd.print(temp_pump_time_sec);
              lcd.print("s       ");
            } else {
              prog_edit = false;
              temp_pump_times[pump_slct - 1] = temp_pump_times[pump_slct - 1] == 0 ? temp_pump_time_old : temp_pump_times[pump_slct - 1];
            }
          }
        }

        if (digitalRead(prog_pin) == LOW) {
          while (digitalRead(prog_pin) == LOW) {}
          progDefVals();

          for (byte i = 0; i < sizeof(temp_pump_amounts); i++) {
            temp_pump_amounts[i] = pump_amounts[i];
          }

          buzzHandle(100, 1);
          prog_step++;
        }
        break;
      case 2:
        if (pump_slct == 0) {
          lcd.setCursor(0, 0);
          lcd.print("-------PRECIO-------");
          lcd.setCursor(0, 3);
          lcd.print("Seleccionar producto");
        } else {
          lcd.setCursor(0, 0);
          lcd.print("-------PRECIO-------");
          lcd.setCursor(0, 1);
          lcd.print("Producto ");
          lcd.print(pump_slct);
          lcd.setCursor(0, 2);
          lcd.print("$");
          lcd.print(pump_amounts[pump_slct - 1]);

          if (temp_pump_amounts[pump_slct - 1] != pump_amounts[pump_slct - 1]) {
            lcd.setCursor(0, 3);
            lcd.print("$");
            lcd.print(temp_pump_amounts[pump_slct - 1]);
          }
        }

        if (digitalRead(pump_btn_pins[0]) == LOW || digitalRead(pump_btn_pins[1]) == LOW) {  //ADD PENDING BOMBS
          if (!prog_edit) {
            if (digitalRead(pump_btn_pins[0]) == LOW) {
              temp_pump_slct = 1;
            } else if (digitalRead(pump_btn_pins[1]) == LOW) {
              temp_pump_slct = 2;
            }
            //ADD PENDING BOMBS

            while (digitalRead(pump_btn_pins[temp_pump_slct - 1]) == LOW) {}

            if (temp_pump_slct == pump_slct) {
              prog_edit = true;
              temp_pump_amount_old = temp_pump_amounts[pump_slct - 1];
              temp_pump_amounts[pump_slct - 1] = 0;
            }

            pump_slct = temp_pump_slct;
            temp_pump_slct = 0;

            setLeds(LOW);
            digitalWrite(led_pins[pump_slct - 1], HIGH);
            lcd.clear();
          } else {
            if (digitalRead(pump_btn_pins[pump_slct - 1]) == LOW) {
              temp_pump_amounts[pump_slct - 1]++;
              delay(200);

              if (temp_pump_amounts[pump_slct - 1] > 99) {
                temp_pump_amounts[pump_slct - 1] = 0;
              }

              lcd.setCursor(1, 3);
              lcd.print(temp_pump_amounts[pump_slct - 1]);
              lcd.print("             ");
            } else {
              prog_edit = false;
              temp_pump_amounts[pump_slct - 1] = temp_pump_amounts[pump_slct - 1] == 0 ? temp_pump_amount_old : temp_pump_amounts[pump_slct - 1];
            }
          }
        }

        if (digitalRead(prog_pin) == LOW) {
          while (digitalRead(prog_pin) == LOW) {}
          progDefVals();

          temp_pump_names[0] = pump_names[0];
          temp_pump_names[1] = pump_names[1];
          //ADD PENDING BOMBS

          buzzHandle(100, 1);
          prog_step++;
        }
        break;
      case 3:
        if (pump_slct == 0) {
          lcd.setCursor(0, 0);
          lcd.print("-------NOMBRE-------");
          lcd.setCursor(0, 3);
          lcd.print("Seleccionar producto");
        } else {
          lcd.setCursor(0, 0);
          lcd.print("-------NOMBRE-------");
          lcd.setCursor(0, 1);
          lcd.print("Producto ");
          lcd.print(pump_slct);
          lcd.setCursor(0, 2);
          lcd.print(pump_names[pump_slct - 1]);

          if (!prog_edit && temp_pump_names[pump_slct - 1] != pump_names[pump_slct - 1]) {
            lcd.setCursor(0, 3);
            lcd.print(temp_pump_names[pump_slct - 1]);
          }

          if (prog_edit) {
            lcd.setCursor(0, 3);
            lcd.print(temp_pump_names[pump_slct - 1]);
            lcd.print((millis() / 500) % 2 ? letr : "_");
          }
        }

        if (digitalRead(pump_btn_pins[0]) == LOW || digitalRead(pump_btn_pins[1]) == LOW) {  //ADD PENDING BOMBS
          if (!prog_edit) {
            if (digitalRead(pump_btn_pins[0]) == LOW) {
              temp_pump_slct = 1;
            } else if (digitalRead(pump_btn_pins[1]) == LOW) {
              temp_pump_slct = 2;
            }
            //ADD PENDING BOMBS

            while (digitalRead(pump_btn_pins[temp_pump_slct - 1]) == LOW) {}

            if (temp_pump_slct == pump_slct) {
              prog_edit = true;
              temp_pump_name_old = temp_pump_names[pump_slct - 1];
              temp_pump_names[pump_slct - 1] = "";

              letr_pointer = 0;
              letr = "";
              dsp_pump_name_counter = 0;
            }

            pump_slct = temp_pump_slct;
            temp_pump_slct = 0;

            setLeds(LOW);
            digitalWrite(led_pins[pump_slct - 1], HIGH);
            lcd.clear();
          } else {
            if (digitalRead(pump_btn_pins[pump_slct - 1]) == LOW) {
              delay(200);
              letr_pointer++;

              letr = letrs[letr_pointer];

              if (letr_pointer == ((sizeof(letrs) / sizeof(letrs[0])) - 1)) {
                letr_pointer = 0;
              }
            } else {
              if (dsp_pump_name_counter < 20) {
                delay(200);

                letr = letrs[letr_pointer];
                temp_pump_names[pump_slct - 1] += letr;

                letr_pointer = 0;
                letr = letrs[letr_pointer];
                dsp_pump_name_counter++;
              } else {
                prog_edit = false;
                temp_pump_names[pump_slct - 1].trim();
                temp_pump_names[pump_slct - 1] = temp_pump_names[pump_slct - 1] == "" ? temp_pump_name_old : temp_pump_names[pump_slct - 1];
              }
            }
          }
        }

        if (digitalRead(prog_pin) == LOW) {
          while (digitalRead(prog_pin) == LOW) {}
          progDefVals();
          temp_company = company;

          buzzHandle(100, 1);
          prog_step++;
        }
        break;
      case 4:
        lcd.setCursor(0, 0);
        lcd.print("---NOMBRE EMPRESA---");
        lcd.setCursor(0, 2);
        lcd.print(company);

        if (!prog_edit && temp_company != company) {
          lcd.setCursor(0, 3);
          lcd.print(temp_company);
        }

        if (prog_edit) {
          lcd.setCursor(0, 3);
          lcd.print(temp_company);
          lcd.print((millis() / 500) % 2 ? letr : "_");
        }

        if (digitalRead(pump_btn_pins[0]) == LOW || digitalRead(pump_btn_pins[1]) == LOW) {
          if (!prog_edit) {
            if (digitalRead(pump_btn_pins[0]) == LOW) {
              pump_slct = 1;
            } else if (digitalRead(pump_btn_pins[1]) == LOW) {
              pump_slct = 2;
            }

            while (digitalRead(pump_btn_pins[pump_slct - 1]) == LOW) {}

            if (pump_slct == 1) {
              prog_edit = true;
              temp_company_old = temp_company;
              temp_company = "";

              letr_pointer = 0;
              letr = "";
              dsp_company_counter = 0;
              digitalWrite(led_pins[0], HIGH);
              lcd.clear();
            }
          } else {
            if (digitalRead(pump_btn_pins[0]) == LOW) {
              delay(200);
              letr_pointer++;

              letr = letrs[letr_pointer];

              if (letr_pointer == ((sizeof(letrs) / sizeof(letrs[0])) - 1)) {
                letr_pointer = 0;
              }
            } else {
              if (dsp_company_counter < 20) {
                delay(200);

                letr = letrs[letr_pointer];
                temp_company += letr;

                letr_pointer = 0;
                letr = letrs[letr_pointer];
                dsp_company_counter++;
              } else {
                temp_company.trim();
                temp_company = temp_company == "" ? temp_company_old : temp_company;
                prog_edit = false;
                setLeds(LOW);
                lcd.clear();
              }
            }
          }
        }

        // if (digitalRead(prog_pin) == LOW) {
        //   while (digitalRead(prog_pin) == LOW) {}
        //   progDefVals();
        //   temp_company = company;

        //   buzzHandle(100, 1);
        //   prog_step++;
        // }
        break;
    }
  }
}