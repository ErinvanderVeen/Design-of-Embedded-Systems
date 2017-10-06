definition module Xenomai

import StdOverloaded
import System._Pointer

:: LED :== Int
:: RTime :== Pointer

instance < RTime

instance - RTime

setupLEDS :: !*World -> (!Int, !*World)

// Waits for the specified RTime
spinTimer :: !RTime !Pointer !*World -> *World

getInterruptTime :: !Pointer !Int !*World -> (!RTime, !*World)

getRTime :: !Pointer !*World -> (!RTime, !*World)

getFDs :: ![Int] !*World -> (![LED], !*World)

blinkLEDS :: ![LED] !*World -> (![LED], !*World)

printCharacter :: ![LED] ![[Bool]] !*World -> (![LED], !*World)

setupInterruptHandlerTask :: !*World -> (!Int, !*World)
