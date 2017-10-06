implementation module ex09

import Xenomai
import StdEnv
import System._Pointer
import System._Posix

import StdDebug

:: Direction = LEFT | RIGHT

interruptHandler :: !Pointer !Int !Direction !RTime ![LED] !*World -> *World
interruptHandler resultP fd direction period leds world
= interruptHandler` fd resultP direction period leds world
where
	interruptHandler` :: !Int !Pointer !Direction !RTime ![LED] !*World -> *World
	interruptHandler` fd resultP direction period leds world
	#! world = waitForNextRight direction resultP fd world
	#! world = spinTimer period resultP world
	#! (_, world) = printCharacter leds e world
	#! (_, world) = printCharacter leds r world
	= interruptHandler` fd resultP RIGHT period leds world
	where
		x = [[True, False, False, False, False, False, False, True],
		      [False, True, False, False, False, False, True, False],
		      [False, False, True, False, False, True, False, False],
		      [False, False, False, True, True, False, False, False],
		      [False, False, True, False, False, True, False, False],
		      [False, True, False, False, False, False, True, False],
		      [True, False, False, False, False, False, False, True]]
		e = [[True, True, True, True, True, True, True, True],
		      [True, False, False, True, False, False, False, True],
		      [True, False, False, True, False, False, False, True],
		      [True, False, False, True, False, False, False, True],
		      [True, False, False, False, False, False, False, True],
		      [True, False, False, False, False, False, False, True],
		      [True, False, False, False, False, False, False, True]]
		r = [[True, True, True, True, True, True, True, True],
		      [False, False, False, False, True, False, False, True],
		      [False, False, False, True, True, False, False, True],
		      [False, False, True, False, True, False, False, True],
		      [False, True, False, False, True, False, False, True],
		      [True, False, False, False, True, True, True, True]]

waitForNextRight :: !Direction !Pointer !Int !*World -> *World
waitForNextRight RIGHT resultP fd world
#! (_, world) = read fd resultP 4 world
#! (_, world) = read fd resultP 4 world
= world
waitForNextRight LEFT resultP fd world
#! (_, world) = read fd resultP 4 world
= world

getPendulumDirection :: !Pointer !Int !*World -> (!Direction, !*World)
getPendulumDirection resultP iFd world
#! t1p = malloc 8
#! t2p = malloc 8
#! (t1, world) = getInterruptTime t1p iFd world
#! (t2, world) = getInterruptTime t2p iFd world
#! (t3, world) = getInterruptTime resultP iFd world
#! isLEFT = t2 - t1 < t3 - t2
#! a = free t1p
#! a = free t2p
| isLEFT = (LEFT, world)
= (RIGHT, world)

// Direction at end = RIGHT
getPeriod :: !Direction !Pointer !Int !*World -> (!RTime, !*World)
getPeriod direction resP fd world
#! t2p = malloc 8
#! world = waitForNextRight direction resP fd world
#! (t1, world) = getRTime resP world
#! world = waitForNextRight RIGHT resP fd world
#! (t2, world) = getRTime t2p world
= (t2 - t1, world)

Start world
#! resultP = malloc 8
#! (iFd, world) = setupInterruptHandlerTask world
#! (_, world) = setupLEDS world
#! (direction, world) = getPendulumDirection resultP iFd world
#! (period, world) = getPeriod direction resultP iFd world
#! (leds, world) = getFDs [0 .. 7] world
= interruptHandler resultP iFd RIGHT period leds world
