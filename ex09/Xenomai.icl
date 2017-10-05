implementation module Xenomai

import System._Pointer

rtTaskCreate :: !Pointer !Pointer !Int !Int !Int !*World -> (!Int, !*World)
rtTaskCreate rttask name stksize prio mode world = code {
	ccall rt_task_create "IIIII:I:A"
}

rtTaskStart :: !Pointer !Pointer !Pointer -> (!Int, !*World)
rtTaskStart task entry arg = code {
	ccall rt_task_start "III:I:A"
}
