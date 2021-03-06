/*정적외부변수를 사용해서 특정한 함수에서만 내용을 수정하도록 함(중요한 부분임)
그리고 스택에 대한 이해도 필요하다
nm명령어는 컴파일을 하면서
size명령어는 영역의 크기가 얼마나 되는지 보여준다*/
//지금 모든 왠만한 부분에서는 이상한 부분이 없는데 리스트 파일을 만들 때 이상하게
//3번 찍히는게 좀 이상해서 그 부분을 고쳐야 됨ㅠㅠ
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<ctype.h>
#include<signal.h>

#define len 80

typedef struct linked_words{//여러 개의 문자열을 받기 위해 선언한 구조체
  char *data;//내용을 포인터 배열로 저장함
  void *com_word;//같은 문자가 있으면 비교하기 위해서 만들어 놓은 포인터
  struct linked_words *next;//다음 문자열을 노드하기 위해 선언한 자기참조 구조체
}LINK_W;

typedef struct compare{
  char *data;
  void *com_word;
}DIR;

typedef struct sorting{
  char *data;
  void *connect;
}St;
typedef struct linked_movie{//MOVIE에 해당하는 구조체를 선언함
  int serial_num;
  char *title;
  char *genre;
  DIR *director;
  int year;
  int time;
  LINK_W *actor;//actor는 1명이 아니기 때문에 또 다른 구조체를 사용해서 여려 개를 저장하려함
  struct linked_movie *next_movie;//다음 MOVIE를 노드하기 위해 선언해줌.
  //근데 이 구조체로 되지 않을 수도 있기 때문에 새로운 구조체를 선언해야 할 것 같음
}LINK_M;

typedef struct linked_director{ //director구조체 형의 내용
  int serial_num;
  char *name;
  char sex;
  int birth;
  LINK_W *title;
  struct linked_director *next_director;
}LINK_D;

typedef struct linked_actor{//ACTOR에 해당하는 구조체를 선언함
  int serial_num;
  char *name;
  char sex;
  int birth;
  LINK_W *title;
  struct linked_actor *next_actor;
}LINK_A;

typedef struct add_movie{
  LINK_M *head;
  LINK_M *tail;
  int size;
}ALL_M;

typedef struct add_actor{
  LINK_A *head;
  LINK_A *tail;
  int size;
}ALL_A;

typedef struct add_director{
  LINK_D *head;
  LINK_D *tail;
  int size;
}ALL_D;


void cntrl_c(int sig);
void make_movie_list(LINK_M **movie);
void make_actor_list(LINK_A **actor);
void make_director_list(LINK_D **director);
void input_multiple(LINK_M *movie);
void add_movie(ALL_M *movie);
int copy_movie_list();
int copy_actor_list();
int copy_director_list();
void compare_movie_word(ALL_M *movie,ALL_D *director,ALL_A *actor);
void compare_actor_word(ALL_M *movie,ALL_A *actor);
void compare_director_word(ALL_M *movie,ALL_D *director);
int compare(const void *, const void *);
int compare_num(const void *, const void *);
void compare_movie(ALL_M *movie,ALL_A *actor);
char *change(char *s, const char *a, const char *b);
char *change(char *s, const char *a, const char *b) {
  char *result, *sr;
  size_t i, count = 0;
  size_t a_length = strlen(a);
	if (a_length < 1) return s;
  size_t b_length = strlen(b);
  if (b_length != a_length) {
    for (i = 0; *(s+i) != '\0';) {
      if (memcmp((s+i), a, a_length) == 0) count++, i += a_length;
      else i++;
    }
  } else i = strlen(s);
  result = (char *) malloc(i + 1 + count * (b_length - a_length));
  if (result == NULL) return NULL;
  sr = result;
  while (*s) {
    if (memcmp(s, a, a_length) == 0) {
      memcpy(sr, b, b_length);
      sr += b_length;
      s  += a_length;
    } else *sr++ = *s++;
  }
  *sr = '\0';
  return result;
}
int compare(const void *a, const void *b){
  return(strcmp(*(char**)a,*(char**)b));
}
int compare_num(const void *a, const void *b){
  if(*(int *)a < *(int *)b)
  return -1;
  else if(*(int *)a == *(int *)b)
  return 0;
  else
  return 1;
}
int compare_w(const void *a, const void *b){
  return strcmp((char*)a,(char*)b);
}
int compare_new(const void *a,const void *b){
  St *m = (St *)a;
  St *n = (St *)b;
  return(strcmp(*(char**)m->data,*(char**)n->data));
}
void make_movie_admin(ALL_M **movie){
  *movie = (ALL_M *)malloc(sizeof(ALL_M));
  (*movie) -> head = (LINK_M *)calloc(1,sizeof(LINK_M));
  (*movie) -> tail = (LINK_M *)calloc(1,sizeof(LINK_M));
  (*movie) -> head -> next_movie = (*movie) -> tail;
  (*movie) -> tail -> next_movie = (*movie) -> tail;
  (*movie) -> size = 0;
}
void make_actor_admin(ALL_A **actor){
  *actor = (ALL_A *)malloc(sizeof(ALL_A));
  (*actor) -> head = (LINK_A *)calloc(1,sizeof(LINK_A));
  (*actor) -> tail = (LINK_A *)calloc(1,sizeof(LINK_A));
  (*actor) -> head -> next_actor = (*actor) -> tail;
  (*actor) -> tail -> next_actor = (*actor) -> tail;
  (*actor) -> size = 0;
}
void make_director_admin(ALL_D **director){
  *director = (ALL_D *)malloc(sizeof(ALL_D));
  (*director) -> head = (LINK_D *)calloc(1,sizeof(LINK_D));
  (*director) -> tail = (LINK_D *)calloc(1,sizeof(LINK_D));
  (*director) -> head -> next_director = (*director) -> tail;
  (*director) -> tail -> next_director = (*director) -> tail;
  (*director) -> size = 0;
}
void save_file(ALL_M *movie,ALL_A *actor,ALL_D *director,char tag,char *option1){
char *check_option,*filename;
int check = strlen(option1);
int i=0;
FILE *file;
filename = (char *)calloc(100,sizeof(char));
check_option = (char *)calloc(100,sizeof(char));
LINK_M *save_movie;
LINK_A *save_actor;
LINK_D *save_director;
LINK_W *save_word;
save_movie = movie -> head;
save_actor = actor -> head;
save_director = director -> head;
scanf("%s%*c",filename);
file = fopen(filename,"w");
  if(tag == 'm'){
      for(int i=0; i<check; i++){
         if(*(option1+i) == 't'){
        *(check_option+0) = 't';
      }
      if(*(option1+i) == 'g'){
        *(check_option+1) = 'g';
      }
      if(*(option1+i) == 'd'){
          *(check_option+2) = 'd';
      }
      if(*(option1+i) == 'y'){
        *(check_option+3) = 'y';
      }
      if(*(option1+i) == 'r'){
        *(check_option+4) = 'r';
      }
               if(*(option1+i) == 'a'){
         *(check_option+5) = 'a';
       }
     }
  while(save_movie -> next_movie != movie -> tail){
  save_movie = save_movie -> next_movie;
  if(*(check_option+0) == 't'){
  fprintf(file,"Title :%s\n",save_movie -> title);
  }
  if(*(check_option+1) == 'g'){
  fprintf(file,"Genre :%s\n",save_movie -> genre);
  }
  if(*(check_option+2) == 'd'){
  fprintf(file,"Director :%s\n",save_movie -> director -> data);
  }
  if(*(check_option+3) == 'y'){
  fprintf(file,"Year :%d\n",save_movie -> year);
  }
  if(*(check_option+4) == 'r'){
  fprintf(file,"Running Time :%d\n",save_movie -> time);
  }
  if(*(check_option+5) == 'a'){
  save_word = save_movie -> actor;
  i=1;
  while(save_word -> next != NULL){
  fprintf(file,"Actor %d : %s\n",i,save_word -> data);
  save_word = save_word -> next;
  i++;
  }
  fprintf(file,"Actor %d : %s\n",i,save_word -> data);
  }
}
fclose(file);
free(option1);
free(check_option);
  }
  else if(tag == 'd'){
    for(int i=0; i<check; i++){
       if(*(option1+i) == 'n'){
      *(check_option+0) = 'n';
    }
    if(*(option1+i) == 's'){
      *(check_option+1) = 's';
    }
    if(*(option1+i) == 'b'){
        *(check_option+2) = 'b';
    }
    if(*(option1+i) == 'm'){
      *(check_option+3) = 'm';
    }
   }
   while(save_director -> next_director != director -> tail){
     save_director = save_director -> next_director;
     if(*(check_option+0) == 'n'){
     fprintf(file,"Name :%s\n",save_director -> name);
     }
     if(*(check_option+1) == 's'){
     fprintf(file,"Sex :%c\n",save_director -> sex);
     }
     if(*(check_option+2) == 'b'){
     fprintf(file,"Birth :%d\n",save_director -> birth);
     }
     if(*(check_option+3) == 'm'){
       save_word = save_director -> title;
       i=1;
       while(save_word -> next != NULL){
     fprintf(file,"Movie %d :%s\n",i,save_word -> data);
     save_word = save_word -> next;
     i++;
   }
   fprintf(file,"Movie %d :%s\n",i,save_word -> data);
     }
   }
	 free(option1);
	 free(check_option);
   fclose(file);
  }
  else if(tag == 'a'){
    for(int i=0; i<check; i++){
       if(*(option1+i) == 'n'){
      *(check_option+0) = 'n';
    }
    if(*(option1+i) == 's'){
      *(check_option+1) = 's';
    }
    if(*(option1+i) == 'b'){
        *(check_option+2) = 'b';
    }
    if(*(option1+i) == 'm'){
      *(check_option+3) = 'm';
    }
   }
   while(save_actor -> next_actor != actor -> tail){
     save_actor = save_actor -> next_actor;
     if(*(check_option+0) == 'n'){
     fprintf(file,"Name :%s\n",save_actor -> name);
     }
     if(*(check_option+1) == 's'){
     fprintf(file,"Sex :%c\n",save_actor -> sex);
     }
     if(*(check_option+2) == 'b'){
     fprintf(file,"Birth :%d\n",save_actor -> birth);
     }
     if(*(check_option+3) == 'm'){
       save_word = save_actor -> title;
       i=1;
       while(save_word -> next != NULL){
     fprintf(file,"Movie %d :%s\n",i,save_word -> data);
     save_word = save_word -> next;
     i++;
   }
   fprintf(file,"Movie %d :%s\n",i,save_word -> data);
     }
   }
	 free(option1);
	 free(check_option);
   fclose(file);
  }
}
void save(ALL_M *movie,ALL_A *actor, ALL_D *director,char tag){
  FILE *file;
  LINK_W *print_word;
  LINK_M *save_movie;
  LINK_A *save_actor;
  LINK_D *save_director;
  save_actor = actor -> head;
  save_director = director -> head;
  save_movie = movie -> head;
  print_word = (LINK_W *)malloc(sizeof(LINK_W));
  if(tag == 'm'){
    file = fopen("movie_list","w");
  while(save_movie -> next_movie != movie -> tail){
    save_movie = save_movie -> next_movie;
    fprintf(file,"%d:%s:%s:%s:%d:%d:",save_movie -> serial_num,save_movie -> title,save_movie -> genre,save_movie -> director -> data,save_movie -> year,save_movie -> time);
    print_word = save_movie -> actor;
    while(print_word -> next != NULL){
      fprintf(file,"%s,",print_word->data);
      print_word = print_word->next;
    }
    fprintf(file,"%s\n", print_word->data);
  }
  fclose(file);
}
else if(tag == 'a'){
  file = fopen("actor_list","w");
  while(save_actor -> next_actor != actor -> tail){
    save_actor = save_actor -> next_actor;
    fprintf(file,"%d:%s:%c:%d",save_actor -> serial_num,save_actor -> name,save_actor ->sex,save_actor -> birth);
    print_word = save_actor -> title;
    while(print_word -> next != NULL){
      fprintf(file,"%s,",print_word -> data);
      print_word = print_word -> next;
    }
    fprintf(file,"%s\n",print_word -> data);
  }
  fclose(file);
}
else if(tag == 'd'){
  file = fopen("director_list","w");
  while(save_director -> next_director != director -> tail){
    save_director = save_director -> next_director;
    fprintf(file,"%d:%s:%c:%d",save_director -> serial_num,save_director -> name,save_director ->sex,save_director -> birth);
    print_word = save_director -> title;
    while(print_word -> next != NULL){
      fprintf(file,"%s,",print_word -> data);
      print_word = print_word -> next;
    }
    fprintf(file,"%s\n",print_word -> data);
  }
  fclose(file);
}
}
void input_multiple(LINK_M *movie){
	movie -> actor = (LINK_W *)malloc(sizeof(LINK_W));
	LINK_W *tmp;
	tmp = movie -> actor;
	char *temp;
	temp=(char*)calloc(1024, sizeof(char));
	scanf("%[^\n]", temp);
  getchar();
	tmp->data = strtok(temp, ",");
	while(tmp->data != NULL){
		char *check;
		check=(char*)malloc(sizeof(char)*30);
		check = strtok(NULL, ",");
		if(check == NULL){
		       tmp->next=NULL;
		       break;
		}
		else{
			tmp->next = (LINK_W *)malloc(sizeof(LINK_W));
			tmp = tmp->next;
			tmp->next=NULL;
			tmp->data = check;
		}
	}
}
void print_multiple_data_log_to_list(FILE *mv_l, LINK_M *movie){
	LINK_W *tmp;
	char *checking_colon;
	tmp = movie -> actor;
	while(tmp->next != NULL){
		checking_colon = change(tmp -> data,":","??;");
		fprintf(mv_l,"%s,",checking_colon);
		printf("%s,",checking_colon);
		tmp = tmp->next;
	}
	checking_colon = change(tmp -> data,":","??;");
	fprintf(mv_l, "%s\n", checking_colon);
	printf("%s\n",checking_colon);
}
void add_movie(ALL_M *movie){
  FILE *mv_l,*mv_s;
  LINK_W *save_word,*check_word;
  LINK_M *save_movie,*new_movie,*check;
  char *input_data, input_text,read_text,checking,*checking_colon;
  int input_num,i,serial = 1;
  save_movie = (LINK_M *)malloc(sizeof(LINK_M));
  check = movie -> head;
  input_data=(char *)malloc(sizeof(char)*30);
 mv_s = fopen("movie_log","r");
 if(mv_s == NULL)
 serial = 1;
 else{
 fscanf(mv_s,"%c",&read_text);
 if(read_text == 'a')
   serial++;
 while(read_text!=EOF){
     fscanf(mv_s,"%c",&read_text);
     if(read_text == '\n'){
       fscanf(mv_s,"%c",&read_text);
         if(read_text == 'a')
           serial++;
         else if(read_text != 'u' && read_text != 'd')
         break;
         }
 }
 fclose(mv_s);
}
  save_movie -> serial_num = serial;
  save_movie -> actor = (LINK_W *)malloc(sizeof(LINK_W));
  printf("title >");
  scanf("%[^\n]%*c",input_data);
  save_movie -> title = (char *)malloc(strlen(input_data)+1);
  strcpy(save_movie -> title, input_data);
  printf("genre >");
  scanf("%[^\n]%*c",input_data);
  save_movie -> genre = (char *)malloc(strlen(input_data)+1);
  strcpy(save_movie -> genre, input_data);
  printf("director >");
  save_movie -> director = (DIR *)malloc(sizeof(DIR));
  scanf("%[^\n]%*c",input_data);
  save_movie -> director -> data = (char *)malloc(strlen(input_data)+1);
  strcpy(save_movie -> director -> data, input_data);
  printf("year >");
  scanf("%d",&input_num);
  save_movie -> year = input_num;
  printf("time >");
  scanf("%d%*c",&input_num);
  save_movie -> time = input_num;
  new_movie = movie -> head;
  printf("actor >");
  input_multiple(save_movie);
  while(check -> next_movie != movie -> tail){
    check = check -> next_movie;
    if(strcmp(check -> title,save_movie -> title)==0){
      printf("@@ You have the same record in movie list\n");
      printf("%d:%s:%s:%s:",check -> serial_num, check -> title,check -> genre,check -> director -> data);
      printf("%d:%d:",check -> year,check -> time);
      check_word = check -> actor;
      while(check_word -> next != NULL){
        printf("%s,",check_word -> data);
        check_word = check_word -> next;
      }
      printf("%s\n",check_word -> data);
      printf("Do You want to add any way?(Y/N)");
      scanf("%c%*c",&checking);
      break;
    }
  }
  if(checking == 'N'||checking == 'n'){
    free(input_data);
		free(save_movie);
  }
  else{
    mv_l=fopen("movie_log","a");
  fprintf(mv_l,"add:%d:",save_movie -> serial_num);
	checking_colon = change(save_movie -> title,":","??;");
  fprintf(mv_l,"%s:",checking_colon);
	checking_colon = change(save_movie -> genre,":","??;");
  fprintf(mv_l,"%s:",checking_colon);
	checking_colon = change(save_movie -> director -> data,":","??;");
  fprintf(mv_l,"%s:",checking_colon);
  fprintf(mv_l,"%4d:",save_movie -> year);
  fprintf(mv_l,"%3d:",save_movie -> time);
  print_multiple_data_log_to_list(mv_l,save_movie);
  while(new_movie -> next_movie != movie -> tail){
  new_movie = new_movie -> next_movie;
}
  new_movie -> next_movie = save_movie;
  save_movie -> next_movie = movie -> tail;
  free(input_data);
  movie -> size++;
  fclose(mv_l);
}
}
int delete_movie(ALL_M *movie,int serial){
  LINK_M *pre_movie, *del_movie;
  FILE *mv_d;
  pre_movie = (LINK_M *)malloc(sizeof(LINK_M));
  del_movie = (LINK_M *)malloc(sizeof(LINK_M));
  pre_movie = movie -> head;
  while(pre_movie -> next_movie != movie -> tail || pre_movie -> serial_num == serial){
    if(pre_movie -> serial_num == serial){
      del_movie -> next_movie = pre_movie -> next_movie;
      mv_d = fopen("movie_log","a");
      fprintf(mv_d,"delete:%d::::::\n", serial);
      fclose(mv_d);
			free(pre_movie -> actor);
      free(pre_movie);
      movie -> size--;
      return 1;
    }
    del_movie = pre_movie;
    pre_movie = pre_movie -> next_movie;
  }
  return 0;
}
int add_movie_list(ALL_M *movie,FILE *mv_l){
  LINK_M *new_movie,*link_movie;
  LINK_W *tmp;
  char *read_data,read_text,*temp;
  int read_num;
  link_movie = (LINK_M *)malloc(sizeof(LINK_M));
  new_movie = (LINK_M *)malloc(sizeof(LINK_M));
  read_data = (char *)malloc(sizeof(char)*1000);
  new_movie -> actor = (LINK_W *)malloc(sizeof(LINK_W));
  tmp = new_movie ->actor;
  temp=(char*)calloc(1024, sizeof(char));
  fscanf(mv_l,"%*c%d",&read_num);
  new_movie -> serial_num = read_num;
  fscanf(mv_l,"%*c%[^:]",read_data);
  new_movie -> title = (char *)malloc(strlen(read_data));
	new_movie -> title = change(read_data,"??;",":");
  fscanf(mv_l,"%*c%[^:]",read_data);
  new_movie -> genre = (char *)malloc(strlen(read_data));
  new_movie -> genre = change(read_data,"??;",":");
  fscanf(mv_l,"%*c%[^:]",read_data);
  new_movie -> director = (DIR *)malloc(sizeof(DIR));
  new_movie -> director -> data = (char *)malloc(strlen(read_data));
  new_movie -> director -> data = change(read_data,"??;",":");
  fscanf(mv_l,"%*c%d",&read_num);
  new_movie -> year = read_num;
  fscanf(mv_l,"%*c%d%*c",&read_num);
  new_movie -> time = read_num;
  fscanf(mv_l,"%[^\n]%*c",temp);
  tmp->data = strtok(temp, ",");
	tmp->data = change(tmp -> data , "??;",":");
	while(tmp->data != NULL){
		char *check;
		check=(char*)malloc(sizeof(char)*30);
		check = strtok(NULL, ",");
		if(check == NULL){
		       tmp->next=NULL;
		       break;
		}
		else{
			tmp->next = (LINK_W *)malloc(sizeof(LINK_W));
			tmp = tmp->next;
			tmp->next=NULL;
			tmp->data = change(check,"??;",":");
		}
	}
  link_movie = movie -> head;
  while(link_movie -> next_movie != movie -> tail)
  link_movie = link_movie -> next_movie;
  link_movie -> next_movie = new_movie;
  new_movie -> next_movie = movie -> tail;
  free(read_data);
}
int delete_movie_list(ALL_M *movie,FILE *mv_l){
  LINK_M *pre_movie, *del_movie;
  int read_num;
  fscanf(mv_l,":%d%*[^\n]%*c",&read_num);
  pre_movie = (LINK_M *)malloc(sizeof(LINK_M));
  del_movie = (LINK_M *)malloc(sizeof(LINK_M));
  pre_movie = movie -> head;
  while(pre_movie -> next_movie != movie -> tail || pre_movie -> serial_num == read_num){
    if(pre_movie -> serial_num == read_num){
      del_movie -> next_movie = pre_movie -> next_movie;
      free(pre_movie);
      return 1;
    }
    del_movie = pre_movie;
    pre_movie = pre_movie -> next_movie;
  }
  return 0;
}
void change_num_to_string(char* str, int num){
	int cnt=1;
        int check = num;
        while(check != check%10){
                cnt++;
                check = check/10;
        }
        for(int i=0; i<cnt; i++){
                int div=1;
                for(int j=0; j<cnt-i-1; j++)
                        div *= 10;
                int res = num/div;
                char input = res+48;
                *(str+i) = input;
                num -= res*div;
        }
        *(str + cnt) = '\0';
}

