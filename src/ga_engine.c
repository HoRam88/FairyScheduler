// ga_engine.c
#include "ga_engine.h"
#include <stdio.h> // 기타 필요한 헤더
#include <stdlib.h>

#define POPULATION_SIZE 100 // 100개의 근무표 후보를 사용

double fitness_check(Schedule *schedule, const ScheduleConfig *config)
{
  // 적합도는 감점식으로 평가하며, 조건에 맞지 않을경우 점수를 뺀다.
  double score = 10000.0;

  for (int i = 0; i < config->num_employees; i++)
  {
    score = 10000;
  }

  // 일별 필요 인원 확인

  return score;
}

// 내부에서만 사용할 함수들 (static으로 선언)
// 초기 개체 생성
static void initialize_population(Schedule population[], const ScheduleConfig *config)
{
}

// 적합도 평가
static void calc_population()
{
}

// 적합도 점수에 따른 내림차순 정렬
static void sort_population()
{
}

// 외부에 공개된 메인 함수
GaResult
run_genetic_algorithm(Employee *employees, int employee_count, const ScheduleConfig *config, FitnessFunc fitness_check)
{
  // 한 개체의 유전자 길이 = 근무자 수 * 해당 월 말일
  int schedule_size = config->num_days * config->num_employees;
  int populaiton_num = config->num_employees * 100;
  GaResult result;
  // Schedule 포인터 2개 생성(현세대, 다음세대)
  Schedule *population = (Schedule *)malloc(sizeof(Schedule) * populaiton_num);
  Schedule *next_generation = (Schedule *)malloc(sizeof(Schedule) * populaiton_num);

  initialize_population(population, config);

  // 이하 유전자 알고리즘에 따른 반복 예정.
  // Do While을 사용하여 적합도 함수결과 내림차순 정렬 후에 조건 비교후 탈출 가능.

  // 현세대 적합도 함수 실행

  // 적합도 함수 결과 내림차순 정렬

  // 교배 기준에 따라 교배하여 다음세대 생성

  // 현세대, 다음세대 포인터 교환

  free(population);
  population = NULL;
  free(next_generation);
  next_generation = NULL;

  return result;
}
