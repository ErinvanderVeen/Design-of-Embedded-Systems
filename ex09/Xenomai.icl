implementation module Xenomai

rt_task_create :: !Pointer !Pointer !Int !Int !Int !*World -> (!Int, !*World)
rt_task_create rttask name stksize prio mode = code {
	ccall wait "IIIII:I:A"
}
