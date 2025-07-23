// include/month_config.h
#ifndef MONTH_CONFIG_H
#define MONTH_CONFIG_H

// ScheduleConfig 구조체 정의
typedef struct
{
  int year;
  int month;
  int num_days;
  int num_employees;
  int population_size;
  int max_generations;
} ScheduleConfig;

// 외부에 공개할 함수 선언
int get_days_in_month(int year, int month);
ScheduleConfig *init_schedule_config(int year, int month, int num_employees);

#endif