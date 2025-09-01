// csv_loader.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "employee.h"
#include "csv_loader.h"

// CSV 파일에서 데이터를 읽어 반환하는 함수
Employee *_load_employees_from_csv(int *employee_count)
{
  // 여기에 CSV 파일을 열고, 한 줄씩 읽어서
  // Employee 구조체 배열을 동적으로 생성하고 채우는 로직이 들어갑니다.

  printf("Loaded employees from CSV file.\n");
  // return employees;
  return NULL; // 임시
}

RawCsvConfig load_ga_data_from_csv(const char *file_path)
{
  RawCsvConfig csv_config;
  csv_config.raw_year = 0;
  csv_config.raw_month = 0;

  char line_buffer[256];
  CsvParseState current_state = PARSE_STATE_INITIAL;
  struct tm data_info = {0};

  FILE *fp;
  fp = fopen(file_path, "r");

  if (fp == NULL)
  {
    perror("Error opening file");
    // 파일을 읽지 못했을 경우 연, 월 0,0인 구조체 반환
    return csv_config;
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
    if (strstr(line_buffer, "Year,Month") != NULL)
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
      sscanf(line_buffer, "%d, %d", &csv_config.raw_year, &csv_config.raw_month);
      printf("  연/월 데이터 파싱: %s\n", line_buffer);
      current_state = PARSE_STATE_YEAR_MONTH_DATA; // 데이터 읽었으니 상태 변경
      break;
    case PARSE_STATE_EMPLOYEE_HEADER:
    case PARSE_STATE_EMPLOYEE_DATA: // 헤더 다음 줄부터는 계속 직원 데이터
                                    // 여기에 strtok/atoi를 이용한 직원 데이터 파싱 로직 추가
      printf("  직원 데이터 파싱: %s\n", line_buffer);
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
  return csv_config;
}
