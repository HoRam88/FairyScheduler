// mock_data.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "employee.h"

// 하드코딩된 데이터를 반환하는 함수
Employee *load_employees(int *employee_count)
{
  *employee_count = 4; // 4명의 직원이 있다고 가정

  // 3명의 직원을 담을 메모리 할당
  Employee *employees = (Employee *)malloc(sizeof(Employee) * (*employee_count));
  if (employees == NULL)
  {
    perror("Failed to allocate memory");
    exit(1);
  }

  // 직원 1
  employees[0].id = 101;
  strcpy(employees[0].name, "이기철");
  employees[0].contract_days = 20;

  // 직원 2
  employees[1].id = 102;
  strcpy(employees[1].name, "이기영");
  employees[1].contract_days = 21;

  // 직원 3
  employees[2].id = 103;
  strcpy(employees[2].name, "김철수");
  employees[2].contract_days = 20;

  // 직원 4
  employees[3].id = 104;
  strcpy(employees[3].name, "성진우");
  employees[3].contract_days = 21;

  // 직원 5
  employees[4].id = 105;
  strcpy(employees[3].name, "김마라");
  employees[4].contract_days = 21;

  printf("Loaded %d employees from MOCK data.\n", *employee_count);
  return employees;
}