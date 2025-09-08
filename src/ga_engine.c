// ga_engine.c
#include "ga_engine.h"
#include <stdio.h> // 기타 필요한 헤더
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// 중복된 개체 탐지 함수
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

// 유전자 개체 저장을 위한 메모리 할당 함수
Schedule *create_population_memory(const ScheduleConfig *config)
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

// 유전자 개체 메모리 반환 함수
void free_population_memory(Schedule **population_ptr, const ScheduleConfig *config)
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

// 날짜 범위를 지정하여 분산을 이용한 패널티 계산을 위한 함수 emp_stats에 저장된 범위만 이용하여 분산을 구함. 원하는 범위만 emp_stats에 넣어 전달하여 구할것.
static double calculate_shift_variance(const EmployeeStats *emp_stats, const ScheduleConfig *config)
{
  double total_variance_penalty = 0.0;

  int num_shift_type = config->shift_type_count;
  int num_employees = config->num_employees;
  double avg[num_shift_type];

  EmployeeStats total;
  // total 초기화
  memset(&total, 0, sizeof(EmployeeStats));

  // 각 근무형태별 평균 근무일수 구하기
  for (int shift = 0; shift < num_shift_type; shift++)
  {
    for (int emp = 0; emp < num_employees; emp++)
    {
      total.shift_counts[shift] += emp_stats[emp].shift_counts[shift];
      // if (shift != 3)
      // {
      //   total.total_work_days += total.shift_counts[shift];
      // }
    }
    avg[shift] = (double)total.shift_counts[shift] / num_employees;
  }

  // (각 근무자별 근무일수 - 평균 근무일수)^2 의 합계 구하기
  for (int shift = 0; shift < num_shift_type; shift++)
  {
    double temp = 0.0;
    double sum = 0.0;

    for (int emp = 0; emp < num_employees; emp++)
    {
      temp = emp_stats[emp].shift_counts[shift] - avg[shift];
      sum += temp * temp;
    }
    // 합계^2 / 근무자 수 == 분산 구하기
    sum = sum / num_employees;

    total_variance_penalty += sum * config->penalty_wight[shift];
  }

  return total_variance_penalty;
}

