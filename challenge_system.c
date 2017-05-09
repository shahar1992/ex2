#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include "challenge_system.h"

/*------------------Macros and Constants--------------------------------------*/
#define NULL_CHECK(ptr) if(ptr==NULL)\
	return NULL_PARAMETER;
#define MAX_WORD_SIZE 51

/*------------------Structs and Typedefs--------------------------------------*/
typedef struct node *Visitor_list;

/*------------------Auxiliary functions declarations.-------------------------*/
static Challenge* find_challenge_ptr_by_id(ChallengeRoomSystem **sys, int id);

static Result free_all_memory(ChallengeRoomSystem *sys);

static Result free_visitor_memory
				(ChallengeRoomSystem *sys,Visitor_list visitor);

static Result free_all_rooms(ChallengeRoomSystem *sys);

static Result free_all_challenges(ChallengeRoomSystem *sys);

static Visitor_list find_visitor_in_list_by_id(ChallengeRoomSystem *sys, int id);

static Result reset_visitor_from_list(Visitor_list visitor);

static Visitor* find_visitor_pointer_by_name
				(ChallengeRoomSystem *sys,char* visitor_name);

/*------------------Functions implementation---------------------------------.*/
Result create_system(char *init_file, ChallengeRoomSystem **sys) {

	FILE *stream = fopen(init_file, "r");
	if (stream == NULL || sys == NULL) {
		return NULL_PARAMETER;
	}
	(*sys)->time = 0;
	char word[MAX_WORD_SIZE];
	fscanf(stream, "%s", word);
	strcpy((*sys)->name, word);
	fscanf(stream, "%s", word);
	(*sys)->challenges_num = atol(word);
	(*sys)->challanges = (Challenge*) malloc(
			sizeof(Challenge) * ((*sys)->challenges_num));
	if ((*sys)->challanges == NULL) {
		return MEMORY_PROBLEM;
	}
	for (int i = 0; i < ((*sys)->challenges_num) ; i++) {
		char word2[MAX_WORD_SIZE], word3[MAX_WORD_SIZE];
		fscanf(stream, "%s %s %s", word, word2, word3);
		init_challenge(&(*sys)->challanges[i], atol(word2), word, atol(word3));
	}
	fscanf(stream, "%s", word);
	(*sys)->rooms_num = atol(word);
	(*sys)->rooms = (ChallengeRoom*) malloc(
			sizeof(ChallengeRoom) * ((*sys)->rooms_num));
	if ((*sys)->rooms == NULL) {
		return MEMORY_PROBLEM;
	}
	for (int i = 0; i < ((*sys)->rooms_num); i++) {
		char word2[MAX_WORD_SIZE];
		fscanf(stream, "%s %s", word, word2);
		init_room(&(*sys)->rooms[i], word, atol(word2));
		for (int j = 0; j < ((*sys)->rooms+i)->num_of_challenges; j++) {
			fscanf(stream, "%s", word);
			(((*sys)->rooms+i)->challenges+j)->challenge =
					find_challenge_ptr_by_id(sys, atol(word));
		}
	}
	fclose(stream);
	(*sys)->visitors_list=NULL;
	(*sys)->visitors_list->next_visitor=NULL;
	(*sys)->visitors_list->visitor=NULL;
	return OK;
}

static Challenge* find_challenge_ptr_by_id(ChallengeRoomSystem **sys, int id) {
	for (int i = 0; i < (*sys)->challenges_num; i++) {
		if (((*sys)->challanges + i)->id == id) {
			return ((*sys)->challanges + i);
		}
	}
	return NULL;
}

