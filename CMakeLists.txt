# CMake 최소 버전 설정
cmake_minimum_required(VERSION 3.10)

# 프로젝트 정보 설정 (이름, 버전, 언어)
project(FairScheduler VERSION 1.0 LANGUAGES C)

# C 표준 설정 (C11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 실행 파일 이름 설정
set(EXECUTABLE_NAME FairScheduler)

# 헤더 파일이 위치한 디렉토리 추가
include_directories(include)

# src 디렉토리의 모든 .c 파일을 찾아 SOURCE_FILES 변수에 저장
aux_source_directory(src SOURCE_FILES)

# 실행 파일 타겟 생성
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

# Pthreads 라이브러리 링크 (멀티스레딩 지원)
# find_package를 사용하는 것이 현대적인 CMake 방식입니다.
find_package(Threads REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE Threads::Threads)

# 빌드 후 메시지 출력
message(STATUS "CMake 구성 완료. 'make' 명령어로 프로젝트를 빌드하세요.")
