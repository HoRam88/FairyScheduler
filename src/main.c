// main.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "employee.h"
#include "shift_type.h"
#include "schedule_config.h"
#include "ga_engine.h"
#include "csv_io.h"
// #include "csv_loader.h"

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    fprintf(stderr, "사용법: %s <CSV_파일_경로>\n", argv[0]);

    return EXIT_FAILURE;
  }

  srand(time(NULL));
  const char *csv_file_path = argv[1];

  // --- 2. CSV 파일에서 데이터 로드 ---
  RawCsvConfig loaded_raw_config;    // CSV에서 읽어올 연, 월 등을 저장
  Employee *loaded_employees = NULL; // CSV에서 읽어올 직원 데이터
  int loaded_employee_count = 0;     // CSV에서 읽어올 직원 수

  // load_from_csv_Config_Employees 함수 호출
  // 이 함수는 loaded_raw_config, loaded_employees, loaded_employee_count를 채워줍니다.
  if (!load_from_csv_Config_Employees(csv_file_path, &loaded_raw_config, &loaded_employees, &loaded_employee_count))
  {
    fprintf(stderr, "CSV 데이터 로드 실패. 프로그램을 종료합니다.\n");
    // load_from_csv_Config_Employees 내부에서 메모리 해제 처리됨
    return EXIT_FAILURE;
  }

  int year_from_csv = loaded_raw_config.raw_year;
  int month_from_csv = loaded_raw_config.raw_month;

  // 요일을 계산하기 위한 구조체 및 변수 초기화
  struct tm date_info = {0};
  date_info.tm_year = year_from_csv - 1900;
  date_info.tm_mon = month_from_csv - 1;

  // 1일
  date_info.tm_mday = 1;

  int day_of_week_index = date_info.tm_wday;

  printf("\nCount: %d\n", loaded_employee_count);

  // ScheduleConfig 초기화는 이후 csv에서 읽어오는 형태로 변경해야 함.
  ScheduleConfig *scheduleconfig = init_schedule_config(year_from_csv, month_from_csv, loaded_employee_count);
  printf("\nScheduleConfig Set\n");

  GaResult result = run_genetic_algorithm(loaded_employees, scheduleconfig->num_employees, scheduleconfig);
  printf("\nresult Done\n");

  EmployeeStats *emp_stats;
  emp_stats = malloc(sizeof(EmployeeStats) * scheduleconfig->num_employees);

  bool emp_success = false;
  emp_success = emp_stats_get(result.best_schedule.schedule, scheduleconfig, emp_stats);

  if (emp_success == false)
  {
    printf("\n!!! emp_state_get_FAIL!!!\n");
  }

  // for (int i = 0; i < scheduleconfig->num_employees; i++)
  // {
  //   printf("\nEmpNo: %d\n", i);
  //   printf("\nemp_stats[%d]->total_work_days: %d\n", i, emp_stats[i].total_work_days);

  //   for (int j = 0; j < 4; j++)
  //   {
  //     printf("[%d]: %d\t", j, emp_stats[i].shift_counts[j]);
  //   }
  // }

  char *output_path = generate_output_path(csv_file_path);

  save_schedule_to_csv(output_path, loaded_employees, emp_stats, scheduleconfig, result);

  // 프로그램 종료 전 메모리 해제

  free_schedule_config(scheduleconfig);
  free(scheduleconfig);
  free(loaded_employees);

  return 0;
}