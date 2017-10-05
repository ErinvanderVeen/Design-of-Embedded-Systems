implementation module ex09

//import Xenomai
import StdEnv

foreign export ccall add
add :: !Int !Int -> Int
add a b = a + b

start_add_task_clean :: !*World -> (!Int, !*World)
start_add_task_clean w = code {
	ccall start_add_task "P:I:A"
}

Start w = start_add_task_clean w
