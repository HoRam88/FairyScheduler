// include/shift_type.h
#ifndef SHIFT_TYPE_H
#define SHIFT_TYPE_H

typedef enum
{
  DAY,     // 주간
  EVENING, // 오후
  NIGHT,   // 야간
  OFF      // 휴무
} ShiftType;

// 변환 함수 선언 (나중에 구현)
char shift_type_to_char(ShiftType type);

#endif