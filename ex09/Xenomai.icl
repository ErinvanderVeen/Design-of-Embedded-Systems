implementation module Xenomai

import StdOverloaded
import System._Pointer
import Data.Func
import System._Posix
import StdBool
import StdInt

:: LED :== Int
:: RTime :== Pointer

instance < RTime
where
	(<) p1 p2
	#! res = helper p1 p2
	| res == 1 = True
	= False
	where
		helper :: !RTime !RTime -> Int
		helper p1 p2 = code {
			ccall less_than "pp:I"
		}

instance - RTime
where
	(-) p1 p2 = code {
		ccall subtract "pp:p"
	}

divRTime :: !RTime !Int -> RTime
divRTime p i = code {
	ccall divRTime "pI:p"
}

multRTime :: !RTime !Int -> RTime
multRTime p i = code {
	ccall multRTime "pI:p"
}

getPixelTime :: RTime
getPixelTime = code {
	ccall get_pixel_time ":p"
}

// Waits for Interrupt, then returns the time at which the interrupt happend
getInterruptTime :: !Pointer !Int !*World -> (!RTime, !*World)
getInterruptTime resultP fd world
#! (_, world) = read fd resultP 4 world
= getRTime resultP world

setupLEDS :: !*World -> (!Int, !*World)
setupLEDS world = code {
	ccall setup_leds ":I:A"
}

getRTime :: !Pointer !*World -> (!RTime, !*World)
getRTime resultP world
#! (resultP, world) = getRTime` resultP world
= (resultP, world)
where
	getRTime` :: !Pointer !*World -> (!RTime, !*World)
	getRTime` p world = code {
		ccall get_timer "p:p:A"
	}

setupInterruptHandlerTask :: !*World -> (!Int, !*World)
setupInterruptHandlerTask w = code {
	ccall setup_interrupt_handler ":I:A"
}

getFDs :: ![Int] !*World -> (![LED], !*World)
getFDs leds world = mapSt getFD leds world

getFD :: !Int !*World -> (!LED, !*World)
getFD l w = code {
	ccall get_fd "I:I:A"
}

// Waits for the specified RTime
spinTimer :: !RTime !*World -> *World
spinTimer rtime world
#! (_, world) = spinTimer` rtime world
= world
where
	spinTimer` :: !RTime !*World -> (!Int, !*World)
	spinTimer`  p w = code {
		ccall spin_timer "p:I:A"
	}

blinkLEDS :: ![LED] !*World -> (![LED], !*World)
blinkLEDS leds world
#! (leds, world) = mapSt ledON leds world
#! (_, world) = waitBlink world
= mapSt ledOFF leds world
where
	ledON :: !LED !*World -> (!LED, !*World)
	ledON led world = code {
		ccall led_on "I:I:A"
	}
	ledOFF :: !LED !*World -> (!LED, !*World)
	ledOFF led world = code {
		ccall led_off "I:I:A"
	}
	waitBlink :: !*World -> (!Int, !*World)
	waitBlink w = code {
		ccall wait_blink ":I:A"
	}

printCharacter :: ![LED] ![[Bool]] !*World -> (![LED], !*World)
printCharacter leds [] world = (leds, world)
printCharacter leds [b: bs] world
#! on = filterLeds leds b
#! (_, world) = blinkLEDS on world
= printCharacter leds bs world
where
	filterLeds :: ![LED] ![Bool] -> [LED]
	filterLeds _ [] = []
	filterLeds [] _ = []
	filterLeds [l : ls] [True : bs] = [l : filterLeds ls bs]
	filterLeds [l : ls] [False : bs] = filterLeds ls bs
