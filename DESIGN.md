# FairyScheduler 설계 문서

## 1. 전체 아키텍처 (Overall Architecture)

이 프로젝트는 유전 알고리즘(Genetic Algorithm)을 사용하여 직원 근무 스케줄을 최적화하는 것을 목표로 한다. 전체적인 데이터 흐름은 다음과 같다.

1.  **`main.c`**: 프로그램의 시작점.
    *   `schedule_config.c`를 통해 근무표 생성에 필요한 설정(연도, 월, 직원 수 등)을 로드한다.
    *   `ga_engine.c`의 `run_genetic_algorithm` 함수를 호출하여 GA(유전자알고리즘)를 실행한다.
    *   최종적으로 얻은 최적의 스케줄을 출력하고 모든 동적 메모리를 해제한다.

2.  **`ga_engine.c`**: 유전 알고리즘의 핵심 로직을 수행한다.
    *   초기 세대(population)를 무작위로 생성한다. (`initialize_population`)
    *   정해진 세대(generation)만큼 또는 특정 조건이 만족될 때까지 다음을 반복한다:
        *   각 개체(근무표)의 품질을 평가한다. (`fitness_check`)
        *   우수한 개체를 선택한다. (`selection` - 구현 예정)
        *   선택된 개체들을 교차(crossover)하여 자식 세대를 생성한다. (`crossover` - 구현 예정)
        *   일부 유전자에 무작위 변이(mutation)를 적용한다. (`mutation` - 구현 예정)
    *   모든 과정이 끝나면 가장 높은 점수를 받은 최종 근무표를 반환한다.

## 2. 핵심 데이터 구조 (Data Structures)

### `ScheduleConfig` (in `schedule_config.h`)
*   **역할**: GA 실행에 필요한 모든 설정 값을 담는 구조체.
*   **멤버**:
    *   `int year`, `int month`: 근무표의 연도와 월.
    *   `int num_days`: 해당 월의 총 일수 (e.g., 31).
    *   `int num_employees`: 총 근무자 수.
    *   `int population_size`: GA에서 사용할 개체(후보 근무표)의 수.
    *   `int max_generations`: GA를 최대로 반복할 세대 수.
    *   `...` (향후 추가될 GA 파라미터들)

### `Schedule` (in `ga_engine.h`)
*   **역할**: 하나의 완성된 근무표. 유전 알고리즘의 **개체(Individual)** 또는 **염색체(Chromosome)**에 해당한다.
*   **멤버**:
    *   `ShiftType *schedule`: **(중요)** 동적으로 할당되는 포인터.
    *   `double fitness`: 해당 근무표의 적합도 점수.
*   **`schedule` 포인터의 메모리 구조**:
    *   **크기**: `(PREV_DAYS + num_days) * num_employees` 만큼의 `ShiftType` 배열을 가리킨다. (`PREV_DAYS`는 3으로 가정)
    *   **레이아웃**: **Column-Major (일별 우선)** 방식을 따른다.
        *   `[1일차(직원1,2,..)], [2일차(직원1,2,..)], ...`
    *   **인덱스 접근 공식**: `schedule[day * num_employees + emp]`

### `ShiftType` (in `shift_type.h`)
*   **역할**: 하루의 근무 형태를 나타내는 열거형(enum).
*   **값**: `D` (Day), `E` (Evening), `N` (Night), `O` (Off). (예: 0, 1, 2, 3)

## 3. 주요 함수 (Key Functions)

### `initialize_population(const ScheduleConfig *config)`
*   **역할**: `config->population_size` 개수만큼의 `Schedule` 개체(전체 근무표)를 생성하여 초기 집단을 만든다.
*   **동작**:
    1.  `population` 배열을 위한 메모리를 할당한다.
    2.  각 `population[i]`에 대해, 내부의 `schedule` 포인터를 위한 메모리를 `(PREV_DAYS + num_days) * num_employees` 크기로 동적 할당한다.
    3.  Column-Major 순서에 따라 `schedule` 배열을 무작위 근무 형태로 채운다.

### `fitness_check(const Schedule *schedule, const ScheduleConfig *config)`
*   **역할**: 단일 `Schedule` 개체(하나의 전체 근무표)를 입력받아, 모든 제약 조건을 검사하고 적합도 점수를 반환한다.
*   **동작**:
    1.  높은 기본 점수(e.g., 10000.0)에서 시작한다.
    2.  **일별 제약 조건 검사**: `for (day = ...)` 루프를 돌며 검사.
        *   `day` 날짜의 모든 직원 근무 정보를 `schedule[day * num_employees + emp]`로 가져온다.
        *   일별 필요 인원, 근무 형태별 최소/최대 인원 등을 확인하고 위반 시 점수를 감점한다.
    3.  **개인별 제약 조건 검사**: `for (emp = ...)` 루프를 돌며 검사.
        *   `emp` 직원의 모든 근무 정보를 `schedule[day * num_employees + emp]`로 가져온다.
        *   "N 근무 후 OFF", 연속 근무일수, 최소 휴무일수 등을 확인하고 위반 시 점수를 감점한다.
    4.  최종 점수를 반환한다.
