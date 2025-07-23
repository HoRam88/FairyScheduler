// src/shift_type.c
#include "shift_type.h" // 헤더 파일 포함

char shift_type_to_char(ShiftType type)
{
  switch (type)
  {
  case DAY:
    return 'D';
  case EVENING:
    return 'E';
  case NIGHT:
    return 'N';
  case OFF:
    return 'O';
  default:
    return '?';
  }
}