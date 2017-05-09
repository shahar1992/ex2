#include "challenge_system.h"

	char* name;
	long challenges_num;
	Challenge *challanges;
	long rooms_num;
	ChallengeRoom *rooms;
	int time;
	struct node {
		Visitor *visitor;
		struct node* next_visitor;
	}*visitors_list;
