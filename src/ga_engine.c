// ga_engine.c
#include "ga_engine.h"
#include <stdio.h> // 기타 필요한 헤더
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define POPULATION_SIZE 100 // 100개의 근무표 후보를 사용

static bool overlapped_detector(const Schedule *population, int current_population_count, int gene_length)
{
  if (current_population_count < 2)
  {
    return false;
  }

  const ShiftType *new_schedule_genes = population[current_population_count - 1].schedule;
  for (int i = 0; i < current_population_count - 1; i++)
  {
    const ShiftType *old_schedule_gens = population[i].schedule;
    if (memcmp(new_schedule_genes, old_schedule_gens, gene_length * sizeof(ShiftType)) == 0)
      return true;
  }
  return false;
}
Schedule *create_population_memeory(const ScheduleConfig *config)
{
  // 유전자 길이 = 당월 말일 * 근무자 수
  int schedule_length = config->num_days * config->num_employees;

  // 1. 모집단 배열(껍데기) 할당
  Schedule *population = (Schedule *)malloc(sizeof(Schedule) * config->population_size);
  if (population == NULL)
  {
    // 메모리 할당 실패 처리
    fprintf(stderr, "Faild to allocate memory for population array\\n");
    return NULL;
  }

  // 2. 각 개체의 유전자 공간 할당
  for (int i = 0; i < config->population_size; i++)
  {
    population[i].schedule = (ShiftType *)malloc(sizeof(ShiftType) * schedule_length);

    if (population[i].schedule == NULL)
    {
      // 만약 할당에 실패하면, 그 전까지 할당했던 모든 메모리를 해제해야 함
      fprintf(stderr, "Failed to allocate memory for schedule of population member %d\\n", i);

      for (int j = 0; j < i; j++)
      {
        free(population[j].schedule);
      }
      free(population);
      return NULL;
    }
  }

  return population;
}

void free_population_memeory(Schedule **population_ptr, const ScheduleConfig *config)
{
  // 이미 해제된 포인터일 경우, 바로 종료
  if (population_ptr == NULL || *population_ptr == NULL)
  {
    return;
  }

  Schedule *population = *population_ptr;
  for (int i = 0; i < config->population_size; i++)
  {
    free(population[i].schedule);
  }
  free(population);

  // 호출한 곳의 원본 포인터를 NULL로 변경
  *population_ptr = NULL;
}

// 각 근무표의 적합도를 계산하는 함수
double fitness_check(const ShiftType *schedule, const ScheduleConfig *config)
{
  double score = 10000.0;

  // day를 기준으로 순회
  for (int day = 0; day < config->num_days; day++)
  {
    int daily_shift_counts[4] = {0}; // {DAY, EVENING, NIGHT, OFF} 카운터

    // 그날의 모든 직원을 순회
    for (int emp = 0; emp < config->num_employees; emp++)
    {
      int current_idx = day * config->num_employees + emp;
      ShiftType current_shift = schedule[current_idx];

      // 1. 데이터 집계
      daily_shift_counts[current_shift]++;

      // 2. 순차 규칙 검사 (N 근무 후 2일 OFF)
      if (current_shift == NIGHT)
      {
        // 경계 검사: 월말에 배열을 벗어나지 않도록 방지
        if (day + 2 < config->num_days)
        {
          ShiftType next_day_shift = schedule[current_idx + config->num_employees];
          ShiftType day_after_next_shift = schedule[current_idx + (2 * config->num_employees)];

          if (next_day_shift != OFF || day_after_next_shift != OFF)
          {
            score -= 2000; // 강한 페널티
          }
        }
      }
    }

    // 3. 일별 규칙 평가 (하루 순회가 끝난 후)
    // 주간, 오후 근무자가 한 명도 없으면 감점
    if (daily_shift_counts[DAY] == 0)
      score -= 500;
    if (daily_shift_counts[EVENING] == 0)
      score -= 500;

    // 야간 근무자가 정확히 1명이 아니면 감점
    if (daily_shift_counts[NIGHT] != 1)
    {
      score -= 1000;
    }
  }

  // 4. 개인별 규칙 최종 평가 (향후 추가)
  // ...

  return score;
}

// 내부에서만 사용할 함수들 (static으로 선언)

// 초기 개체 생성시 제약조건을 사용한 생성
static Schedule *gen_limited(Schedule *gen, int start_idx, int end_idx)
{
}

