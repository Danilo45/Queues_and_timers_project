/*
 * task_handler.c
 *
 *  Created on: Apr 30, 2025
 *      Author: danilo
 */

#include "main.h"

void process_command(command_t *cmd);
void extract_command(command_t *cmd);

state_t curr_state = sMainMenu;

//MENU TASK
void menu_task(void* parameters)
{

  while(1)
  {

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

void extract_command(command_t *cmd){
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
  while(1)
  {

  }

}

//RTC TASK
void rtc_task(void* parameters)
{
  while(1)
  {

  }

}
