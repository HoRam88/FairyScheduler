// employee.h 근무자 구조체 정의
#ifndef EMPLOYEE_H
#define EMPLOYEE_H

typedef struct
{
  int id;            // 직원 고유 ID
  char name[50];     // 직원 이름
  int contract_days; // 월 계약 근무일 수
} Employee;

typedef struct
{
  int total_work_days;
  int shift_counts[4];
} EmployeeStats;

typedef struct
{
  int year;             // 연
  int month;            // 월
  int day_of_week_idex; // 요일

} FromCsv;



#endif