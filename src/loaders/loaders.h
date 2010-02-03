#include "text_file.hpp"

int load_conf (char *name, char *memory, const struct data_index index[]);
profile *load_profile (char *path);
script_struct *load_object(char *path);
void spawn_object(script_struct *script, dReal x, dReal y, dReal z);
int load_track (char *path);
car_struct *load_car (char *path);
void spawn_car(car_struct *target, dReal x, dReal y, dReal z);
void remove_object(object_struct *target);
void remove_car (car_struct* target);
