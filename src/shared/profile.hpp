#define UNUSED_KEY SDLK_QUESTION //key that's not used during race ("safe" default)

//profile: stores the user's settings (including key list)
typedef struct profile_struct {
	//the car the user is controlling
	car_struct *car;
	struct profile_struct *next;
	struct profile_struct *prev;

	//settings (loaded from conf)
	dReal steer_speed;
	dReal steer_max;
	dReal throttle_speed;

	//keys (loaded from keys.lst)
	SDLKey up;
	SDLKey down;
	SDLKey right;
	SDLKey left;
	SDLKey soft_break;
	SDLKey drift_break;

	SDLKey cam_x_pos;
	SDLKey cam_x_neg;
	SDLKey cam_y_pos;
	SDLKey cam_y_neg;
	SDLKey cam_z_pos;
	SDLKey cam_z_neg;

	camera_settings cam[4];
	int camera;
	SDLKey cam1;
	SDLKey cam2;
	SDLKey cam3;
	SDLKey cam4;
} profile;

extern profile *profile_head;

