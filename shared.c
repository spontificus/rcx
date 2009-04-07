//create and destroy objects, components and swap buffers (not used now)
//(should only be used by other abstraction functions (look in loader.c)
//and not directly, except for free_all to do quick full free


//allocate new script storage, and add it to list
//(not used yet, only for storing 3d list pointers...)
script *allocate_script(void)
{
	printf("> allocating script\n");
	script *tmp = malloc(sizeof(script));
	tmp->next = script_head;
	script_head = tmp;

	if (!tmp->next)
		printf("(first registered object)\n");

	return script_head;
}

//allocate a new object, add it to the list and returns its pointer
object *allocate_object (void)
{
	printf("> allocating object\n");
	object *object_next = object_head;

	object_head = malloc(sizeof(object));

	object_head->prev = NULL;
	object_head->next = object_next;

	if (object_next)
		object_next->prev = object_head;
	else
		printf("(first registered object)\n");

	//default values
	object_head->cmp_count = 0; //counts how many components
	object_head->collide_space = false; //objects components doesn't collide with each other

	//set space and jointgroup to NULL?
	object_head->space_id = NULL;
	object_head->joint_group = NULL;
	return object_head;
}

//allocates a new component, returns its pointer (and uppdate its object's cmp_count),
//ads it to the component list
component *allocate_component (script *script, object *obj)
{
	printf("> allocating component\n");
	component *tmp_component = malloc(sizeof(component));

	//parent object
	tmp_component->object_parent = obj;
	if (obj)
		obj->cmp_count += 1;
	else
		printf("(not registering to any object)\n");

	//assign its physics data
	tmp_component->script = script;
	if (!script)
		printf("(not assigning any event handling script)\n");

	//ad it to the list
	tmp_component->next = component_head;
	component_head = tmp_component;
	component_head->prev = NULL;

	if (component_head->next)
		component_head->next->prev = component_head;
	else
		printf("(first registered component)\n");

	//now lets set some default values...
	component_head->graphics_list = NULL; //default, the component isn't rendered
	component_head->mu = 0.0; //no friction
	component_head->erp = 0.8; //good default error reduction
	component_head->cfm = 0.0; //no bouncy collision
	component_head->slip1 = 0.0; //no FDS slip in direction 1
	component_head->slip2 = 0.0; //no FDS slip in direction 2

	component_head->collide = true; //on collision, create opposing forces
	component_head->collision_event = false; //no collision yet

	return tmp_component;
}

//allocates car, ad to list...
//(no object allocations - since we want to allocate car without spawning)
car *allocate_car(void)
{
	printf("> allocating car\n");
	car *car_next = car_head;
	car_head = malloc(sizeof(car));

	//ad to list
	car_head->prev=NULL;
	car_head->next=car_next;
	if (car_next)
		car_next->prev=car_head;
	else
		printf("(first registered)\n");


	//default values
	car_head->body_graphics = NULL;
	car_head->wheel_graphics = NULL;
	car_head->drift_breaks = true; //if the user does nothing, lock wheels
	car_head->breaks = false;
	car_head->throttle = 0;
	car_head->steering = 0;

	//TODO: default mass, friction, etc...?

	return car_head;
}

//allocates new link in 3d rendering list
graphics_list *allocate_graphics_list (void)
{
	printf("> allocating 3d list storage\n");

	graphics_list * tmp_3d = malloc (sizeof(graphics_list));
	//add to list
	tmp_3d->next = graphics_list_head;
	graphics_list_head = tmp_3d;

	if (!tmp_3d->next)
		printf("(first one)\n");

	//default values
	graphics_list_head->render_list = glGenLists(1);
	graphics_list_head->file = NULL; //filename

	return graphics_list_head;
}

//destroys an object
void free_object(object *target)
{
	//lets just hope the given pointer is ok...
	printf("> frees object\n");

	//1: remove it from the list
	if (target->prev == NULL) //first link
	{
		printf("(object is head)\n");
		object_head = target->next;
	}
	else
		target->prev->next = target->next;

	if (target->next) //not last link
		target->next->prev = target->prev;
	else
		printf("(object is last)\n");



	//check if has jointgroup
	if (target->joint_group)
	{
		printf("\nFIXME: free_object - detected jointgroup, destroying\n\n");
		dJointGroupDestroy (target->joint_group);
	}

	//2: remove it from memory

	free(target);
}

//destroys a component, and removes it from the list (and remove object if all components are gone)
void free_component(component *target)
{
	//lets just hope the given pointer is ok...
	printf("> frees component\n");

	//1: remove it from the list
	if (!target->prev) //head in list, change head pointer
	{
		printf("(component is head)\n");
		component_head = target->next;
	}
	else //not head in list, got a previous link to update
		target->prev->next = target->next;

	if (target->next) //not last link in list
		target->next->prev = target->prev;
	else
		printf("(component is last)\n");

	//2: remove it from memory

	free(target);
}



//run before starting full erase of object/component lists (at race end)
void free_car (car *target)
{
	printf("> freeing car\n");

	//remove from list
	if (!target->prev) //head
		car_head = target->next;
	else //not head
		target->prev->next = target->next;

	if (target->next) //not last
		target->next->prev = target->prev;

	free_object (target->object);
	//remove car

	free (target);
}

//free all things allocated (to simplify memory leak preventing)
//simple dirty way of removing everything
void free_all (void)
{
	printf("--- Freeing all data\n");
	printf(">>> TODO: optimize???\n");
	//first thing to destroy: cars
	printf(">>> TODO: change from free_car to destroy_car?\n\n");
	while (car_head)
		free_car(car_head);

	//free_component - frees objects to (when they become empty)
	while (component_head)
		free_component(component_head);

	//only place where scripts and 3d lists are removed
	script *script_tmp = script_head;
	while (script_head)
	{
		script_tmp = script_head->next;
		free (script_head);
		script_head = script_tmp;
	}

	//destroy loaded 3d files
	graphics_list *graphics_list_tmp;
	while (graphics_list_head)
	{
		graphics_list_tmp = graphics_list_head; //copy from from list
		graphics_list_head = graphics_list_head->next; //remove from list

		glDeleteLists (graphics_list_tmp->render_list, 1);
		free (graphics_list_tmp);
	}

	//no need to destroy track, since it's not allocated by program
	printf("---\n\n");
}
