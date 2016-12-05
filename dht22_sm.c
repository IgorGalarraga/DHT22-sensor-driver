#include "dht22_sm.h"

static enum dht22_state
(*state_functions[COUNT_STATES])(struct dht22_sm *sm) = {
	get_next_state_idle,
	get_next_state_responding,
	get_next_state_finished,
	get_next_state_error
};

static void (*handler_functions[COUNT_STATES])(struct dht22_sm *sm) = {
	handle_idle,
	noop,
	handle_finished,
	noop
};

void noop(struct dht22_sm *sm) { }

struct dht22_sm *create_sm(void)
{
	struct dht22_sm *sm;
	struct mutex lock;

	sm = kmalloc(sizeof(struct dht22_sm), GFP_KERNEL);
	if (!sm) {
		pr_err("Could not create state machine. Exiting...\n");
		return ERR_PTR(-ENOMEM);
	}

	mutex_init(&lock);
	sm->lock = lock;

	sm->reset = reset_dht22_sm;

	return sm;
}

void destroy_sm(struct dht22_sm *sm)
{
	mutex_destroy(&sm->lock);
	kfree(sm);
}

enum dht22_state get_next_state_idle(struct dht22_sm *sm)
{
	if (sm->error)
		return ERROR;

	if (sm->triggered)
		return RESPONDING;

	return IDLE;
}

enum dht22_state get_next_state_responding(struct dht22_sm *sm)
{
	if (sm->error)
		return ERROR;

	if (sm->finished)
		return FINISHED;

	return RESPONDING;
}

enum dht22_state get_next_state_finished(struct dht22_sm *sm)
{
	if (sm->error)
		return ERROR;

	return IDLE;
}

enum dht22_state get_next_state_error(struct dht22_sm *sm)
{
	return IDLE;
}

void handle_idle(struct dht22_sm *sm)
{
	if (sm->dirty)
		schedule_work(&sm->cleanup_work);
}

void handle_finished(struct dht22_sm *sm)
{
	schedule_work(&sm->work);
}

void change_dht22_sm_state(struct dht22_sm *sm)
{
	sm->state = (*state_functions[sm->state])(sm);
}

void handle_dht22_state(struct dht22_sm *sm)
{
	(*handler_functions[sm->state])(sm);
}

void reset_dht22_sm(struct dht22_sm *sm)
{
	sm->state = IDLE;
	sm->finished = false;
	sm->triggered = false;
	sm->error = false;
	sm->dirty = false;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Filip Kolev");
MODULE_DESCRIPTION("A test module for the DHT22 sensor.");
MODULE_VERSION("0.1");