void update_movie(ALL_M *movie, char *option, int serial){
	FILE *mv_l;
	mv_l = fopen("movie_log","a");
	LINK_M *tmp = movie->head;
	int state;
	LINK_M *update,*checking;
	while(tmp->next_movie != movie->tail){
		tmp = tmp->next_movie;
		if(tmp->serial_num == serial){
			update = tmp;
			break;
		}
	}
	checking = movie -> head;
	int check = strlen(option);
	char *check_option = (char*)calloc(6, sizeof(char));
	char *revise = (char*)malloc(sizeof(char)*2);
	char *str;
	str = (char*)calloc(30, sizeof(char));
	int num;
	for(int i=0; i<check; i++){
		if(*(option+i) == 't') *(check_option+0) = 't';
		if(*(option+i) == 'g') *(check_option+1) = 'g';
		if(*(option+i) == 'd') *(check_option+2) = 'd';
		if(*(option+i) == 'y') *(check_option+3) = 'y';
		if(*(option+i) == 'r') *(check_option+4) = 'r';
		if(*(option+i) == 'a') *(check_option+5) = 'a';
	}
	fprintf(mv_l, "update:%d:", serial);
	if(*(check_option+0) == 't'){
		state = 0;
	       printf("title > ");
	       scanf("%[^\n]", str);
				  getchar();
				 while(checking -> next_movie != movie -> tail){
					 checking = checking -> next_movie;
					 if((strcmp(str, checking->title))==0){
						 printf("You have the same record.\n");
						 printf("title : %s\n", checking->title);
						 printf("Do you want to revise?(Y : yes, N: no)\n");
						 scanf("%s", revise);
						 getchar();
						 if((strcmp(revise, "Y") == 0)) fprintf(mv_l, "%s:", checking->title);
						 else if((strcmp(revise, "N")==0)) fprintf(mv_l, "=:");
						 state = 1;
						 break;
					 }
				 }
	       if(state != 1) fprintf(mv_l, "%s:", str);
	}
	else if(*(check_option+0) == '\0') fprintf(mv_l, "=:");
	if(*(check_option+1) == 'g'){
		printf("genre > ");
                scanf("%[^\n]", str);
                getchar();
								while(checking -> next_movie != movie -> tail){
			 					 checking = checking -> next_movie;
			 					 if((strcmp(str, checking->title))==0){
			 						 printf("You have the same record.\n");
			 						 printf("title : %s\n", checking->title);
			 						 printf("Do you want to revise?(Y : yes, N: no)\n");
			 						 scanf("%s", revise);
			 						 getchar();
			 						 if((strcmp(revise, "Y") == 0)) fprintf(mv_l, "%s:", checking->title);
			 						 else if((strcmp(revise, "N")==0)) fprintf(mv_l, "=:");
			 						 state = 1;
			 						 break;
			 					 }
			 				 }
			 	       if(state != 1) fprintf(mv_l, "%s:", str);
	}
	else if(*(check_option+1) == '\0') fprintf(mv_l, "=:");
	if(*(check_option+2) == 'd'){
		printf("director > ");
                scanf("%[^\n]", str);
                getchar();
                if((strcmp(str, update->director->data))==0){
			printf("You have the same record.\n");
                        printf("director : %s\n", update->director->data);
                        printf("Do you want to revise?(Y : yes, N: no)\n");
                        scanf("%s", revise);
			getchar();
                        if(strcmp(revise, "Y")==0) fprintf(mv_l, "%s:", update->director->data);
			else if((strcmp(revise, "N")==0)) fprintf(mv_l, "=:");
		}
                else fprintf(mv_l, "%s:", str);
	}
	else if(*(check_option+2) == '\0') fprintf(mv_l, "=:");
	if(*(check_option+3) == 'y'){
		printf("year > ");
                scanf("%d", &num);
		char *snum = (char*)calloc(2, sizeof(char));
		change_num_to_string(snum, num);
                if(update->year == num){
			printf("You have the same record.\n");
                        printf("year : %d\n", update->year);
                        printf("Do you want to revise?(Y : yes, N: no)\n");
                        scanf("%s", revise);
			getchar();
                        if(strcmp(revise, "Y")==0) fprintf(mv_l, "%d:", update->year);
			else if((strcmp(revise, "N")==0)) fprintf(mv_l, "=:");
		}
                else fprintf(mv_l, "%d:", num);
	}
	else if(*(check_option+3) == '\0') fprintf(mv_l, "=:");
	if(*(check_option+4) == 'r'){
		printf("running time > ");
                scanf("%d", &num);
		char *snum = (char*)calloc(2, sizeof(char));
		change_num_to_string(snum, num);
                if(update->time == num){
			printf("You have the same record.\n");
                        printf("running time : %d\n", update->time);
                        printf("Do you want to revise?(Y : yes, N: no)\n");
                        scanf("%s", revise);
			getchar();
                        if(strcmp(revise, "Y")==0) fprintf(mv_l, "%d:", update->time);
			else if((strcmp(revise, "N")==0)) fprintf(mv_l, "=:");
		}
                else fprintf(mv_l, "%d:", num);
	}
	else if(*(check_option+4) == '\0') fprintf(mv_l, "=:");
        if(*(check_option+5) == 'a'){
	       printf("actor > ");

	       LINK_M *ttmp = (LINK_M*)malloc(sizeof(LINK_M));
	       input_multiple(ttmp);

	       int a=1, b=1;
	       LINK_W *new;
	       new = ttmp->actor;

	       while(new ->next != NULL){
		       a++;
		      new = new->next;
	       }
	       new = ttmp->actor;
	       LINK_W *org;
	       org = update->actor;
	       while(org->next != NULL){
		       b++;
		      org = org->next;
	       }
	       org = update->actor;
	       if(a != b){
		       while(new->next != NULL){
			       fprintf(mv_l, "%s,", new->data);
			       new = new->next;
		       }
		       fprintf(mv_l, "%s\n", new->data);
	       }
	       else{
		       char *gar1 = (char*)calloc(200, sizeof(char));
		       char *gar2 = (char*)calloc(200, sizeof(char));
		       while(new->next != NULL){
			       strcpy(gar1+strlen(gar1), new->data);
			       strcat(gar1, ",");
			       new = new->next;
		       }
		       strcat(gar1, new->data);
		       while(org->next != NULL){
			       strcpy(gar2+strlen(gar2), org->data);
			       strcat(gar2, ",");
			       org = org->next;
		       }
		       strcat(gar2, org->data);
		       new = ttmp->actor;
		       org = update->actor;
		       if(strcmp(gar1, gar2)==0){
			       printf("You have the same records.\n");
			       printf("actors : %s\n", gar1);
			       printf("Do you want to revise?(Y : yes, N : no)\n");
			       scanf("%s", revise);
			       getchar();
			       if(strcmp(revise, "Y")==0){
				       while(org->next != NULL){
					       fprintf(mv_l, "%s,", org->data);
					       org = org->next;
				       }
				       fprintf(mv_l, "%s\n", org->data);
			       }
			       else if(strcmp(revise, "N")==0) fprintf(mv_l, "=\n");
		       }
		       else{
			       while(new->next != NULL){
				       fprintf(mv_l, "%s,", new->data);
				       new = new->next;
			       }
			       fprintf(mv_l, "%s\n", new->data);
		       }
	       }

	}
	else if(*(check_option+5) == '\0') fprintf(mv_l, "=\n");
	fclose(mv_l);
}

void update_movie_list(ALL_M *movie,FILE *mv_l){
	char *read_data = (char*)calloc(30, sizeof(char));
	int read_num;
	fscanf(mv_l, "%*c%d", &read_num);

	LINK_M *tmp = (LINK_M*)malloc(sizeof(LINK_M));
	tmp = movie->head;
	LINK_M *update = (LINK_M*)malloc(sizeof(LINK_M));
	while(tmp->next_movie != movie->tail){
		tmp = tmp->next_movie;
		if(tmp->serial_num == read_num){
			update = tmp;
			break;
		}
	}
	update->serial_num = read_num;

	fscanf(mv_l, "%*c%[^:]", read_data);
	if(strcmp(read_data, "=")!=0)
		strcpy(update->title, read_data);

	fscanf(mv_l, "%*c%[^:]", read_data);
	if(strcmp(read_data, "=")!=0)
		strcpy(update->genre, read_data);

	fscanf(mv_l, "%*c%[^:]", read_data);
	if(strcmp(read_data, "=")!=0)
		strcpy(update->director->data, read_data);

	fscanf(mv_l, "%*c%[^:]", read_data);
	if(strcmp(read_data, "=")!=0){
		read_num = atoi(read_data);
		update->year = read_num;
	}

	fscanf(mv_l, "%*c%[^:]%*c", read_data);
	if(strcmp(read_data, "=")!=0){
		read_num = atoi(read_data);
		update->time = read_num;
	}

	char *temp = (char*)calloc(300, sizeof(char));
	fscanf(mv_l, "%[^\n]%*c", temp);
	if(strcmp(temp, "=")!=0){
		LINK_W *h = (LINK_W*)malloc(sizeof(LINK_W));
		LINK_W *l = (LINK_W*)malloc(sizeof(LINK_W));
		h = update->actor;
		while(h->next != NULL){
			LINK_W *d = (LINK_W*)malloc(sizeof(LINK_W));
			d = h;
			l = d->next;
			free(d->data);
			free(d);
			h = l;
		}
		free(l->data);
		free(l);
		update->actor = (LINK_W *)malloc(sizeof(LINK_W));
		h->data = strtok(temp, ",");
		while(h->data != NULL){
			char *check = (char*)malloc(sizeof(char)*30);
			check = strtok(NULL, ",");
			if(check == NULL){
				h->next = NULL;
				break;
			}
			else{
				h->next = (LINK_W *)malloc(sizeof(LINK_W));
				h = h->next;
				h->next = NULL;
				h->data = check;
			}
		}
	}
}
int scan_movie_log(ALL_M *movie){
  LINK_M *save_movie;
  LINK_W *save_word,*last_word;
  FILE *mv_l;
  char *input_data,input_text,read_text,garbage;
  int input_num,serial,i;
  mv_l=fopen("movie_log","r");
  save_movie=(LINK_M *)malloc(sizeof(LINK_M));
  input_data=(char *)malloc(sizeof(char)*30);
  while(1){
    if(mv_l == NULL){
      return 1;
      break;
    }
  fscanf(mv_l,"%[^:]u",input_data);
  if(strcmp(input_data,"add")==0){
    add_movie_list(movie,mv_l);
    movie -> size++;
    read_text=fgetc(mv_l);
    fseek(mv_l,-1,SEEK_CUR);
    if(read_text==EOF)
    break;
  }
  else if(strcmp(input_data,"update")==0){
		update_movie_list(movie,mv_l);
    read_text=fgetc(mv_l);
    fseek(mv_l,-1,SEEK_CUR);
    if(read_text==EOF)
    break;
  }
  else if(strcmp(input_data,"delete")==0){
    delete_movie_list(movie,mv_l);
    movie -> size--;
    read_text=fgetc(mv_l);
    fseek(mv_l,-1,SEEK_CUR);
    if(read_text==EOF)
    break;
  }
  else
  break;
}
FILE *file;
file = fopen("movie_list","w");
LINK_W *print_word;
LINK_M *print_movie;
print_movie = movie -> head;
print_word = (LINK_W *)malloc(sizeof(LINK_W));
    while(print_movie -> next_movie != movie -> tail){
      print_movie = print_movie -> next_movie;
      fprintf(file,"%d:%s:%s:%s:%d:%d:",print_movie -> serial_num,print_movie -> title,print_movie -> genre,print_movie -> director -> data,print_movie -> year,print_movie -> time);
      print_word = print_movie -> actor;
      while(print_word -> next != NULL){
        fprintf(file,"%s,",print_word->data);
        print_word = print_word->next;
      }
      fprintf(file,"%s\n", print_word->data);
    }
      fclose(file);
return 0;
}

int print_movie_list(ALL_M *movie,int serial){
  LINK_M *print_movie;
  LINK_W *print_word;
  LINK_D *print_director;
  LINK_A *print_actor;
  int i=1;
  print_movie=(LINK_M*)malloc(sizeof(LINK_M));
  print_movie=movie -> head;
  print_word = (LINK_W *)malloc(sizeof(LINK_W));
  while(print_movie -> next_movie != movie -> tail){
    print_movie = print_movie -> next_movie;
    if(print_movie -> serial_num == serial){
      print_director = (LINK_D*)print_movie -> director -> com_word;
    printf("  %d, %s, %s\n",print_movie -> serial_num,print_movie -> title,print_movie -> genre);
    if(print_movie -> director -> com_word == NULL)
    printf(" D: %s (  -  )\n",print_movie -> director -> data);
    else{
      printf(" D: %s (%d)\n",print_movie -> director -> data,print_director -> birth);
    }
    print_word = print_movie -> actor;
    while(print_word -> next != NULL){
      print_actor = (LINK_A *)print_word -> com_word;
      if(print_word -> com_word == NULL)
  		printf("  A%d : %s (  -  )\n",i,print_word->data);
      else
      printf("  A%d : %s (%d)\n",i,print_word->data,print_actor -> birth );
  		print_word = print_word->next;
      i++;
  	}
    print_actor = (LINK_A *)print_word -> com_word;
    if(print_word -> com_word == NULL)
    printf("  A%d : %s (  -  )\n",i,print_word->data);
    else
    printf("  A%d : %s (%d)\n",i,print_word->data,print_actor -> birth);
    return 1;
  }
}
}

void input_multiple_act(LINK_A *actor){
	actor -> title = (LINK_W *)malloc(sizeof(LINK_W));
	LINK_W *tmp;
	tmp = actor -> title;
	char *temp;
	temp=(char*)calloc(1024, sizeof(char));
	scanf("%[^\n]%*c", temp);
	tmp->data = strtok(temp, ",");
	while(tmp->data != NULL){
		char *check;
		check=(char*)malloc(sizeof(char)*30);
		check = strtok(NULL, ",");
		if(check == NULL){
		       tmp->next=NULL;
		       break;
		}
		else{
			tmp->next = (LINK_W *)malloc(sizeof(LINK_W));
			tmp = tmp->next;
			tmp->next=NULL;
			tmp->data = check;
		}
	}
}
void print_multiple_data_log_to_list_act(FILE *act_l, LINK_A *actor){
	LINK_W *tmp;
	char *checking_colon;
	tmp = actor->title;
	while(tmp->next != NULL){
		checking_colon = change(tmp->data,":","??;");
		fprintf(act_l,"%s,",checking_colon);
		tmp = tmp->next;
	}
	checking_colon = change(tmp->data,":","??;");
	fprintf(act_l, "%s\n",checking_colon);
}

