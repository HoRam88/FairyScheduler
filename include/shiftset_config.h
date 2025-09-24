// include/shiftset_config.h
#ifndef SHIFTSET_CONFIG_H
#define SHIFTSET_CONFIG_H

// '하나의 근무 형태'에 필요한 정보를 담는 구조체
typedef struct
{
  char code;         // 근무 코드 (예: 'D', 'E', 'N', 'O')
  int min_employees; // 해당 근무의 최소 필요 인원
  int max_employees; // 해당 근무의 최대 배치 가능 인원
} ShiftRequirement;

// 여러 '근무 형태'의 목록을 관리하는 설정 구조체
typedef struct
{
  ShiftRequirement *requirements; // ShiftRequirement 구조체의 동적 배열
  int count;                      // 현재 저장된 근무 형태의 개수
} ShiftSetConfig;
#endif