# FairScheduler

A high-performance, automatic shift scheduler using a Genetic Algorithm, written in C.
<br>
유전자 알고리즘을 이용한 자동 교대근무 스케줄러

---

## 📖 소개 (Introduction)

수동으로 교대근무표를 작성하는 것은 많은 시간을 소모하며, 모든 근무자에게 공정함을 보장하기 어렵습니다. `FairScheduler`는 복잡한 근무 조건과 제약 사항을 만족시키면서 장기적인 공정성까지 고려하여 최적의 근무표를 자동으로 생성하는 것을 목표로 하는 C 기반의 프로젝트입니다.

---

## ✨ 주요 기능 (Features)

- **유전자 알고리즘 기반 최적화 엔진:** 다수의 제약 조건 속에서 최적의 해를 탐색합니다.
- **계층적 적합도 함수:** 필수 규칙을 우선 만족시키고, 그 안에서 공정성 점수를 최대화하는 다중 목표 최적화를 수행합니다.
- **장기적 공정성 보장:** 과거 근무 기록(누적 데이터)을 다음 달 스케줄링에 반영하여 불공정을 보정합니다.
- **성능 최적화:** 멀티스레딩을 활용하여 적합도 평가 연산을 병렬 처리하고, 실행 환경의 CPU 코어 수를 동적으로 감지하여 자원을 최대한 활용합니다.
- **크로스플랫폼 지원:** CMake 빌드 시스템을 도입하여 macOS, Windows, Linux 환경을 모두 지원합니다.

---

## 🛠️ 기술 스택 (Tech Stack)

- **Language:** C (C11 Standard)
- **Build System:** CMake
- **Threading:** Pthreads (POSIX Threads)

---

## 🚀 설치 및 실행 방법 (Installation & Usage)

*(추후 업데이트 예정)*

```bash
# 1. 저장소 복제
git clone [https://github.com/사용자이름/FairScheduler.git](https://github.com/사용자이름/FairScheduler.git)
cd FairScheduler

# 2. 빌드
mkdir build && cd build
cmake ..
make

# 3. 실행
./FairScheduler ../path/to/employees.csv ../path/to/request.csv