void add_actor(ALL_A *actor){
  FILE *act_l,*act_s;
  LINK_W *save_word,*check_word;
  LINK_A *save_actor,*new_actor,*check;
  char *input_data, input_text,read_text,checking,*checking_colon;
  int input_num,i,serial = 1;
  save_actor = (LINK_A *)malloc(sizeof(LINK_A));
  check = actor -> head;
  input_data=(char *)malloc(sizeof(char)*30);
  act_s = fopen("actor_log","r");
  if(act_s == NULL)
  serial = 1;
  else{
  fscanf(act_s,"%c",&read_text);
  if(read_text == 'a')
   serial++;
  while(read_text!=EOF){
     fscanf(act_s,"%c",&read_text);
     if(read_text == '\n'){
       fscanf(act_s,"%c",&read_text);
         if(read_text == 'a')
           serial++;
         else if(read_text != 'u' && read_text != 'd')
         break;
         }
  }
  fclose(act_s);
  }
  save_actor -> serial_num = serial;
  printf("name >");
  scanf("%[^\n]%*c",input_data);
  save_actor -> name = (char *)malloc(strlen(input_data)+1);
  strcpy(save_actor -> name, input_data);
  printf("sex >");
  scanf("%c%*c",&input_text);
  save_actor -> sex = input_text;
  printf("birth >");
  scanf("%d%*c",&input_num);
  save_actor -> birth = input_num;
  printf("best_movies >");
  input_multiple_act(save_actor);
  while(check -> next_actor != actor -> tail){
    check = check -> next_actor;
    if(strcmp(check -> name,save_actor -> name)==0){
      printf("@@ You have the same record in actor list\n");
      printf("%d:%s:%c:%d:",check -> serial_num,check -> name,check -> sex, check -> birth);
      check_word = check -> title;
      while(check_word -> next != NULL){
        printf("%s,",check_word -> data);
        check_word = check_word -> next;
      }
      printf("%s\n",check_word -> data);
      printf("Do You want to add any way?(Y/N)");
      scanf("%c%*c",&checking);
      break;
    }
  }
  if(checking == 'N'||checking == 'n'){
    free(input_data);
  }
  else{
    act_l=fopen("actor_log","a");
  fprintf(act_l,"add:%d:",save_actor -> serial_num);
	checking_colon = change(save_actor -> name, ":","??;");
  fprintf(act_l,"%s:",checking_colon);
  fprintf(act_l,"%c:",save_actor ->sex);
  fprintf(act_l,"%8d:",save_actor -> birth);
  print_multiple_data_log_to_list_act(act_l,save_actor);
  new_actor = actor -> head;
  while(new_actor -> next_actor != actor -> tail)
  new_actor = new_actor -> next_actor;
  new_actor -> next_actor = save_actor;
  save_actor -> next_actor = actor -> tail;
  free(input_data);
  actor -> size ++;
  fclose(act_l);
  }
}
int delete_actor(ALL_A *actor,int serial){
  LINK_A *pre_actor, *del_actor;
  FILE *act_d;
  pre_actor = (LINK_A *)malloc(sizeof(LINK_A));
  del_actor = (LINK_A *)malloc(sizeof(LINK_A));
  pre_actor = actor -> head;
  while(pre_actor -> next_actor != actor -> tail || pre_actor -> serial_num == serial){
    if(pre_actor -> serial_num == serial){
      del_actor -> next_actor = pre_actor -> next_actor;
      act_d = fopen("actor_log","a");
      fprintf(act_d,"delete:%d::::::\n", serial);
      fclose(act_d);
      free(pre_actor);
      actor -> size --;
      return 1;
    }
    del_actor = pre_actor;
    pre_actor = pre_actor -> next_actor;
  }
  return 0;
}
int add_actor_list(ALL_A *actor,FILE *act_l){
  LINK_A *new_actor,*link_actor;
  LINK_W *tmp;
  char *read_data,read_text,*temp;
  int read_num;
  link_actor = (LINK_A *)malloc(sizeof(LINK_A));
  new_actor = (LINK_A *)malloc(sizeof(LINK_A));
  read_data = (char *)malloc(sizeof(char)*1000);
  new_actor -> title = (LINK_W *)malloc(sizeof(LINK_W));
  tmp = new_actor ->title;
  temp=(char*)calloc(1024, sizeof(char));
  fscanf(act_l,"%*c%d",&read_num);
  new_actor -> serial_num = read_num;
  fscanf(act_l,"%*c%[^:]",read_data);
  new_actor -> name = (char *)malloc(strlen(read_data));
	new_actor -> name = change(read_data,"??;",":");
  fscanf(act_l,"%*c%c",&read_text);
  new_actor -> sex = read_text;
  fscanf(act_l,"%*c%d:",&read_num);
  new_actor -> birth = read_num;
  fscanf(act_l,"%[^\n]%*c",temp);
  tmp->data = strtok(temp, ",");
	tmp -> data = change(tmp -> data,"??;",":");
	while(tmp->data != NULL){
		char *check;
		check=(char*)malloc(sizeof(char)*30);
		check = strtok(NULL, ",");
		if(check == NULL){
		       tmp->next=NULL;
		       break;
		}
		else{
			tmp->next = (LINK_W *)malloc(sizeof(LINK_W));
			tmp = tmp->next;
			tmp->next=NULL;
			tmp->data = change(check,"??;",":");
		}
	}
  link_actor = actor -> head;
  while(link_actor -> next_actor != actor -> tail)
  link_actor = link_actor -> next_actor;
  link_actor -> next_actor = new_actor;
  new_actor -> next_actor = actor -> tail;
  free(read_data);
  if(read_text=='\n')
  return 1;
}
int delete_actor_list(ALL_A *actor,FILE *act_l){
  LINK_A *pre_actor, *del_actor;
  int read_num;
  fscanf(act_l,":%d%*[^\n]%*c",&read_num);
  pre_actor = (LINK_A *)malloc(sizeof(LINK_A));
  del_actor = (LINK_A *)malloc(sizeof(LINK_A));
  pre_actor = actor -> head;
  while(pre_actor -> next_actor != actor -> tail || pre_actor -> serial_num == read_num){
    if(pre_actor -> serial_num == read_num){
      del_actor -> next_actor = pre_actor -> next_actor;
      free(pre_actor);
      return 1;
    }
    del_actor = pre_actor;
    pre_actor = pre_actor -> next_actor;
  }
  return 0;
}
void update_actor(ALL_A *actor, char *option, int serial){
	FILE *act_l;
	act_l = fopen("actor_log","a");
	LINK_A *tmp = actor->head;
	LINK_A *update,*checking;
  int state;
	while(tmp->next_actor != actor->tail){
		tmp = tmp->next_actor;
		if(tmp->serial_num == serial){
			update = tmp;
			break;
		}
	}
	printf("%d\n",update -> serial_num);
  checking = actor -> head;
	int check = strlen(option);
	char *check_option = (char*)calloc(4, sizeof(char));
	char *revise = (char*)malloc(sizeof(char)*2);
	char *str;
	char c;
	str = (char*)calloc(30, sizeof(char));
	int num;
	for(int i=0; i<check; i++){
		if(*(option+i) == 'n') *(check_option+0) = 'n';
		if(*(option+i) == 's') *(check_option+1) = 's';
		if(*(option+i) == 'b') *(check_option+2) = 'b';
		if(*(option+i) == 'm') *(check_option+3) = 'm';
	}
	fprintf(act_l, "update:%d:", serial);
	if(*(check_option+0) == 'n'){
		state = 0;
	       printf("name > ");
	       scanf("%[^\n]", str);
	       getchar();
         while(checking -> next_actor != actor -> tail){
					 checking = checking -> next_actor;
					 if((strcmp(str, checking->name))==0){
						 printf("You have the same record.\n");
						 printf("title : %s\n", checking->name);
						 printf("Do you want to revise?(Y : yes, N: no)\n");
						 scanf("%s%*c", revise);
						 if((strcmp(revise, "Y") == 0)) fprintf(act_l, "%s:", checking->name);
						 else if((strcmp(revise, "N")==0)) fprintf(act_l, "=:");
						 state = 1;
						 break;
					 }
				 }
	       if(state != 1) fprintf(act_l, "%s:", str);
	}
	else if(*(check_option+0) == '\0') fprintf(act_l, "=:");
	if(*(check_option+1) == 's'){
		printf("sex > ");
                scanf("%c", c);
                if(c == update->sex){
			printf("You have the same record.\n");
                        printf("sex : %c\n", update->sex);
                        printf("Do you want to revise?(Y : yes, N: no)\n");
                        scanf("%s%*c", revise);
                        if(strcmp(revise, "Y")==0) fprintf(act_l, "%c:", update->sex);
			else if((strcmp(revise, "N")==0)) fprintf(act_l, "=:");
		}
                else fprintf(act_l, "%c:", c);
	}
	else if(*(check_option+1) == '\0') fprintf(act_l, "=:");
	if(*(check_option+2) == 'b'){
		printf("birth > ");
                scanf("%d", &num);
		char *snum = (char*)calloc(2, sizeof(char));
		change_num_to_string(snum, num);
                if(update->birth == num){
			printf("You have the same record.\n");
                        printf("birth : %d\n", update->birth);
                        printf("Do you want to revise?(Y : yes, N: no)\n");
                        scanf("%s", revise);
												getchar();
                        if(strcmp(revise, "Y")==0) fprintf(act_l, "%d:", update->birth);
			else if((strcmp(revise, "N")==0)) fprintf(act_l, "=:");
		}
                else fprintf(act_l, "%d:", num);
	}
	else if(*(check_option+2) == '\0') fprintf(act_l, "=:");
        if(*(check_option+3) == 'm'){
	       printf("movies > ");

	       LINK_A *ttmp = (LINK_A*)malloc(sizeof(LINK_A));
	       input_multiple_act(ttmp);

	       int a=1, b=1;
	       LINK_W *new;
	       new = ttmp->title;

	       while(new ->next != NULL){
		       a++;
		      new = new->next;
	       }
	       new = ttmp->title;
	       LINK_W *org;
	       org = update->title;
	       while(org->next != NULL){
		       b++;
		      org = org->next;
	       }
	       org = update->title;
	       if(a != b){
		       while(new->next != NULL){
			       fprintf(act_l, "%s,", new->data);
			       new = new->next;
		       }
		       fprintf(act_l, "%s\n", new->data);
	       }
	       else{
		       char *gar1 = (char*)calloc(200, sizeof(char));
		       char *gar2 = (char*)calloc(200, sizeof(char));
		       while(new->next != NULL){
			       strcpy(gar1+strlen(gar1), new->data);
			       strcat(gar1, ",");
			       new = new->next;
		       }
		       strcat(gar1, new->data);
		       while(org->next != NULL){
			       strcpy(gar2+strlen(gar2), org->data);
			       strcat(gar2, ",");
			       org = org->next;
		       }
		       strcat(gar2, org->data);
		       new = ttmp->title;
		       org = update->title;
		       if(strcmp(gar1, gar2)==0){
			       printf("You have the same records.\n");
			       printf("movies : %s\n", gar1);
			       printf("Do you want to revise?(Y : yes, N : no)\n");
			       scanf("%s%*c", revise);
			       if(strcmp(revise, "Y")==0){
				       while(org->next != NULL){
					       fprintf(act_l, "%s,", org->data);
					       org = org->next;
				       }
				       fprintf(act_l, "%s\n", org->data);
			       }
			       else if(strcmp(revise, "N")==0) fprintf(act_l, "=\n");
		       }
		       else{
			       while(new->next != NULL){
				       fprintf(act_l, "%s,", new->data);
				       new = new->next;
			       }
			       fprintf(act_l, "%s\n", new->data);
		       }
	       }

	}
	else if(*(check_option+3) == '\0') fprintf(act_l, "=\n");
	fclose(act_l);
}

void update_actor_list(ALL_A *actor,FILE *act_l){
	char *read_data = (char*)calloc(30, sizeof(char));
	int read_num;
	char read_one;
	fscanf(act_l, "%*c%d", &read_num);

	LINK_A *tmp = (LINK_A*)malloc(sizeof(LINK_A));
	tmp = actor->head;
	LINK_A *update = (LINK_A*)malloc(sizeof(LINK_A));
	while(tmp->next_actor != actor->tail){
		tmp = tmp->next_actor;
		if(tmp->serial_num == read_num){
			update = tmp;
			break;
		}
	}
	update->serial_num = read_num;

	fscanf(act_l, "%*c%[^:]", read_data);
	if(strcmp(read_data, "=")!=0)
		strcpy(update->name, read_data);

	fscanf(act_l, "%*c%c", &read_one);
	if(read_one != '=')
		update->sex = read_one;

	fscanf(act_l, "%*c%[^:]", read_data);
	if(strcmp(read_data, "=")!=0){
		read_num = atoi(read_data);
		update->birth = read_num;
	}

	char *temp = (char*)calloc(300, sizeof(char));
	fscanf(act_l, "%[^\n]%*c", temp);
	if(strcmp(temp, "=")!=0){
		LINK_W *h = (LINK_W*)malloc(sizeof(LINK_W));
		LINK_W *l = (LINK_W*)malloc(sizeof(LINK_W));
		h = update->title;
		while(h->next != NULL){
			LINK_W *d = (LINK_W*)malloc(sizeof(LINK_W));
			d = h;
			l = d->next;
			free(d->data);
			free(d);
			h = l;
		}
		free(l->data);
		free(l);
		update->title = (LINK_W *)malloc(sizeof(LINK_W));
		h->data = strtok(temp, ",");
		while(h->data != NULL){
			char *check = (char*)malloc(sizeof(char)*30);
			check = strtok(NULL, ",");
			if(check == NULL){
				h->next = NULL;
				break;
			}
			else{
				h->next = (LINK_W *)malloc(sizeof(LINK_W));
				h = h->next;
				h->next = NULL;
				h->data = check;
			}
		}
	}
}

int scan_actor_log(ALL_A *actor){
  LINK_A *save_actor;
  LINK_W *save_word,*last_word;
  FILE *act_l;
  char *input_data,input_text,read_text,garbage;
  int input_num,serial,i=1;
  act_l=fopen("actor_log","r");
  save_actor=(LINK_A *)malloc(sizeof(LINK_A));
  input_data=(char *)malloc(sizeof(char)*30);
  while(1){
    if(act_l == NULL){
      return 1;
      break;
    }
  fscanf(act_l,"%[^:]",input_data);
  if(strcmp(input_data,"add")==0){
    add_actor_list(actor,act_l);
    actor -> size ++;
    i++;
    read_text=fgetc(act_l);
    fseek(act_l,-1,SEEK_CUR);
    if(read_text==EOF)
    break;
  }
  else if(strcmp(input_data,"update")==0){
		update_actor_list(actor,act_l);
    read_text=fgetc(act_l);
    fseek(act_l,-1,SEEK_CUR);
    if(read_text==EOF)
    break;
  }
  else if(strcmp(input_data,"delete")==0){
    delete_actor_list(actor,act_l);
    actor -> size --;
    read_text=fgetc(act_l);
    fseek(act_l,-1,SEEK_CUR);
    if(read_text==EOF)
    break;
  }
  else
  break;
}
FILE *file;
file = fopen("actor_list","w");
LINK_W *print_word;
LINK_A *print_actor;
print_actor = actor -> head;
print_word = (LINK_W *)malloc(sizeof(LINK_W));
    while(print_actor -> next_actor != actor -> tail){
      print_actor = print_actor -> next_actor;
      fprintf(file,"%d:%s:%c:%d:",print_actor -> serial_num,print_actor -> name,print_actor -> sex,print_actor -> birth);
      print_word = print_actor -> title;
      while(print_word -> next != NULL){
        fprintf(file,"%s,",print_word->data);
        print_word = print_word->next;
      }
      fprintf(file,"%s\n", print_word->data);
    }
      fclose(file);
return 0;
}
int print_actor_list(ALL_A *actor,int serial){
  LINK_A *print_actor;
  LINK_W *print_word;
  LINK_M *print_movie;
  int i=1;
  print_actor=(LINK_A*)malloc(sizeof(LINK_A));
  print_actor=actor -> head;
  print_word = (LINK_W *)malloc(sizeof(LINK_W));
  while(print_actor -> next_actor != actor -> tail){
    print_actor = print_actor -> next_actor;
    if(print_actor -> serial_num == serial){
    printf("  %d, %s, %c,  %8d\n",print_actor -> serial_num,print_actor -> name,print_actor -> sex,print_actor -> birth);
    print_word = print_actor -> title;
    while(print_word -> next != NULL){
      print_movie = (LINK_M *)print_word -> com_word;
      if(print_movie == NULL)
  		printf("  M%d : %s(  -  )\n",i,print_word->data);
      else
      printf("  M%d : %s(%d,%d)\n",i,print_word->data,print_movie -> year,print_movie -> time);
  		print_word = print_word->next;
      i++;
  	}
    print_movie = (LINK_M *)print_word -> com_word;
    if(print_movie == NULL)
    printf("  M%d : %s(  -  )\n",i,print_word->data);
    else
    printf("  M%d : %s(%d,%d)\n",i,print_word->data,print_movie -> year,print_movie -> time);
    break;
  }
}
return 1;
}
void input_multiple_dir(LINK_D *director){
	director -> title = (LINK_W *)malloc(sizeof(LINK_W));
	LINK_W *tmp;
	tmp = director -> title;
	char *temp;
	temp=(char*)calloc(1024, sizeof(char));
	scanf("%[^\n]%*c", temp);
	tmp->data = strtok(temp, ",");
	while(tmp->data != NULL){
		char *check;
		check=(char*)malloc(sizeof(char)*30);
		check = strtok(NULL, ",");
		if(check == NULL){
		       tmp->next=NULL;
		       break;
		}
		else{
			tmp->next = (LINK_W *)malloc(sizeof(LINK_W));
			tmp = tmp->next;
			tmp->next=NULL;
			tmp->data = check;
		}
	}
}
void print_multiple_data_log_to_list_dir(FILE *dir_l, LINK_D *director){
	LINK_W *tmp;
	char *checking_colon;
	tmp = director->title;
	while(tmp->next != NULL){
		checking_colon = change(tmp->data,":","??;");
		fprintf(dir_l,"%s,",checking_colon);
		tmp = tmp->next;
	}
	checking_colon = change(tmp->data,":","??;");
	fprintf(dir_l, "%s\n",checking_colon);
}

void add_director(ALL_D *director){
  FILE *dir_l,*dir_s;
  LINK_W *save_word,*check_word;
  LINK_D *save_director,*new_director,*check;
  char *input_data, input_text,read_text,checking,*checking_colon;
  int input_num,i,serial = 1;
  save_director = (LINK_D *)malloc(sizeof(LINK_D));
  check = director -> head;
  input_data=(char *)malloc(sizeof(char)*30);
  dir_s = fopen("director_log","r");
  if(dir_s == NULL)
  serial = 1;
  else{
  fscanf(dir_s,"%c",&read_text);
  if(read_text == 'a')
   serial++;
  while(read_text!=EOF){
     fscanf(dir_s,"%c",&read_text);
     if(read_text == '\n'){
       fscanf(dir_s,"%c",&read_text);
         if(read_text == 'a')
           serial++;
         else if(read_text != 'u' && read_text != 'd')
         break;
         }
  }
  fclose(dir_s);
  }
  save_director -> serial_num = serial;
  printf("name >");
  scanf("%[^\n]%*c",input_data);
  save_director -> name = (char *)malloc(strlen(input_data)+1);
  strcpy(save_director -> name, input_data);
  printf("sex >");
  scanf("%c%*c",&input_text);
  save_director -> sex = input_text;
  printf("birth >");
  scanf("%d%*c",&input_num);
  save_director -> birth = input_num;
  printf("best_movies >");
  input_multiple_dir(save_director);
  while(check -> next_director != director -> tail){
    check = check -> next_director;
    if(strcmp(check -> name,save_director -> name)==0){
      printf("@@ You have the same record in director list\n");
      printf("%d:%s:%c:%d:",check -> serial_num,check -> name,check -> sex, check -> birth);
      check_word = check -> title;
      while(check_word -> next != NULL){
        printf("%s,",check_word -> data);
        check_word = check_word -> next;
      }
      printf("%s\n",check_word -> data);
      printf("Do You want to add any way?(Y/N)");
      scanf("%c%*c",&checking);
      break;
    }
  }
  if(checking == 'N'||checking == 'n'){
    free(input_data);
  }
  else{
    dir_l=fopen("director_log","a");
  fprintf(dir_l,"add:%d:",save_director -> serial_num);
	checking_colon = change(save_director -> name, ":","??;");
  fprintf(dir_l,"%s:",checking_colon);
  fprintf(dir_l,"%c:",save_director ->sex);
  fprintf(dir_l,"%8d:",save_director -> birth);
  print_multiple_data_log_to_list_dir(dir_l,save_director);
  new_director = director -> head;
  while(new_director -> next_director != director -> tail)
  new_director = new_director -> next_director;
  new_director -> next_director = save_director;
  save_director -> next_director = director -> tail;
  free(input_data);
  director -> size ++;
  fclose(dir_l);
  }
}
int delete_director(ALL_D *director,int serial){
  LINK_D *pre_director, *del_director;
  FILE *dir_d;
  pre_director = (LINK_D *)malloc(sizeof(LINK_D));
  del_director = (LINK_D *)malloc(sizeof(LINK_D));
  pre_director = director -> head;
  while(pre_director -> next_director != director -> tail || pre_director -> serial_num == serial){
    if(pre_director -> serial_num == serial){
      del_director -> next_director = pre_director -> next_director;
      dir_d = fopen("director_log","a");
      fprintf(dir_d,"delete:%d::::::\n", serial);
      fclose(dir_d);
			free(pre_director -> title);
      free(pre_director);
      return 1;
      director -> size--;
    }
    del_director = pre_director;
    pre_director = pre_director -> next_director;
  }
  return 0;
}
int add_director_list(ALL_D *director,FILE *dir_l){
  LINK_D *new_director,*link_director;
  LINK_W *tmp;
  char *read_data,read_text,*temp;
  int read_num;
  link_director = (LINK_D *)malloc(sizeof(LINK_D));
  new_director = (LINK_D *)malloc(sizeof(LINK_D));
  read_data = (char *)malloc(sizeof(char)*1000);
  new_director -> title = (LINK_W *)malloc(sizeof(LINK_W));
  tmp = new_director ->title;
  temp=(char*)calloc(1024, sizeof(char));
  fscanf(dir_l,"%*c%d",&read_num);
  new_director -> serial_num = read_num;
  fscanf(dir_l,"%*c%[^:]s",read_data);
  new_director -> name = (char *)malloc(strlen(read_data));
	new_director -> name = change(read_data,"??;",":");
  fscanf(dir_l,"%*c%c",&read_text);
  new_director -> sex = read_text;
  fscanf(dir_l,"%*c%d:",&read_num);
  new_director -> birth = read_num;
  fscanf(dir_l,"%[^\n]%*c",temp);
  tmp->data = strtok(temp, ",");
	tmp -> data = change(tmp -> data,"??;",":");
	while(tmp->data != NULL){
		char *check;
		check=(char*)malloc(sizeof(char)*30);
		check = strtok(NULL, ",");
		if(check == NULL){
		       tmp->next=NULL;
		       break;
		}
		else{
			tmp->next = (LINK_W *)malloc(sizeof(LINK_W));
			tmp = tmp->next;
			tmp->next=NULL;
			tmp->data = change(check,"??;",":");
		}
	}
  link_director = director -> head;
  while(link_director -> next_director != director -> tail)
  link_director = link_director -> next_director;
  link_director -> next_director = new_director;
  new_director -> next_director = director -> tail;
  free(read_data);
  if(read_text=='\n')
  return 1;
}

