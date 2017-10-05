module ex09

import Xenomai
import StdEnv
import System._Pointer
import System._Posix

//foreign export ccall interrupt_handler_clean
//interrupt_handler_clean :: !Int -> !Int
createInterruptHandlerTask :: !*World -> (!Pointer, !*World)
createInterruptHandlerTask w = code {
	ccall create_interrupt_handler_task ":I:A"
}

getInterruptHandlerVoid :: !*World -> (!Pointer, !*World)
getInterruptHandlerVoid w = code {
	ccall get_interrupt_handler_void ":I:A"
}

createInterruptTask :: !*World -> (!Pointer, !*World)
createInterruptTask world
# (taskP, world) = createInterruptHandlerTask world
# (voidP, world) = getInterruptHandlerVoid world
# taskName = packString "Frouke"
# taskNameP = malloc 7
# taskNameP = writeCharArray taskNameP taskName
# (_, world) = rtTaskCreate taskP taskNameP 0 50 0 world
= (taskP, world)

Start world
# (iTaskP, world) = createInterruptTask world
# (iVoidP, world) = getInterruptHandlerVoid world
# (_, world) = rtTaskStart iTaskP iVoidP 0
= world

