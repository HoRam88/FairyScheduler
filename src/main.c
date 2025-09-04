// main.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "employee.h"
#include "shift_type.h"
#include "schedule_config.h"
#include "ga_engine.h"
#include "csv_loader.h"

Employee *load_employees(int *employee_count);

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    fprintf(stderr, "사용법: %s <CSV_파일_경로>\n", argv[0]);

    return EXIT_FAILURE;
  }

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

  for (int i = 0; i < loaded_employee_count; i++)
  {
    printf("\nEmpNo: %d\nName: %s\nWorkDays: %d\n", (loaded_employees + (i))->id, (loaded_employees + (i))->name, (loaded_employees + (i))->contract_days);
  }

  printf("\nloaded_csv_year: %d\nloaded_csv_month: %d\n\n", loaded_raw_config.raw_year, loaded_raw_config.raw_month);

  return 0;

  int year_from_csv = 2025;
  int month_from_csv = 8;
  int employees_form_csv;

  // 요일을 계산하기 위한 구조체 및 변수 초기화
  struct tm date_info = {0};
  date_info.tm_year = year_from_csv - 1900;
  date_info.tm_mon = month_from_csv - 1;

  // 1일
  date_info.tm_mday = 1;

  int day_of_week_index = date_info.tm_wday;

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