int delete_director_list(ALL_D *director,FILE *dir_l){
  LINK_D *pre_director, *del_director;
  int read_num;
  fscanf(dir_l,":%d%*[^\n]%*c",&read_num);
  pre_director = (LINK_D *)malloc(sizeof(LINK_D));
  del_director = (LINK_D *)malloc(sizeof(LINK_D));
  pre_director = director -> head;
  while(pre_director -> next_director != director -> tail || pre_director -> serial_num == read_num){
    if(pre_director -> serial_num == read_num){
      del_director -> next_director = pre_director -> next_director;
      free(pre_director);
      return 1;
    }
    del_director = pre_director;
    pre_director = pre_director -> next_director;
  }
  return 0;
}
void update_director(ALL_D *director, char *option, int serial){
	FILE *dir_l;
	dir_l = fopen("director_log","a");
	LINK_D *tmp = director->head;
	LINK_D *update;
	while(tmp->next_director != director->tail){
		tmp = tmp->next_director;
		if(tmp->serial_num == serial){
			update = tmp;
			break;
		}
	}
	printf("%d\n",update -> serial_num);
	int check = strlen(option);
	char *check_option = (char*)calloc(4, sizeof(char));
	char *revise = (char*)malloc(sizeof(char)*2);
	char *str;
	char c;
	str = (char*)calloc(30, sizeof(char));
	int num;
	for(int i=0; i<check; i++){
		if(*(option+i) == 'n') *(check_option+0) = 'n';
		if(*(option+i) == 's') *(check_option+1) = 's';
		if(*(option+i) == 'b') *(check_option+2) = 'b';
		if(*(option+i) == 'm') *(check_option+3) = 'm';
	}
	fprintf(dir_l, "update:%d:", serial);
	if(*(check_option+0) == 'n'){
	       printf("name > ");
	       scanf("%[^\n]", str);
	       getchar();
	       if((strcmp(str, update->name))==0){
		       printf("You have the same record.\n");
		       printf("name : %s\n", update->name);
		       printf("Do you want to revise?(Y : yes, N: no)\n");
		       scanf("%s", revise);
		       getchar();
		       if((strcmp(revise, "Y") == 0)) fprintf(dir_l, "%s:", update->name);
		       else if((strcmp(revise, "N")==0)) fprintf(dir_l, "=:");
	       }
	       else fprintf(dir_l, "%s:", str);
	}
	else if(*(check_option+0) == '\0') fprintf(dir_l, "=:");
	if(*(check_option+1) == 's'){
		printf("sex > ");
                scanf("%c", c);
                getchar();
                if(c == update->sex){
			printf("You have the same record.\n");
                        printf("sex : %c\n", update->sex);
                        printf("Do you want to revise?(Y : yes, N: no)\n");
                        scanf("%s", revise);
			getchar();
                        if(strcmp(revise, "Y")==0) fprintf(dir_l, "%c:", update->sex);
			else if((strcmp(revise, "N")==0)) fprintf(dir_l, "=:");
		}
                else fprintf(dir_l, "%c:", c);
	}
	else if(*(check_option+1) == '\0') fprintf(dir_l, "=:");
	if(*(check_option+2) == 'b'){
		printf("birth > ");
                scanf("%d", &num);
		char *snum = (char*)calloc(2, sizeof(char));
		change_num_to_string(snum, num);
                if(update->birth == num){
			printf("You have the same record.\n");
                        printf("birth : %d\n", update->birth);
                        printf("Do you want to revise?(Y : yes, N: no)\n");
                        scanf("%s", revise);
			getchar();
                        if(strcmp(revise, "Y")==0) fprintf(dir_l, "%d:", update->birth);
			else if((strcmp(revise, "N")==0)) fprintf(dir_l, "=:");
		}
                else fprintf(dir_l, "%d:", num);
	}
	else if(*(check_option+2) == '\0') fprintf(dir_l, "=:");
        if(*(check_option+5) == 'm'){
	       printf("movies > ");

	       LINK_D *ttmp = (LINK_D*)malloc(sizeof(LINK_D));
	       input_multiple_dir(ttmp);

	       int a=1, b=1;
	       LINK_W *new;
	       new = ttmp->title;

	       while(new ->next != NULL){
		       a++;
		      new = new->next;
	       }
	       new = ttmp->title;
	       LINK_W *org;
	       org = update->title;
	       while(org->next != NULL){
		       b++;
		      org = org->next;
	       }
	       org = update->title;
	       if(a != b){
		       while(new->next != NULL){
			       fprintf(dir_l, "%s,", new->data);
			       new = new->next;
		       }
		       fprintf(dir_l, "%s\n", new->data);
	       }
	       else{
		       char *gar1 = (char*)calloc(200, sizeof(char));
		       char *gar2 = (char*)calloc(200, sizeof(char));
		       while(new->next != NULL){
			       strcpy(gar1+strlen(gar1), new->data);
			       strcat(gar1, ",");
			       new = new->next;
		       }
		       strcat(gar1, new->data);
		       while(org->next != NULL){
			       strcpy(gar2+strlen(gar2), org->data);
			       strcat(gar2, ",");
			       org = org->next;
		       }
		       strcat(gar2, org->data);
		       new = ttmp->title;
		       org = update->title;
		       if(strcmp(gar1, gar2)==0){
			       printf("You have the same records.\n");
			       printf("movies : %s\n", gar1);
			       printf("Do you want to revise?(Y : yes, N : no)\n");
			       scanf("%s", revise);
			       getchar();
			       if(strcmp(revise, "Y")==0){
				       while(org->next != NULL){
					       fprintf(dir_l, "%s,", org->data);
					       org = org->next;
				       }
				       fprintf(dir_l, "%s\n", org->data);
			       }
			       else if(strcmp(revise, "N")==0) fprintf(dir_l, "=\n");
		       }
		       else{
			       while(new->next != NULL){
				       fprintf(dir_l, "%s,", new->data);
				       new = new->next;
			       }
			       fprintf(dir_l, "%s\n", new->data);
		       }
	       }

	}
	else if(*(check_option+5) == '\0') fprintf(dir_l, "=\n");
	fclose(dir_l);
}

void update_director_list(ALL_D *director,FILE *dir_l){
	char *read_data = (char*)calloc(30, sizeof(char));
	int read_num;
	char read_one;
	fscanf(dir_l, "%*c%d", &read_num);

	LINK_D *tmp = (LINK_D*)malloc(sizeof(LINK_D));
	tmp = director->head;
	LINK_D *update = (LINK_D*)malloc(sizeof(LINK_D));
	while(tmp->next_director != director->tail){
		tmp = tmp->next_director;
		if(tmp->serial_num == read_num){
			update = tmp;
			break;
		}
	}
	update->serial_num = read_num;

	fscanf(dir_l, "%*c%[^:]", read_data);
	if(strcmp(read_data, "=")!=0)
		strcpy(update->name, read_data);

	fscanf(dir_l, "%*c%c", &read_one);
	if(read_one != '=')
		update->sex = read_one;

	fscanf(dir_l, "%*c%[^:]", read_data);
	if(strcmp(read_data, "=")!=0){
		read_num = atoi(read_data);
		update->birth = read_num;
	}

	char *temp = (char*)calloc(300, sizeof(char));
	fscanf(dir_l, "%[^\n]%*c", temp);
	if(strcmp(temp, "=")!=0){
		LINK_W *h = (LINK_W*)malloc(sizeof(LINK_W));
		LINK_W *l = (LINK_W*)malloc(sizeof(LINK_W));
		h = update->title;
		while(h->next != NULL){
			LINK_W *d = (LINK_W*)malloc(sizeof(LINK_W));
			d = h;
			l = d->next;
			free(d->data);
			free(d);
			h = l;
		}
		free(l->data);
		free(l);
		update->title = (LINK_W *)malloc(sizeof(LINK_W));
		h->data = strtok(temp, ",");
		while(h->data != NULL){
			char *check = (char*)malloc(sizeof(char)*30);
			check = strtok(NULL, ",");
			if(check == NULL){
				h->next = NULL;
				break;
			}
			else{
				h->next = (LINK_W *)malloc(sizeof(LINK_W));
				h = h->next;
				h->next = NULL;
				h->data = check;
			}
		}
	}
}

