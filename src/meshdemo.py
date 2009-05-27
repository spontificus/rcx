# -*- coding: utf-8 -*-
#/usr/bin/env python

import pygame
from pygame.locals import *

import random
from math import *
import math
import pdb

class point:
	def __init__(self,x,y):
		self.x = x
		self.y = y

class side:
	def __init__(self,p1,p2):
		self.p1 = p1
		self.p2 = p2

class mesh:
	def __init__(self):
		self.sides = "";

	def split(self):
		tx = 0
		ty = 0
		for side in sides:
			tx += side.p1.x
			ty += side.p1.y
			
		

def main():
	# init pygame
	pygame.init()    

	# setup the display
	screen = pygame.display.set_mode((800, 600),1)

	# and window title
	pygame.display.set_caption('Mesh Filling Test')

	bgnd_group = pygame.sprite.RenderPlain()

	while 1:
		# clear screen
		screen.fill((0,0,0))

		for event in pygame.event.get():
			if event.type == KEYDOWN:
				if event.key == K_ESCAPE:
					return

		# update the screen                                
		bgnd_group.draw(screen)
			
		pygame.display.flip()
						

#this calls the 'main' function when this script is executed
if __name__ == '__main__': main()
