// ga_engine.h
#ifndef GA_ENGINE_H
#define GA_ENGINE_H

#include "employee.h"
#include "shift_type.h"
#include "schedule_config.h"

// GA 실행 결과를 담을 구조체 (예시)
typedef struct
{
  Schedule best_schedule;
} GaResult;

typedef struct
{
  ShiftType schedule[35];
  // 28~31까지 모든 경우를 포함할 수 있으며 이후 개선사항으로 전달 3일근무를 참조하여 N근무자의 경우 OFF를 보장하는 기능을 추가하기 위함.

  double fitness;
} Schedule;

typedef double (*FitnessFunc)(Schedule *schedule, const ScheduleConfig *config);

GaResult run_genetic_algorithm(
    Employee *employees,
    int employee_count,
    const ScheduleConfig *config,
    FitnessFunc fitness_func);

#endif