int scan_director_log(ALL_D *director){
  LINK_D *save_director;
  LINK_W *save_word,*last_word;
  FILE *dir_l;
  char *input_data,input_text,read_text,garbage;
  int input_num,serial,i=1;
  dir_l=fopen("director_log","r");
  save_director=(LINK_D *)malloc(sizeof(LINK_D));
  input_data=(char *)malloc(sizeof(char)*30);
  while(1){
    if(dir_l == NULL){
      return 1;
      break;
    }
  fscanf(dir_l,"%[^:]",input_data);
  if(strcmp(input_data,"add")==0){
    add_director_list(director,dir_l);
      director -> size ++;
    read_text=fgetc(dir_l);
    fseek(dir_l,-1,SEEK_CUR);
    if(read_text==EOF)
    break;
  }
  else if(strcmp(input_data,"update")==0){
		update_director_list(director,dir_l);
    read_text=fgetc(dir_l);
    fseek(dir_l,-1,SEEK_CUR);
    if(read_text==EOF)
    break;
  }
  else if(strcmp(input_data,"delete")==0){
    delete_director_list(director,dir_l);
      director -> size --;
    read_text=fgetc(dir_l);
    fseek(dir_l,-1,SEEK_CUR);
    if(read_text==EOF)
    break;
  }
  else
  break;
}
FILE *file;
file = fopen("director_list","w");
LINK_W *print_word;
LINK_D *print_director;
print_director = director -> head;
print_word = (LINK_W *)malloc(sizeof(LINK_W));
    while(print_director -> next_director != director -> tail){
      print_director = print_director -> next_director;
      fprintf(file,"%d:%s:%c:%d:",print_director -> serial_num,print_director -> name,print_director -> sex,print_director -> birth);
      print_word = print_director -> title;
      while(print_word -> next != NULL){
        fprintf(file,"%s,",print_word->data);
        print_word = print_word->next;
      }
      fprintf(file,"%s\n", print_word->data);
    }
      fclose(file);
return 0;
}
int print_director_list(ALL_D *director,int serial){
  LINK_D *print_director;
  LINK_W *print_word;
  LINK_M *print_movie;
  int i=1;
  print_director=(LINK_D*)malloc(sizeof(LINK_D));
  print_director=director -> head;
  print_word = (LINK_W *)malloc(sizeof(LINK_W));
  while(print_director -> next_director != director -> tail){
    print_director = print_director -> next_director;
    if(print_director -> serial_num == serial){
    printf("  %d, %s, %c,  %8d\n",print_director -> serial_num,print_director -> name,print_director -> sex,print_director -> birth);
    print_word = print_director -> title;
    while(print_word -> next != NULL){
      print_movie = (LINK_M *)print_word -> com_word;
      if(print_movie == NULL)
  		printf("  M%d : %s(  -  )\n",i,print_word->data);
      else
      printf("  M%d : %s(%d,%d)\n",i,print_word->data,print_movie -> year,print_movie -> time);
  		print_word = print_word->next;
      i++;
  	}
    print_movie = (LINK_M *)print_word -> com_word;
    if(print_movie == NULL)
    printf("  M%d : %s(  -  )\n",i,print_word->data);
    else
    printf("  M%d : %s(%d,%d)\n",i,print_word->data,print_movie -> year,print_movie -> time);
    return 1;
  }
}
}
void sorting(ALL_M *movie, ALL_A *actor,ALL_D *director,char tag,char option){
  char **copy,**copy_dir,checking,*check_sex;
  int copy_num,*num;
  int i,check,k,ch;
  LINK_M *save_movie,*print_movie;
  LINK_A *save_actor,*print_actor;
  LINK_D *save_director,*print_director;
  LINK_W *print_word,*saving;
  save_movie = movie -> head;
  save_actor = actor -> head;
  save_director = director -> head;
  copy_dir = (char **)calloc(100,sizeof(char *));
  copy = (char **)calloc(100,sizeof(char *));
  check_sex = (char *)calloc(1000,sizeof(char));
  num = (int *)calloc(1000,sizeof(int));
  if(tag == 'm'){
    if(option == 't'){
      while(save_movie -> next_movie != movie -> tail){
        save_movie = save_movie -> next_movie;
        *(copy+i) = (char *)calloc(1024,sizeof(char));
        *(copy+i) = save_movie -> title;
        i++;
      }
      qsort((void *)copy,movie -> size,sizeof(char *), compare);
      save_movie = movie -> head;
      for(check=0;check<movie -> size;check++){
      while(save_movie -> next_movie != movie -> tail){
        if(strcmp(*(copy+check),save_movie -> next_movie -> title)==0){
          print_movie = save_movie -> next_movie;
          saving = print_movie -> actor;
          printf("%s:%s:%s:%d:%d:",print_movie -> title,print_movie -> genre,print_movie -> director -> data,print_movie -> year,print_movie -> time);
          while(print_movie -> actor -> next != NULL){
            printf("%s:",print_movie -> actor -> data);
            print_movie -> actor = print_movie -> actor -> next;
          }
          printf("%s\n",print_movie -> actor -> data);
          print_movie -> actor = saving;
          save_movie -> next_movie = save_movie -> next_movie -> next_movie;
        save_movie = movie -> head;
        break;
      }
      save_movie = save_movie -> next_movie;
    }
    }
  }

  else if(option == 'g'){
    while(save_movie -> next_movie != movie -> tail){
      save_movie = save_movie -> next_movie;
      *(copy+i) = (char *)calloc(1024,sizeof(char));
      *(copy+i) = save_movie -> genre;
      i++;
    }
    qsort((void *)copy,movie -> size,sizeof(char *), compare);
    save_movie = movie -> head;
    for(check=0;check<movie -> size;check++){
    while(save_movie -> next_movie != movie -> tail){
      if(strcmp(*(copy+check),save_movie -> next_movie -> genre)==0){
        print_movie = save_movie -> next_movie;
        saving = print_movie -> actor;
        printf("%s:%s:%s:%d:%d:",print_movie -> title,print_movie -> genre,print_movie -> director -> data,print_movie -> year,print_movie -> time);
        while(print_movie -> actor -> next != NULL){
          printf("%s:",print_movie -> actor -> data);
          print_movie -> actor = print_movie -> actor -> next;
        }
        printf("%s\n",print_movie -> actor -> data);
        print_movie -> actor = saving;
        save_movie -> next_movie = save_movie -> next_movie -> next_movie;
      save_movie = movie -> head;
      break;
    }
    save_movie = save_movie -> next_movie;
  }
  }
  }
  else if(option == 'd'){
    while(save_movie -> next_movie != movie -> tail){
      save_movie = save_movie -> next_movie;
      *(copy+i) = (char *)calloc(1024,sizeof(char));
      *(copy+i) = save_movie -> director -> data;
      i++;
    }
    qsort((void *)copy,movie -> size,sizeof(char *), compare);
    save_movie = movie -> head;
    for(check=0;check<movie -> size;check++){
    while(save_movie -> next_movie != movie -> tail){
      if(strcmp(*(copy+check),save_movie -> next_movie -> director -> data)==0){
        print_movie = save_movie -> next_movie;
        saving = print_movie -> actor;
        printf("%s:%s:%s:%d:%d:",print_movie -> title,print_movie -> genre,print_movie -> director -> data,print_movie -> year,print_movie -> time);
        while(print_movie -> actor -> next != NULL){
          printf("%s:",print_movie -> actor -> data);
          print_movie -> actor = print_movie -> actor -> next;
        }
        printf("%s\n",print_movie -> actor -> data);
        print_movie -> actor = saving;
        save_movie -> next_movie = save_movie -> next_movie -> next_movie;
      save_movie = movie -> head;
      break;
    }
    save_movie = save_movie -> next_movie;
  }
  }
  }
  else if(option == 'a'){
    while(save_movie -> next_movie != movie -> tail){
      save_movie = save_movie -> next_movie;
      *(copy+i) = (char *)calloc(1024,sizeof(char));
      *(copy+i) = save_movie -> actor -> data;
      i++;
    }
    qsort((void *)copy,movie -> size,sizeof(char *), compare);
    save_movie = movie -> head;
    for(check=0;check<movie -> size;check++){
    while(save_movie -> next_movie != movie -> tail){
      if(strcmp(*(copy+check),save_movie -> next_movie -> actor -> data)==0){
        print_movie = save_movie -> next_movie;
        saving = print_movie -> actor;
        printf("%s:%s:%s:%d:%d:",print_movie -> title,print_movie -> genre,print_movie -> director -> data,print_movie -> year,print_movie -> time);
        while(print_movie -> actor -> next != NULL){
          printf("%s:",print_movie -> actor -> data);
          print_movie -> actor = print_movie -> actor -> next;
        }
        printf("%s\n",print_movie -> actor -> data);
        print_movie -> actor = saving;
        save_movie -> next_movie = save_movie -> next_movie -> next_movie;
      save_movie = movie -> head;
      break;
    }
    save_movie = save_movie -> next_movie;
  }
  }
  }
  else if(option == 'y'){
    while(save_movie -> next_movie != movie -> tail){
      save_movie = save_movie -> next_movie;
      *(num+i) = save_movie -> year;
      i++;
    }
    qsort(num,movie -> size,sizeof(int), compare_num);
    save_movie = movie -> head;
    for(check=0;check<movie -> size;check++){
    while(save_movie -> next_movie != movie -> tail){
      if(*(num+check) == save_movie -> next_movie -> year){
        print_movie = save_movie -> next_movie;
        saving = print_movie -> actor;
        printf("%s:%s:%s:%d:%d:",print_movie -> title,print_movie -> genre,print_movie -> director -> data,print_movie -> year,print_movie -> time);
        while(print_movie -> actor -> next != NULL){
          printf("%s:",print_movie -> actor -> data);
          print_movie -> actor = print_movie -> actor -> next;
        }
        printf("%s\n",print_movie -> actor -> data);
        print_movie -> actor = saving;
        save_movie -> next_movie = save_movie -> next_movie -> next_movie;
      save_movie = movie -> head;
      break;
    }
    save_movie = save_movie -> next_movie;
  }
  }
  }
  else if(option == 'r'){
    while(save_movie -> next_movie != movie -> tail){
      save_movie = save_movie -> next_movie;
      *(num+i) = save_movie -> time;
      i++;
    }
    qsort(num,movie -> size,sizeof(int), compare_num);
    save_movie = movie -> head;
    for(check=0;check<movie -> size;check++){
    while(save_movie -> next_movie != movie -> tail){
      if(*(num+check) == save_movie -> next_movie -> time){
        print_movie = save_movie -> next_movie;
        saving = print_movie -> actor;
        printf("%s:%s:%s:%d:%d:",print_movie -> title,print_movie -> genre,print_movie -> director -> data,print_movie -> year,print_movie -> time);
        while(print_movie -> actor -> next != NULL){
          printf("%s:",print_movie -> actor -> data);
          print_movie -> actor = print_movie -> actor -> next;
        }
        printf("%s\n",print_movie -> actor -> data);
        print_movie -> actor = saving;
        save_movie -> next_movie = save_movie -> next_movie -> next_movie;
      save_movie = movie -> head;
      break;
    }
    save_movie = save_movie -> next_movie;
  }
  }
  }
}//마무리
else if(tag == 'd'){
  if(option == 'n'){
    while(save_director -> next_director != director -> tail){
    	save_director = save_director -> next_director;
    	*(copy+i) = (char *)calloc(1024,sizeof(char));
    	*(copy+i) = save_director -> name;
    	i++;
    }
    qsort((void *)copy,director -> size,sizeof(char *), compare);
    save_director = director -> head;
    for(check=0;check<director -> size;check++){
    while(save_director -> next_director != director -> tail){
    	if(strcmp(*(copy+check),save_director -> next_director -> name)==0){
    		print_director = save_director -> next_director;
    		saving = print_director -> title;
    		printf("%s:%c:%d:",print_director -> name,print_director -> sex,print_director -> birth);
        while(print_director -> title -> next != NULL){
          printf("%s,",print_director -> title -> data);
    			print_director -> title = print_director -> title -> next;
    		}
    		printf("%s\n",print_director -> title -> data);
    		print_director -> title = saving;
    		save_director -> next_director = save_director -> next_director -> next_director;
    	save_director = director -> head;
    	break;
    }
    save_director = save_director -> next_director;
    }
    }
  }
else if(option == 's'){
  while(save_director -> next_director != director -> tail){
    save_director = save_director -> next_director;
    *(check_sex+i) = save_director -> sex;
    i++;
  }
qsort((void *)check_sex,director -> size,sizeof(char ), compare_w);
  save_director = director -> head;
  for(check=0;check<director -> size;check++){
  while(save_director -> next_director != director -> tail){
    if(*(check_sex+check)==save_director -> next_director -> sex){
      print_director = save_director -> next_director;
      saving = print_director -> title;
      printf("%s:%c:%d:",print_director -> name,print_director -> sex,print_director -> birth);
      while(print_director -> title -> next != NULL){
        printf("%s,",print_director -> title -> data);
        print_director -> title = print_director -> title -> next;
      }
      printf("%s\n",print_director -> title -> data);
      print_director -> title = saving;
      save_director -> next_director = save_director -> next_director -> next_director;
    save_director = director -> head;
    break;
  }
  save_director = save_director -> next_director;
  }
  }
}
else if(option == 'b'){
  while(save_director -> next_director != director -> tail){
  	save_director = save_director -> next_director;
  	*(num+i) = save_director -> birth;
  	i++;
  }
  qsort(num,director -> size,sizeof(int), compare_num);
  save_director = director -> head;
  for(check=0;check<director -> size;check++){
  while(save_director -> next_director != director -> tail){
  	if(*(num+check) == save_director -> next_director -> birth){
      print_director = save_director -> next_director;
      saving = print_director -> title;
      printf("%s:%c:%d:",print_director -> name,print_director -> sex,print_director -> birth);
      while(print_director -> title -> next != NULL){
        printf("%s,",print_director -> title -> data);
        print_director -> title = print_director -> title -> next;
      }
      printf("%s\n",print_director -> title -> data);
      print_director -> title = saving;
      save_director -> next_director = save_director -> next_director -> next_director;
    save_director = director -> head;
    break;
  }
  save_director = save_director -> next_director;
  }
}
}
else if(option == 'm'){
  while(save_director -> next_director != director -> tail){
    save_director = save_director -> next_director;
    *(copy+i) = (char *)calloc(1024,sizeof(char));
    *(copy+i) = save_director -> title -> data;
    i++;
  }
  qsort((void *)copy,director -> size,sizeof(char *), compare);
  save_director = director -> head;
  for(check=0;check<director -> size;check++){
  while(save_director -> next_director != director -> tail){
    if(strcmp(*(copy+check),save_director -> next_director -> title -> data)==0){
      print_director = save_director -> next_director;
      saving = print_director -> title;
      printf("%s:%c:%d:",print_director -> name,print_director -> sex,print_director -> birth);
      while(print_director -> title -> next != NULL){
        printf("%s,",print_director -> title -> data);
        print_director -> title = print_director -> title -> next;
      }
      printf("%s\n",print_director -> title -> data);
      print_director -> title = saving;
      save_director -> next_director = save_director -> next_director -> next_director;
    save_director = director -> head;
    break;
  }
  save_director = save_director -> next_director;
  }
  }
}
}
else if(tag == 'a'){
  if(option == 'n'){
    while(save_actor -> next_actor != actor -> tail){
      save_actor = save_actor -> next_actor;
      *(copy+i) = (char *)calloc(1024,sizeof(char));
      *(copy+i) = save_actor -> name;
      i++;
    }
    qsort((void *)copy,actor -> size,sizeof(char *), compare);
    save_actor = actor -> head;
    for(check=0;check<actor -> size;check++){
    while(save_actor -> next_actor != actor -> tail){
      if(strcmp(*(copy+check),save_actor -> next_actor -> name)==0){
        print_actor = save_actor -> next_actor;
        saving = print_actor -> title;
        printf("%s:%c:%d:",print_actor -> name,print_actor -> sex,print_actor -> birth);
        while(print_actor -> title -> next != NULL){
          printf("%s,",print_actor -> title -> data);
          print_actor -> title = print_actor -> title -> next;
        }
        printf("%s\n",print_actor -> title -> data);
        print_actor -> title = saving;
        save_actor -> next_actor = save_actor -> next_actor -> next_actor;
      save_actor = actor -> head;
      break;
    }
    save_actor = save_actor -> next_actor;
    }
    }
  }
else if(option == 's'){
  while(save_actor -> next_actor != actor -> tail){
    save_actor = save_actor -> next_actor;
    *(check_sex+i) = save_actor -> sex;
    i++;
  }
qsort((void *)check_sex,actor -> size,sizeof(char ), compare_w);
  save_actor = actor -> head;
  for(check=0;check<actor -> size;check++){
  while(save_actor -> next_actor != actor -> tail){
    if(*(check_sex+check)==save_actor -> next_actor -> sex){
      print_actor = save_actor -> next_actor;
      saving = print_actor -> title;
      printf("%s:%c:%d:",print_actor -> name,print_actor -> sex,print_actor -> birth);
      while(print_actor -> title -> next != NULL){
        printf("%s,",print_actor -> title -> data);
        print_actor -> title = print_actor -> title -> next;
      }
      printf("%s\n",print_actor -> title -> data);
      print_actor -> title = saving;
      save_actor -> next_actor = save_actor -> next_actor -> next_actor;
    save_actor = actor -> head;
    break;
  }
  save_actor = save_actor -> next_actor;
  }
  }
}
else if(option == 'b'){
  while(save_actor -> next_actor != actor -> tail){
  	save_actor = save_actor -> next_actor;
  	*(num+i) = save_actor -> birth;
  	i++;
  }
  qsort(num,actor -> size,sizeof(int), compare_num);
  save_actor = actor -> head;
  for(check=0;check<actor -> size;check++){
  while(save_actor -> next_actor != actor -> tail){
  	if(*(num+check) == save_actor -> next_actor -> birth){
      print_actor = save_actor -> next_actor;
      saving = print_actor -> title;
      printf("%s:%c:%d:",print_actor -> name,print_actor -> sex,print_actor -> birth);
      while(print_actor -> title -> next != NULL){
        printf("%s,",print_actor -> title -> data);
        print_actor -> title = print_actor -> title -> next;
      }
      printf("%s\n",print_actor -> title -> data);
      print_actor -> title = saving;
      save_actor -> next_actor = save_actor -> next_actor -> next_actor;
    save_actor = actor -> head;
    break;
  }
  save_actor = save_actor -> next_actor;
  }
}
}
else if(option == 'm'){
  while(save_actor -> next_actor != actor -> tail){
    save_actor = save_actor -> next_actor;
    *(copy+i) = (char *)calloc(1024,sizeof(char));
    *(copy+i) = save_actor -> title -> data;
    i++;
  }
  qsort((void *)copy,actor -> size,sizeof(char *), compare);
  save_actor = actor -> head;
  for(check=0;check<actor -> size;check++){
  while(save_actor -> next_actor != actor -> tail){
    if(strcmp(*(copy+check),save_actor -> next_actor -> title -> data)==0){
      print_actor = save_actor -> next_actor;
      saving = print_actor -> title;
      printf("%s:%c:%d:",print_actor -> name,print_actor -> sex,print_actor -> birth);
      while(print_actor -> title -> next != NULL){
        printf("%s,",print_actor -> title -> data);
        print_actor -> title = print_actor -> title -> next;
      }
      printf("%s\n",print_actor -> title -> data);
      print_actor -> title = saving;
      save_actor -> next_actor = save_actor -> next_actor -> next_actor;
    save_actor = actor -> head;
    break;
  }
  save_actor = save_actor -> next_actor;
  }
  }
}
}
}
void sorting_file(ALL_M *movie, ALL_A *actor,ALL_D *director,char tag,char option){
	char *filename;
	FILE *file;
  filename = (char *)calloc(1000,sizeof(char));
	scanf("%[^\n]%*c",filename);
	file = fopen(filename,"w");
	char **copy,**copy_word,checking,*check_sex;
  int copy_num,*num;
  int i,check,k,ch;
  LINK_M *save_movie,*print_movie;
  LINK_A *save_actor,*print_actor;
  LINK_D *save_director,*print_director;
  LINK_W *print_word,*saving;
  save_movie = movie -> head;
  save_actor = actor -> head;
  save_director = director -> head;
  copy = (char **)calloc(1,sizeof(char *));
  copy_word = (char **)calloc(1,sizeof(char *));
  num = (int *)calloc(1000,sizeof(int));
  check_sex = (char *)calloc(1000,sizeof(char));
  if(tag == 'm'){
    if(option == 't'){
      while(save_movie -> next_movie != movie -> tail){
      	save_movie = save_movie -> next_movie;
      	*(copy+i) = (char *)calloc(1024,sizeof(char));
      	*(copy+i) = save_movie -> title;
      	i++;
      }
      qsort((void *)copy,movie -> size,sizeof(char *), compare);
      save_movie = movie -> head;
      for(check=0;check<movie -> size;check++){
      while(save_movie -> next_movie != movie -> tail){
      	if(strcmp(*(copy+check),save_movie -> next_movie -> title)==0){
      		print_movie = save_movie -> next_movie;
      		saving = print_movie -> actor;
      		fprintf(file,"%s:%s:%s:%d:%d:",print_movie -> title,print_movie -> genre,print_movie -> director -> data,print_movie -> year,print_movie -> time);
      		while(print_movie -> actor -> next != NULL){
      			fprintf(file,"%s:",print_movie -> actor -> data);
      			print_movie -> actor = print_movie -> actor -> next;
      		}
      		fprintf(file,"%s\n",print_movie -> actor -> data);
      		print_movie -> actor = saving;
      		save_movie -> next_movie = save_movie -> next_movie -> next_movie;
      	save_movie = movie -> head;
      	break;
      }
      save_movie = save_movie -> next_movie;
      }
      }
    fclose(file);
  }

  else if(option == 'g'){
    while(save_movie -> next_movie != movie -> tail){
      save_movie = save_movie -> next_movie;
      *(copy+i) = (char *)calloc(1024,sizeof(char));
      *(copy+i) = save_movie -> genre;
      i++;
    }
    qsort((void *)copy,movie -> size,sizeof(char *), compare);
    save_movie = movie -> head;
    for(check=0;check<movie -> size;check++){
    while(save_movie -> next_movie != movie -> tail){
      if(strcmp(*(copy+check),save_movie -> next_movie -> genre)==0){
        print_movie = save_movie -> next_movie;
        saving = print_movie -> actor;
        fprintf(file,"%s:%s:%s:%d:%d:",print_movie -> title,print_movie -> genre,print_movie -> director -> data,print_movie -> year,print_movie -> time);
        while(print_movie -> actor -> next != NULL){
          fprintf(file,"%s:",print_movie -> actor -> data);
          print_movie -> actor = print_movie -> actor -> next;
        }
        fprintf(file,"%s\n",print_movie -> actor -> data);
        print_movie -> actor = saving;
        save_movie -> next_movie = save_movie -> next_movie -> next_movie;
      save_movie = movie -> head;
      break;
    }
    save_movie = save_movie -> next_movie;
    }
    }
  fclose(file);
  }
  else if(option == 'd'){
    while(save_movie -> next_movie != movie -> tail){
      save_movie = save_movie -> next_movie;
      *(copy+i) = (char *)calloc(1024,sizeof(char));
      *(copy+i) = save_movie -> director -> data;
      i++;
    }
    qsort((void *)copy,movie -> size,sizeof(char *), compare);
    save_movie = movie -> head;
    for(check=0;check<movie -> size;check++){
    while(save_movie -> next_movie != movie -> tail){
      if(strcmp(*(copy+check),save_movie -> next_movie -> director -> data)==0){
        print_movie = save_movie -> next_movie;
        saving = print_movie -> actor;
        fprintf(file,"%s:%s:%s:%d:%d:",print_movie -> title,print_movie -> genre,print_movie -> director -> data,print_movie -> year,print_movie -> time);
        while(print_movie -> actor -> next != NULL){
          fprintf(file,"%s:",print_movie -> actor -> data);
          print_movie -> actor = print_movie -> actor -> next;
        }
        fprintf(file,"%s\n",print_movie -> actor -> data);
        print_movie -> actor = saving;
        save_movie -> next_movie = save_movie -> next_movie -> next_movie;
      save_movie = movie -> head;
      break;
    }
    save_movie = save_movie -> next_movie;
    }
    }
  fclose(file);
  }
  else if(option == 'a'){
    while(save_movie -> next_movie != movie -> tail){
      save_movie = save_movie -> next_movie;
      *(copy+i) = (char *)calloc(1024,sizeof(char));
      *(copy+i) = save_movie -> actor -> data;
      i++;
    }
    qsort((void *)copy,movie -> size,sizeof(char *), compare);
    save_movie = movie -> head;
    for(check=0;check<movie -> size;check++){
    while(save_movie -> next_movie != movie -> tail){
      if(strcmp(*(copy+check),save_movie -> next_movie -> actor -> data)==0){
        print_movie = save_movie -> next_movie;
        saving = print_movie -> actor;
        fprintf(file,"%s:%s:%s:%d:%d:",print_movie -> title,print_movie -> genre,print_movie -> director -> data,print_movie -> year,print_movie -> time);
        while(print_movie -> actor -> next != NULL){
          fprintf(file,"%s:",print_movie -> actor -> data);
          print_movie -> actor = print_movie -> actor -> next;
        }
        fprintf(file,"%s\n",print_movie -> actor -> data);
        print_movie -> actor = saving;
        save_movie -> next_movie = save_movie -> next_movie -> next_movie;
      save_movie = movie -> head;
      break;
    }
    save_movie = save_movie -> next_movie;
    }
    }
  fclose(file);
  }
  else if(option == 'y'){
    while(save_movie -> next_movie != movie -> tail){
    	save_movie = save_movie -> next_movie;
    	*(num+i) = save_movie -> year;
    	i++;
    }
    qsort(num,movie -> size,sizeof(int), compare_num);
    save_movie = movie -> head;
    for(check=0;check<movie -> size;check++){
    while(save_movie -> next_movie != movie -> tail){
    	if(*(num+check) == save_movie -> next_movie -> year){
    		print_movie = save_movie -> next_movie;
    		saving = print_movie -> actor;
    		fprintf(file,"%s:%s:%s:%d:%d:",print_movie -> title,print_movie -> genre,print_movie -> director -> data,print_movie -> year,print_movie -> time);
    		while(print_movie -> actor -> next != NULL){
    			fprintf(file,"%s:",print_movie -> actor -> data);
    			print_movie -> actor = print_movie -> actor -> next;
    		}
    		fprintf(file,"%s\n",print_movie -> actor -> data);
    		print_movie -> actor = saving;
    		save_movie -> next_movie = save_movie -> next_movie -> next_movie;
    	save_movie = movie -> head;
    	break;
    }
    save_movie = save_movie -> next_movie;
    }
    }
    fclose(file);
  }
  else if(option == 'r'){
    while(save_movie -> next_movie != movie -> tail){
    	save_movie = save_movie -> next_movie;
    	*(num+i) = save_movie -> time;
    	i++;
    }
    qsort(num,movie -> size,sizeof(int), compare_num);
    save_movie = movie -> head;
    for(check=0;check<movie -> size;check++){
    while(save_movie -> next_movie != movie -> tail){
    	if(*(num+check) == save_movie -> next_movie -> time){
    		print_movie = save_movie -> next_movie;
    		saving = print_movie -> actor;
    		fprintf(file,"%s:%s:%s:%d:%d:",print_movie -> title,print_movie -> genre,print_movie -> director -> data,print_movie -> year,print_movie -> time);
    		while(print_movie -> actor -> next != NULL){
    			fprintf(file,"%s:",print_movie -> actor -> data);
    			print_movie -> actor = print_movie -> actor -> next;
    		}
    		fprintf(file,"%s\n",print_movie -> actor -> data);
    		print_movie -> actor = saving;
    		save_movie -> next_movie = save_movie -> next_movie -> next_movie;
    	save_movie = movie -> head;
    	break;
    }
    save_movie = save_movie -> next_movie;
    }
    }
    fclose(file);
  }
}
else if(tag == 'd'){
  if(option == 'n'){
    while(save_director -> next_director != director -> tail){
    	save_director = save_director -> next_director;
    	*(copy+i) = (char *)calloc(1024,sizeof(char));
    	*(copy+i) = save_director -> name;
    	i++;
    }
    qsort((void *)copy,director -> size,sizeof(char *), compare);
    save_director = director -> head;
    for(check=0;check<director -> size;check++){
    while(save_director -> next_director != director -> tail){
    	if(strcmp(*(copy+check),save_director -> next_director -> name)==0){
    		print_director = save_director -> next_director;
    		saving = print_director -> title;
    		fprintf(file,"%s:%c:%d:",print_director -> name,print_director -> sex,print_director -> birth);
        while(print_director -> title -> next != NULL){
          fprintf(file,"%s,",print_director -> title -> data);
    			print_director -> title = print_director -> title -> next;
    		}
    		fprintf(file,"%s\n",print_director -> title -> data);
    		print_director -> title = saving;
    		save_director -> next_director = save_director -> next_director -> next_director;
    	save_director = director -> head;
    	break;
    }
    save_director = save_director -> next_director;
    }
    }
    fclose(file);
  }
else if(option == 's'){
  while(save_director -> next_director != director -> tail){
    save_director = save_director -> next_director;
    *(check_sex+i) = save_director -> sex;
    i++;
  }
qsort((void *)check_sex,director -> size,sizeof(char ), compare_w);
  save_director = director -> head;
  for(check=0;check<director -> size;check++){
  while(save_director -> next_director != director -> tail){
    if(*(check_sex+check)==save_director -> next_director -> sex){
      print_director = save_director -> next_director;
      saving = print_director -> title;
      fprintf(file,"%s:%c:%d:",print_director -> name,print_director -> sex,print_director -> birth);
      while(print_director -> title -> next != NULL){
        fprintf(file,"%s,",print_director -> title -> data);
        print_director -> title = print_director -> title -> next;
      }
      fprintf(file,"%s\n",print_director -> title -> data);
      print_director -> title = saving;
      save_director -> next_director = save_director -> next_director -> next_director;
    save_director = director -> head;
    break;
  }
  save_director = save_director -> next_director;
  }
  }
  fclose(file);
}
else if(option == 'b'){
  while(save_director -> next_director != director -> tail){
  	save_director = save_director -> next_director;
  	*(num+i) = save_director -> birth;
  	i++;
  }
  qsort(num,director -> size,sizeof(int), compare_num);
  save_director = director -> head;
  for(check=0;check<director -> size;check++){
  while(save_director -> next_director != director -> tail){
  	if(*(num+check) == save_director -> next_director -> birth){
      print_director = save_director -> next_director;
      saving = print_director -> title;
      fprintf(file,"%s:%c:%d:",print_director -> name,print_director -> sex,print_director -> birth);
      while(print_director -> title -> next != NULL){
        fprintf(file,"%s,",print_director -> title -> data);
        print_director -> title = print_director -> title -> next;
      }
      fprintf(file,"%s\n",print_director -> title -> data);
      print_director -> title = saving;
      save_director -> next_director = save_director -> next_director -> next_director;
    save_director = director -> head;
    break;
  }
  save_director = save_director -> next_director;
  }
}
fclose(file);
}
else if(option == 'm'){
  while(save_director -> next_director != director -> tail){
    save_director = save_director -> next_director;
    *(copy+i) = (char *)calloc(1024,sizeof(char));
    *(copy+i) = save_director -> title -> data;
    i++;
  }
  qsort((void *)copy,director -> size,sizeof(char *), compare);
  save_director = director -> head;
  for(check=0;check<director -> size;check++){
  while(save_director -> next_director != director -> tail){
    if(strcmp(*(copy+check),save_director -> next_director -> title -> data)==0){
      print_director = save_director -> next_director;
      saving = print_director -> title;
      fprintf(file,"%s:%c:%d:",print_director -> name,print_director -> sex,print_director -> birth);
      while(print_director -> title -> next != NULL){
        fprintf(file,"%s,",print_director -> title -> data);
        print_director -> title = print_director -> title -> next;
      }
      fprintf(file,"%s\n",print_director -> title -> data);
      print_director -> title = saving;
      save_director -> next_director = save_director -> next_director -> next_director;
    save_director = director -> head;
    break;
  }
  save_director = save_director -> next_director;
  }
  }
  fclose(file);
}
}
else if(tag == 'a'){
  if(option == 'n'){
		while(save_actor -> next_actor != actor -> tail){
      save_actor = save_actor -> next_actor;
      *(copy+i) = (char *)calloc(1024,sizeof(char));
      *(copy+i) = save_actor -> name;
      i++;
    }
    qsort((void *)copy,actor -> size,sizeof(char *), compare);
    save_actor = actor -> head;
    for(check=0;check<actor -> size;check++){
    while(save_actor -> next_actor != actor -> tail){
      if(strcmp(*(copy+check),save_actor -> next_actor -> name)==0){
        print_actor = save_actor -> next_actor;
        saving = print_actor -> title;
        fprintf(file,"%s:%c:%d:",print_actor -> name,print_actor -> sex,print_actor -> birth);
        while(print_actor -> title -> next != NULL){
          fprintf(file,"%s,",print_actor -> title -> data);
          print_actor -> title = print_actor -> title -> next;
        }
        fprintf(file,"%s\n",print_actor -> title -> data);
        print_actor -> title = saving;
        save_actor -> next_actor = save_actor -> next_actor -> next_actor;
      save_actor = actor -> head;
      break;
    }
    save_actor = save_actor -> next_actor;
    }
    }
		fclose(file);
  }
else if(option == 's'){
  while(save_actor -> next_actor != actor -> tail){
    save_actor = save_actor -> next_actor;
    *(check_sex+i) = save_actor -> sex;
    i++;
  }
qsort((void *)check_sex,actor -> size,sizeof(char ), compare_w);
  save_actor = actor -> head;
  for(check=0;check<actor -> size;check++){
  while(save_actor -> next_actor != actor -> tail){
    if(*(check_sex+check)==save_actor -> next_actor -> sex){
      print_actor = save_actor -> next_actor;
      saving = print_actor -> title;
      fprintf(file,"%s:%c:%d:",print_actor -> name,print_actor -> sex,print_actor -> birth);
      while(print_actor -> title -> next != NULL){
        fprintf(file,"%s,",print_actor -> title -> data);
        print_actor -> title = print_actor -> title -> next;
      }
      fprintf(file,"%s\n",print_actor -> title -> data);
      print_actor -> title = saving;
      save_actor -> next_actor = save_actor -> next_actor -> next_actor;
    save_actor = actor -> head;
    break;
  }
  save_actor = save_actor -> next_actor;
  }
  }
	fclose(file);
}
else if(option == 'b'){
  while(save_actor -> next_actor != actor -> tail){
  	save_actor = save_actor -> next_actor;
  	*(num+i) = save_actor -> birth;
  	i++;
  }
  qsort(num,actor -> size,sizeof(int), compare_num);
  save_actor = actor -> head;
  for(check=0;check<actor -> size;check++){
  while(save_actor -> next_actor != actor -> tail){
  	if(*(num+check) == save_actor -> next_actor -> birth){
      print_actor = save_actor -> next_actor;
      saving = print_actor -> title;
      fprintf(file,"%s:%c:%d:",print_actor -> name,print_actor -> sex,print_actor -> birth);
      while(print_actor -> title -> next != NULL){
        fprintf(file,"%s,",print_actor -> title -> data);
        print_actor -> title = print_actor -> title -> next;
      }
      fprintf(file,"%s\n",print_actor -> title -> data);
      print_actor -> title = saving;
      save_actor -> next_actor = save_actor -> next_actor -> next_actor;
    save_actor = actor -> head;
    break;
  }
  save_actor = save_actor -> next_actor;
  }
}
fclose(file);
}
else if(option == 'm'){
  while(save_actor -> next_actor != actor -> tail){
    save_actor = save_actor -> next_actor;
    *(copy+i) = (char *)calloc(1024,sizeof(char));
    *(copy+i) = save_actor -> title -> data;
    i++;
  }
  qsort((void *)copy,actor -> size,sizeof(char *), compare);
  save_actor = actor -> head;
  for(check=0;check<actor -> size;check++){
  while(save_actor -> next_actor != actor -> tail){
    if(strcmp(*(copy+check),save_actor -> next_actor -> title -> data)==0){
      print_actor = save_actor -> next_actor;
      saving = print_actor -> title;
      fprintf(file,"%s:%c:%d:",print_actor -> name,print_actor -> sex,print_actor -> birth);
      while(print_actor -> title -> next != NULL){
        fprintf(file,"%s,",print_actor -> title -> data);
        print_actor -> title = print_actor -> title -> next;
      }
      fprintf(file,"%s\n",print_actor -> title -> data);
      print_actor -> title = saving;
      save_actor -> next_actor = save_actor -> next_actor -> next_actor;
    save_actor = actor -> head;
    break;
  }
  save_actor = save_actor -> next_actor;
  }
  }
	fclose(file);
}
}
}
void search_print_m(LINK_M *search_movie){  //지정된 문자열을 찾았을 때 해당 레코드 전체를 출력하는 함수
  LINK_W *actor;  //actor가 포인트하는 곳을 초기화 시키기 위한 변수
  actor = search_movie -> actor;  //actor의 위치를 첫 번째 acotr의 위치로 지정해 준다.
  printf("%d:%s:%s:%s:%d:%d:",search_movie -> serial_num, search_movie -> title, search_movie -> genre, search_movie -> director -> data, search_movie -> year, search_movie -> time);  //해당 레코드 정보 출력
  while(search_movie -> actor ->  next != NULL){  //해당 레코드의 actor 정보 출력
    printf("%s,", search_movie -> actor -> data);
    search_movie -> actor = search_movie -> actor -> next;
    }
  printf("%s\n", search_movie -> actor -> data);  //해당 레코드의 마지막 actor부분 출력
  search_movie -> actor = actor;  //마지막으로 간 actor의 노드를 초기화 시켜줌
}
void search_m(ALL_M *movie, char *string){  //movie링크드리스트에서 지정된 문자열 찾는 함수
  LINK_W *actor;  //actor가 포인트하는 곳을 초기화 시키기 위한 변수
  char *string_y;
  char *string_t;
  LINK_M *search_movie = movie -> head;
  string_y = (char *)calloc(sizeof(char), 4);
  string_t = (char *)calloc(sizeof(char), 4);
  option = (char *)calloc(sizeof(char), 3);
  int word_l = 0;
  int word_noq = 0;
  int word_samechar = 0;
  int word_star = 0;
  int word_qnum = 0;
  int data_len = 0;
  int gap_star = 0;
  word_l = strlen(string);
  for(int i = 0; i < word_l; i++){
    if(*(string + i) == '*')
      word_star++;
  }

  for(int i = 0; i < word_l; i++){
    if(*(string + i) != '?')
      word_noq++;
  }

  for(int i = 0; i < word_l; i++){
    if(*(string + i) == '?')
      word_qnum++;
  }

  if(word_star != 0){
    if(*(string + word_l - 1) == '*'){
      while(search_movie -> next_movie != movie -> tail){
        search_movie = search_movie -> next_movie;
        change_num_to_string(string_y, search_movie -> year);
        change_num_to_string(string_t, search_movie -> time);

        if(word_qnum != 0){
          word_samechar = 0;
          for(int i = 0; i <(word_l - 1); i++){
            if(*(string + i) == *(search_movie -> title +i))
              word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
        else{
          word_samechar = 0;
          for(int i = 0; i <(word_l - 1); i++){
            if(*(string + i) == *(search_movie -> title + i))
              word_samechar++;
          }
          if(word_samechar == (word_l - word_star)){
            search_print_m(search_movie);
            continue;
          }
        }
//title
        if(word_qnum != 0){
          word_samechar = 0;
          for(int i = 0; i <(word_l - 1); i++){
            if(*(string + i) == *(search_movie -> genre +i))
              word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
        else{
          word_samechar = 0;
          for(int i = 0; i <(word_l - 1); i++){
            if(*(string + i) == *(search_movie -> genre + i))
              word_samechar++;
          }
          if(word_samechar == (word_l - word_star)){
            search_print_m(search_movie);
            continue;
          }
        }
//genre
        if(word_qnum != 0){
          word_samechar = 0;
          for(int i = 0; i <(word_l - 1); i++){
            if(*(string + i) == *(search_movie -> director -> data +i))
              word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
        else{
          word_samechar = 0;
          for(int i = 0; i <(word_l - 1); i++){
            if(*(string + i) == *(search_movie -> director -> data + i))
              word_samechar++;
          }
          if(word_samechar == (word_l - word_star)){
            search_print_m(search_movie);
            continue;
          }
        }
//director data

        if(word_qnum != 0){
          word_samechar = 0;
          for(int i = 0; i <(word_l - 1); i++){
            if(*(string + i) == *(string_y +i))
              word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
        else{
          word_samechar = 0;
          for(int i = 0; i <(word_l - 1); i++){
            if(*(string + i) == *(string_y + i))
              word_samechar++;
          }
          if(word_samechar == (word_l - word_star)){
            search_print_m(search_movie);
            continue;
          }
        }
//year
        if(word_qnum != 0){
          word_samechar = 0;
          for(int i = 0; i <(word_l - 1); i++){
            if(*(string + i) == *(string_t +i))
              word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
        else{
          word_samechar = 0;
          for(int i = 0; i <(word_l - 1); i++){
            if(*(string + i) == *(string_t + i))
              word_samechar++;
          }
          if(word_samechar == (word_l - word_star)){
            search_print_m(search_movie);
            continue;
          }
        }
//time

        actor = search_movie -> actor;
        if(word_qnum != 0){
          word_samechar = 0;
          for(int i = 0; i < (word_l - 1); i++){
            if(*(string + i) == *(search_movie -> actor -> data + i)){
              word_samechar++;
            }
          }
          if(word_samechar == word_noq - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
        else{
          word_samechar = 0;
          for(int i = 0; i < (word_l - 1); i++){
            if(*(string + i) == *(search_movie -> actor -> data + i)){
              word_samechar++;
            }
          }
          if(word_samechar == word_l - word_star){
            search_print_m(search_movie);
            continue;
          }
        }

        while(search_movie -> actor -> next != NULL){
          word_samechar = 0;
          search_movie -> actor = search_movie -> actor -> next;
          if(word_qnum != 0){
            word_samechar = 0;
            for(int i = 0; i < (word_l - 1); i++){
              if(*(string + i) == *(search_movie -> actor -> data + i)){
                word_samechar++;
              }
            }
            if(word_samechar == word_noq - word_star){
              search_movie -> actor = actor;
              search_print_m(search_movie);
              break;
            }
          }
          else{
            word_samechar = 0;
            for(int i = 0; i < (word_l - 1); i++){
              if(*(string + i) == *(search_movie -> actor -> data + i)){
                word_samechar++;
              }
            }
            if(word_samechar == word_l - word_star){
              search_movie -> actor = actor;
              search_print_m(search_movie);
              break;
            }
          }
        }
        search_movie -> actor = actor;
      }// while
    } //if(*(string + word_l - 1) == '*')


    else if(*string == '*'){
      while(search_movie -> next_movie != movie -> tail){
        search_movie = search_movie -> next_movie;
        change_num_to_string(string_y, search_movie -> year);
        change_num_to_string(string_t, search_movie -> time);

        if(word_qnum != 0){
          data_len = strlen(search_movie -> title);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(search_movie -> title + gap_star + i))
              word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
        else{
          data_len = strlen(search_movie -> title);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(search_movie -> title + gap_star + i))
              word_samechar++;
          }
          if(word_samechar == word_l - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
//title
        if(word_qnum != 0){
          data_len = strlen(search_movie -> genre);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(search_movie -> genre + gap_star + i))
              word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
        else{
          data_len = strlen(search_movie -> genre);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(search_movie -> genre + gap_star + i))
              word_samechar++;
          }
          if(word_samechar == word_l - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
//genre
        if(word_qnum != 0){
          data_len = strlen(search_movie -> director -> data);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(search_movie -> director -> data + gap_star + i))
              word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
        else{
          data_len = strlen(search_movie -> director -> data);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(search_movie -> director -> data + gap_star + i))
              word_samechar++;
          }
          if(word_samechar == word_l - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
//director -> data
        if(word_qnum != 0){
          data_len = strlen(string_y);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(string_y + gap_star + i))
              word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
        else{
          data_len = strlen(string_y);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(string_y + i))
              word_samechar++;
          }
          if(word_samechar == word_l - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
//year
        if(word_qnum != 0){
          data_len = strlen(string_t);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(string_t + gap_star + i))
              word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
        else{
          data_len = strlen(string_t);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(string_t + i))
              word_samechar++;
          }
          if(word_samechar == word_l - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
//time
        actor = search_movie -> actor;
        if(word_qnum != 0){
          data_len = strlen(search_movie -> actor -> data);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(search_movie -> actor -> data + gap_star + i))
              word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_m(search_movie);
            continue;
          }
        }
        else{
          data_len = strlen(search_movie -> actor -> data);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(search_movie -> actor -> data + gap_star + i))
              word_samechar++;
          }
          if(word_samechar == word_l - word_star){
            search_print_m(search_movie);
            continue;
          }
        }

        while(search_movie -> actor -> next != NULL){
          search_movie -> actor = search_movie -> actor -> next;
          if(word_qnum != 0){
            data_len = strlen(search_movie -> actor -> data);
            gap_star = data_len - (word_l - word_star);
            word_samechar = 0;
            for(int i = 0; i<(word_l - 1); i++){
              if(*(string + i + 1) == *(search_movie -> actor -> data + gap_star + i))
                word_samechar++;
            }
            if(word_samechar == word_noq - word_star){
              search_movie -> actor = actor;
              search_print_m(search_movie);
              break;
            }
          }
          else{
            data_len = strlen(search_movie -> actor -> data);
            gap_star = data_len - (word_l - word_star);
            word_samechar = 0;
            for(int i = 0; i<(word_l - 1); i++){
              if(*(string + i + 1) == *(search_movie -> actor -> data + gap_star + i))
                word_samechar++;
            }
            if(word_samechar == word_l - word_star){
              search_movie -> actor = actor;
              search_print_m(search_movie);
              break;
            }
          }
        }
        search_movie -> actor = actor;

      } //while
    } //else if(*string == '*')
  } //if word_star

  else if(word_l == word_noq){
    while(search_movie -> next_movie != movie -> tail){
      search_movie = search_movie -> next_movie;
      if(strcmp(string, search_movie->title) == 0){
        search_print_m(search_movie);
        continue;
      }
      if(strcmp(string, search_movie -> genre) == 0){
        search_print_m(search_movie);
        continue;
      }
      if(strcmp(string, search_movie -> director -> data) == 0){
        search_print_m(search_movie);
        continue;
      }
      if(atoi(string) == search_movie -> year){
        search_print_m(search_movie);
        continue;
      }
      if(atoi(string) == search_movie -> time){
        search_print_m(search_movie);
        continue;
      }
      else{
        actor = search_movie -> actor;
        while(search_movie -> actor -> next != NULL || strcmp(search_movie -> actor -> data, string) == 0){
        if(strcmp(search_movie -> actor -> data, string) == 0){
          search_movie -> actor = actor;
          search_print_m(search_movie);
          break;
          }
          search_movie -> actor = search_movie -> actor -> next;
        }
        search_movie -> actor = actor;
      }
    }
  }

  else if(word_l != word_noq){
    while(search_movie -> next_movie != movie -> tail){
      search_movie = search_movie -> next_movie;
      change_num_to_string(string_y, search_movie -> year);
      change_num_to_string(string_t, search_movie -> time);

      if(strlen(search_movie -> title) == word_l){
        word_samechar = 0;
        for(int i = 0; i < word_l; i++){
          if(*(string + i) == *(search_movie -> title + i))
            word_samechar++;
          }
        if(word_samechar == word_noq){
          search_print_m(search_movie);
          continue;
        }
      }
      if(strlen(search_movie -> genre) == word_l){
        word_samechar = 0;
        for(int i = 0; i < word_l; i++){
          if(*(string + i) == *(search_movie -> genre + i))
            word_samechar++;
          }
        if(word_samechar == word_noq){
          search_print_m(search_movie);
          continue;
        }
      }

      if(strlen(search_movie -> director -> data) == word_l){
        word_samechar = 0;
        for(int i = 0; i < word_l; i++){
          if(*(string + i) == *(search_movie -> director -> data + i))
            word_samechar++;
          }
        if(word_samechar == word_noq){
          search_print_m(search_movie);
          continue;
        }
      }

      if(strlen(string_y) == word_l){
        word_samechar = 0;
        for(int i = 0; i < word_l; i++){
          if(*(string + i) == *(string_y + i))
            word_samechar++;
          }
        if(word_samechar == word_noq){
          search_print_m(search_movie);
          continue;
        }
      }
      if(strlen(string_t) == word_l){
        word_samechar = 0;
        for(int i = 0; i < word_l; i++){
          if(*(string + i) == *(string_t + i))
            word_samechar++;
          }
        if(word_samechar == word_noq){
          search_print_m(search_movie);
          continue;
        }
      }

      actor = search_movie -> actor;
      if(strlen(search_movie -> actor -> data) == word_l){
        word_samechar = 0;
        for(int i = 0; i < word_l; i++){
          if(*(string + i) == *(search_movie -> actor -> data +i))
            word_samechar++;
        }
      if(word_samechar == word_noq){
        search_print_m(search_movie);
        continue;
        }
      }

      while(search_movie -> actor -> next != NULL){
        word_samechar = 0;
        search_movie -> actor = search_movie -> actor -> next;
        if(strlen(search_movie -> actor -> data) == word_l){
          for(int i = 0; i < word_l; i++){
            if(*(string + i) == *(search_movie -> actor -> data + i))
              word_samechar++;
          }
            if(word_samechar == word_noq){
              search_movie -> actor = actor;
              search_print_m(search_movie);
              break;
            }
        }
      }

      search_movie -> actor = actor;
    } //while
  } //else if

} // function
void search_print_d(LINK_D *search_director){
  LINK_W *title;
  title = search_director -> title;
  printf("%d:%s:%c:%d:", search_director -> serial_num, search_director -> name, search_director ->sex, search_director -> birth );
  while(search_director -> title -> next != NULL){
    printf("%s,", search_director -> title -> data);
    search_director -> title = search_director -> title -> next;
  }
  printf("%s\n", search_director -> title -> data);
  search_director -> title = title;
}

void search_d(ALL_D *director, char *string){
  LINK_W *title;
  char *string_y;
  LINK_D *search_director = director -> head;
  string_y = (char *)calloc(sizeof(char), 8);
  int word_l = 0;
  int word_noq = 0;
  int word_samechar =0;
  int word_star = 0;
  int word_qnum = 0;
  int data_len = 0;
  int gap_star = 0;
  word_l =strlen(string);

  for(int i = 0; i < word_l; i++){
    if(*(string +i) == '*')
    word_star++;
  }

  for(int i = 0; i < word_l; i++){
    if(*(string + i) != '?')
    word_noq++;
  }

  for(int i = 0; i < word_l; i++){
    if(*(string + i) == '?')
    word_qnum++;
  }

  if(word_star != 0){
    if(*(string + word_l - 1) == '*'){
      while(search_director -> next_director != director -> tail){
        search_director = search_director -> next_director;
        change_num_to_string(string_y, search_director -> birth);

        if(word_qnum != 0){
          word_samechar = 0;
          for(int i = 0; i < (word_l -1); i++){
            if(*(string + i ) == *(search_director -> name + i))
            word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_d(search_director);
            continue;
          }
        }
        else{
          word_samechar =0;
          for(int i = 0; i < (word_l - 1); i++){
            if(*(string + i ) == *(search_director -> name + i))
              word_samechar++;
          }
          if(word_samechar == (word_l - word_star)){
            search_print_d(search_director);
            continue;
            }
          }
          //Name
          if(word_qnum != 0){
            word_samechar = 0;
            for(int i = 0; i <(word_l - 1); i++){
              if(*(string + i) == *(string_y +i))
                word_samechar++;
            }
            if(word_samechar == word_noq - word_star){
              search_print_d(search_director);
              continue;
            }
          }
          else{
            word_samechar = 0;
            for(int i = 0; i <(word_l - 1); i++){
              if(*(string + i) == *(string_y + i))
                word_samechar++;
            }
            if(word_samechar == (word_l - word_star)){
              search_print_d(search_director);
              continue;
            }
          }
          //Birth
          title = search_director -> title;
          if(word_qnum != 0){
            word_samechar = 0;
            for(int i = 0; i < (word_l - 1); i++){
              if(*(string + i) == *(search_director -> title -> data + i)){
                word_samechar++;
              }
            }
            if(word_samechar == word_noq - word_star){
              search_print_d(search_director);
              continue;
            }
          }
          else{
            word_samechar = 0;
            for(int i = 0; i < (word_l - 1); i++){
              if(*(string + i) == *(search_director -> title -> data + i)){
                word_samechar++;
              }
            }
            if(word_samechar == word_l - word_star){
              search_print_d(search_director);
              continue;
            }
          }

          while(search_director -> title -> next != NULL){
            word_samechar = 0;
            search_director -> title = search_director -> title -> next;
            if(word_qnum != 0){
              word_samechar = 0;
              for(int i = 0; i < (word_l - 1); i++){
                if(*(string + i) == *(search_director -> title -> data + i)){
                  word_samechar++;
                }
              }
              if(word_samechar == word_noq - word_star){
                search_director -> title = title;
                search_print_d(search_director);
                break;
              }
            }
            else{
              word_samechar = 0;
              for(int i = 0; i < (word_l - 1); i++){
                if(*(string + i) == *(search_director -> title -> data + i)){
                  word_samechar++;
                }
              }
              if(word_samechar == word_l - word_star){
                search_director -> title = title;
                search_print_d(search_director);
                break;
              }
            }
          }
          search_director -> title = title;
      } //while
    } //if(*(string + word_l - 1) == '*')

    else if(*string == '*'){
      while(search_director -> next_director != director -> tail){
        search_director = search_director -> next_director;
        change_num_to_string(string_y, search_director -> birth);

        if(word_qnum != 0){
          data_len = strlen(search_director -> name);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i < (word_l - 1); i++){
            if(*(string + i + 1) == *(search_director -> name + gap_star + i))
            word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_d(search_director);
            continue;
          }
        }
        else{
          data_len = strlen(search_director -> name);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l -1); i++){
            if(*(string + i + 1) == *(search_director -> name + gap_star + i))
            word_samechar++;
          }
          if(word_samechar == word_l - word_star){
            search_print_d(search_director);
            continue;
          }
        }
        if(word_qnum != 0){
          data_len = strlen(string_y);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i < (word_l - 1); i++){
            if(*(string + i + 1) == *(string_y + gap_star + i))
            word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_d(search_director);
            continue;
          }
        }
        else{
          data_len = strlen(string_y);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i < (word_l -1); i++){
            if(*(string + i + 1) == *(string_y + i))
            word_samechar++;
          }
          if(word_samechar == word_l - word_star){
            search_print_d(search_director);
            continue;
          }
        }

        title = search_director -> title;
        if(word_qnum != 0){
          data_len = strlen(search_director -> title -> data);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(search_director -> title -> data + gap_star + i))
              word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_d(search_director);
            continue;
          }
        }
        else{
          data_len = strlen(search_director -> title -> data);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(search_director -> title -> data + gap_star + i))
              word_samechar++;
          }
          if(word_samechar == word_l - word_star){
            search_print_d(search_director);
            continue;
          }
        }

        while(search_director -> title -> next != NULL){
          search_director -> title = search_director -> title -> next;
          if(word_qnum != 0){
            data_len = strlen(search_director -> title -> data);
            gap_star = data_len - (word_l - word_star);
            word_samechar = 0;
            for(int i = 0; i<(word_l - 1); i++){
              if(*(string + i + 1) == *(search_director -> title -> data + gap_star + i))
                word_samechar++;
            }
            if(word_samechar == word_noq - word_star){
              search_director -> title = title;
              search_print_d(search_director);
              break;
            }
          }

          else{
            data_len = strlen(search_director -> title -> data);
            gap_star = data_len - (word_l - word_star);
            word_samechar = 0;
            for(int i = 0; i<(word_l - 1); i++){
              if(*(string + i + 1) == *(search_director -> title -> data + gap_star + i))
                word_samechar++;
            }
            if(word_samechar == word_l - word_star){
              search_director -> title = title;
              search_print_d(search_director);
              break;
            }
          }
        }
        search_director -> title = title;
      } //while
    } //(*string == '*')
  } //if(word_star != 0)
  else if(word_l == word_noq){
    while(search_director -> next_director != director -> tail){
      search_director = search_director -> next_director;
      if(strcmp(string, search_director -> name) == 0){
        search_print_d(search_director);
        continue;
      }
      if(*string == search_director -> sex){
        search_print_d(search_director);
        continue;
      }
      if(atoi(string) == search_director -> birth){
        search_print_d(search_director);
        continue;
      }
      else{
        title = search_director -> title;
        while(search_director -> title -> next != NULL || strcmp(search_director -> title -> data, string) == 0){
          if(strcmp(search_director -> title -> data, string) == 0){
            search_director -> title = title;
            search_print_d(search_director);
            break;
          }
          search_director -> title = search_director -> title -> next;
        }
        search_director ->title = title;
      }
    }
  }
  else if(word_l != word_noq){
    while(search_director -> next_director != director -> tail){
      search_director = search_director -> next_director;
      change_num_to_string(string_y, search_director -> birth);

      if(strlen(search_director -> name) == word_l){
        word_samechar =0;
        for(int i =0; i < word_l; i++){
          if(*(string + i) == *(search_director -> name +i))
          word_samechar++;
        }
        if(word_samechar == word_noq){
          search_print_d(search_director);
          continue;
        }
      }
      //name
      if(strlen(string_y) == word_l){
        word_samechar = 0;
        for(int i = 0; i < word_l; i++){
          if(*(string + i) == *(string_y + i))
            word_samechar++;
        }
        if(word_samechar == word_noq){
          search_print_d(search_director);
          continue;
        }
      }
      //Birth
      if(word_l == 1){
        if(*string == '?'){
          search_print_d(search_director);
          continue;
        }
      }
      // ?한개만 있을때
      title = search_director -> title;
      if(strlen(search_director -> title -> data) == word_l){
        word_samechar = 0;
        for(int i = 0; i < word_l; i++){
          if(*(string + i) == *(search_director -> title -> data +i))
            word_samechar++;
        }
        if(word_samechar == word_noq){
          search_print_d(search_director);
          continue;
        }
      }
      title = search_director -> title;
      if(strlen(search_director -> title -> data) == word_l){
        word_samechar = 0;
        for(int i = 0; i < word_l; i++){
          if(*(string + i) == *(search_director -> title -> data +i))
            word_samechar++;
        }
      if(word_samechar == word_noq){
        search_print_d(search_director);
        continue;
        }
      }

      while(search_director -> title -> next != NULL){
        word_samechar = 0;
        search_director -> title = search_director -> title -> next;
        if(strlen(search_director -> title -> data) == word_l){
          for(int i = 0; i < word_l; i++){
            if(*(string + i) == *(search_director -> title -> data + i))
              word_samechar++;
          }
            if(word_samechar == word_noq){
              search_director -> title = title;
              search_print_d(search_director);
              break;
            }
        }
      }
      search_director -> title = title;
    } //while
  } //else(word_l != word_noq)

} //function

