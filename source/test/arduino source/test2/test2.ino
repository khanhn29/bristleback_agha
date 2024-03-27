#include "library.h"

#define INTERVAL 100

void setup() {
  pinMode(DOOR, INPUT);
  pinMode(INLET, INPUT);
  pinMode(NOZZLE, INPUT);

  analogReference(DEFAULT);  //5V for Vref-pin of ADC

  pinMode(CONTROL_5V_12V, OUTPUT);
  pinMode(QUAT, OUTPUT);
  pinMode(GAS_SEN_PW_PIN, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(ALARM, OUTPUT);

  Serial.begin(9600);
  Serial.setTimeout(10);
  Serial.println("ready");

  digitalWrite(QUAT, ON_QUAT);
  digitalWrite(CONTROL_5V_12V, RUN_5V);
  digitalWrite(LED_GREEN, TURN_ON_LED);
  digitalWrite(LED_RED, TURN_ON_LED);
  delay(500);
  digitalWrite(QUAT, OFF_QUAT);
  digitalWrite(GAS_SEN_PW_PIN, TURN_OFF_SENSOR);
  digitalWrite(LED_GREEN, TURN_OFF_LED);
  digitalWrite(LED_RED, TURN_OFF_LED);
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  static unsigned long previousMillis = 0; // Stores the last time the LED was updated
  const long interval = INTERVAL; // Interval at which to execute
  unsigned long currentMillis = millis();
  warning_mode_t led_mode = no_fault;
  static gas_alarm_mode_t alarm_mode = error_none;
  static int temperature_threshold = 1000;
  static connect_mode_t connect_mode = OFF_MODE;

  if(currentMillis - previousMillis > interval || previousMillis > currentMillis)
  {
    // Serial.print("currentMillis");
    // Serial.println(currentMillis);
    // Serial.print("previousMillis");
    // Serial.println(previousMillis);
    if(Serial.available() > 0)
    {
      String command = Serial.readString();
      execute_command(command, &alarm_mode, &temperature_threshold, &connect_mode);
    }

    led_mode = calculate_status_led(temperature_threshold);
    perform(led_mode);
    controlAlarm(alarm_mode);
    runConnectMode(&connect_mode);

    previousMillis = currentMillis;
  }
}

void runConnectMode(connect_mode_t* cm)
{
  static unsigned int count = 0;
  const int starting_timeout = 3; //30 seconds -> 30 000 ms
  String prefix = "adc,";
  String suffix = ";";
  String to_send = "";
  int gas_value = 0;

  to_send.reserve(64);
  count++;
  count = count>(INTERVAL-1) ? 0:count;
  switch(*cm)
  {
    case ON_MODE:
      //Gui adc gas voi interval
      if(count % 7 == 0)
      {
        gas_value = analogRead(GAS_SEN_ADC_PIN);
        to_send = prefix + gas_value +suffix;
        Serial.println(to_send);
      }
      break;
    case STARTING_MODE:
      // Sau khi nhan duoc lenh CONNECT chay starting mode
      // Bat quat 5V, bat nguon sensor
      // Khi chay het thoi gian starting mode thi chuyen qua ON_MODE
      digitalWrite(QUAT, ON_QUAT);
      digitalWrite(CONTROL_5V_12V, RUN_5V);
      digitalWrite(GAS_SEN_PW_PIN, TURN_ON_SENSOR);
      if(count > starting_timeout*1000/INTERVAL)
        *cm = ON_MODE;
      break;
    default:
      //OFF MODE
      count = 0;
      turn_off_device();
      ;
  }
}

/// @brief Control led status and on/off gas, motor power
void perform(warning_mode_t mode)
{
  static unsigned int count = 0;
  count++;
  count = count>(INTERVAL-1) ? 0:count;

  switch(mode)
  {
    case door_open_or_inlet_removed:
      if(count % 5 == 0)
      {
      // Serial.print("count");
      // Serial.println(count);
      // Serial.print("digitalRead(INLET)");
      // Serial.println(digitalRead(INLET));
        digitalWrite(LED_RED, !digitalRead(LED_RED));
      }
      digitalWrite(LED_GREEN, TURN_OFF_LED);
      turn_off_device();
      break;
    case nozzle_blocked:
      if(count % 5 == 0)
      {
        digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
        digitalWrite(LED_RED, digitalRead(LED_GREEN));
      }
      turn_off_device();
      break;
    case in_maintenance_mode:
      if(count % 5 == 0)
      {
        digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
        digitalWrite(LED_RED, !digitalRead(LED_GREEN));
      }
      break;
    case in_temperature_not_ready_mode:
      if(count % 5 == 0)
      {
        digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
      }
      digitalWrite(LED_RED, TURN_OFF_LED);
      turn_off_device();
      break;
    case no_fault:
      digitalWrite(LED_GREEN, TURN_ON_LED);
      digitalWrite(LED_RED, TURN_OFF_LED);
      break;
    default:
      Serial.println("Unknown mode");
  }
}

void controlAlarm(gas_alarm_mode_t alarm_mode)
{
  static unsigned int count = 0;
  count++;
  count = count>(INTERVAL-1) ? 0:count;
  switch (alarm_mode)
  {
    case error_orange:
    {
      if(count % 10 == 0) // alarm interval 500ms
      {
        digitalWrite(ALARM, !digitalRead(ALARM));
      }
      break;
    }
    case error_red:
    {
      if(count % 5 == 0) // alarm interval 200ms
      {
        digitalWrite(ALARM, !digitalRead(ALARM));
      }
      break;
    }
    default:
    {
      digitalWrite(ALARM, 0); //turn off alarm when no fault
    }
  }
}

warning_mode_t calculate_status_led(int temperature_threshold)
{
  // Serial.print("digitalRead(INLET)");
  // Serial.println(digitalRead(INLET));
  warning_mode_t warning_led_mode = no_fault;
  static warning_mode_t last = no_fault;
  int g_value_adc_1 = analogRead(TEMP_SEN_ADC_PIN);
   if(digitalRead(DOOR) == IS_OPEN || digitalRead(INLET) == IS_REMOVED)
   {
      warning_led_mode = door_open_or_inlet_removed;
   }
   else if(digitalRead(NOZZLE) == IS_BLOCKED)
   {
      warning_led_mode = nozzle_blocked;
   }
   else if(g_value_adc_1 >= temperature_threshold)
   {
      // Serial.print("g_value_adc_1 = ");
      // Serial.println(g_value_adc_1);
      warning_led_mode = in_temperature_not_ready_mode;
   }
   else
   {
      warning_led_mode = no_fault;
   }

  if (last !=  warning_led_mode)
  {
    last = warning_led_mode;
    switch(warning_led_mode)
    {
      case no_fault:
        Serial.println("no_fault");
        break;
      case nozzle_blocked:
        Serial.println("nozzle_blocked");
        break;
      case in_maintenance_mode:
        Serial.println("in_maintenance_mode");
        break;
      case in_temperature_not_ready_mode:
        Serial.println("in_temperature_not_ready_mode");
        break;
      case door_open_or_inlet_removed:
        Serial.println("door_open_or_inlet_removed");
        break;
      case serial_link_to_siu_fault:
        Serial.println("serial_link_to_siu_fault");
        break;
      case other_fault:
        Serial.println("other_fault");
        break;
      case led_check:
        Serial.println("led_check");
        break;
      default:
        Serial.println("unknown led mode");
    }
  }
  
   return warning_led_mode;
}

void turn_off_device()
{
   digitalWrite(CONTROL_5V_12V, RUN_5V);
   digitalWrite(QUAT, OFF_QUAT);
   digitalWrite(GAS_SEN_PW_PIN, TURN_OFF_SENSOR);  
}

void execute_command(String command, gas_alarm_mode_t* alarm_mode, int* temperature_threshold, connect_mode_t* connect_mode) {
  // Convert the command to lowercase
  command.toLowerCase();
  
  // Split the command into two parts: the command itself and the value (if any)
  int spaceIndex = command.indexOf(',');
  // Extract the action substring
  String action, valueString;
  int value = 0; // Default value if no value provided
  
  if (spaceIndex >= 0) { // Check for greater than or equal to 0
    action = command.substring(0, spaceIndex);
    valueString = command.substring(spaceIndex + 1);
    // Convert the value string to an integer
    value = valueString.toInt();
  } else {
    action = command;
  }
  Serial.print("action is: ");
  Serial.println(action);
  // Convert the action to lowercase
  action.toLowerCase();

  // Perform actions based on the command
  if (action.indexOf("connect") >= 0) { // Check if "connect" is in action
    Serial.println("Connecting...");
    //Turn on motor in TIMEOUT_STARTING_MODE millisecond
    digitalWrite(QUAT, ON_QUAT);
    digitalWrite(CONTROL_5V_12V, RUN_12V);
    *connect_mode = STARTING_MODE;
  } else if (action.indexOf("off") >= 0) { // Check if "off" is in action
    //will turn off power of gas, sensors, motor
    Serial.println("Turning off...");
    *connect_mode = OFF_MODE;
  } else if (action.indexOf("threshold") >= 0) { // Check if "threshold" is in action
    //Set threshold of temperature
    Serial.print("Setting temperature threshold to ");
    Serial.println(value);
    *temperature_threshold = value;
  } else if (action.indexOf("gas") >= 0) { // Check if "gas" is in action
    //Set alarm mode based on gas's value
    //if gas > threshold then PC will receive the value, then PC send back command to control alarm mode
    Serial.print("Setting gas mode to ");
    Serial.println(value);
    *alarm_mode = value;
  } else {
    Serial.println("Invalid command");
  }
}
