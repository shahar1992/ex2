#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include "visitor_room.h"


Result init_challenge_activity(ChallengeActivity *activity,
		Challenge *challenge) {

	if ((activity == NULL) || (challenge == NULL)) {
		return NULL_PARAMETER;
	}
	activity->challenge = challenge;
	activity->start_time = 0;
	activity->visitor = NULL;
	return OK;
}

Result reset_challenge_activity(ChallengeActivity *activity) {

	if (activity == NULL) {
		return NULL_PARAMETER;
	}
	activity->challenge = NULL;
	activity->start_time = 0;
	activity->visitor = NULL;
	return OK;

}

Result init_visitor(Visitor *visitor, char *name, int id) {

	if (visitor == NULL || name == NULL) {
		return NULL_PARAMETER;
	}
	visitor->visitor_name = malloc(sizeof(char) * (strlen(name) + 1));
	if (visitor->visitor_name == NULL) {
		return MEMORY_PROBLEM;
	}
	visitor->current_challenge = NULL;
	visitor->room_name = NULL;
	visitor->visitor_id = id;

	return OK;
}

Result reset_visitor(Visitor *visitor) {

	if (visitor == NULL) {
		return NULL_PARAMETER;
	}
	free(visitor->visitor_name);
	visitor->visitor_name = NULL;
	visitor->visitor_id = 0;
	visitor->room_name = NULL;
	visitor->current_challenge = NULL;

	return OK;
}

Result init_room(ChallengeRoom *room, char *name, int num_challenges) {

	if (room == NULL || name == NULL) {
		return NULL_PARAMETER;
	}
	if (num_challenges < 1) {
		return ILLEGAL_PARAMETER;
	}
	room->num_of_challenges = num_challenges;
	room->challenges = malloc(sizeof(ChallengeActivity) * num_challenges);
	if (room->challenges == NULL) {
		free(room->name);
		return MEMORY_PROBLEM;
	}
	room->name = (malloc(sizeof(char) * (strlen(name) + 1)));
	if (room->name == NULL) {
		return MEMORY_PROBLEM;
	}
	strcpy(room->name, name);

	return OK;
}

Result reset_room(ChallengeRoom *room) {

	if (room == NULL) {
		return NULL_PARAMETER;
	}

	free(room->name);
	for(int i=0 ; i< room->num_of_challenges ; i++) {
		reset_challenge_activity(room->challenges+i);
	}
	free(room->challenges);
	room->challenges = NULL;
	room->num_of_challenges = 0;

	return OK;
}

Result num_of_free_places_for_level(ChallengeRoom *room, Level level,
		int *places) {
	if ((room == NULL) || (places == NULL)) {
		return NULL_PARAMETER;
	}
	int counter = 0;
	for (int i = 0; i < room->num_of_challenges; i++) {
		Level challenge_level = room->challenges[i].challenge->level;
		if ((challenge_level == level) || (level == All_Levels)) {
			if (room->challenges[i].visitor == NULL) {
				counter++;
			}
		}
	}
	*places = counter;
	return OK;
}

Result change_room_name(ChallengeRoom *room, char *new_name) {
	if (room == NULL || new_name == NULL) {
		return NULL_PARAMETER;
	}
	room->name = realloc(room->name, sizeof(char) * (strlen(new_name) + 1));
	if (room->name == NULL) {
		return MEMORY_PROBLEM;
	}
	strcpy(room->name, new_name);

	return OK;
}

Result room_of_visitor(Visitor *visitor, char **room_name) {

	if (visitor == NULL || room_name == NULL) {
		return NULL_PARAMETER;
	}
	if (visitor->room_name == NULL) {
		return NOT_IN_ROOM;
	}
	*room_name = malloc(sizeof(char)*((strlen(*visitor->room_name) + 1)));
	if (*room_name == NULL) {
		return MEMORY_PROBLEM;
	} else {
		strcpy(*room_name, *(visitor->room_name));
	}

	return OK;
}

Result visitor_enter_room(ChallengeRoom *room, Visitor *visitor, Level level,
		int start_time) {
	/* the challenge to be chosen is the lexicographically named smaller one that has
	 the required level. assume all names are different. */
	if (visitor == NULL || room == NULL) {
		return NULL_PARAMETER;
	}
	if (visitor->room_name != NULL) {
		return ALREADY_IN_ROOM;
	}
	for (int i = 0; i < room->num_of_challenges ; i++) {
		Level challenge_level = room->challenges[i].challenge->level;
		if ((challenge_level == level) || (level == All_Levels)) {
			if (room->challenges[i].visitor == NULL) {
				visitor->room_name = &(room->name);
				visitor->current_challenge = &(room->challenges[i]);
				inc_num_visits(visitor->current_challenge->challenge);
				visitor->current_challenge->visitor = visitor;
				visitor->current_challenge->start_time = start_time;

				return OK;
			}
		}
	}

	return NO_AVAILABLE_CHALLENGES;
}

Result visitor_quit_room(Visitor *visitor, int quit_time) {

	if(visitor==NULL) {
		return NULL_PARAMETER;
	}
	else if(visitor->room_name==NULL) {
		return NOT_IN_ROOM;
	}
	int play_time=quit_time-(visitor->current_challenge->start_time);
	Result check;
	if(play_time < visitor->current_challenge->challenge->best_time) {
		check=set_best_time_of_challenge
				(visitor->current_challenge->challenge,play_time);
		if (check != OK) {
			return check;
		}
	}
	check=init_challenge_activity
		(visitor->current_challenge,visitor->current_challenge->challenge);
	if(check!=OK) {
		return check;
	}
	check=reset_visitor(visitor);
	return OK;
}
