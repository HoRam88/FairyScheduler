// ga_engine.h
#ifndef GA_ENGINE_H
#define GA_ENGINE_H

#include <stdbool.h>
#include "employee.h"
#include "shift_type.h"
#include "schedule_config.h"

// Schedule 구조체 정의를 앞으로 이동
typedef struct
{
  ShiftType *schedule; // 근무자 수 * 당월 말일

  double fitness;
} Schedule;

// GA 실행 결과를 담을 구조체 (예시)
typedef struct
{
  Schedule best_schedule;
} GaResult;

typedef double (*FitnessFunc)(Schedule *schedule, const ScheduleConfig *config);

GaResult run_genetic_algorithm(
    Employee *employees,
    int employee_count,
    const ScheduleConfig *config);

bool emp_stats_get(const ShiftType *result_schedule, const ScheduleConfig *config, EmployeeStats *emp_stats);

#endif