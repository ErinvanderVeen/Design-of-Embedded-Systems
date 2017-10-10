implementation module ex09

import Xenomai
import StdEnv
import System._Pointer
import System._Posix

:: Direction = LEFT | RIGHT
:: Period :== (RTime, RTime)

interruptHandler :: !Pointer !Int !Direction !Period ![LED] !*World -> *World
interruptHandler resultP fd direction period leds world
= interruptHandler` fd resultP direction period leds world
where
	interruptHandler` :: !Int !Pointer !Direction !Period ![LED] !*World -> *World
	interruptHandler` fd resultP direction (lperiod, rperiod) leds world
	#! world = waitForNextRight direction resultP fd world
	#! world = spinTimer lperiod world
	#! (_, world) = printCharacter leds x world
	#! world = spinTimer rperiod world
	#! (_, world) = printCharacter leds x world
	= interruptHandler` fd resultP RIGHT (lperiod, rperiod) leds world
	where
		x = [[True, False, False, False, False, False, False, True],
		     [False, True, False, False, False, False, True, False],
		     [False, False, True, False, False, True, False, False],
		     [False, False, False, True, True, False, False, False],
		     [False, False, True, False, False, True, False, False],
		     [False, True, False, False, False, False, True, False],
		     [True, False, False, False, False, False, False, True],
		     [False, False, False, False, False, False, False, False]]
		e = [[True, True, True, True, True, True, True, True],
		     [True, False, False, True, False, False, False, True],
		     [True, False, False, True, False, False, False, True],
		     [True, False, False, True, False, False, False, True],
		     [True, False, False, False, False, False, False, True],
		     [True, False, False, False, False, False, False, True],
		     [True, False, False, False, False, False, False, True]]
		r = [[True, True, True, True, True, True, True, True],
		     [True, False, False, True, False, False, False, False],
		     [True, False, False, True, True, False, False, False],
		     [True, False, False, True, False, True, False, False],
		     [True, False, False, True, False, False, True, False],
		     [True, True, True, True, False, False, False, True]]
		i = [[True, True, True, True, True, True, True, True]]
		n = [[True, True, True, True, True, True, True, True],
		     [False, True, False, False, False, False, False, False],
		     [False, False, True, True, True, False, False, False],
		     [False, False, False, False, True, True, False, False],
		     [False, False, False, False, False, False, True, True],
		     [True, True, True, True, True, True, True, True]]

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
| isLEFT = (LEFT, world)
= (RIGHT, world)

// Direction at end = RIGHT
getPeriod :: !Pointer !Int !*World -> (!RTime, !*World)
getPeriod resP fd world
#! t1p = malloc 8
#! (t1, world) = getRTime t1p world
#! t2p = malloc 8
#! (t2, world) = getInterruptTime t2p fd world
= (divRTime (t2 - t1) 2, world)

Start world
#! resultP = malloc 8
#! (iFd, world) = setupInterruptHandlerTask world
#! (_, world) = setupLEDS world
#! (direction, world) = getPendulumDirection resultP iFd world
#! world = waitForNextRight direction resultP iFd world
#! (rperiod, world) = getPeriod resultP iFd world
#! (lperiod, world) = getPeriod resultP iFd world
#! lwait = divRTime (rperiod - lperiod) 2
#! rwait = multRTime (rperiod - lwait) 2
#! (leds, world) = getFDs [0 .. 7] world
#! pixeltime = getPixelTime
#! pixeltime = multRTime pixeltime 7
#! lwait2 = lwait - pixeltime
#! rwait2 = rwait - pixeltime
= interruptHandler resultP iFd RIGHT (lwait2, rwait2) leds world