// 각 근무표의 적합도를 계산하는 함수
double fitness_check(const ShiftType *schedule, const ScheduleConfig *config)
{
  double score = config->fitness_init_score;
  int num_employees = config->num_employees;

  // 근무자별 근무형태를 저장할 수 있는 구조체 배열 선언 및 초기화.
  EmployeeStats emp_stats[num_employees];

  // emp_stats 초기화
  for (int emp_count = 0; emp_count < num_employees; emp_count++)
  {
    for (int shift_type = 0; shift_type < config->shift_type_count; shift_type++)
    {
      emp_stats[emp_count].shift_counts[shift_type] = 0;
    }
    emp_stats[emp_count].total_work_days = 0;
  }

  // 근무형태 합계를 저장할 수 있는 구조체 선언 및 초기화.
  EmployeeStats emp_total_stats;

  // emp_total_stats 초기화
  emp_total_stats.total_work_days = 0;
  for (int shift_type = 0; shift_type < config->shift_type_count; shift_type++)
  {
    emp_total_stats.shift_counts[shift_type] = 0;
  }

  // day를 기준으로 순회
  for (int day = 0; day < config->num_days; day++)
  {
    int count_days = 0;
    int daily_shift_counts[4] = {0}; // {DAY, EVENING, NIGHT, OFF} 카운터

    // 그날의 모든 직원을 순회
    for (int emp = 0; emp < config->num_employees; emp++)
    {
      int current_idx = day * config->num_employees + emp;
      ShiftType current_shift = schedule[current_idx];

      // 1. 데이터 집계
      daily_shift_counts[current_shift]++;

      // 근무형태 합계와 각 근무자별 근무형태 구조체에 합계 저장.
      emp_stats[emp].shift_counts[current_shift]++;
      emp_stats[emp].total_work_days++;

      emp_total_stats.shift_counts[current_shift]++;
      // 2. 순차 규칙 검사 (N 근무 후 2일 OFF)
      if (current_shift == NIGHT)
      {
        // 경계 검사: 월말에 배열을 벗어나지 않도록 방지

        // N 근무 이후 O O 보장을 위한 조건 검사
        if (day + 2 < config->num_days)
        {
          ShiftType next_day_shift = schedule[current_idx + config->num_employees];
          ShiftType day_after_next_shift = schedule[current_idx + (2 * config->num_employees)];

          if (next_day_shift != OFF || day_after_next_shift != OFF)
          {
            score -= config->penalty_wight_night_off_off; // 강한 페널티
          }
        }
        // N N O O 근무형태일 경우 가점 부여(+)
        if (day + 3 < config->num_days)
        {
          ShiftType next_day_shift = schedule[current_idx + config->num_employees];
          ShiftType day_after_next_shift = schedule[current_idx + (2 * config->num_employees)];
          ShiftType three_days_after_shift = schedule[current_idx + (3 * num_employees)];

          if (next_day_shift == NIGHT &&
              day_after_next_shift == OFF &&
              three_days_after_shift == OFF)
          {
            // N N O O 근무형태일 경우 가점 부여(+)
            score += config->positive_wight_night_night_off_off;
          }
        }
      }
    }

    // 3. 일별 규칙 평가 (하루 순회가 끝난 후)
    // 주간, 오후 근무자가 한 명도 없으면 감점
    if (daily_shift_counts[DAY] == 0)
      score -= 5000;
    if (daily_shift_counts[EVENING] == 0)
      score -= 5000;

    // 야간 근무자가 정확히 1명이 아니면 감점
    if (daily_shift_counts[NIGHT] != 1)
    {
      score -= 20000;
    }

    // 분산 패널티 적용 조건 검사 1주일단위 또는 마지막 날 일 때 적용
    if ((day + 1) % 7 == 0 || day == config->num_days - 1)
    {
      score -= calculate_shift_variance(emp_stats, config);

      // emp_stats 초기화
      for (int emp_count = 0; emp_count < num_employees; emp_count++)
      {
        for (int shift_type = 0; shift_type < config->shift_type_count; shift_type++)
        {
          emp_stats[emp_count].shift_counts[shift_type] = 0;
        }
        emp_stats[emp_count].total_work_days = 0;
      }
    }
  }

  // 4. 개인별 규칙 최종 평가 (향후 추가)
  // ...

  return score;
}

// 내부에서만 사용할 함수들 (static으로 선언)

// // 초기 개체 생성시 제약조건을 사용한 생성
// static Schedule *gen_limited(Schedule *gen, int start_idx, int end_idx)
// {
// }

