definition module Xenomai

import System._Pointer

rtTaskCreate :: !Pointer !Pointer !Int !Int !Int !*World -> (!Int, !*World)

rtTaskStart :: !Pointer !Pointer !Pointer -> (!Int, !*World)
