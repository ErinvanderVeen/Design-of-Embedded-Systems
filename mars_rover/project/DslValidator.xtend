/*
 * generated by Xtext 2.12.0
 */
package org.vanderveen.ev3rt.validation

import org.eclipse.xtext.validation.Check
import org.vanderveen.ev3rt.behaviourDSL.Behaviour
import org.vanderveen.ev3rt.behaviourDSL.BehaviourDSLPackage.Literals
import org.vanderveen.ev3rt.behaviourDSL.ColorValue
import org.vanderveen.ev3rt.behaviourDSL.Condition
import org.vanderveen.ev3rt.behaviourDSL.Distance
import org.vanderveen.ev3rt.behaviourDSL.Mission
import org.vanderveen.ev3rt.behaviourDSL.Operator
import org.vanderveen.ev3rt.behaviourDSL.Sensor
import org.vanderveen.ev3rt.behaviourDSL.TouchValue

/**
 * This class contains custom validation rules. 
 *
 * See https://www.eclipse.org/Xtext/documentation/303_runtime_concepts.html#validation
 */
class DslValidator extends AbstractDslValidator {
	
	@Check
	def checkForIdenticalBehaviorName(Mission root){
		
		var behaviors = root.behaviours
		for(var i = 0; i < behaviors.size; i++)
		{
			for(var j = i+1; j< behaviors.size; j++)
			{
				if(behaviors.get(i).name.equals(behaviors.get(j).name))
					error("Identical behavior names are not allowed", Literals.MISSION__BEHAVIOURS, j);	
			}
		}
	}
	
	
	@Check
	def checkForIdentialVariables(Mission root){
		
		var variables = root.variables
		for(var i = 0; i < variables.size; i++)
		{
			for(var j = i+1; j< variables.size; j++)
			{
				if(variables.get(i).name.equals(variables.get(j).name) &&
				   variables.get(i).value.equals(variables.get(j).value)
				)
				   
					error("Duplicate variable initialization", Literals.MISSION__VARIABLES, j);	
			}
		}
	}
	
		@Check
	def checkForSensor(Behaviour b){
		
		for(var i = 0; i < b.conditions.size; i++)
		{
			if(b.conditions.get(i) instanceof Condition) {
				var c = b.conditions.get(i) as Condition
				if(c.sensor == Sensor.COLOR_L || c.sensor == Sensor.COLOR_C || c.sensor == Sensor.COLOR_R) {
					if(!(c.value instanceof ColorValue))
						error("Incorrect Value for ColorSensor", Literals.BEHAVIOUR__CONDITIONS, i)
				}
			}
			
			
			if(b.conditions.get(i) instanceof Condition) {
				var c = b.conditions.get(i) as Condition
				if(c.sensor == Sensor.SONIC_F || c.sensor == Sensor.SONIC_B) {
					if(!(c.value instanceof Distance))
						error("Incorrect Value for UltrasonicSensor", Literals.BEHAVIOUR__CONDITIONS, i)
				}
			}
			
			if(b.conditions.get(i) instanceof Condition) {
				var c = b.conditions.get(i) as Condition
				if(c.sensor == Sensor.TOUCH_L || c.sensor == Sensor.TOUCH_R) {
					if(!(c.value instanceof TouchValue))
						error("Incorrect Value for TouchSensor", Literals.BEHAVIOUR__CONDITIONS, i)
				}
			}
			
			if(b.conditions.get(i) instanceof Condition) {
				var c = b.conditions.get(i) as Condition
				if(c.sensor == Sensor.COLOR_L || c.sensor == Sensor.COLOR_R || c.sensor ==Sensor.COLOR_C) {
					if(c.operator != Operator.EQ && c.operator != Operator.NEQ)
						error("Incorrect Operator for ColorSensor", Literals.BEHAVIOUR__CONDITIONS, i)
				}
			}
			
			if(b.conditions.get(i) instanceof Condition) {
				var c = b.conditions.get(i) as Condition
				if(c.sensor == Sensor.TOUCH_L || c.sensor == Sensor.TOUCH_R) {
					if(c.operator != Operator.EQ && c.operator != Operator.NEQ)
						error("Incorrect Operator for TouchSensor", Literals.BEHAVIOUR__CONDITIONS, i)
				}
			}
			
		}
	}
}