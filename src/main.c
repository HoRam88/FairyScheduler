// main.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "employee.h"
#include "shift_type.h"
#include "schedule_config.h"
#include "ga_engine.h"

// 함수 선언
Employee *load_employees(int *employee_count);

int main()
{

  int year_from_csv = 2025;
  int month_from_csv = 8;
  int employees_form_csv;

  Employee *employee_list = load_employees(&employees_form_csv);

  printf("\nCount: %d\n", employees_form_csv);

  srand(time(NULL));

  // ScheduleConfig 초기화는 이후 csv에서 읽어오는 형태로 변경해야 함.
  ScheduleConfig *scheduleconfig = init_schedule_config(year_from_csv, month_from_csv, employees_form_csv);

  GaResult result = run_genetic_algorithm(employee_list, scheduleconfig->num_employees, scheduleconfig);

  // 프로그램 종료 전 메모리 해제
  free(scheduleconfig);
  free(employee_list);

  return 0;
}