void search_print_a(LINK_A *search_actor){
  LINK_W *title;
  title = search_actor -> title;
  printf("%d:%s:%c:%d:", search_actor -> serial_num, search_actor -> name, search_actor ->sex, search_actor -> birth );
  while(search_actor -> title -> next != NULL){
    printf("%s,", search_actor -> title -> data);
    search_actor -> title = search_actor -> title -> next;
  }
  printf("%s\n", search_actor -> title -> data);
  search_actor -> title = title;
}

void search_a(ALL_A *actor, char *string){
  LINK_W *title;
  char *string_y;
  LINK_A *search_actor = actor -> head;
  string_y = (char *)calloc(sizeof(char), 8);
  int word_l = 0;
  int word_noq = 0;
  int word_samechar =0;
  int word_star = 0;
  int word_qnum = 0;
  int data_len = 0;
  int gap_star = 0;
  word_l =strlen(string);

  for(int i = 0; i < word_l; i++){
    if(*(string +i) == '*')
    word_star++;
  }

  for(int i = 0; i < word_l; i++){
    if(*(string + i) != '?')
    word_noq++;
  }

  for(int i = 0; i < word_l; i++){
    if(*(string + i) == '?')
    word_qnum++;
  }

  if(word_star != 0){
    if(*(string + word_l - 1) == '*'){  //*마지막
      while(search_actor -> next_actor != actor -> tail){
        search_actor = search_actor -> next_actor;
        change_num_to_string(string_y, search_actor -> birth);

        if(word_qnum != 0){ //*마지막에 ?가 중간
          word_samechar = 0;
          for(int i = 0; i < (word_l -1); i++){
            if(*(string + i ) == *(search_actor -> name + i))
            word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_a(search_actor);
            continue;
          }
        }
        else{
          word_samechar =0;
          for(int i = 0; i < (word_l - 1); i++){
            if(*(string + i ) == *(search_actor -> name + i))
              word_samechar++;
          }
          if(word_samechar == (word_l - word_star)){
            search_print_a(search_actor);
            continue;
            }
          }
          //Name
          if(word_qnum != 0){
            word_samechar = 0;
            for(int i = 0; i <(word_l - 1); i++){
              if(*(string + i) == *(string_y +i))
                word_samechar++;
            }
            if(word_samechar == word_noq - word_star){
              search_print_a(search_actor);
              continue;
            }
          }
          else{
            word_samechar = 0;
            for(int i = 0; i <(word_l - 1); i++){
              if(*(string + i) == *(string_y + i))
                word_samechar++;
            }
            if(word_samechar == (word_l - word_star)){
              search_print_a(search_actor);
              continue;
            }
          }
          //Birth
          title = search_actor -> title;
          if(word_qnum != 0){
            word_samechar = 0;
            for(int i = 0; i < (word_l - 1); i++){
              if(*(string + i) == *(search_actor -> title -> data + i)){
                word_samechar++;
              }
            }
            if(word_samechar == word_noq - word_star){
              search_print_a(search_actor);
              continue;
            }
          }
          else{
            word_samechar = 0;
            for(int i = 0; i < (word_l - 1); i++){
              if(*(string + i) == *(search_actor -> title -> data + i)){
                word_samechar++;
              }
            }
            if(word_samechar == word_l - word_star){
              search_print_a(search_actor);
              continue;
            }
          }

          while(search_actor -> title -> next != NULL){
            word_samechar = 0;
            search_actor -> title = search_actor -> title -> next;
            if(word_qnum != 0){
              word_samechar = 0;
              for(int i = 0; i < (word_l - 1); i++){
                if(*(string + i) == *(search_actor -> title -> data + i)){
                  word_samechar++;
                }
              }
              if(word_samechar == word_noq - word_star){
                search_actor -> title = title;
                search_print_a(search_actor);
                break;
              }
            }
            else{
              word_samechar = 0;
              for(int i = 0; i < (word_l - 1); i++){
                if(*(string + i) == *(search_actor -> title -> data + i)){
                  word_samechar++;
                }
              }
              if(word_samechar == word_l - word_star){
                search_actor -> title = title;
                search_print_a(search_actor);
                break;
              }
            }
          }
          search_actor -> title = title;
      } //while
    } //if(*(string + word_l - 1) == '*')

    else if(*string == '*'){  //*가 앞에
      while(search_actor -> next_actor != actor -> tail){
        search_actor = search_actor -> next_actor;
        change_num_to_string(string_y, search_actor -> birth);

        if(word_qnum != 0){
          data_len = strlen(search_actor -> name);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i < (word_l - 1); i++){
            if(*(string + i + 1) == *(search_actor -> name + gap_star + i))
            word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_a(search_actor);
            continue;
          }
        }
        else{
          data_len = strlen(search_actor -> name);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l -1); i++){
            if(*(string + i + 1) == *(search_actor -> name + gap_star + i))
            word_samechar++;
          }
          if(word_samechar == word_l - word_star){
            search_print_a(search_actor);
            continue;
          }
        }
        if(word_qnum != 0){
          data_len = strlen(string_y);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i < (word_l - 1); i++){
            if(*(string + i + 1) == *(string_y + gap_star + i))
            word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_a(search_actor);
            continue;
          }
        }
        else{
          data_len = strlen(string_y);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i < (word_l -1); i++){
            if(*(string + i + 1) == *(string_y + i))
            word_samechar++;
          }
          if(word_samechar == word_l - word_star){
            search_print_a(search_actor);
            continue;
          }
        }

        title = search_actor -> title;
        if(word_qnum != 0){
          data_len = strlen(search_actor -> title -> data);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(search_actor -> title -> data + gap_star + i))
              word_samechar++;
          }
          if(word_samechar == word_noq - word_star){
            search_print_a(search_actor);
            continue;
          }
        }
        else{
          data_len = strlen(search_actor -> title -> data);
          gap_star = data_len - (word_l - word_star);
          word_samechar = 0;
          for(int i = 0; i<(word_l - 1); i++){
            if(*(string + i + 1) == *(search_actor -> title -> data + gap_star + i))
              word_samechar++;
          }
          if(word_samechar == word_l - word_star){
            search_print_a(search_actor);
            continue;
          }
        }

        while(search_actor -> title -> next != NULL){
          search_actor -> title = search_actor -> title -> next;
          if(word_qnum != 0){
            data_len = strlen(search_actor -> title -> data);
            gap_star = data_len - (word_l - word_star);
            word_samechar = 0;
            for(int i = 0; i<(word_l - 1); i++){
              if(*(string + i + 1) == *(search_actor -> title -> data + gap_star + i))
                word_samechar++;
            }
            if(word_samechar == word_noq - word_star){
              search_actor -> title = title;
              search_print_a(search_actor);
              break;
            }
          }

          else{
            data_len = strlen(search_actor -> title -> data);
            gap_star = data_len - (word_l - word_star);
            word_samechar = 0;
            for(int i = 0; i<(word_l - 1); i++){
              if(*(string + i + 1) == *(search_actor -> title -> data + gap_star + i))
                word_samechar++;
            }
            if(word_samechar == word_l - word_star){
              search_actor -> title = title;
              search_print_a(search_actor);
              break;
            }
          }
        }
        search_actor -> title = title;
      } //while
    } //(*string == '*')
  } //if(word_star != 0)
  else if(word_l == word_noq){
    while(search_actor -> next_actor != actor -> tail){
      search_actor = search_actor -> next_actor;
      if(strcmp(string, search_actor -> name) == 0){
        search_print_a(search_actor);
        continue;
      }
      if(*string == search_actor -> sex){
        search_print_a(search_actor);
        continue;
      }
      if(atoi(string) == search_actor -> birth){
        search_print_a(search_actor);
        continue;
      }
      else{
        title = search_actor -> title;
        while(search_actor -> title -> next != NULL || strcmp(search_actor -> title -> data, string) == 0){
          if(strcmp(search_actor -> title -> data, string) == 0){
            search_actor -> title = title;
            search_print_a(search_actor);
            break;
          }
          search_actor -> title = search_actor -> title -> next;
        }
        search_actor ->title = title;
      }
    }
  }
  else if(word_l != word_noq){
    while(search_actor -> next_actor != actor -> tail){
      search_actor = search_actor -> next_actor;
      change_num_to_string(string_y, search_actor -> birth);

      if(strlen(search_actor -> name) == word_l){
        word_samechar =0;
        for(int i =0; i < word_l; i++){
          if(*(string + i) == *(search_actor -> name +i))
          word_samechar++;
        }
        if(word_samechar == word_noq){
          search_print_a(search_actor);
          continue;
        }
      }
      //name
      if(strlen(string_y) == word_l){
        word_samechar = 0;
        for(int i = 0; i < word_l; i++){
          if(*(string + i) == *(string_y + i))
            word_samechar++;
        }
        if(word_samechar == word_noq){
          search_print_a(search_actor);
          continue;
        }
      }
      //Birth
      if(word_l == 1){
        if(*string == '?'){
          search_print_a(search_actor);
          continue;
        }
      }
      // ?한개만 있을때
      title = search_actor -> title;
      if(strlen(search_actor -> title -> data) == word_l){
        word_samechar = 0;
        for(int i = 0; i < word_l; i++){
          if(*(string + i) == *(search_actor -> title -> data +i))
            word_samechar++;
        }
        if(word_samechar == word_noq){
          search_print_a(search_actor);
          continue;
        }
      }
      title = search_actor -> title;
      if(strlen(search_actor -> title -> data) == word_l){
        word_samechar = 0;
        for(int i = 0; i < word_l; i++){
          if(*(string + i) == *(search_actor -> title -> data +i))
            word_samechar++;
        }
      if(word_samechar == word_noq){
        search_print_a(search_actor);
        continue;
        }
      }

      while(search_actor -> title -> next != NULL){
        word_samechar = 0;
        search_actor -> title = search_actor -> title -> next;
        if(strlen(search_actor -> title -> data) == word_l){
          for(int i = 0; i < word_l; i++){
            if(*(string + i) == *(search_actor -> title -> data + i))
              word_samechar++;
          }
            if(word_samechar == word_noq){
              search_actor -> title = title;
              search_print_a(search_actor);
              break;
            }
        }
      }
      search_actor -> title = title;
    } //while
  } //else(word_l != word_noq)

} //function

