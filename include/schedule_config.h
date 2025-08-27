// include/month_config.h
#ifndef MONTH_CONFIG_H
#define MONTH_CONFIG_H

// ScheduleConfig 구조체 정의
typedef struct
{
  int year;                           // 연도
  int month;                          // 월
  int num_days;                       // 해당 월 말일
  int num_employees;                  // 근무자 수
  int population_size;                // 유전자 개체 수
  int max_generations;                // 최대 반복 세대
  int gene_length;                    // 유전자 개체의 길이
  int elite_count;                    // 엘리트 갯수
  double mutation_rate;               // 돌연변이 확률
  int shift_type_count;               // 근무 형태의 수
  double penalty_wight[4];            // 근무형태별 분산을 이용한 패널티 적용시 가중치
  double penalty_wight_total_days;    // 평균 근무일수 분산에 대한 패널티값
  double fitness_init_score;          // 적합도 초기 점수
  double penalty_wight_night_off_off; // Night근무 후 OFF, OFF 보장에 대한 가중치
  double positive_wight_night_night_off_off;  //N N O O 근무에 대한 +가중치
} ScheduleConfig;

// 외부에 공개할 함수 선언
int get_days_in_month(int year, int month);
ScheduleConfig *init_schedule_config(int year, int month, int num_employees);

#endif