Result destroy_system(ChallengeRoomSystem *sys, int destroy_time,
		char **most_popular_challenge_p, char **challenge_best_time) {
	if (sys == NULL || most_popular_challenge_p == NULL
			|| challenge_best_time == NULL) {
		return NULL_PARAMETER;
	}
	if (destroy_time < sys->time) {
		return ILLEGAL_TIME;
	}
	all_visitors_quit(sys, destroy_time);
	most_popular_challenge(sys, most_popular_challenge_p);
	char *temp_best_time_challenge = NULL;
	for (int i = 0; i < (sys->challenges_num); i++) {
		int best_time = 0;
		best_time_of_system_challenge(sys, (sys->challanges + i)->name,
				&(sys->challanges->best_time));
		if ((sys->challanges + i)->best_time < best_time) {
			temp_best_time_challenge = sys->challanges[i].name;
		}
	}
	if (temp_best_time_challenge != NULL) {
		strcpy(*challenge_best_time, temp_best_time_challenge);
	}
	free_all_memory(sys);
	return OK;
}

Result visitor_arrive(ChallengeRoomSystem *sys, char *room_name,
		char *visitor_name, int visitor_id, Level level, int start_time){
	if(start_time < (sys->time)){
		return ILLEGAL_TIME;
	}
	if(sys==NULL){
		return NULL_PARAMETER;
	}
	if( (room_name==NULL) || (visitor_name==NULL) ){
		return ILLEGAL_PARAMETER;
	}
	Visitor_list new_visitor=malloc(sizeof(Visitor_list));
	Result result=init_visitor(new_visitor->visitor,visitor_name,visitor_id);
	if(result!=OK){
		return result;
	}
	for(long i=0 ; i<(sys->rooms_num) ; i++){
		if(strcmp(sys->rooms[i].name,room_name)==0){
			result=visitor_enter_room(&(sys->rooms[i]),new_visitor->visitor,
					level,start_time);
			if(result==OK){
				new_visitor->next_visitor = sys->visitors_list;
				sys->visitors_list=new_visitor;
				return OK;
			}
			else{
				reset_visitor(new_visitor->visitor);
				free(new_visitor);
				return result;
			}
		}
	}
	return ILLEGAL_PARAMETER;
}

Result visitor_quit(ChallengeRoomSystem *sys, int visitor_id, int quit_time){
	if(sys==NULL) {
		return NULL_PARAMETER;
	}
	Visitor_list visitor=find_visitor_in_list_by_id(sys,visitor_id);
	Result result=visitor_quit_room(visitor->visitor,quit_time);
	if(result!=OK) {
		return result;
	}
	free_visitor_memory(sys,visitor);
	return OK;//NOT FINISHED.
}

Result all_visitors_quit(ChallengeRoomSystem *sys, int quit_time) {
	if(sys==NULL){
		return NULL_PARAMETER;
	}
	while(sys->visitors_list!=NULL){
		Result result=visitor_quit_room(sys->visitors_list->visitor,quit_time);
		if(result!=OK){
			return result;
		}
		free_visitor_memory(sys,sys->visitors_list);
	}
	return OK;//NOT FINISHED.
}

Result system_room_of_visitor
			(ChallengeRoomSystem *sys, char *visitor_name, char **room_name){
	if(sys==NULL){
		return NULL_PARAMETER;
	}
	if(visitor_name==NULL || room_name==NULL){
		return ILLEGAL_PARAMETER;
	}
	return room_of_visitor
			(find_visitor_pointer_by_name(sys,visitor_name),room_name);
}

Result change_challenge_name
			(ChallengeRoomSystem *sys, int challenge_id, char *new_name){
	if(sys==NULL || new_name==NULL){
		return NULL_PARAMETER;
	}
	for(int i=0 ; i < sys->challenges_num ; i++){
		if((sys->challanges+i)->id==challenge_id){
			return change_name(sys->challanges+i,new_name);
		}
	}
	return ILLEGAL_PARAMETER;
}

Result change_system_room_name(ChallengeRoomSystem *sys, char *current_name,
		char *new_name) {
	if (sys == NULL || current_name == NULL || new_name == NULL) {
		return NULL_PARAMETER;
	}
	for (int i = 0; i < (sys->rooms_num); i++) {
		if (strcmp(sys->rooms[i].name, current_name) == 0) {
			return change_room_name(&sys->rooms[i],new_name);
		}
	}
	return ILLEGAL_PARAMETER;
}

