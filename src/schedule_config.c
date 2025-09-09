// src/month_config.c
#include <stdlib.h>
#include "schedule_config.h"

// 윤년인지 확인하는 내부 함수 (static)
static int is_leap_year(int year)
{
  // 4의 배수이면 윤년
  return (year % 4 == 0);
}

// 해당 월의 마지막 날을 반환하는 함수 (외부 공개)
int get_days_in_month(int year, int month)
{
  switch (month)
  {
  case 4:  // 4월
  case 6:  // 6월
  case 9:  // 9월
  case 11: // 11월
    return 30;
  case 2: // 2월
    return is_leap_year(year) ? 29 : 28;
  default: // 나머지 1, 3, 5, 7, 8, 10, 12월
    return 31;
  }
}

// ScheduleConfig 구조체를 초기화 하고, 그 포인터를 반환하는 함수
ScheduleConfig *init_schedule_config(int year, int month, int num_employees)
{

  // 1. 구조체를 위한 메모리 할당
  ScheduleConfig *config = (ScheduleConfig *)malloc(sizeof(ScheduleConfig));
  if (config == NULL)
  {
    // 에러 처리
    return NULL;
  }

  config->mutation_rate = 0.08;

  // 2. 전달받은 값과 계산된 값으로 구조체 초기화
  config->year = year;
  config->month = month;
  config->num_employees = num_employees;
  config->num_days = get_days_in_month(year, month);
  config->gene_length = config->num_days * config->num_employees;

  // 3. 나중에 CSV에서 읽어올 값들은 고정값으로 설정.
  config->population_size = num_employees * 200;
  config->max_generations = 1000;
  config->shift_type_count = 4;

  if (config->population_size < 50)
  {
    config->elite_count = 1;
  }
  else
  {
    config->elite_count = (int)(config->population_size * 0.02);
  }

  // 초기 적합도 점수 1백만으로 설정
  config->fitness_init_score = 1000000.0;

  // 각 근무형태에 대한 패널티 가중치
  config->penalty_wight[0] = 800.0;
  config->penalty_wight[1] = 800.0;
  config->penalty_wight[2] = 1000.0;
  config->penalty_wight[3] = 0.0;

  config->penalty_wight_total_days = 1500;
  config->penalty_wight_night_off_off = 2500;
  config->positive_wight_night_night_off_off = 3500;
  return config;
}