void commands(ALL_D *director,ALL_A *actor,ALL_M *movie){
  //명령어를 입력받아서 그에 해당하는 함수를 출력하기 위해 선언한 변수
  //serach print add update delete sort save end
  char *command,*input_data,*string, *check_input_data;//문자열을 입력받을 char형 포인터 배열을 선언하였음
  char input_text,option;//옵션을 받을 char형 변수를 선언함
  char checking;
  FILE *check_movie,*check_actor,*check_director;
  check_movie = fopen("movie_list","r");
  check_actor = fopen("actor_list","r");
  check_director = fopen("director_list","r");
  if(check_movie != NULL)
  copy_movie_list();
  if(check_actor != NULL)
  copy_actor_list();
  if(check_director != NULL)
  copy_director_list();
  fclose(check_movie);
  fclose(check_actor);
  fclose(check_director);
  int input_num,i;
  scan_movie_log(movie);
  scan_actor_log(actor);
  scan_director_log(director);
  compare_movie_word(movie,director,actor);
  compare_actor_word(movie,actor);
  compare_director_word(movie,director);
    printf("You can use add, update, delete, search, sort, save, end commands.\n");//사용 가능한 명령어를 보여줌
  while(1){
    printf("(movie)");
    input_data = (char *)calloc(100,sizeof(char));
  command =(char *)malloc(sizeof(char)*10);//command포인터에 동적 메모리를 할당하였음
  check_input_data = (char *)calloc(100,sizeof(char));
  string = (char *)calloc(100,sizeof(char));
  scanf("%s", command);//명령어를 입력받음
  if(strcmp(command, "add") == 0){//파일에 새로운 내용을 더함
    scanf("%*c%c%*c",&input_text);//'\n'을 무시하고 명령어를 입력받음
    if(input_text=='d'){//director에 내용을 추가함
      add_director(director);
      free(movie);
      free(director);
      free(actor);
      make_movie_admin(&movie);
      make_actor_admin(&actor);
      make_director_admin(&director);
      scan_movie_log(movie);
      scan_actor_log(actor);
      scan_director_log(director);
      compare_movie_word(movie,director,actor);
      compare_actor_word(movie,actor);
      compare_director_word(movie,director);
    printf("@@done\n");
  }
    else if(input_text=='m'){//movie에 내용을 추가함
      add_movie(movie);
      free(movie);
      free(director);
      free(actor);
      make_movie_admin(&movie);
      make_actor_admin(&actor);
      make_director_admin(&director);
      scan_movie_log(movie);
      scan_actor_log(actor);
      scan_director_log(director);
      compare_movie_word(movie,director,actor);
      compare_actor_word(movie,actor);
      compare_director_word(movie,director);
    printf("@@done\n");
  }
    else if(input_text=='a'){//actor에 내용을 추가함
      add_actor(actor);
      free(movie);
      free(director);
      free(actor);
      make_movie_admin(&movie);
      make_actor_admin(&actor);
      make_director_admin(&director);
      scan_movie_log(movie);
      scan_actor_log(actor);
      scan_director_log(director);
      compare_movie_word(movie,director,actor);
      compare_actor_word(movie,actor);
      compare_director_word(movie,director);
    printf("@@done\n");
  }
}
  else if(strcmp(command,"update")==0){//파일을 업데이트함
    scanf("%*c%c",&input_text);
    if(input_text=='m'){
      getchar();
			scanf("%s%*c",input_data);
			scanf("%d%*c",&input_num);
			printf("%d\n",input_num);
			update_movie(movie,input_data,input_num);
			free(movie);
			make_movie_admin(&movie);
			scan_movie_log(movie);
			compare_movie_word(movie,director,actor);
      compare_actor_word(movie,actor);
      compare_director_word(movie,director);
	  }
    else if(input_text=='a'){
			getchar();
			scanf("%s%*c",input_data);
			scanf("%d%*c",&input_num);
			printf("%d\n",input_num);
			update_actor(actor,input_data,input_num);
			free(actor);
			make_actor_admin(&actor);
			scan_actor_log(actor);
			compare_movie_word(movie,director,actor);
      compare_actor_word(movie,actor);
      compare_director_word(movie,director);
    }
    else if(input_text='d'){
			getchar();
			scanf("%s%*c",input_data);
			scanf("%d%*c",&input_num);
			printf("%d\n",input_num);
			update_director(director,input_data,input_num);
			free(director);
			make_director_admin(&director);
			scan_director_log(director);
			compare_movie_word(movie,director,actor);
      compare_actor_word(movie,actor);
      compare_director_word(movie,director);
    }
  }
  else if(strcmp(command,"delete")==0){//파일에서 내용을 지워줌
    scanf("%*c%c",&input_text);
    if(input_text == 'm'){
      scanf("%d",&input_num);
      i=delete_movie(movie,input_num);
      compare_movie_word(movie,director,actor);
      compare_actor_word(movie,actor);
      compare_director_word(movie,director);
      if(i!=1)
      printf("No such record\n");
    }
    else if(input_text == 'a'){
      scanf("%d",&input_num);
      i=delete_actor(actor,input_num);
      compare_movie_word(movie,director,actor);
      compare_actor_word(movie,actor);
      compare_director_word(movie,director);
      if(i!=1)
      printf("No such record\n");
  }
  else if(input_text == 'd'){
    scanf("%d",&input_num);
    i=delete_director(director,input_num);
    compare_movie_word(movie,director,actor);
    compare_actor_word(movie,actor);
    compare_director_word(movie,director);
    if(i!=1)
    printf("No such record\n");
  }
}
  else if(strcmp(command,"sort")==0){//파일의 내용을 정렬하여서 출력함
    scanf("%*c%c",&input_text);
    scanf("%*c%c",&option);
    char check = getchar();
    if(check == '\n'){
    sorting(movie,actor,director,input_text,option);
    free(movie);
    free(director);
    free(actor);
    make_movie_admin(&movie);
    make_actor_admin(&actor);
    make_director_admin(&director);
    scan_movie_log(movie);
    scan_actor_log(actor);
    scan_director_log(director);
    compare_movie_word(movie,director,actor);
    compare_actor_word(movie,actor);
    compare_director_word(movie,director);
  }
    else if(check ==' '){
      char checking;
      scanf("%*c%c",&checking);
      if(checking == 'f'){
      getchar();
      sorting_file(movie,actor,director,input_text,option);
      make_movie_admin(&movie);
      make_actor_admin(&actor);
      make_director_admin(&director);
      scan_movie_log(movie);
      scan_actor_log(actor);
      scan_director_log(director);
      compare_movie_word(movie,director,actor);
      compare_actor_word(movie,actor);
      compare_director_word(movie,director);
    }
    }
  }
  else if(strcmp(command,"print")==0){//파일의 내용을 출력함
    scanf("%*c%c%*c",&input_text);
    if(input_text=='m'){
      scanf("%d",&input_num);
      i=print_movie_list(movie,input_num);
      if(i!=1){
        printf("No such record\n");
      }
    }
      else if(input_text =='a'){
        scanf("%d",&input_num);
        i=print_actor_list(actor,input_num);
        if(i!=1)
          printf("No such record\n");
      }
      else if(input_text =='d'){
        scanf("%d",&input_num);
        i=print_director_list(director,input_num);
        if(i!=1)
          printf("No such record\n");
      }
    }
  else if(strcmp(command,"search")==0){//키워드 검색으로 리스트 상에서 지정된 문자열을 찾아서 출력함
    getchar();
    checking = getchar();
    if(checking == '-'){
      scanf("%s%*c",input_data);
      scanf("%[^\n]%*c",string);
      i = strlen(input_data);
      for(int j = 0; j < i; j++){
        if(*(input_data + j) == 'm') *(check_input_data + 0) = 'm';
        if(*(input_data + j) == 'd') *(check_input_data + 1) = 'd';
        if(*(input_data + j) == 'a') *(check_input_data + 2) = 'a';
      }
      if(*(check_input_data + 0) == 'm')
        search_m(movie, string);
      if(*(check_input_data + 1) == 'd')
        search_d(director, string);
      if(*(check_input_data + 2) == 'a')
        search_a(actor, string);
    }
    else{
      scanf("%[^\n]%*c",string);
      *input_data = checking;
      strcat(input_data,string);
      search_m(movie, input_data);
      search_d(director, input_data);
      search_a(actor, input_data);
    }
  }
  else if(strcmp(command,"save")==0){//LIST파일에 내용을 저장함
    scanf("%*c%c",&input_text);
    checking = getchar();
    if(checking == '\n'){
      save(movie,actor,director,input_text);
    }
    else if(checking == ' '){
      scanf("%*c%s%*c",input_data);
      checking = getchar();
      if(checking == '-'){
        checking = getchar();
        if(checking == 'f'){
          save_file(movie,actor,director,input_text,input_data);
      }
      }
    }
  }
  else if(strcmp(command,"end")==0){//프로그램의 실행을 멈춘다
    //파일의 내용을 저장할 필요가 있으면 저장도 해주면 좋다
    copy_movie_list();
    copy_actor_list();
    copy_director_list();
    free(movie);
    free(actor);
    free(director);
    make_movie_admin(&movie);
    make_actor_admin(&actor);
    make_director_admin(&director);
    scan_movie_log(movie);
    scan_actor_log(actor);
    scan_director_log(director);
    exit(1);
  }
  }
}
void cntrl_c(int sig){//신호를 받아서 ctrl + C 를 통해 끄려고 할 때 끌지 말지 유무를 물어봄
  char *answer; //나중에 포인터로 바꾸기
  answer=(char *)calloc(100,sizeof(char));
  printf("\n프로그램을 종료하시겠습니까? (y/n)");
  scanf("%[^\n]%*c", answer);
  if(*answer =='y' || *answer == 'Y')
    exit(1);
  }
