/*
 * Copyright (C) 2003-2004 Josh A. Beam
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SCENE_H
#define SCENE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define M_INFINITY 50.0f
#define LIGHTMAP_SIZE 16

#ifndef glActiveTextureARB
extern void glActiveTextureARB(GLenum);
#endif
#ifndef glMultiTexCoord2fARB
extern void glMultiTexCoord2fARB(GLenum, float, float);
#endif


struct surface {
	float vertices[4][3];
	float matrix[9];

	float s_dist, t_dist;
};

struct surfaces {
        struct surface *s;
        struct surfaces *nxt;
} surfaces;

struct cube {
	struct surface *surfaces[6];
	float position[3];
} cube;

static float light_pos[3] = { 20.0f, 0.0f, 8.0f };
static float light_color[3] = { 1.0f, 1.0f, 1.0f };
static float sphere_pos[3] = { -10.0f, -5.0f, -10.0f };
static float cam_rot[3] = { 22.0f, 0.0f, 0.0f };
static struct cube *cubes[4];
static int lighting = 1;


static float dot_product(float v1[3], float v2[3]);
static void normalize(float v[3]);
static void cross_product(const float *v1, const float *v2, float *out);
static void multiply_vector_by_matrix(const float m[9], float v[3]);
static struct surface * new_surface(float vertices[4][3]);

static void render_surface_shadow_volume(struct surface *surf, GLfloat matrix[16], float *light_pos);
static struct cube * new_cube(float size);
static unsigned int generate_lightmap(struct surface *surf, float *position);
void scene_toggle_lighting();
static void draw_shadow();
static void render_surface(struct surface *surf, float *position);
static void render_cube(struct cube *c);
static void render_cube_shadow(struct cube *c, GLfloat matrix[16]);
static void render_gen_shadow(struct surfaces *sc, GLfloat matrix[16]);
void scene_render();
static unsigned int get_ticks();
void scene_cycle();

#endif