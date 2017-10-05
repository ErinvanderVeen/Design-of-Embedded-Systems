definition module Xenomai

import System._Pointer

rt_task_create :: !Pointer !Pointer !Int !Int !Int !*World -> (!Int, !*World)
