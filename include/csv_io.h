#ifndef CSV_IO_H
#define CSV_IO_H
#include <stdbool.h>
#include <ga_engine.h>

typedef struct
{
  int raw_year;
  int raw_month;

} RawCsvConfig;

// csv파일을 읽으면서 수행할 반복문 내에서 상태전환을 위한 enum 선언.
typedef enum
{
  PARSE_STATE_INITIAL,           // 초기 상태: 아직 아무 섹션도 파악되지 않음
  PARSE_STATE_YEAR_MONTH_HEADER, // "Year,Month" 헤더를 읽은 상태
  PARSE_STATE_YEAR_MONTH_DATA,   // 연/월 데이터를 읽은 상태 (다음은 빈 줄 예상)
  PARSE_STATE_BLANK_LINE,        // 빈 줄을 읽은 상태 (다음은 직원 헤더 예상)
  PARSE_STATE_EMPLOYEE_HEADER,   // "EmpNo,Name,WorkDays" 헤더를 읽은 상태
  PARSE_STATE_EMPLOYEE_DATA      // 직원 데이터를 읽는 상태
} CsvParseState;

RawCsvConfig load_ga_data_from_csv(const char *file_path);

bool load_from_csv_Config_Employees(const char *file_path,
                                    RawCsvConfig *config_out, // 출력: ScheduleConfig 초기화를 위한 연, 월 등의 정보
                                    Employee **employees_out, // 출력: 직원 정보를 가지는 포인터 배열
                                    int *employee_count       // 출력: 직원의 수
);

char *generate_output_path(const char *input_path);
bool save_schedule_to_csv(const char *input_path, const Employee *emp_list, const EmployeeStats *emp_stats, const ScheduleConfig *config, const GaResult result);
#endif