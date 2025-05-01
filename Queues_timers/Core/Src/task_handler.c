/*
 * task_handler.c
 *
 *  Created on: Apr 30, 2025
 *      Author: danilo
 */

#include "main.h"

void process_command(command_t *cmd);
int extract_command(command_t *cmd);

state_t curr_state = sMainMenu;

const char* msg_invalid = "\nInvalid option!!!\n";

//MENU TASK
void menu_task(void* parameters)
{
  uint32_t cmd_addr;

  command_t *cmd;

  int option;

  const char* msg_menu = "\n========================\n"
                         "|         Menu         |\n"
                         "========================\n"
                         "LED effect    ----> 0\n"
                         "Date and time ----> 1\n"
                         "Exit          ----> 2\n"
                         "Enter your choice here : ";

  while(1)
  {
    xQueueSend(queue_print, &msg_menu, portMAX_DELAY);

    xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);

    cmd = (command_t*)cmd_addr;

    if(cmd->len == 1){
      option = cmd->payload[0] - 48; //ascii to number
      switch(option){
      case 0:
        curr_state = sLedEffect;
        xTaskNotify(handle_led_task, 0, eNoAction);
        break;
      case 1:
        curr_state = sRTCMenu;
        xTaskNotify(handle_rtc_task, 0, eNoAction);
      case 2:
        break;
      default:
        xQueueSend(queue_print, &msg_invalid, portMAX_DELAY);
        continue;
      }
    }else{
      //invalid entry;
      xQueueSend(queue_print, &msg_invalid, portMAX_DELAY);

    }
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
  }
}

//COMMAND TASK
void cmd_task(void* parameters){
  BaseType_t status;
  command_t cmd;
  while(1)
  {
    status = xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
    if(status == pdTRUE){
      process_command(&cmd);
    }

  }

}

void process_command(command_t *cmd){
  extract_command(cmd);

  switch(curr_state){
          case sMainMenu:
            xTaskNotify(handle_menu_task, (uint32_t)cmd, eSetValueWithOverwrite);
          break;
          case sLedEffect:
            xTaskNotify(handle_led_task, (uint32_t)cmd, eSetValueWithOverwrite);
          break;
          case sRTCMenu:
          case sRTCTimeConfig:
          case sRTCDateConfig:
          case sRTCReport:
            xTaskNotify(handle_rtc_task, (uint32_t)cmd, eSetValueWithOverwrite);
          break;

  }

}

int extract_command(command_t *cmd){
  uint8_t item;
  BaseType_t status;

  status = uxQueueMessagesWaiting(queue_data);
  if(!status){
    return -1;
  }
  uint8_t i = 0;
  do{
    status = xQueueReceive(queue_data, &item,0);
    if(status == pdTRUE){
      cmd->payload[i++] = item;
    }
  }while(item!='\n');

  cmd->payload[i-1] = '\0';
  cmd->len = i-1;
  return 0;
}

//PRINT TASK
void print_task(void* parameters)
{
  while(1)
  {

  }

}

//LED TASK
void led_task(void* parameters)
{
  uint32_t cmd_addr;
  command_t *cmd;
  const char* msg_led = "========================\n"
              "|      LED Effect     |\n"
              "========================\n"
              "(none,e1,e2,e3,e4)\n"
              "Enter your choice here : ";

  while(1){
    /*Wait for notification (Notify wait) */
    xTaskNotifyWait(0,0, NULL, portMAX_DELAY);
    /*Print LED menu */
    xQueueSend(queue_print, &msg_led, portMAX_DELAY);
    /*wait for LED command (Notify wait) */
    xTaskNotifyWait(0,0, &cmd_addr, portMAX_DELAY);
    cmd = (command_t*)cmd_addr;

    if(cmd->len <= 4)
    {
      if(! strcmp((char*)cmd->payload,"none")){
        led_effect_stop();
      }

      else if (! strcmp((char*)cmd->payload,"e1")){
        led_effect(1);
      }
      else if (! strcmp((char*)cmd->payload,"e2")){
        led_effect(2);
      }
      else if (! strcmp((char*)cmd->payload,"e3"))
        led_effect(3);
      else if (! strcmp((char*)cmd->payload,"e4")){
        led_effect(4);
      }

      else{
        /*print invalid message */
        xQueueSend(queue_print, &msg_invalid, portMAX_DELAY);
      }

    }else{
      /*print invalid message */
      xQueueSend(queue_print, &msg_invalid, portMAX_DELAY);

    /*update state variable */
    curr_state = sMainMenu;

    /*Notify menu task */
    xTaskNotify(handle_menu_task,0,eNoAction);

  }
  }

}

//RTC TASK
void rtc_task(void* parameters)
{
  while(1)
  {

  }

}