Result best_time_of_system_challenge(ChallengeRoomSystem *sys,
		char *challenge_name, int *time) {
	if (sys == NULL || challenge_name == NULL || time == NULL) {
		return NULL_PARAMETER;
	}
	for (int i = 0; i < (sys->challenges_num); i++) {
		if (strcmp((sys->challanges+i)->name, challenge_name) == 0) {
			return best_time_of_challenge(&sys->challanges[i], time);
		}
	}
	return ILLEGAL_PARAMETER;
}

Result most_popular_challenge(ChallengeRoomSystem *sys, char **challenge_name){
	if (sys == NULL || challenge_name == NULL) {
		return NULL_PARAMETER;
	}
	Challenge *most_popular_challenge = NULL;
	int max_num_visits=0;
	for(int i=0 ; i<(sys->challenges_num) ; i++) {
		if(sys->challanges[i].num_visits>max_num_visits) {
			max_num_visits=sys->challanges[i].num_visits;
			most_popular_challenge=&sys->challanges[i];
		}
	}
	if(most_popular_challenge==NULL) {
		*challenge_name=NULL;
	}
	else{
		*challenge_name=(char*)malloc
				((sizeof(char))*(strlen(most_popular_challenge->name)));
		if(*challenge_name==NULL) {
			return MEMORY_PROBLEM;
		}
		strcpy(*challenge_name,most_popular_challenge->name);
	}
	return OK;
}

static Result free_all_memory(ChallengeRoomSystem *sys) {
	free_all_rooms(sys);
	free_all_challenges(sys);
	return OK;
}

static Result free_visitor_memory
				(ChallengeRoomSystem *sys,Visitor_list visitor){
	Visitor_list current_visitor=sys->visitors_list;
	while(current_visitor!=NULL){
		if (visitor == sys->visitors_list) {
			sys->visitors_list=sys->visitors_list->next_visitor;
			reset_visitor_from_list(visitor);
			return OK;
		}
		if(visitor == current_visitor->next_visitor){
			current_visitor->next_visitor=visitor->next_visitor;
			reset_visitor_from_list(visitor);
			return OK;
		}
	}
	return NOT_IN_ROOM;
}

static Result reset_visitor_from_list(Visitor_list visitor){
	reset_visitor(visitor->visitor);
	visitor->visitor=NULL;
	visitor->next_visitor=NULL;
	free(visitor);
	return OK;
}


static Result free_all_rooms(ChallengeRoomSystem *sys) {
	for(int i=0 ; i<(sys->rooms_num) ; i++){
		Result result=reset_room(sys->rooms+i);
		if(result!=OK) {
			return result;
		}
	}
	free(sys->rooms);
	return OK;
}

static Result free_all_challenges(ChallengeRoomSystem *sys) {
	for(int i=0 ; i<(sys->challenges_num) ; i++) {
		Result result=reset_challenge(sys->challanges+i);
		if(result!=OK){
			return result;
		}
	}
	free(sys->challanges);
	return OK;
}

static Visitor_list find_visitor_in_list_by_id(ChallengeRoomSystem *sys, int id){
	Visitor_list current_visitor=sys->visitors_list;
	while(current_visitor!=NULL){
		if(current_visitor->visitor->visitor_id==id){
			return current_visitor;
		}
		current_visitor=current_visitor->next_visitor;
	}
	return NULL;
}

static Visitor* find_visitor_pointer_by_name
				(ChallengeRoomSystem *sys,char* visitor_name){
	Visitor_list current_visitor=sys->visitors_list;
	while(current_visitor!=NULL){
		if(strcmp(visitor_name,current_visitor->visitor->visitor_name)==0){
			return current_visitor->visitor;
		}
		current_visitor=current_visitor->next_visitor;
	}
	return NULL;
}
