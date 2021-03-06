﻿
//최종 어셈블리프로그램이 동작할려면
// 목적 프로그램의 시작 메모리 주소는 1000번지 => COPY START 1000
//연상 연산 코드를 동등한 기계어로 전환 => OPTAB 이용
//심벌 피연산자를 동등한 기계주소로 전환 => SYMTAB 이용
//기계 명령어를 적당한 형식에 맞추어서 생성 => 직접/인덱스 주소 지정 방식
//원시 프로그램에 나타난 데이타 상수를 기계 내부 표현으로 전환 => 필요한 만큼의 문자(C'...'), 16진수(X'...')상수
//목적 프로그램과 어셈블러 리스트를 작성

/*
sic 기계구조(ADT)짜야함

메모리
크기: 32,768 =2^15 byte

레지스터 :  24비트 5개
	0: A	누산기
	1: X	인덱스
	2: L	연결
	8: PC	프로그램 카운터
	9: SW	상태워드

데이터 형식
	정수 : 24비트 2진수
	음수 : 2의 보수
	문자 : 8bit ASCII코드(오브젝트 코드도 아스키, 16진수 문자로 표현됨)

명령어 형식 :  24BIT

	OPCODE + INDEX REGESITER + ADDRESS
	8			1				15

주소지정 방식


명령어 집합



입력과 출력


*/

//패스 1
//1. 프로그램내의 모든문에 주소를 배정
//2. 패스 2에서 사용하기 위해 모든 레이블에 배정된 주소 값들을 저장 =>SYMTAB
//3. 어셈블러 지시자들에 관련된 처리를 부분적으로 처리 => BYTE, RESW 등과 같은 주소 배정에 영향을 주는 처리를 포함

//패스 2
//1. 명령어를 어셈블 => 연산자 코드를 번역하고(OPTAB), 주소를 조사
//2. BYTE, WORD등으로 정의되는 데이타 값을 생성
//3. 패스 1 동안에 이루어지지 않는 어셈블러 지시자의 처리
//4. 목적 프로그램과 어셈블러 리스트를 출력

//OPTAB: 연상 명령어를 찾아서 기계어를 번역하는데 사용
//1. 적어도 명령어 코드와 해당되는 기계 코들를 포함
//2. 명령어 형식과 길이에 대한 정보를 포함하기도 함(서로 다른 명령어를 갖는 기계)
//3. 패스 1에서 원시 프로그램의 명령 코드를 조사하고 확인하는데 사용
//4. 패스 2에서 명령어를 기계어로 번역하는데 사용
//5. 보통 연상 명령어 코드를 키로 갖는 해쉬 테이블로 구성
//6. 내용은 실행 시간에 테이블로 로드되는 것이 아니라, 어셈블러가 작성될때 미리 정의됨
//7. 명령 코드가 추가 또는 삭제될 필요가 없기 때분에 대부분 정적 테이블로 구성

//SYMTAB: 레이블에 배정된 값을 저장하는데 사용
//1. 원시 프로그램의 각 레이블의 이름과 값(주소), 오류 상태(한 기호가 두 장소에서 정의되는 경우)를 나타내기 위한 플래그를 포함
//2. 레이블이 붙여진 데이타 영역이나 명령의 종류, 길이 등에 대한 정보도 포함할 수 있음
//3. 패스 1에서 레이블은 배정되는 주소와 함께 입력
//4. 패스 2에서 피연산자로 사용된 기호는 SYMTAB를 조사하여 어셈블된 명령어에 삽입하기 위한 주소를 찾음
//5. 삽입과 검색의 효율을 위해 보통 해쉬 테이블로 구성

//LOCCTR: 주소 배정을 처리하기 위한 변수 
//1. START문에서 나타낸 시작 주소로 초기화
//2. 각 원시 문장이 나타날 때마다 어셈블된 명령어나 생성된 데이타 영역의 길이가 더해짐
//3. 원시 프로그램에서 레이블을 만날 때 마다 LOCCTR의 현재 값을 레이블의 주소로 배정

//중간화일: 패스 1동안에 수행되어지는 작업을 보존하기 위해 사용
//1. 패스 1은 보통 원시 프로그램과 함께 배정된 주소, 오류 플래그등을 포함하며 패스 2에서 입력으로 사용
//2. OPTAB과 SYMTAB을 가리키는 포인터를 각각 사용되는 연산 코드와 기호에 보존시킴으로써 테이블 탐색의 중복을 피할수 있도록 함

//Sorce DATA
//1. 원시 프로그램은 LABEL, OPCOE와 OPERAND 필드의 고정된 형식으로 작성된다고 가정
//2. 이 필드중의 하나가 수를 표현하는 문자열을 포함하면 접두 문자로 '#'과 함께 그 수의 값을 나타냄 
//3. 모듈 - 기호 테이블의 탐색, 원시 프로그램 읽기


/*
* 한 라인은 분리자로 구분된 토큰에 따라
1개: OPCODE
2개: OPCODE OPERAND
3개: LABEL OPCODE OPERAND
* 피연산자가 ,로 구분된 2개인 경우 붙여 씀 ex) addr r1,r2
*/


#include <stdio.h>
#include <stdlib.h> //exit()
#include <string.h> //strlen()
#include <ctype.h> //isdigit()
#include <math.h> //pow()

#define SYMMAX 100

int locctr;  //위치 계수기
int progleng; //전체 프로그램의 길이
struct oneline { //소스 프로그램의 한 라인은 고정된 필드로 구성
	char loc[10]; //기계 주소, 중간코드에서 사용
	char label[10];
	char opcode[10];
	char operand[10];
};

struct operators {
	char name[10];
	char code[9];
};

struct operators instruction[26] = { //OPTAB
	{ "LDA", "00" },{ "LDX", "04" },{ "STA", "0C" },{ "STX", "10" },//레지스터로부터 읽고 쓰는 명령
	{ "ADD", "18" },{ "SUB", "1C" },{ "MUL", "20" },{ "DIV", "24" },//정수연산 명령어들
	{ "COMP", "28" },//비교 명령어
	{ "J", "3C" },{ "JEQ", "30" },{ "JGT", "34" },{ "JLT", "38" },//조건부 점프 명령어
	{ "AND", "40" },
	
	{ "JSUB", "48" },{ "LDCH", "50" },{ "LDL", "08" },
	{ "OR", "44" },{ "RD", "D8" },
	{ "RSUB", "4C" },{ "STCH", "54" },{ "STL", "14" },
	{ "STSW", "E8" },{ "TD", "E0" },
	{ "TIX", "2C" },{ "WD", "DC" }
};

struct entry {
	char name[10]; //레이블의 이름
	int value;  //레이블에 배정된 값
	int errorflag; //한 기호가 두 장소에서 정의되는 오류를 나타내는 플래그, error: set
};
struct entry symtable[SYMMAX]; //심벌 테이블 배열

int lastentry = 0; //symtable에서 새로운 라벨이 들어갈 배열 첨자 위치

void path1();
void path2();
struct oneline readline(FILE *, int);
int lookup(char *);
void insert(char *, int);
int srchop(char *); //OPCODE를 위해 OPTAB를 검색, 리턴값: 검색된 OP의 배열의 첨자 + 1, 실패시 0
int xtoi(char *);
int srchoperand(char *s);
char * file1[20];
char * file2[20];