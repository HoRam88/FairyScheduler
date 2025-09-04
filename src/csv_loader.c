// csv_loader.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "employee.h"
#include "csv_loader.h"

// 초기 employee 포인터 배열의 동적할당용 길이
#define INITIAL_EMPLOYEE_CAPACITY 4

// CSV 파일에서 데이터를 읽어 반환하는 함수
Employee *_load_employees_from_csv(int *employee_count)
{
  // 여기에 CSV 파일을 열고, 한 줄씩 읽어서
  // Employee 구조체 배열을 동적으로 생성하고 채우는 로직이 들어갑니다.

  printf("Loaded employees from CSV file.\n");
  // return employees;
  return NULL; // 임시
}

bool resize_up_Employee(Employee **employees_ptr, int *current_capacity_ptr)
{
  // 현재 용량이 0이면 초기 용량(4)으로 시작, 아니면 2배로 확장
  int new_capacity = (*current_capacity_ptr == 0) ? INITIAL_EMPLOYEE_CAPACITY : (*current_capacity_ptr * 2);

  // realloc의 첫 번째 인자는 void* 여야 합니다. *employees_ptr는 Employee* 타입입니다.
  Employee *new_ptr = (Employee *)realloc(*employees_ptr, sizeof(Employee) * new_capacity);

  if (new_ptr == NULL)
  {
    fprintf(stderr, "Error: Employee 배열 메모리 확장 실패 (resize_up).\n");
    // realloc 실패 시 *employees_ptr (원본 포인터)는 여전히 유효합니다.
    return false; // 실패 반환
  }

  *employees_ptr = new_ptr;             // 호출한 쪽의 원본 포인터 업데이트
  *current_capacity_ptr = new_capacity; // 호출한 쪽의 원본 용량 업데이트
  return true;                          // 성공 반환
}

bool resize_set_Employee(Employee **employees_ptr, int target_size)
{

  {
    // 목표 크기가 0이면 메모리 해제
    if (target_size == 0)
    {
      free(*employees_ptr);
      *employees_ptr = NULL;
      return true;
    }

    Employee *new_ptr = (Employee *)realloc(*employees_ptr, sizeof(Employee) * target_size);

    if (new_ptr == NULL)
    {
      fprintf(stderr, "Error: Employee 배열 메모리 재설정 실패 (resize_set).\n");
      // realloc 실패 시 *employees_ptr (원본 포인터)는 여전히 유효합니다.
      return false; // 실패
    }

    *employees_ptr = new_ptr; // 호출한 쪽의 원본 포인터 업데이트
    return true;              // 성공
  }
}

