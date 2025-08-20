// include/month_config.h
#ifndef MONTH_CONFIG_H
#define MONTH_CONFIG_H

// ScheduleConfig 구조체 정의
typedef struct
{
  int year;            // 연도
  int month;           // 월
  int num_days;        // 해당 월 말일
  int num_employees;   // 근무자 수
  int population_size; // 유전자 개체 수
  int max_generations; // 최대 반복 세대
} ScheduleConfig;

// 외부에 공개할 함수 선언
int get_days_in_month(int year, int month);
ScheduleConfig *init_schedule_config(int year, int month, int num_employees);

#endif