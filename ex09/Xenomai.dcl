definition module Xenomai

import StdOverloaded
import System._Pointer

:: LED :== Int
:: RTime :== Pointer

instance < RTime

instance - RTime

divRTime :: !RTime !Int -> RTime
multRTime :: !RTime !Int -> RTime

getPixelTime :: RTime

setupLEDS :: !*World -> (!Int, !*World)

// Waits for the specified RTime
spinTimer :: !RTime !*World -> *World

getInterruptTime :: !Pointer !Int !*World -> (!RTime, !*World)

getRTime :: !Pointer !*World -> (!RTime, !*World)

getFDs :: ![Int] !*World -> (![LED], !*World)

blinkLEDS :: ![LED] !*World -> (![LED], !*World)

printCharacter :: ![LED] ![[Bool]] !*World -> (![LED], !*World)

setupInterruptHandlerTask :: !*World -> (!Int, !*World)
