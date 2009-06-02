# -*- coding: utf-8 -*-
#/usr/bin/env python

import pygame
from pygame.locals import *

import random
from math import *
import math
import pdb

AREALIMIT = 250

class point:
	def __init__(self,x,y):
		self.x = x
		self.y = y

class side:
	def __init__(self,p1,p2):
		self.p1 = p1
		self.p2 = p2
		
	def addToLeft(self,ls):
		self.l = ls
		ls.r = self
		
	def addToRight(self,rs):
		self.r = rs
		rs.l = self

class mesh:
	def __init__(self):
		self.sides = [];

	def split(self,screen):
		tx = 0
		ty = 0
		i = 0
		j = 1
		l = len(self.sides)
		area = 0
		
		for cside in self.sides:
			pygame.draw.line(screen, (255,255,255), (cside.p1.x, cside.p1.y), (cside.p2.x, cside.p2.y))
			
			tx += cside.p1.x
			ty += cside.p1.y
			
			area += cside.p1.x * cside.p2.y
			area -= cside.p1.y * cside.p2.x
			
		cx = tx / l
		cy = ty / l
		
		cp = point(cx,cy)
		
		area /= 2
		
		if (abs(area) > AREALIMIT):
			# recurse through child nodes
			for cside in self.sides:
				pmp = point((cside.l.p1.x + cside.l.p2.x) / 2, (cside.l.p1.y + cside.l.p2.y) / 2)
				nmp = point((cside.p1.x + cside.p2.x) / 2, (cside.p1.y + cside.p2.y) / 2)
						
				s1 = side(cside.p1, nmp)
				s2 = side(nmp, cp)
				s3 = side(cp, pmp)
				s4 = side(pmp, cside.p1)
				
				s1.addToRight(s2)
				s2.addToRight(s3)
				s3.addToRight(s4)
				s4.addToRight(s1)
				
				tsides = []
				tsides.append(s1)
				tsides.append(s2)
				tsides.append(s3)
				tsides.append(s4)
				
				tm = mesh()
				tm.sides = tsides
				
				tm.split(screen)
		elif len(self.sides) == 4:
			pygame.draw.line(screen, (255,255,255), (self.sides[0].p1.x, self.sides[0].p1.y), (self.sides[2].p1.x, self.sides[2].p1.y))
			pygame.draw.line(screen, (255,255,255), (self.sides[1].p1.x, self.sides[1].p1.y), (self.sides[3].p1.x, self.sides[3].p1.y))

def drawMesh(screen,m):
	m.split(screen)


def main():
	# init pygame
	pygame.init()    

	# setup the display
	screen = pygame.display.set_mode((800, 600),1)

	# and window title
	pygame.display.set_caption('Mesh Filling Test')

	bgnd_group = pygame.sprite.RenderPlain()
	
	p1 = point(10,10)
	p2 = point(700,300)
	p3 = point(50,500)
	
	s1 = side(p1,p2)
	s2 = side(p2,p3)
	s3 = side(p3,p1)
	
	s1.addToRight(s2)
	s2.addToRight(s3)
	s3.addToRight(s1)
	
	sides = []
	sides.append(s1)
	sides.append(s2)
	sides.append(s3)
	
	m = mesh()
	m.sides = sides

	ts = s1
	
	while 1:
		# clear screen
		screen.fill((0,0,0))

		for event in pygame.event.get():
			if event.type == KEYDOWN:
				if event.key == K_ESCAPE:
					return
					
			if event.type == pygame.MOUSEBUTTONDOWN:
				(b1,b2,b3) = pygame.mouse.get_pressed()
				if b1 == True:
					ts = ts.l
					
				if b2 == True:
					sr = ts.r
					tp = point(0,0)
					ns = side(tp,ts.p2)
					ts.p2 = tp
					ns.addToLeft(ts)
					ns.addToRight(sr)
					sides.insert(sides.index(ts),ns)
					ts = ns
					
					
					
				if b3 == True:
					ts = ts.r

		# update the screen                                
		bgnd_group.draw(screen)
		
		(mx,my) = pygame.mouse.get_pos()
		ts.p1.x = mx
		ts.p1.y = my
		
		drawMesh(screen,m)
			
		pygame.display.flip()
						

#this calls the 'main' function when this script is executed
if __name__ == '__main__': main()
