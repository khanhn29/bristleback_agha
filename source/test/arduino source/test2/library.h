#ifndef LIBRARY_H
#define LIBRARY_H

#define MAX_RECIEVE_BUF 50

#define OFF_QUAT 1
#define ON_QUAT 0

#define RUN_5V 1
#define RUN_12V 0

#define STATUS_OFF 1
#define STATUS_ON 0

typedef enum{
   OFF_MODE,
   ON_MODE,
   STARTING_MODE
} connect_mode_t;

#define TURN_ON_LED 1
#define TURN_OFF_LED 0

#define TURN_ON_SENSOR 1
#define TURN_OFF_SENSOR 0

#define TRUE 1
#define FALSE 0

#define TIMEOUT_ON_MODE 80 //Thoi gian giua moi lan gui adc
#define TIMEOUT_STARTING_MODE 500 //Thoi gian bat quat sau khi nhan duoc tin hieu bat

#define QUAT 23
#define CONTROL_5V_12V 53 //Dieu khien relay su dung 5V hay 12V
#define GAS_SEN_PW_PIN A6
#define GAS_SEN_ADC_PIN A1
#define TEMP_SEN_ADC_PIN A0
#define LED_GREEN A9
#define LED_RED A8
#define ALARM 4

#define DOOR 10
#define IS_OPEN LOW
#define IS_CLOSE HIGH

#define INLET 8
#define IS_REMOVED LOW
#define IS_NOT_REMOVED HIGH

#define NOZZLE 9
#define IS_BLOCKED HIGH
#define IS_NOT_BLOCKED LOW

// #define DELAY_DIV_NUM 40
// #define DELAY_DIV_NUM_ALARM 20
// #define DELAY_SEND_ERR_DIV_NUM 100

typedef enum
{
   no_fault,
   nozzle_blocked,
   in_maintenance_mode,
   in_temperature_not_ready_mode,
   door_open_or_inlet_removed,
   serial_link_to_siu_fault,
   other_fault,
   led_check
}warning_mode_t;

typedef enum
{
   error_none,
   error_orange,
   error_red
}gas_alarm_mode_t;

#endif