// 초기 개체 생성
static Schedule *initialize_population(const ScheduleConfig *config)
{
  // 유전자 길이 = 당월 말일 * 근무자 수
  int schedule_length = config->num_days * config->num_employees;

  // 1. 근무표 메모리 할당
  Schedule *population = create_population_memeory(config);

  // 유전자 개체 수 만큼 순회
  for (int p_idx = 0; p_idx < config->population_size; p_idx++)
  {
    bool is_overlapped;
    do
    {
      for (int gene_idx = 0; gene_idx < schedule_length; gene_idx++)
      {
        population[p_idx].schedule[gene_idx] = rand() % 4;
      }

      is_overlapped = overlapped_detector(population, p_idx + 1, schedule_length);
    } while (is_overlapped);

    // 기본 적합도 0.0 초기화.
    population[p_idx].fitness = 0.0;
  }

  // 최종 생성된 유전자 개체 포인터 반환.
  return population;
}

int compare_schedule_desc(const void *a, const void *b)
{
  // void 포인터를 Schedule 포인터로 변환
  const Schedule *scheduleA = (const Schedule *)a;
  const Schedule *scheduleB = (const Schedule *)b;

  // scheduleB의 fitness가 더 크면, B를 앞으로 보내야 하므로 양수를 반환
  if (scheduleA->fitness < scheduleB->fitness)
  {
    return 1;
  }
  // scheduleA의 fitness가 더 크면, A를 앞으로 보내야 하므로 음수를 반환
  else if (scheduleA->fitness > scheduleB->fitness)
  {
    return -1;
  }
  // 두 fitness값이 같으면 0을 반환
  else
  {
    return 0;
  }
}

// 적합도 점수에 따른 내림차순 정렬
static void sort_population(Schedule *population, const ScheduleConfig *config)
{
  qsort(population,              // 정렬할 배열
        config->population_size, // 배열의 요소 갯수
        sizeof(Schedule),        // 각 요소의 크기
        compare_schedule_desc);  // 비교 기준 함수
}

static void print_population(const Schedule *population, const ScheduleConfig *config)
{
  int pop_size = config->population_size;
  int month_end_days = config->num_days;
  int emp_num = config->num_employees;

  for (int pop_num = 0; pop_num < pop_size; pop_num++)
  {
    int schedule_point = 0;
    printf("pop Number: %d\t fitness: %lf\n Schedule\n", pop_num, population[pop_num].fitness);

    for (int day = 0; day < month_end_days; day++)
    {
      for (int emp_count = 0; emp_count < emp_num; emp_count++)
      {
        int temp_print = (emp_count == 0) ? day : emp_count;
        printf("%d \t", temp_print);
      }
    }

    for (int day = 0; day < month_end_days; day++)
    {
      for (int emp_count = 0; emp_count < emp_num; emp_count++)
      {
        printf("%d \t", population[pop_num].schedule[schedule_point]);
        schedule_point++;
      }
    }

    printf("\n\n");
  }
}

static GaResult population_best_result(Schedule population, const ScheduleConfig *config)
{
  GaResult result;

  result.best_schedule.schedule = (ShiftType *)malloc(sizeof(ShiftType) * config->num_days * config->num_employees);
  result.best_schedule.fitness = population.fitness;

  for (int i = 0; i < config->num_days * config->num_employees; i++)
  {
    result.best_schedule.schedule[i] = population.schedule[i];
  }

  return result;
}

// 외부에 공개된 메인 함수
GaResult
run_genetic_algorithm(Employee *employees, int employee_count, const ScheduleConfig *config)
{
  // 한 개체의 유전자 길이 = 근무자 수 * 해당 월 말일
  GaResult result;
  // Schedule 포인터 2개 생성(현세대, 다음세대)
  Schedule *population;
  Schedule *next_generation = create_population_memeory(config);

  population = initialize_population(config);

  for (int idx = 0; idx < config->population_size; idx++)
  {
    population[idx].fitness = fitness_check(population[idx].schedule, config);
  }

  sort_population(population, config);

  print_population(population, config);

  result = population_best_result(population[0], config);

  // 이하 유전자 알고리즘에 따른 반복 예정.
  // Do While을 사용하여 적합도 함수결과 내림차순 정렬 후에 조건 비교후 탈출 가능.

  // 현세대 적합도 함수 실행

  // 적합도 함수 결과 내림차순 정렬

  // 교배 기준에 따라 교배하여 다음세대 생성

  // 현세대, 다음세대 포인터 교환

  free_population_memeory(&population, config);
  free_population_memeory(&next_generation, config);

  return result;
}
