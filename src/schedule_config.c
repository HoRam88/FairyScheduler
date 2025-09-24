// src/month_config.c
#include <stdlib.h>
#include <stdbool.h>
#include "schedule_config.h"

static bool set_shiftrequirement(ShiftSetConfig *shiftconfig, int count, char *shift, int *max, int *min)
{

  ShiftRequirement *requirement = NULL;
  requirement = malloc(sizeof(ShiftRequirement) * count);

  if (requirement == NULL || shiftconfig == NULL)
  {
    free(shiftconfig->requirements);
    free(shiftconfig);

    return false;
  }

  shiftconfig->requirements = requirement;

  shiftconfig->count = count;

  for (int i = 0; i < count; i++)
  {
    requirement[i].code = shift[i];
    requirement[i].max_employees = max[i];
    requirement[i].min_employees = min[i];
  }

  return true;
}

static ShiftSetConfig *create_shift_requirements(int count, const char *shift, const int *max, const int *min)
{
  // 1. 껍데기 구조체(ShiftSetConfig)를 위한 메모리 할당
  ShiftSetConfig *config = malloc(sizeof(ShiftSetConfig));
  if (config == NULL)
  {
    return NULL;
  }
  // 2. 내부 배열(requirements)을 위한 메모리 할당
  config->requirements = malloc(sizeof(ShiftRequirement) * count);
  if (config->requirements == NULL)
  {
    free(config);
    return NULL;
  }
  // 3. 모든 메모리 할당이 성공했으면, 값들을 채워넣습니다.
  config->count = count;

  for (int i = 0; i < count; i++)
  {
    config->requirements[i].code = shift[i];
    config->requirements[i].max_employees = max[i];
    config->requirements[i].min_employees = min[i];
  }
  // 4. 모든 값이 채워진, 완성된 객체의 포인터를 반환합니다.
  return config;
}

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

  config->shift_requirements = NULL;

  config->shift_requirements->requirements = NULL;

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

// 생성(create) 함수와 짝을 이루는 해제(free) 함수 일부 동적 메모리를 사용하는 변수에 대한 메모리 해제
void free_schedule_config(ScheduleConfig *config)
{
  if (config != NULL)
  {
    // shift_requirements 내부의 배열을 먼저 해제해야 함
    if (config->shift_requirements != NULL)
    {
      free(config->shift_requirements->requirements);
      free(config->shift_requirements);
    }

    free(config); // 마지막으로 껍데기 해제
  }
}