bool load_from_csv_Config_Employees(const char *file_path,
                                    RawCsvConfig *config_out, // 출력: ScheduleConfig 초기화를 위한 연, 월 등의 정보
                                    Employee **employees_out, // 출력: 직원 정보를 가지는 포인터 배열
                                    int *employee_count       // 출력: 직원의 수
)
{
  // 함수 최종 반환을 위한 성공/실패 변수
  bool is_success = false;

  // employees_out의 resize사용 후 실패 반환시 확인용 변수
  bool emp_resize_success = false;

  // config_out 메모리 할당 및 초기화.
  config_out->raw_year = 0;
  config_out->raw_month = 0;

  // employees 메모리 할당
  int emp_capacity = 0;
  *employee_count = 0;

  // employees_out 초기 메모리 할당
  emp_resize_success = resize_up_Employee(employees_out, &emp_capacity);
  printf("\nemp_capacity: %d\n", *&emp_capacity);

  if (!emp_resize_success)
  {
    // emp_resize_success == false일 때, 메모리 할당 실패로 종료.

    return false;
  }

  CsvParseState current_state = PARSE_STATE_INITIAL;
  struct tm data_info = {0};

  char line_buffer[256];
  FILE *fp;
  fp = fopen(file_path, "r");

  if (fp == NULL)
  {
    perror("Error opening file");
    // 파일을 읽지 못했을 경우 연, 월 0,0인 구조체 반환
    return false;
  }

  while (fgets(line_buffer, sizeof(line_buffer), fp) != NULL)
  {

    // 줄 끝의 개행 문자 및 캐리지 리턴 제거
    line_buffer[strcspn(line_buffer, "\n")] = 0;
    line_buffer[strcspn(line_buffer, "\r")] = 0;

    // 빈 줄이거나 주석 줄은 건너뜁니다.
    if (strlen(line_buffer) == 0 || line_buffer[0] == '#')
    {
      // 빈 줄이나 주석은 상태 변경 없이 건너뜁니다.
      continue;
    }

    // printf("line_buffer: %s\n", line_buffer);

    // --- 상태 전환 로직 (헤더를 기준으로) ---
    // 헤더를 먼저 확인하여 상태를 전환합니다.
    // strstr은 문자열 안에 특정 부분 문자열이 있는지 확인합니다.
    if (strstr(line_buffer, "Year, Month,") != NULL)
    {
      current_state = PARSE_STATE_YEAR_MONTH_HEADER;
      printf("상태 전환: PARSE_STATE_YEAR_MONTH_HEADER\n");
      continue; // 헤더 줄은 데이터가 아니므로 다음 줄로
    }
    else if (strstr(line_buffer, "EmpNo,Name,WorkDays") != NULL)
    {
      current_state = PARSE_STATE_EMPLOYEE_HEADER;
      printf("상태 전환: PARSE_STATE_EMPLOYEE_HEADER\n");
      continue; // 헤더 줄은 데이터가 아니므로 다음 줄로
    }

    // --- 현재 상태에 따른 데이터 처리 ---
    // switch 문을 사용하여 현재 상태에 따라 다른 파싱 로직을 수행합니다.
    switch (current_state)
    {
    case PARSE_STATE_YEAR_MONTH_HEADER:
      // "Year,Month" 헤더 다음 줄은 연/월 데이터
      // 여기에 sscanf를 이용한 연/월 파싱 로직 추가 (예: sscanf(line_buffer, "%d,%d,", &year, &month);)
      sscanf(line_buffer, "%d, %d", &config_out->raw_year, &config_out->raw_month);
      printf("  연/월 데이터 파싱: %s\n", line_buffer);
      current_state = PARSE_STATE_YEAR_MONTH_DATA; // 데이터 읽었으니 상태 변경
      break;
    case PARSE_STATE_EMPLOYEE_HEADER:
    case PARSE_STATE_EMPLOYEE_DATA: // 헤더 다음 줄부터는 계속 직원 데이터
      // printf("  직원 데이터 파싱: %s\n", line_buffer);

      // 만약 employees_out의 크기를 다 사용하고 있다면 크기를 2배로 늘린다.
      if (*employee_count == emp_capacity)
      {
        emp_resize_success = resize_up_Employee(employees_out, &emp_capacity);

        if (emp_resize_success == false)
        {
          // emp_resize_success == false일 때, 메모리 할당 실패로 종료.
          for (int i = 0; i < *employee_count; i++)
          {
            free(&employees_out[i]);
          }
          free(*employees_out);
          *employees_out = NULL;
          return false;
        }
      }

      // 파싱을 위한 임시 포인터
      printf("\nemployee_count: %d\n", *(employee_count));
      Employee *emp = &(*employees_out)[*employee_count];
      // strtok를 사용하여 파싱
      char *token = strtok(line_buffer, ",");

      // id 파싱
      if (token != NULL)
      {
        emp->id = atoi(token);
      }

      token = strtok(NULL, ",");

      // name 파싱
      if (token != NULL)
      {
        strncpy(emp->name, token, sizeof(emp->name) - 1);

        // 버퍼 오버플로우 예방용 emp->name[50]에 NULL 명시적 입력
        emp->name[sizeof(emp->name) - 1] = '\0';
      }
      token = strtok(NULL, ",\n\r"); // 개행 및 캐리지 리턴
      // contract_days 파싱
      if (token != NULL)
      {
        emp->contract_days = atoi(token);
      }

      *employee_count = *(employee_count) + 1;

      current_state = PARSE_STATE_EMPLOYEE_DATA; // 직원 데이터 상태 유지
      break;
    case PARSE_STATE_INITIAL:
    case PARSE_STATE_YEAR_MONTH_DATA: // 연/월 데이터 읽은 후의 상태
    case PARSE_STATE_BLANK_LINE:
      // 이 상태들에서는 특별히 파싱할 데이터가 없거나, 이미 처리된 상태
      // 또는 예상치 못한 데이터가 나타난 경우
      printf("경고: 예상치 못한 데이터 형식 또는 위치: %s (현재 상태: %d)\n", line_buffer, current_state);
      break;
    }
  }

  // 최종 종료 전 employee_count를 입력된 직원 수 만큼으로 맞추고, 포인터 크기도 그에 맞게 재조정
  employee_count--;
  emp_resize_success = resize_set_Employee(employees_out, *employee_count);
  is_success = true;

  return is_success;
}
