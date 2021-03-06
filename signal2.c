#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>
void cntrl_c(int sig);

int main(){ //control_c 확인하기 위해서 설정한 무한루프
  signal(SIGINT, cntrl_c);  //SIGINT : control-c
  while(1)
    printf("무한루프\n");
  return 0;
}

  void cntrl_c(int sig){
    char * answer;
    answer = (char *)malloc(4 * sizeof(char)); //나중에 포인터로 바꾸기
    printf("%d번 신호가 발생했습니다.\n", sig);
    printf("프로그램을 종료하시겠습니까? (y/n)");
    scanf("%s", answer);
    if(!strcmp(answer, "y") || !strcmp(answer, "Y"))
      exit(1);
    }
