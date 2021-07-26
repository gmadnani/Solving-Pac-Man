#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <assert.h>

#include "ai.h"
#include "utils.h"
#include "priority_queue.h"


struct heap h;

float get_reward( node_t* n );

/**
 * Function called by pacman.c
*/
void initialize_ai(){
	heap_init(&h);
}

/**
 * function to copy a src into a dst state
*/
void copy_state(state_t* dst, state_t* src){
	//Location of Ghosts and Pacman
	memcpy( dst->Loc, src->Loc, 5*2*sizeof(int) );

    //Direction of Ghosts and Pacman
	memcpy( dst->Dir, src->Dir, 5*2*sizeof(int) );

    //Default location in case Pacman/Ghosts die
	memcpy( dst->StartingPoints, src->StartingPoints, 5*2*sizeof(int) );

    //Check for invincibility
    dst->Invincible = src->Invincible;

    //Number of pellets left in level
    dst->Food = src->Food;

    //Main level array
	memcpy( dst->Level, src->Level, 29*28*sizeof(int) );

    //What level number are we on?
    dst->LevelNumber = src->LevelNumber;

    //Keep track of how many points to give for eating ghosts
    dst->GhostsInARow = src->GhostsInARow;

    //How long left for invincibility
    dst->tleft = src->tleft;

    //Initial points
    dst->Points = src->Points;

    //Remiaining Lives
    dst->Lives = src->Lives;

}

node_t* create_init_node( state_t* init_state ){
	node_t * new_n = (node_t *) malloc(sizeof(node_t));
	new_n->parent = NULL;
	new_n->priority = 0;
	new_n->depth = 0;
	new_n->num_childs = 0;
	copy_state(&(new_n->state), init_state);
	new_n->acc_reward =  get_reward( new_n );
	return new_n;

}


float heuristic( node_t* n ){

	float h = 0;
	float i = 0, l = 0, g = 0;
	assert(n->parent != NULL);

	// eaten a fruit and becomes invincible
	if (n->state.Invincible == 1 && n->parent->state.Invincible == 0) {
		i = 10;
	}

	// a life has been lost
	if (n->state.Lives == n->parent->state.Lives - 1) {
		l = 10;
	}

	// game is over
	if (n->state.Lives == 0) {
		g = 100;
	}

	h = i - l - g;

	return h;
}

float get_reward ( node_t* n ){
	float reward = 0;

	if (n->parent == NULL) {
		return 0;
	}

	float h_n = heuristic(n);
	float score_n = n->state.Points;
	float score_p = n->parent->state.Points;
	float discount = pow(0.99,n->depth);

	reward = h_n + score_n - score_p;

	return discount * reward;
}

/**
 * Apply an action to node n and return a new node resulting from executing the action
*/
bool applyAction(node_t* n, node_t** new_node, move_t action ){

	bool changed_dir = false;
	changed_dir = execute_move_t( &((*new_node)->state), action );

	if (changed_dir) {
		(*new_node)->priority = (n->priority) - 1;
		(*new_node)->acc_reward = get_reward(*new_node);
		(*new_node)->depth = n->depth + 1;
		(*new_node)->move = action;
		(*new_node)->parent = n;
		// update child and reward
		node_t *curr = n;
		while (curr != NULL) {
			(curr->num_childs)++;
			(*new_node)->acc_reward += curr->acc_reward;
			curr = curr->parent;
		}
	}

	return changed_dir;

}


/**
 * Find best action by building all possible paths up to budget
 * and back propagate using either max or avg
 */

move_t get_next_move( state_t init_state, int budget, propagation_t propagation, char* stats ){

	float best_action_score[SIZE];
	for(unsigned i = 0; i < SIZE; i++) {
	    best_action_score[i] = -1;
		}

	unsigned generated_nodes = 0;
	unsigned expanded_nodes = 0;
	unsigned max_depth = 0;

	// Add the initial node
	node_t* n = create_init_node( &init_state );
	// Use the max heap API provided in priority_queue.h
	heap_push(&h, n);

	int explored_count = 0;
	int size = 1;
	node_t **explored = (node_t **)malloc(sizeof(node_t *));
	assert(explored);

	while (h.count > 0) {
		node_t *curr = heap_delete(&h);
		if (curr->depth > max_depth) {
			max_depth = curr->depth;
		}
		if (explored_count == size) {
			size *= 2;
			explored = (node_t **) realloc(explored, sizeof(node_t *) * size);
			assert(explored != NULL);
		}
		explored[explored_count++] = curr;
		if (expanded_nodes < budget) {
			expanded_nodes++;
			for (int i = 0; i < SIZE; i++) {
				node_t* new = create_init_node( &(curr->state) );
				if (applyAction(curr, &new, i)) {
					generated_nodes++;

					// propagation
					if (propagation == avg) {
						// Average
						if (curr->depth == 0) {
							best_action_score[new->move] = new->state.Points;
						} else {
							node_t *new_node = curr;
							while (new_node->depth != 1) {
								new_node = new_node->parent;
							}
							int childs = new_node->num_childs;
							best_action_score[new_node->move] = (best_action_score[new_node->move]\
								 * childs + new->state.Points) / (childs + 1);
						}
					} else {
						// Maximize
						node_t *new_node = new;
						while (new_node->depth != 1) {
							new_node = new_node->parent;
						}
						if (best_action_score[new_node->move] < new->state.Points) {
							best_action_score[new_node->move] = new->state.Points;
						}
					}

					// life check
					if (new->state.Lives < curr->state.Lives){
						free(new);
					} else {
						heap_push(&h, new);
					}
				} else {
					free(new);
				}
			}
		}
	}

	// free explored
	for(int i = 0; i< explored_count; i++){
		free(explored[i]);
	}
	free(explored);

	// choose best action
	move_t best_action = rand() % SIZE;
	float best_score = -1;
	for(int i = 0; i < SIZE; i++ ){
		if(best_action_score[i] > best_score){
			best_score = best_action_score[i];
			best_action = i;
		}
	}
	// tie
	int tie[SIZE];
	int index = 0;
	for(int i=0; i<SIZE; i++ ){
		if (best_score == best_action_score[i]) {
			tie[index++] = i;
		}
	}
	best_action = tie[rand() % index];

	sprintf(stats, "Max Depth: %d Expanded nodes: %d  Generated nodes: %d\n",max_depth,expanded_nodes,generated_nodes);

	if(best_action == left)
		sprintf(stats, "%sSelected action: Left\n",stats);
	if(best_action == right)
		sprintf(stats, "%sSelected action: Right\n",stats);
	if(best_action == up)
		sprintf(stats, "%sSelected action: Up\n",stats);
	if(best_action == down)
		sprintf(stats, "%sSelected action: Down\n",stats);

	sprintf(stats, "%sScore Left %f Right %f Up %f Down %f",stats,best_action_score[left],best_action_score[right],best_action_score[up],best_action_score[down]);

	return best_action;
}
