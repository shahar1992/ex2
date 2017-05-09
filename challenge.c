/*-------------------Start of Include files------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "challenge.h"
/*-------------------End of Include files------------------------------------*/

/*-------------------Start of MACRO decelerations------------------------*/

/*-------------------End of MACRO decelerations-----------------------*/

/*-------------------Start of function implementation------------------------*/

/* This function initialize a given structure,
 * returns NULL_PARAMETER as a result if 1 or more arguments given were NULL
 * returns MEMORY_PROBLEM as a result if there were memory problems
 * return OK if the given struct was successfully initialized */
Result init_challenge(Challenge *challenge, int id, char *name, Level level){
	if( (challenge==NULL)||(name==NULL)){
		return NULL_PARAMETER;
	}
	challenge->name= malloc(sizeof(char)*(strlen(name)+1));
	if(challenge->name==NULL) return MEMORY_PROBLEM ;
	strcpy(challenge->name,name);
	challenge->id=id;
	challenge->level=level;
	challenge->best_time=0;
	challenge->num_visits=0;
	return OK;
}

/* This function resets a given structure,
 * returns NULL_PARAMETER as a result if challenge points to NULL
 * return OK if the given struct was successfully reset */
Result reset_challenge(Challenge *challenge){
	if(challenge==NULL){
		return NULL_PARAMETER;
	}
	challenge->best_time=0;
	challenge->id=0;
	challenge->level=Easy;
	free(challenge->name);
	challenge->name=NULL;
	challenge->num_visits=0;
	return OK;
}

Result change_name(Challenge *challenge, char *name){
	if( (challenge==NULL)||(name==NULL)){
		return NULL_PARAMETER;
	}
	challenge->name= realloc(challenge->name,sizeof(char)*(strlen(name)+1));
	if(challenge->name==NULL){
		return MEMORY_PROBLEM ;
	}
	strcpy(challenge->name,name);
	return OK;

}

Result set_best_time_of_challenge(Challenge *challenge, int time){
	if(challenge==NULL){
		return NULL_PARAMETER;
	}
	if( (time<0) || (time>challenge->best_time) ){//check- time> best time?
		return ILLEGAL_PARAMETER ;
	}
	challenge->best_time=time;
	return OK;

}

Result best_time_of_challenge(Challenge *challenge, int *time){
	if(challenge==NULL) return NULL_PARAMETER;
	*time=challenge->best_time;
	return OK;
}

Result inc_num_visits(Challenge *challenge){
	if(challenge==NULL) return NULL_PARAMETER;
	(challenge->num_visits)++;
	return OK;
}

Result num_visits(Challenge *challenge, int *visits){
	if(challenge==NULL) return NULL_PARAMETER;
	*visits=challenge->num_visits;
	return OK;
}