int copy_movie_list(){
    FILE *f1, *f2;
    char *line;
    char c;
    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    int y, m1, d, h, m2, s;
    char *filename;
    line = (char *)calloc(1024,sizeof(char));
    filename = (char *)calloc(1024,sizeof(char));
  f1 = fopen("movie_list", "r");
  f2 = fopen("movie_list", "a");
  y = t -> tm_year+1900;
  m1 = t -> tm_mon;
  d = t -> tm_mday;
  h = t -> tm_hour;
  m2 = t -> tm_min;
  s = t -> tm_sec;
  sprintf(filename, "movie_list.%d%d%d%d%d", t -> tm_year+1900, t -> tm_mon+1,t -> tm_hour, t -> tm_min, t -> tm_sec);
  //파일열기
  f2 = fopen(filename, "w");

  //파일 한라인씩 읽고 복사 fgets 교과서에 나와있는데 프로젝트에 사용가능한지 모름
  while(fgets(line, len -1, f1))
  {
    fputs(line, f2);
  }
  fclose(f1);
  fclose(f2);
  free(line);
  free(filename);
  }
  int copy_actor_list(){
      FILE *f1, *f2;
      char *line;
      char c;
      struct tm *t;
      time_t now;
      now = time(NULL);
      t = localtime(&now);
      int y, m1, d, h, m2, s;
      char *filename;
      line = (char *)calloc(1024,sizeof(char));
      filename = (char *)calloc(1024,sizeof(char));
    f1 = fopen("actor_list", "r");
    f2 = fopen("actor_list", "a");
    y = t -> tm_year+1900;
    m1 = t -> tm_mon;
    d = t -> tm_mday;
    h = t -> tm_hour;
    m2 = t -> tm_min;
    s = t -> tm_sec;
    sprintf(filename, "actor_list.%d%d%d%d%d", t -> tm_year+1900, t -> tm_mon+1, t -> tm_hour, t -> tm_min, t -> tm_sec);
    //파일열기
    f2 = fopen(filename, "w");

    //파일 한라인씩 읽고 복사 fgets 교과서에 나와있는데 프로젝트에 사용가능한지 모름
    while(fgets(line, len -1, f1))
    {
      fputs(line, f2);
    }
    fclose(f1);
    fclose(f2);
    free(line);
    free(filename);
    }
    int copy_director_list(){
        FILE *f1, *f2;
        char *line;
        char c;
        struct tm *t;
        time_t now;
        now = time(NULL);
        t = localtime(&now);
        int y, m1, d, h, m2, s;
        char *filename;
        line = (char *)calloc(1024,sizeof(char));
        filename = (char *)calloc(1024,sizeof(char));
      f1 = fopen("director_list", "r");
      f2 = fopen("director_list", "a");
      y = t -> tm_year+1900;
      m1 = t -> tm_mon;
      d = t -> tm_mday;
      h = t -> tm_hour;
      m2 = t -> tm_min;
      s = t -> tm_sec;
      sprintf(filename, "director_list.%d%d%d%d%d", t -> tm_year+1900, t -> tm_mon+1, t -> tm_hour, t -> tm_min, t -> tm_sec);
      //파일열기
      f2 = fopen(filename, "w");

      //파일 한라인씩 읽고 복사 fgets 교과서에 나와있는데 프로젝트에 사용가능한지 모름
      while(fgets(line, len -1, f1))
      {
        fputs(line, f2);
      }
      fclose(f1);
      fclose(f2);
      free(line);
      free(filename);
      }

    void compare_movie_word(ALL_M *movie,ALL_D *director,ALL_A *actor){
      LINK_M *com_movie,*link_movie;
      LINK_A *com_actor,*link_actor;
      LINK_D *com_director,*link_director;
      LINK_W *com_word,*last_word,*last;
      com_movie = movie -> head;
      link_movie = movie -> head;
      com_actor = actor -> head;
      com_director = director -> head;
      link_director = director -> head;
      link_actor = actor -> head;
      while(com_movie -> next_movie != movie -> tail){//무비 구조체 확인
        com_movie = com_movie -> next_movie;
        while(com_director -> next_director != director -> tail){//디렉터를 끝까지 보내
          com_director = com_director -> next_director;
          if(strcmp(com_movie -> director -> data,com_director -> name)==0){
            //무비의 영화 감독과 감독의 이름이 같을 때
            com_movie -> director -> com_word = com_director;
          }
        }
        com_director = link_director;
      }
      com_movie = movie -> head;
      while(com_movie -> next_movie != movie -> tail){
        com_movie = com_movie -> next_movie;
        com_word = com_movie -> actor;
        while(com_actor -> next_actor != actor -> tail){
          com_actor = com_actor -> next_actor;
          if(strcmp(com_word -> data,com_actor -> name)==0){
            com_word -> com_word = com_actor;
          }
        }
              com_actor = actor -> head;
              while(com_word -> next != NULL){
          last = com_word -> next;
          com_actor = actor -> head;
          while(com_actor -> next_actor != actor -> tail){
            com_actor = com_actor -> next_actor;
            last = com_word -> next;
            if(strcmp(com_word -> data,com_actor -> name)==0){
              com_word -> com_word = com_actor;
            }
            else if(strcmp(last -> data,com_actor -> name)==0){
              last -> com_word = com_actor;
            }
          }
          com_word = com_word -> next;
        }
      }
    }
    void compare_actor_word(ALL_M *movie,ALL_A *actor){
      LINK_M *com_movie;
      LINK_A *com_actor;
      LINK_W *com_word,*last_word,*last;
      com_movie = movie -> head;
      com_actor = actor -> head;
      while(com_actor -> next_actor != actor -> tail){
        com_actor = com_actor -> next_actor;
        com_word = com_actor -> title;
        while(com_movie -> next_movie != movie -> tail){
          com_movie = com_movie -> next_movie;
          if(strcmp(com_word -> data,com_movie -> title)==0){
            com_word -> com_word = com_movie;
          }
        }
        com_movie = movie -> head;
        while(com_word -> next != NULL){
          last = com_word -> next;
          com_movie = movie -> head;
          while(com_movie -> next_movie != movie -> tail){
            com_movie = com_movie -> next_movie;
            last = com_word -> next;
            if(strcmp(com_word -> data,com_movie -> title)==0){
              com_word -> com_word = com_movie;
            }
            else if(strcmp(last -> data,com_movie -> title)==0){
              last -> com_word = com_movie;
            }
          }
          com_word = com_word -> next;
        }
      }
      }
    void compare_director_word(ALL_M *movie,ALL_D *director){
      LINK_M *com_movie;
      LINK_D *com_director;
      LINK_W *com_word,*last;
      com_movie = movie -> head;
      com_director = director -> head;
      while(com_director -> next_director != director -> tail){
        com_director = com_director -> next_director;
        com_word = com_director -> title;
        while(com_movie -> next_movie != movie -> tail){
          com_movie = com_movie -> next_movie;
          if(strcmp(com_word -> data,com_movie -> title)==0){
            com_word -> com_word = com_movie;
          }
        }
        com_movie = movie -> head;
        while(com_word -> next != NULL){
          last = com_word -> next;
          com_movie = movie -> head;
          while(com_movie -> next_movie != movie -> tail){
            com_movie = com_movie -> next_movie;
						last = com_word -> next;
            if(strcmp(com_word -> data,com_movie -> title)==0){
              com_word -> com_word = com_movie;
            }
            else if(strcmp(last -> data,com_movie -> title)==0){
              last -> com_word = com_movie;
            }
          }
          com_word = com_word -> next;
        }
      }
    }

int main(){
  signal(SIGINT, cntrl_c);
  ALL_M *movie;
  ALL_D *director;
  ALL_A *actor;
  make_movie_admin(&movie);
  make_actor_admin(&actor);
  make_director_admin(&director);
  printf(">> Welcome to My Movie <<\n");//프로그램의 시작을 알림
  printf("File Loading.....\n");
  commands(director,actor,movie);
  return 0;
}