// 초기 개체 생성
static Schedule *initialize_population(const ScheduleConfig *config)
{
  // 유전자 길이 = 당월 말일 * 근무자 수
  int schedule_length = config->num_days * config->num_employees;

  // 1. 근무표 메모리 할당
  Schedule *population = create_population_memory(config);

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

// qsort를 사용하기위한 판별함수 내림차순정렬 목적.
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

// 적합도 점수에 따른 내림차순 정렬함수
static void sort_population(Schedule *population, const ScheduleConfig *config)
{
  qsort(population,              // 정렬할 배열
        config->population_size, // 배열의 요소 갯수
        sizeof(Schedule),        // 각 요소의 크기
        compare_schedule_desc);  // 비교 기준 함수
}

// 교배 동작부분. 부모 포인터 2개, 자식이 될 포인터2개를 전달하여 교배.
static void crossover(const Schedule *parentA, const Schedule *parentB, Schedule *childA, Schedule *childB, const ScheduleConfig *config)
{
  int crossover_point = rand() % (config->gene_length - 1) + 1;

  childA->fitness = 0.0;
  childB->fitness = 0.0;

  // for (int idx = 0; idx < config->gene_length; idx++)
  // {
  //   childA->schedule[idx] = (idx < crossover_point) ? parentA->schedule[idx] : parentB->schedule[idx];
  //   childB->schedule[idx] = (idx > crossover_point) ? parentA->schedule[idx] : parentB->schedule[idx];
  // }

  memcpy(childA->schedule, parentA->schedule, crossover_point * sizeof(ShiftType));
  memcpy(childA->schedule + crossover_point, parentB->schedule + crossover_point, (config->gene_length - crossover_point) * sizeof(ShiftType));

  memcpy(childB->schedule, parentB->schedule, crossover_point * sizeof(ShiftType));
  memcpy(childB->schedule + crossover_point, parentA->schedule + crossover_point, (config->gene_length - crossover_point) * sizeof(ShiftType));
}

// 엘리트 보존 함수
static void save_elite(const Schedule *now_generation, Schedule *next_generation, const ScheduleConfig *config)
{
  for (int idx = 0; idx < config->elite_count; idx++)
  // 반복문 조건 수정
  {
    // schedule (유전자 데이터) 복사
    memcpy(next_generation[idx].schedule, now_generation[idx].schedule, config->gene_length * sizeof(ShiftType));

    // fitness 값 복사 (추가)
    next_generation[idx].fitness = now_generation[idx].fitness;
  }
}

// 돌연변이 적용 함수
static void mutate_individual(Schedule *child, const ScheduleConfig *config)
{

  for (int i = 0; i < config->gene_length; i++)
  {
    if ((double)rand() / RAND_MAX < config->mutation_rate)
    {
      child->schedule[i] = rand() % 4;
    }
  }
}

// 유전자 교배를 위한 토너먼트 함수
static int select_parent_tournament(const ScheduleConfig *config, int count)
{
  // 교배 인덱스 선정 함수. 반드시 내림차순정렬(sort_population)을 실행한 후 사용할 것.
  int temp = 0;
  int num_return = rand() % config->population_size;
  for (int i = 1; i < count; i++)
  {
    temp = rand() % config->population_size;

    num_return = num_return < temp ? num_return : temp;
  }

  return num_return;
}

// 유전자 교배 실행 함수(다음세대 생성 함수)
static void run_crossover_Do(Schedule *now_generation, Schedule *next_generation, const ScheduleConfig *config)
{
  // 교배 실행 함수.
  save_elite(now_generation, next_generation, config);
  int index1, index2;
  bool is_overlapped1;
  bool is_overlapped2;
  for (int idx = config->elite_count; idx < config->population_size - 1; idx += 2)
  {
    index1 = select_parent_tournament(config, 10);
    do
    {
      index2 = select_parent_tournament(config, 10);

      // index1, index2가 동일하다면 반복하여 다른 개체 2개를 선택할 것.
    } while (index1 == index2);

    // printf("tournament result: %d, %d\n]n", index1, index2);

    do
    {
      crossover(&now_generation[index1], &now_generation[index2], &next_generation[idx], &next_generation[idx + 1], config);
      mutate_individual(&next_generation[idx], config);
      mutate_individual(&next_generation[idx + 1], config);

      is_overlapped1 = overlapped_detector(next_generation, idx + 1, config->gene_length);
      is_overlapped2 = overlapped_detector(next_generation, idx + 2, config->gene_length);
    } while (is_overlapped1 || is_overlapped2);
  }
  // printf("\nrun_crossover_section_end\n");
}

// 디버깅을 위한 개체 프린트 함수. 0 ~ print_end_idx까지의 개체를 출력
static void print_population(const Schedule *population, const ScheduleConfig *config, int print_end_idx)
{
  // 개체 출력 함수. 인덱스기준 0~print_end_idx까지 출력함.
  int pop_size = config->population_size;
  int month_end_days = config->num_days;
  int emp_num = config->num_employees;
  int work_day[emp_num][4];

  for (int emp_count = 0; emp_count < emp_num; emp_count++)
  {
    for (int work = 0; work < 4; work++)
    {
      work_day[emp_count][work] = 0;
    }
  }
  // print_end_idx가 개체 수 보다 클 경우, 개체수로 조정.
  if (print_end_idx > config->max_generations)
  {
    print_end_idx = config->max_generations;
  }

  for (int pop_num = 0; pop_num < print_end_idx; pop_num++)
  {
    int schedule_point = 0;
    printf("pop Number: %d\t fitness: %lf\n Schedule\n", pop_num, population[pop_num].fitness);

    for (int day = 0; day < month_end_days; day++)
    {

      for (int emp_count = 0; emp_count < emp_num; emp_count++)
      {
        work_day[emp_count][population[pop_num].schedule[schedule_point]]++;
        printf("%c \t", shift_type_to_char(population[pop_num].schedule[schedule_point]));
        schedule_point++;
      }
      printf("\n\n");
    }

    printf("EmpNo\t\t");
    for (ShiftType work = 0; work < 4; work++)
    {
      printf("%c\t", shift_type_to_char(work));
    }
    printf("\n");

    for (int emp_count = 0; emp_count < emp_num; emp_count++)
    {
      printf("EmpNo: %d\t", emp_count + 1);
      for (int work = 0; work < 4; work++)
      {
        printf("%d\t", work_day[emp_count][work]);
      }
      printf("\n");
    }

    printf("\n\n");
  }
}

// 최종 반환을 위한 개체 선택 함수
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

// 적합도 함수. Schedule 포인터배열을 전부 순회하도록 실행.
static void fitness_Do(Schedule **now_generation_ptr, const ScheduleConfig *config)
{
  Schedule *now_generation = *now_generation_ptr;
  for (int idx = 0; idx < config->population_size; idx++)
  {
    now_generation[idx].fitness = fitness_check(now_generation[idx].schedule, config);
  }
}

// now_generation_ptr과 next_generation_ptr를 교환. 메모리 주소만 교환하여 변수명은 그대로 활용하기 위함.
static void switchGen(Schedule **now_generation_ptr, Schedule **next_generation_ptr)
{
  Schedule *tempSchedule;

  // now_generation_ptr, next_generation_ptr의 포인터 주소 교환을 위한 함수.
  tempSchedule = *now_generation_ptr;
  *now_generation_ptr = *next_generation_ptr;
  *next_generation_ptr = tempSchedule;
}

// 외부에 공개된 메인 함수 최종결과 반환.
GaResult run_genetic_algorithm(Employee *employees, int employee_count, const ScheduleConfig *config)
{
  // 한 개체의 유전자 길이 = 근무자 수 * 해당 월 말일
  GaResult result;
  // Schedule 포인터 2개 생성(현세대, 다음세대)
  Schedule *population;
  Schedule *next_generation = create_population_memory(config);

  int generation_count = 0;

  population = initialize_population(config); // 유전자 개체 초기화 및 초기세대 생성
  fitness_Do(&population, config);            // 적합도 함수 실행
  sort_population(population, config);        // 적합도 기준 내림차순 정렬
  printf("\rgeneration_count: %d, fitness: %lf", generation_count, population[0].fitness);
  while (generation_count < config->max_generations)
  {
    generation_count++;
    printf("\rGeneration: %d / %d | Best Fitness: %lf", generation_count, config->max_generations, population[0].fitness);
    // if (generation_count % 50 == 0)
    // {
    //   printf("\rGeneration: %d / %d | Best Fitness: %lf", generation_count, config->max_generations, population[0].fitness);
    // }

    if (population[0].fitness > config->fitness_init_score)
      break;
    run_crossover_Do(population, next_generation, config);
    switchGen(&population, &next_generation);
    fitness_Do(&population, config);
    sort_population(population, config);
  }

  printf("\ngeneration_count: %d\n", generation_count);
  result = population_best_result(population[0], config);

  print_population(population, config, 1);
  // 이하 유전자 알고리즘에 따른 반복 예정.
  // Do While을 사용하여 적합도 함수결과 내림차순 정렬 후에 조건 비교후 탈출 가능.

  // 교배 기준에 따라 교배하여 다음세대 생성

  // 현세대, 다음세대 포인터 교환

  free_population_memory(&population, config);
  free_population_memory(&next_generation, config);

  return result;
}
